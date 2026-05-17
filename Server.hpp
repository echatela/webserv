#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

class Server
{
private:
	int	_domain;
	int	_port;
	int	_service;
	int	_protocol;
	int	_backlog;
	u_long	_interface;

	int	_socket;
	struct sockaddr_in	_address;

public:
	Server(int domain = AF_INET, int port = 80, int service = SOCK_STREAM,
	int protocol = 0, int backlog = 10, u_long interface = INADDR_ANY);
	Server(const Server & src);
	~Server();
	Server &	operator=(const Server & rhs);

	void	launch();
};
