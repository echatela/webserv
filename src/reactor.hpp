#ifndef REACTOR_HPP
#define REACTOR_HPP

#include "epoll.hpp"
#include "listen.hpp"
#include "connection.hpp"
#include "config.hpp"
#include <set>
#include <vector>

class Reactor
{
private:
	const Config			config_;

	Epoll				epoll_;
	std::vector<Listen*>		listens_;
	std::vector<Connection*>	connections_;
//	std::vector<EventHandler*>	event_handlers_;

	std::set<EventHandler*>		closed_;

	void	Dispatch(int n);
	void	CloseHandler();

	Reactor();
	Reactor(const Reactor & src);
	Reactor &	operator=(const Reactor & rhs);

public:
	Reactor(const Config & config);

	void	Run();
	void	AddConnection(int fd, const Listen & listen);

	~Reactor();
};

#endif
