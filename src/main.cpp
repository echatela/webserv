#include "Reactor.hpp"

int	main(int argc, char **argv)
{
	(void)argc, (void)argv;
	// ConfigParser	parser(argv);
	Config	config;
	// config = parser.getConfig();
	Reactor	reactor(config);

	reactor.run();
}
