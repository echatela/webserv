#ifndef LISTEN_HPP
#define LISTEN_HPP

#include "epoll.hpp"
#include "event_handler.hpp"
#include <stdint.h>
#include <netinet/in.h>

class Reactor;

class Listen : public EventHandler
{
private:
	int		fd_;
	Epoll &		epoll_;
	Reactor &	reactor_;

	Listen();
	Listen(const Listen & src);
	Listen &	operator=(const Listen & rhs);

public:
	Listen(sockaddr_in addr, Epoll & epoll, Reactor & reactor);

	int	HandleEvent(uint32_t events);

	int	get_fd() const;

	~Listen();
};

#endif
