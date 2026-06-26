#include "cgi_handler.hpp"
#include "epoll.hpp"
#include "event_handler.hpp"
#include <cstddef>
#include <ctime>
#include <exception>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <unistd.h>
#include "conn_handler.hpp"

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
	if (events & EPOLLERR) {
		if (conn_)
			conn_->OnCgiError(502);
		return kClose;
	}

	if (CheckTimeout(time(NULL)) == kClose)
		return kClose;

	char	read_buf[kReadBufferSize];
	ssize_t	n = read(stdout_fd_, read_buf, kReadBufferSize);
	if (n < 0) {
		if (conn_)
			conn_->OnCgiError(502);
		return kClose;
	} else if (n == 0) {
		if (conn_) {
			if (!output_buf_.empty())
				conn_->OnCgiDone(output_buf_);
			else
				conn_->OnCgiError(502);
		}
		return kClose;
	} else {
		output_buf_.append(read_buf, n);
		return kKeep;
	}
}

int	CgiHandler::CheckTimeout(time_t now)
{
	if (difftime(now, start_time_) >= kTimeoutSecs) {
		if (conn_)
			conn_->OnCgiError(504);
		return kClose;
	}
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
	try {
		epoll_.Del(stdout_fd_);
	} catch (std::exception&) {}
	close(stdout_fd_);
}
