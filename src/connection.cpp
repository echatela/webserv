#include "connection.hpp"
#include "event_handler.hpp"
#include "webserv.hpp"
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

Connection::Connection(int fd, Epoll & epoll)
: fd_(fd), epoll_(epoll)
{

	if (webserv::fd::SetNonBlock(fd_) < 0) {
		close(fd_);
		throw std::runtime_error("Couldn't set Connection nonblock...\n");
	}
	if (webserv::fd::SetCloExec(fd_) < 0) {
		close(fd_);
		throw std::runtime_error("Couldn't set Connection cloexec...\n");
	}
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
	epoll_.Del(fd_);
	close(fd_);
}
