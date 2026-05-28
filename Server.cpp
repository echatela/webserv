#include "Server.hpp"
#include "User.hpp"
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <fstream>

#define BUFFER_SIZE 16000

static int	set_nonblock(int fd)
{
	int	flags = fcntl(fd, F_GETFL);
	if (flags == -1)
		return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static std::string	read_file(const char *path)
{
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if (!file)
		throw std::runtime_error("Failed to open test.html\n");

	std::ostringstream content;
	content << file.rdbuf();
	return content.str();
}

Server::Server(int domain, int port, int service, int protocol, int backlog,
	       u_long interface, int eventsFlags)
	: _domain(domain), _port(port), _service(service), _protocol(protocol),
	_backlog(backlog), _interface(interface), _eventsFlags(eventsFlags)
{
	_address.sin_family = _domain;
	_address.sin_port = htons(_port);
	_address.sin_addr.s_addr = htonl(_interface);

	_socket = socket(_domain, _service, _protocol);
	if (_socket < 0)
		throw std::runtime_error("Failed to initialize/connect to socket...\n");

	if (bind(_socket, (struct sockaddr*)&_address, sizeof(_address)) < 0)
		throw std::runtime_error("Failed to bind socket...\n");

	if (listen(_socket, _backlog) < 0)
		throw std::runtime_error("Failed to start listening...\n");

	if (set_nonblock(_socket) < 0)
		throw std::runtime_error("Failed to set nonblocking socket...\n");

	_epfd = epoll_create1(EPOLL_CLOEXEC);
	if (_epfd < 0)
		throw std::runtime_error("Failed to create epollfd...\n");

	_ev.events = _eventsFlags;
	_ev.data.ptr = NULL;
	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, _socket, &_ev) < 0)
		throw std::runtime_error("Failed epoll_ctl(add _socket)");
}

Server::~Server()
{ }

void	Server::launch()
{
	while (1)
	{
		int	n = epoll_wait(_epfd, _events, MAX_EVENTS, -1);
		std::cout << "=== WAITING FOR CONNECTION ===\n";
		if (n < 0) {
			if (errno == EINTR)
				continue;
			throw std::runtime_error("Failed on epoll_wait()...\n");
		}

		for (int i = 0; i < n; i++) {
			struct epoll_event	*e = &_events[i];

			if (e->data.ptr == NULL) {
			// event on listening socket, accept loop (ET)
				char buffer[BUFFER_SIZE];
				// while (1) {
                    int addrlen = sizeof(_address);
					int newSocket = accept(_socket, (struct sockaddr*)&_address, (socklen_t*)&addrlen);
					if (newSocket < 0)
                    break;
					ssize_t bytesRead = read(newSocket, buffer, BUFFER_SIZE - 1);
                    
					if (bytesRead >= 0) {
					
                        buffer[bytesRead] = '\0';
						std::cout << buffer;
					} else
                        break;
					
                    std::string html = read_file("test.html");
                    std::ostringstream response;
                    response << "HTTP/1.1 200 OK\r\n"
                        << "Content-Type: text/html; charset=UTF-8\r\n"
                        << "Content-Length: " << html.size() << "\r\n\r\n"
                        << html;
                    std::string responseText = response.str();
					write(newSocket, responseText.c_str(), responseText.size());
					close(newSocket);
				// }
                    // ssize_t bytesRead1 = read(newSocket, buffer, BUFFER_SIZE - 1);
                    // int i = 5, j = 8;
                    // int start = i - 1;
                    // int len = j - i + 1;
					// if (bytesRead >= start + len) {
					// 	std::string nameUser(buffer + start, len);
					// 	std::cout << "Parsed name: " << nameUser << std::endl;
					}
		}
	}
}
