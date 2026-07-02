#include "reactor/reactor.hpp"
#include <exception>
#include "config/config_parser.hpp"
#include "config/config_parser.hpp"
#include <iostream>


int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Please enter configuration file.\n";
		return 1;	
	}
	try
	{
		ConfigLexer 		Lexer(argv[1]);
		ConfigParser 		config_parser(Lexer.Tokenize());
		std::vector<Config>	configs = config_parser.Parse(); // -> vecteur direct 
		// std::cout << "config " << config.get_servers_info()[0].listen_info.port << std::endl;
		Reactor	reactor(configs);// -> envoyer vecteur 
		reactor.Run();
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
