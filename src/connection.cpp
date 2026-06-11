#include "connection.hpp"
#include "event_handler.hpp"
#include <cstddef>
#include <iostream>
#include <stdint.h>
#include <exception>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

Connection::Connection(int fd, const Listen & listen, Epoll & epoll)
: fd_(fd), state_(kReading), epoll_(epoll), listen_(listen)
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

	if (state_ == kReading && events & EPOLLIN) {
		char read_buf[kReadBufferSize];

		size_t n = recv(fd_, read_buf, kReadBufferSize, 0);
		if (n <= 0)
			return kClose;
		std::cout << read_buf << std::endl;


		// HttpRequest request();
		// HttpResponse response = 
		// int ret = parser_.add(read_buf, n);
		// switch(ret)
		// case kNeed:
		// 	return kKeep;
		// case kComplete:
		// 	request = parser_.get_request();
		// 	response = parser.handler();
		// 	response >> write_buf;
		// 	state_ = kWriting;
		// 	epoll_.Mod(fd_, EPOLLOUT, this);
		// 	return kKeep
		// case kError:
		// 	error_response >> write_buf;
		// 	state_ = kWriting;
		// 	epoll_.Mod(fd_, EPOLLOUT, this);
		// 	return kKeep;
		// default:
		// 	return kClose;
		// 
	} else if (state_ == kWriting && events & EPOLLOUT) {
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

Connection::~Connection()
{
	try {
		epoll_.Del(fd_);
	} catch (std::exception &) {}
	close(fd_);
}
