#pragma once

#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAX_EVENTS	128

class Server
{
private:
	int	_domain;
	int	_port;
	int	_service;
	int	_protocol;
	int	_backlog;
	u_long	_interface;
	struct sockaddr_in	_address;

	int	_socket;

	int	_eventsFlags;
	struct epoll_event	_ev;
	struct epoll_event	_events[MAX_EVENTS];

	int	_epfd;

public:
	Server(int domain = AF_INET, int port = 8080, int service = SOCK_STREAM,
	int protocol = 0, int backlog = 10, u_long interface = INADDR_ANY,
	int eventsFlags = EPOLLIN | EPOLLET);
	Server(const Server & src);
	~Server();
	Server &	operator=(const Server & rhs);

	void	launch();
};