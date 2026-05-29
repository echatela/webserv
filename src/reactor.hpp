#ifndef REACTOR_HPP
#define REACTOR_HPP

#include "listen.hpp"
#include "connection.hpp"
#include "config.hpp"
#include <vector>

class Reactor
{
private:
	const Config		config_;
	std::vector<Listen>	listens_;
	std::vector<Connection>	connections_;

public:
	Reactor();
	Reactor(Config const & config);
	Reactor(Reactor const & src);
	~Reactor();

	Reactor &	operator=(Reactor const & rhs);

	void	Run();
};

#endif
