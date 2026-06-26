#include "conn_handler.hpp"
#include "cgi_handler.hpp"
#include "cgi_in_handler.hpp"
#include "event_handler.hpp"
#include "epoll.hpp"
#include "listen_handler.hpp"
#include "reactor.hpp"
#include "../webserv.hpp"

#include <cstddef>
#include <ctime>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include <exception>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <vector>

ConnHandler::ConnHandler(sockaddr_in addr, int fd, const ListenHandler & listen,
			 Epoll & epoll, Reactor & reactor)
: fd_(fd), state_(kReading), addr_(addr), write_off_(0),
	last_activity_(time(NULL)), epoll_(epoll), reactor_(reactor), listen_(listen), cgi_(NULL)
{
	try {
		router_.set_config(const_cast<Config &>(listen_.config())); // a remodifier dans la classe pour eviter le cast degueu
		epoll_.Add(fd_, EPOLLIN, this);
	} catch (std::exception &) {
		close(fd_);
		throw;
	}
}

int	ConnHandler::HandleEvent(uint32_t events)
{
	if (events & (EPOLLERR | EPOLLHUP))
		return kClose;

	if (CheckTimeout(time(NULL)) == kClose)
		return kClose;
	last_activity_ = time(NULL);

	if (state_ == kReading && events & EPOLLIN) {
		char read_buf[kReadBufferSize];

		ssize_t n = recv(fd_, read_buf, kReadBufferSize - 1, 0);
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
		
	} else if (state_ == kWriting && events & EPOLLOUT) {
		size_t	remaining = write_buf_.size() - write_off_;
		ssize_t	n = send(
			fd_, &write_buf_[0] + write_off_, remaining, 0);

		if (n > 0) {
			write_off_ += n;
			// if HTTP 1.1 need to keep alive and turn to EPOLLIN
			if (write_off_ == write_buf_.size())
				return kClose;
			return kKeep;
		}
	}
	return kClose;
}

void	ConnHandler::HandleRequest()
{
	parser_.ParseRequest(request_);
	std::cout << parser_.get_buf() << std::endl;
	
	RouteResult result = router_.ProcessRequest(request_);
	switch (result.get_type()) {
		case kRouteResponse: {
			HttpResponse response = result.get_response();

			write_buf_ = response.ToCharVector();
			state_ = kWriting;
			epoll_.Mod(fd_, EPOLLOUT, this);
			break;
		}
		case kRouteCgi:
			StartCgi(result.get_plan());
			break;
		default:
			break;
	}
}



std::vector<std::string> ConnHandler::BuildCgiEnv(const CgiPlan & plan) const
{
	std::vector<std::string>	env;
	env.push_back("AUTH_TYPE="); /* AUTH_TYPE don't need to be defined
	because we don't do authentification */
	std::stringstream content_length;
	content_length << "CONTENT_LENGTH=" << request_.get_body().size();
	env.push_back(content_length.str());
	std::stringstream content_type;
	content_type << "CONTENT_TYPE=" << request_.get_header("content-type");
	env.push_back("CONTENT_TYPE=");
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("PATH_INFO=" + plan.path_info);
	env.push_back("PATH_TRANSLATED=");
	env.push_back("QUERY_STRING=" + plan.query_string);
	env.push_back("REMOTE_ADDR=" + webserv::utils::AddrToString(addr_));
	env.push_back("REMOTE_HOST=" + webserv::utils::AddrToString(addr_));
	env.push_back("REMOTE_IDENT="); // authentification, no need to include
	env.push_back("REMOTE_USER="); // authentification, no need to include
	env.push_back("REQUEST_METHOD=" + request_.get_method()); // a changer selon la methode
	env.push_back("SCRIPT_NAME=" + plan.script_name);
	std::string	server_name = listen_.config().server_name();
	// Value "Host" in the request
	if (server_name.empty())
		server_name = listen_.listen_info().host;
	env.push_back("SERVER_NAME=" + server_name);
	env.push_back("SERVER_PORT=" + listen_.listen_info().port);
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("SERVER_SOFTWARE=webserv/1.0");

	return env;
}

void	ConnHandler::StartCgi(const CgiPlan & plan)
{
	std::string			path;
	std::string			script_dir;
	std::vector<std::string>	argv_str;
	std::vector<std::string>	envp_str;


	path = plan.interpreter.empty() ? plan.script_path : plan.interpreter;

	script_dir = webserv::utils::Basedir(plan.script_path);

	argv_str.push_back(webserv::utils::Basename(path));
	if (path == plan.interpreter)
		argv_str.push_back(plan.script_path);

	envp_str = BuildCgiEnv(plan);

	int	out_pipe[2];
	if (pipe(out_pipe) < 0)
		throw std::runtime_error("pipe() failed");

	if (webserv::fd::SetNonBlock(out_pipe[0]) < 0)
		throw std::runtime_error("SetNonBlock() failed");

	int	in_pipe[2];
	if (pipe(in_pipe) < 0)
		throw std::runtime_error("pipe() failed");

	if (webserv::fd::SetNonBlock(in_pipe[1]) < 0)
		throw std::runtime_error("SetNonBlock() failed");

	std::vector<char *>	argv = webserv::utils::ToCStrArray(argv_str);
	std::vector<char *>	envp = webserv::utils::ToCStrArray(envp_str);

	pid_t	pid = fork();
	if (pid < 0)
		throw std::runtime_error("fork() failed");

	if (pid == 0) {
		if (dup2(in_pipe[0], STDIN_FILENO) < 0)
			_exit(1);
		if (dup2(out_pipe[1], STDOUT_FILENO) < 0)
			_exit(1);
		close(in_pipe[0]); close(in_pipe[1]);
		close(out_pipe[0]); close(out_pipe[1]);
		if (chdir(script_dir.c_str()) < 0)
			_exit(1);
		execve(path.c_str(), &argv[0], &envp[0]);
		_exit(1);
	}

	close(in_pipe[0]);
	if (request_.get_body().empty())
		close(in_pipe[1]);
	else {
		CgiInHandler *cgi_in = new CgiInHandler(
			in_pipe[1], epoll_, request_.get_body());
		reactor_.AddEventHandler(cgi_in);
	}
	close(out_pipe[1]);
	cgi_ = new CgiHandler(pid, out_pipe[0], *this, epoll_);
	reactor_.AddEventHandler(cgi_);
}

void	ConnHandler::OnCgiDone(const std::string & output)
{
	cgi_ = NULL;
	HttpResponse	response = router_.CgiResponse(output);
	write_buf_ = response.ToCharVector();
	state_ = kWriting;
	epoll_.Mod(fd_, EPOLLOUT, this);
}

void	ConnHandler::OnCgiError(int status)
{
	cgi_ = NULL;
	HttpResponse	response = router_.ErrorResponse(status);
	write_buf_ = response.ToCharVector();
	state_ = kWriting;
	epoll_.Mod(fd_, EPOLLOUT, this);
}

void	ConnHandler::Detach() { cgi_ = NULL; }

int	ConnHandler::CheckTimeout(time_t now)
{
	if (difftime(now, last_activity_) >= kTimeoutSecs)
		return kClose;
	return kKeep;
}

ConnHandler::~ConnHandler()
{
	if (cgi_)
		cgi_->Detach();
	try {
		epoll_.Del(fd_);
	} catch (std::exception &) {}
	close(fd_);
}
