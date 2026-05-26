#ifndef SERVER_HPP
#define SERVER_HPP

#include "Config.hpp"
#include "Listener.hpp"
#include "Connection.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <vector>

class Server
{
private:
	Config const		_config;
	std::vector<Listener>	_listeners;

public:
	Server();
	Server(Config const & config);
	Server(Server const & src);
	~Server();

	Server &	operator=(Server const & rhs);

	void	run();
};

#endif
