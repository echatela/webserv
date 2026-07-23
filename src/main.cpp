#include "reactor/reactor.hpp"
#include <exception>
#include "config/config_parser.hpp"
#include "config/config_parser.hpp"
#include <iostream>


int	main(int argc, char **argv)
{
	std::string	conf_path;

	if (argc == 1) {
		conf_path = "conf/default.conf";
	} else if (argc == 2) {
		conf_path = argv[1];
	} else {
		std::cout << "Usage: ./webserv [configuration.conf]\n";
		return 1;
	}
	try
	{
		ConfigLexer 		Lexer(conf_path);
		ConfigParser 		config_parser(Lexer.Tokenize());
		std::vector<Config>	configs = config_parser.Parse();
		Reactor	reactor(configs);
		reactor.Run();
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
