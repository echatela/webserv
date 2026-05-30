#include "connection.hpp"
#include "event_handler.hpp"
#include "webserv.hpp"
#include <cstdint>
#include <exception>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

Connection::Connection(int fd, Epoll & epoll)
: fd_(fd), epoll_(epoll)
{
	webserv::fd::SetNonBlock(fd_);
	webserv::fd::SetCloExec(fd_);
	try {
		epoll_.add(fd_, EPOLLIN, this);
	} catch (std::exception &) {
		close(fd_);
		throw;
	}
}

int	Connection::HandleEvent(uint32_t events)
{
	if (events & (EPOLLIN | EPOLLHUP))
		return kClose;

	if (events & EPOLLIN) {
		char read_buf[kReadBufferSize];

		int n = recv(fd_, read_buf, kReadBufferSize, 0);
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
	epoll_.del(fd_);
	close(fd_);
}
