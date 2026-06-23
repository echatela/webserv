#include "cgi_handler.hpp"
#include "epoll.hpp"
#include "event_handler.hpp"
#include <cstddef>
// #include <cstdint>
#include <ctime>
#include <exception>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <unistd.h>
#include "conn_handler.hpp"

// NOTE: need to understand how to detach conn and cgi

CgiHandler::CgiHandler(pid_t pid, int stdout_fd, ConnHandler& conn,
		       Epoll& epoll)
: pid_(pid), stdout_fd_(stdout_fd), conn_(&conn), epoll_(epoll),
	start_time_(time(NULL))
{
	try {
		epoll_.Add(stdout_fd_, EPOLLIN, this);
	} catch (std::exception&) {
		close(stdout_fd);
		throw;
	}
}

int	CgiHandler::HandleEvent(uint32_t events)
{
	std::cout << "in handle cgi event\n";

	if (events & EPOLLHUP) {
		if (conn_)
			conn_->OnCgiDone(output_buf_);
		return kClose;
	}

	if (events & (EPOLLERR | EPOLLHUP))
		return kClose;

	if (CheckTimeout(time(NULL)) == kClose)
		return kClose;

	char	read_buf[kReadBufferSize];
	ssize_t	n = read(stdout_fd_, read_buf, kReadBufferSize);
	if (n < 0) {
		return kClose;
	} else if (n == 0) {
		if (conn_)
			conn_->OnCgiDone(output_buf_);
		return kClose;
	} else {
		output_buf_.append(read_buf, n);
		return kKeep;
	}
}

int	CgiHandler::CheckTimeout(time_t now)
{
	if (difftime(now, start_time_) >= kTimeoutSecs)
		return kClose;
	return kKeep;
}

void	CgiHandler::Detach()
{
	conn_ = NULL;
}

CgiHandler::~CgiHandler()
{
	int	status;
	pid_t	r = waitpid(pid_, &status, WNOHANG);
	if (r == 0) {
		kill(pid_, SIGKILL);
		waitpid(pid_, &status, 0);
	}

	if (conn_)
		conn_->Detach();
	epoll_.Del(stdout_fd_);
	close(stdout_fd_);
}
