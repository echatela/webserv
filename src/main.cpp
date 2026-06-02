#include "reactor.hpp"
#include <exception>
#include <iostream>


int	main(int argc, char **argv)
{
	(void)argc, (void)argv;
	// ConfigParser	config_parser(argv);
	// config = parser.get_config();
	
	try
	{
		Config	config;
		Reactor	reactor(config);
		reactor.Run();
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
