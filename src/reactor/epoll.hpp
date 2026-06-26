#ifndef EPOLL_HPP
#define EPOLL_HPP

#include <sys/epoll.h>

#define MAX_EVENTS 128

class Epoll
{
private:
	int			epfd_;
	struct epoll_event	events_[MAX_EVENTS];

	Epoll(Epoll const & src);
	Epoll &	operator=(Epoll const & rhs);

public:
	Epoll();

	void	Add(int fd, int events, void *data);
	void	Mod(int fd, int events, void *data);
	void	Del(int fd);

	int	Wait(int timeout_ms);

	const struct epoll_event *	events() const;

	~Epoll();
};

#endif
