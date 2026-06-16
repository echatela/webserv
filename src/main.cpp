#include "reactor/reactor.hpp"
#include <exception>
#include "config/config_parser.hpp"
#include "http_protocol/http_request.hpp"
#include "http_protocol/http_response.hpp"
#include "http_protocol/router.hpp"
#include "config/config_parser.hpp"
#include <iostream>


int	main(int argc, char **argv)
{
	if (argc != 2)
		return 1;	
	try
	{
		ConfigLexer Lexer(argv[1]);
		ConfigParser config_parser(Lexer.Tokenize());
		Config	config(config_parser); // -> vecteur direct 
		std::cout << "config " << config.get_servers_info()[0].listen_info.port << std::endl;
		Reactor	reactor(config);// -> envoyer vecteur 
		reactor.Run();
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
