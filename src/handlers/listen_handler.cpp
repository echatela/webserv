#include "listen_handler.hpp"
#include "conn_handler.hpp"
#include "event_handler.hpp"
#include "../webserv.hpp"
#include <stdint.h>
#include <exception>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include "reactor.hpp"

ListenHandler::ListenHandler(const ListenInfo & info, Epoll & epoll, Reactor & reactor,
	       const Config & config)
: epoll_(epoll), reactor_(reactor), config_(config), listen_info_(info)
{
	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_ < 0)
		throw std::runtime_error("Couldn't initialize socket...\n");

	int opt = 1;
	if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("Couldn't set adress to be reuseable...\n");
		
	if (bind(fd_, (struct sockaddr *)&info.address, sizeof(info.address))
		< 0) {
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

int	ListenHandler::HandleEvent(uint32_t events)
{
	if (events & (EPOLLERR | EPOLLHUP))
		return kKeep;

	struct sockaddr_in	addr;
	socklen_t		len = sizeof(addr);
	int	client_fd = accept(fd_, reinterpret_cast<
			struct sockaddr *>(&addr), &len);
	if (client_fd < 0)
		return kKeep;

	if (webserv::fd::SetNonBlock(client_fd) < 0
		|| webserv::fd::SetCloExec(client_fd) < 0) {
		close(client_fd);
		return kKeep;
	}

	ConnHandler *	conn = NULL;
	try {
		conn = new ConnHandler(addr, client_fd, *this, epoll_, reactor_);
		reactor_.AddEventHandler(conn);
	} catch (std::exception &) {}
	return kKeep;
}

int	ListenHandler::fd() const { return fd_; }

const Config &	ListenHandler::config() const { return config_; }

Reactor &	ListenHandler::reactor() const { return reactor_; }

const ListenInfo &	ListenHandler::listen_info() const { return listen_info_; }

ListenHandler::~ListenHandler()
{
	try {
		epoll_.Del(fd_);
	} catch (std::exception &) {}
	close(fd_);
}
