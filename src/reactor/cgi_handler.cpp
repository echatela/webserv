#include "cgi_handler.hpp"
#include "epoll.hpp"
#include "event_handler.hpp"
#include <cstddef>
// #include <cstdint>
#include <exception>
#include <sys/epoll.h>
#include <unistd.h>
#include "conn_handler.hpp"

// NOTE: need to understand how to detach conn and cgi

CgiHandler::CgiHandler(pid_t pid, int stdout_fd, ConnHandler& conn,
		       Epoll& epoll)
: pid_(pid), stdout_fd_(stdout_fd), connection_(&conn), epoll_(epoll)
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
	if (events && EPOLLERR | EPOLLHUP)
		return kClose;

	if (CheckTimeout(time(NULL)) == kClose)
		return kClose;

	char	read_buf[kReadBufferSize];
	size_t	n = read(stdout_fd_, read_buf, kReadBufferSize - 1);
	if (n <= 0) {
		return kClose;
	} else if (read_buf[n] == '\0') {
		connection_->OnCgiDone(output_buf_);
		return kClose;
	}
	return kKeep;
}

int	CgiHandler::CheckTimeout(time_t now)
{
	if (difftime(now, start_time_) >= kTimeoutSecs)
		return kClose;
	return kKeep;
}

CgiHandler::~CgiHandler()
{
	connection_->Detach();
	epoll_.Del(stdout_fd_);
	close(stdout_fd_);
}
