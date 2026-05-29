#include "Listen.hpp"
#include "webserv.hpp"
#include <unistd.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <fcntl.h>

Listen::Listen(sockaddr_in addr)
{
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0)
		throw std::runtime_error("Couldn't initialize socket...\n");

	if (bind(_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("Couldn't bind socket...\n");

	if (listen(_fd, SOMAXCONN) < 0)
		throw std::runtime_error("Couldn't set socket to listen...\n");

	if (webserv::fd::setNonblock(_fd) < 0)
		throw std::runtime_error("Couldn't set socket nonblock...\n");

	if (webserv::fd::setCloexec(_fd) < 0)
		throw std::runtime_error("Couldn't set socket cloexec...\n");
}

Listen::Listen(Listen const & src)
{
	*this = src;
}

Listen::~Listen()
{
	close(_fd);
}

Listen &	Listen::operator=(Listen const & rhs)
{
	if (this != &rhs)
	{
		// Copy attributes here
	}
	return (*this);
}
