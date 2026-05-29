#include "epoll.hpp"
#include "webserv.hpp"
#include <stdexcept>
#include <sys/epoll.h>
#include <unistd.h>

Epoll::Epoll()
{
	epfd_ = epoll_create(1);
	if (epfd_ < 0)
		throw std::runtime_error("Couldn't epoll_create()...\n");

	if (webserv::fd::SetCloExec(epfd_) < 0)
		throw std::runtime_error("Couldn't SetCloExec(epfd_)...\n");
}

void	Epoll::add(int fd, int events, void *data)
{
	struct epoll_event	ev;
	ev.events = events;
	ev.data.ptr = data;

	if (epoll_ctl(epfd_, events, fd, &ev) < 0)
		throw std::runtime_error("Couldn't epoll_ctl(add)...\n");
}

void	Epoll::mod(int fd, int events, void *data)
{
	struct epoll_event	ev;
	ev.events = events;
	ev.data.ptr = data;

	if (epoll_ctl(epfd_, events, fd, &ev) < 0)
		throw std::runtime_error("Couldn't epoll_ctl(mod)...\n");
}

void	Epoll::del(int fd)
{
	if (epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL) < 0)
		throw std::runtime_error("Couldn't epoll_ctl(del)...\n");
}

int	Epoll::wait()
{
	return (epoll_wait(epfd_, events_, MAX_EVENTS, -1));
}

const struct epoll_event *	Epoll::get_events() const { return events_; }

Epoll::~Epoll()
{
	close(epfd_);
}
