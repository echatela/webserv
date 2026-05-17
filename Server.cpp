#include "Server.hpp"
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 16000

Server::Server(int domain, int port, int service, int protocol, int backlog,
	       u_long interface)
	: _domain(domain), _port(port), _service(service), _protocol(protocol),
	_backlog(backlog), _interface(interface)
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
}

Server::~Server()
{ }

void	Server::launch()
{
	char buffer[BUFFER_SIZE];
	while (1) {
		std::cout << "=== WAITING FOR CONNECTION ===\n";
		int addrlen = sizeof(_address);
		int newSocket = accept(_socket, (struct sockaddr*)&_address,
			  (socklen_t*)&addrlen);
		ssize_t bytesRead = read(newSocket, buffer, BUFFER_SIZE - 1);

		if (bytesRead >= 0) {
			buffer[bytesRead] = '\0';
			std::cout << buffer;
		} else {
			throw std::runtime_error("Error reading buffer...\n");
		}
		const char *response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html; charset=UTF-8\r\n\r\n"
			"<!DOCTYPE html>\r\n"
			"<html>\r\n"
			"<head>\r\n"
			"<title>Testing Basic HTTP-SERVER</title>\r\n"
			"</head>\r\n"
			"<body>\r\n"
			"Hello, Edouard!\r\n"
			"</body>\r\n"
			"</html>\r\n";
		write(newSocket, response, std::strlen(response));
		close(newSocket);
	}
}
