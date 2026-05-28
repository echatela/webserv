#include "Server.hpp"

Server::Server()
{
}

Server::Server(Config const & config)
: _config(config)
{
	const std::vector<ListenInfo>	listensInfo = _config.getListensInfo();

	for (int i = 0; i < listensInfo.size(); i++) {

	}
}

Server::Server(Server const & src)
{
	*this = src;
}

Server::~Server()
{
}

Server &	Server::operator=(Server const & rhs)
{
	if (this != &rhs)
	{
		// Copy attributes here
	}
	return (*this);
}
