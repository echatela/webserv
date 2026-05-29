#ifndef REACTOR_HPP
#define REACTOR_HPP

#include "epoll.hpp"
#include "listen.hpp"
#include "connection.hpp"
#include "config.hpp"
#include <vector>

class Reactor
{
private:
	const Config			config_;
	Epoll				epoll_;
	std::vector<Listen*>		listens_;
	std::vector<Connection*>	connections_;

	Reactor();
	Reactor(const Reactor & src);
	Reactor &	operator=(const Reactor & rhs);

public:
	Reactor(const Config & config);
	~Reactor();

	void	Run();
};

#endif
