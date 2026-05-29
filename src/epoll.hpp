#ifndef EPOLL_HPP
#define EPOLL_HPP

#include <sys/epoll.h>

#define MAX_EVENTS 128

class Epoll
{
private:
	int	epfd_;
	struct epoll_event	events_[MAX_EVENTS];

	Epoll(Epoll const & src);
	Epoll &	operator=(Epoll const & rhs);

public:
	Epoll();

	void	add(int fd, int events, void *data);
	void	mod(int fd, int events, void *data);
	void	del(int fd);

	int	wait();

	const struct epoll_event *	get_events() const;

	~Epoll();
};

#endif
