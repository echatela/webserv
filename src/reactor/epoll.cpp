#include "epoll.hpp"
#include "../webserv.hpp"
#include <stdexcept>
#include <sys/epoll.h>
#include <unistd.h>

Epoll::Epoll()
{
	epfd_ = epoll_create(1);
	if (epfd_ < 0)
		throw std::runtime_error("epoll_create() failed");

	if (webserv::fd::SetCloExec(epfd_) < 0)
		throw std::runtime_error("SetCloExec() failed");
}

void	Epoll::Add(int fd, int events, void *data)
{
	struct epoll_event	ev;
	ev.events = events;
	ev.data.ptr = data;

	if (epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) < 0)
		throw std::runtime_error("epoll_ctl(add) failed");
}

void	Epoll::Mod(int fd, int events, void *data)
{
	struct epoll_event	ev;
	ev.events = events;
	ev.data.ptr = data;

	if (epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev) < 0)
		throw std::runtime_error("epoll_ctl(mod) failed");
}

void	Epoll::Del(int fd)
{
	if (epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL) < 0)
		throw std::runtime_error("epoll_ctl(del) failed");
}

int	Epoll::Wait(int timeout_ms)
{
	return (epoll_wait(epfd_, events_, MAX_EVENTS, timeout_ms));
}

const struct epoll_event *	Epoll::events() const { return events_; }

Epoll::~Epoll()
{
	close(epfd_);
}
