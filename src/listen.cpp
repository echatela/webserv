#include "listen.hpp"
#include "connection.hpp"
#include "event_handler.hpp"
#include "webserv.hpp"
#include <stdint.h>
#include <exception>
#include <unistd.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include "reactor.hpp"

Listen::Listen(sockaddr_in addr, Epoll & epoll, Reactor & reactor, const ServerConfig & config)
: epoll_(epoll), reactor_(reactor), config_(config)
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
	try {
		epoll_.Add(fd_, EPOLLIN, this);
	} catch (std::exception&) {
		close(fd_);
		throw std::runtime_error("Couldn't add socket to epoll...\n");
	}
}

int	Listen::HandleEvent(uint32_t events)
{
	if (events & (EPOLLERR | EPOLLHUP))
		return kKeep;

	int	client_fd = accept(fd_, NULL, NULL);
	if (client_fd < 0)
		return kKeep;

	if (webserv::fd::SetNonBlock(fd_) < 0
		|| webserv::fd::SetCloExec(fd_) < 0) {
		close(fd_);
		return kKeep;
	}

	Connection *	conn = NULL;
	try {
		conn = new Connection(client_fd, *this, epoll_);
		reactor_.AddEventHandler(conn);
	} catch (std::exception &) {}
	return kKeep;
}

int	Listen::get_fd() const { return fd_; }

const ServerConfig &	Listen::get_config() const { return config_; }

Listen::~Listen()
{
	try {
		epoll_.Del(fd_);
	} catch (std::exception &) {}
	close(fd_);
}
