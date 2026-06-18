#include "connection.hpp"
#include "cgi_handler.hpp"
#include "event_handler.hpp"
#include "epoll.hpp"
#include "listen.hpp"
#include "../webserv.hpp"

#include <cstddef>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <exception>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

Connection::Connection(int fd, const Listen & listen, Epoll & epoll)
: fd_(fd), state_(kReading), write_off_(0), last_activity_(time(NULL)),
	epoll_(epoll), listen_(listen), cgi_(NULL)
{
	try {
		router_.set_config(listen_.get_config());
		epoll_.Add(fd_, EPOLLIN, this);
	} catch (std::exception &) {
		close(fd_);
		throw;
	}
}

int	Connection::HandleEvent(uint32_t events)
{
	if (events & (EPOLLERR | EPOLLHUP))
		return kClose;

	last_activity_ = time(NULL);

	if (state_ == kReading && events & EPOLLIN) {
		char read_buf[kReadBufferSize];

		size_t n = recv(fd_, read_buf, kReadBufferSize - 1, 0);
		if (n <= 0)
			return kClose;
		read_buf[n] = '\0';

		int ret = parser_.Add(read_buf, n);
		switch(ret) {
			case false:
				return kKeep;
			case true:
				HandleRequest();
				return kKeep;
			default:
				return kClose;
		}
		
	}
	else if (state_ == kWriting && events & EPOLLOUT)
	{
		size_t	remaining = write_buf_.size() - write_off_;
		size_t	n = send(
			fd_, write_buf_.data() + write_off_, remaining, 0);

		if (n > 0) {
			write_off_ += n;
			// if HTTP 1.1 need to keep alive and turn to EPOLLIN
			if (write_off_ == write_buf_.size())
				return kClose;
			return kKeep;
		} else if (n <= 0)
			return kClose;
	}
	return kKeep;
}

void	Connection::HandleRequest()
{
	parser_.ParseRequest(request_);
	std::cout << parser_.get_buf() << std::endl;

	RouteResult result = router_.HandleRequest(request_);
	switch (result.get_type()) {
		case kRouteResponse:
			HttpResponse response = result.get_response();
			std::cout << response.ToString() << std::endl;

			write_buf_ = response.ToCharVector();
			state_ = kWriting;
			epoll_.Mod(fd_, EPOLLOUT, this);
			break;
		case kRouteCgi:
			StartCgi(result.get_plan());
			break;
		default:
			break;
	}
}

void	Connection::StartCgi(const CgiPlan & plan)
{
	// Construire argv et envp MAINTENANT, dans le parent.
	// Pourquoi avant le fork ? Parce qu'allouer de la mémoire / lancer des
	// exceptions après un fork, c'est fragile. On veut un enfant minimaliste.
	//   argv = { interpreter, script_path, NULL }   (ou { script_path, NULL } si interpreter vide)
	//   envp = { "REQUEST_METHOD=GET", "QUERY_STRING=...", ..., NULL }
	
	int	out_pipe[2];
	if (pipe(out_pipe) < 0)
		throw std::runtime_error("pipe() failed");

	pid_t	pid = fork();
	if (pid < 0) {
		close(out_pipe[0]); close(out_pipe[1]);
		throw std::runtime_error("fork() failed");
	}

	if (pid == 0) {
		dup2(out_pipe[1], STDOUT_FILENO);
		close(out_pipe[0]);
		close(out_pipe[1]);
		chdir(/* path */);
		execve(path, argv, envp);
		_exit(1);
	}

	close(out_pipe[1]);
	webserv::fd::SetNonBlock(out_pipe[0]);
	webserv::fd::SetCloExec(out_pipe[0]);

	CgiHandler *	cgi = new CgiHandler(pid, out_pipe[0], this, listen_.get_reactor());
}

int	Connection::CheckTimeout(time_t now)
{
	if (difftime(now, last_activity_) >= kTimeoutSecs)
		return kClose;
	return kKeep;
}

Connection::~Connection()
{
	try {
		epoll_.Del(fd_);
	} catch (std::exception &) {}
	close(fd_);
}
