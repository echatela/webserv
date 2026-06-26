#include "cgi_in_handler.hpp"
#include "epoll.hpp"

#include <cstddef>
#include <exception>
#include <string>
#include <sys/types.h>
#include <unistd.h>

CgiInHandler::CgiInHandler(int stdin_fd, Epoll & epoll,
			   const std::string & body)
: stdin_fd_(stdin_fd), epoll_(epoll),
	write_buf_(body.begin(), body.end()), write_off_(0)
{
	try {
		epoll_.Add(stdin_fd_, EPOLLOUT, this);
	} catch (std::exception&) {
		close(stdin_fd_);
		throw;
	}
}

int	CgiInHandler::HandleEvent(uint32_t events)
{
	if (events & (EPOLLERR | EPOLLHUP)) {}
		return kClose;

	if (events & EPOLLOUT) {
		size_t remaining = write_buf_.size() - write_off_;
		ssize_t n = write(
			stdin_fd_, &write_buf_[0] + write_off_, remaining);
		if (n > 0) {
			write_off_ += n;
			if (write_off_ == write_buf_.size())
				return kClose;
			return kKeep;
		}
	}
	return kClose;
}

CgiInHandler::~CgiInHandler()
{
	try {
		epoll_.Del(stdin_fd_);
	} catch (std::exception&) {}
	close(stdin_fd_);
}
