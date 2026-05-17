#include <cstdio>
#include <exception>
#include <iostream>
#include <sys/socket.h>
#include "Server.hpp"

int	main()
{
	try {
		Server	server;
		server.launch();
	} catch (std::exception& e) {
		std::cerr << e.what();
		perror("");
	}
	return 0;
}
