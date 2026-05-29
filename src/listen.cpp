#include "listen.hpp"
#include "webserv.hpp"
#include <cstdint>
#include <unistd.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <fcntl.h>

Listen::Listen(sockaddr_in addr)
{
	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_ < 0)
		throw std::runtime_error("Couldn't initialize socket...\n");

	if (bind(fd_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		close(fd_);
		throw std::runtime_error("Couldn't bind socket...\n");
	}
	if (listen(fd_, SOMAXCONN) < 0) {
		close(fd_);
		throw std::runtime_error("Couldn't set socket to listen...\n");
	}
	if (webserv::fd::SetNonBlock(fd_) < 0) {
		close(fd_);
		throw std::runtime_error("Couldn't set socket nonblock...\n");
	}
	if (webserv::fd::SetCloExec(fd_) < 0) {
		close(fd_);
		throw std::runtime_error("Couldn't set socket cloexec...\n");
	}
}

int	Listen::HandleEvent(uint32_t events)
{
	
}

int	Listen::get_fd() const { return fd_; }

Listen::~Listen()
{
	close(fd_);
}
