#include "Server.hpp"

Server::Server()
{
}

Server::Server(Config const & config)
{
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
