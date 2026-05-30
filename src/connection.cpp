#include "connection.hpp"
#include "event_handler.hpp"
#include <cstdint>
#include <exception>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

Connection::Connection(int fd, const Listen & listen, Epoll & epoll)
: fd_(fd), listen_(listen), epoll_(epoll)
{
	try {
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

	if (events & EPOLLIN) {
		char read_buf[kReadBufferSize];

		size_t n = recv(fd_, read_buf, kReadBufferSize, 0);
		if (n <= 0)
			return kClose;

		buf_.insert(buf_.end(), read_buf, read_buf + n);
	}

	if (events & EPOLLOUT) {
	}

	return kKeep;
}

Connection::~Connection()
{
	try {
		epoll_.Del(fd_);
	} catch (std::exception &) {}
	close(fd_);
}
