#include "Server.hpp"

int	main(int argc, char **argv)
{
	(void)argc, (void)argv;
	// ConfigParser	parser(argv);
	Config	config;
	// config = parser.getConfig();
	Server	server(config);

	server.run();
}
