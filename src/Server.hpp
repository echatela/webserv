#ifndef SERVER_HPP
#define SERVER_HPP

#include "Listen.hpp"
#include "Connection.hpp"
#include "Config.hpp"
#include <vector>

class Server
{
private:
	Config const		_config;
	std::vector<Listen>	_listens;
	std::vector<Connection>	_connections;

public:
	Server();
	Server(Config const & config);
	Server(Server const & src);
	~Server();

	Server &	operator=(Server const & rhs);

	void	run();
};

#endif
