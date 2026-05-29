#ifndef REACTOR_HPP
#define REACTOR_HPP

#include "Listen.hpp"
#include "Connection.hpp"
#include "Config.hpp"
#include <vector>

class Reactor
{
private:
	Config const		_config;
	std::vector<Listen>	_listens;
	std::vector<Connection>	_connections;

public:
	Reactor();
	Reactor(Config const & config);
	Reactor(Reactor const & src);
	~Reactor();

	Reactor &	operator=(Reactor const & rhs);

	void	run();
};

#endif
