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

	Reactor();
	Reactor(Reactor const & src);
	Reactor &	operator=(Reactor const & rhs);

public:
	Reactor(Config const & config);
	~Reactor();

	void	Run();
};

#endif
