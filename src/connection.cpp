#include "connection.hpp"
#include "event_handler.hpp"
#include "webserv.hpp"
#include <cstdint>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

Connection::Connection(int fd) : fd_(fd)
{
	webserv::fd::SetNonBlock(fd_);
	webserv::fd::SetCloExec(fd_);
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
	close(fd_);
}
