#include "http_request.hpp"
#include "http_response.hpp"
#include "router.hpp"
#include "../config/config_parser.hpp"

int main()
{
	HttpParser parser;
	try {
		ConfigLexer Lexer("Configuration.conf");
		ConfigParser config_parser(Lexer.Tokenize());
		Config	config(config_parser);
		ServerConfig serv = config.get_servers_info()[0];
		
		std::string request = "GET ../prout/test.css HTTP/1.1\r\nHost: localhost\nUser-Agent: Mo";
	std::string request2 = "zilla/5.0";
	
	parser.add(request, request.size());
	parser.add(request2, request2.size());
	
	std::cout << "BUF => " << std::endl
	<< "- - - - - - - - - - - -" << std::endl
	<< parser.getBuf() << std::endl << std::endl;
	
	std::cout << "_________________________________________" << std::endl;
	HttpRequest req;
	std::cout << "CODE PARSING => " << std::endl
	<< "- - - - - - - - - - - -" << std::endl
	<< parser.parseRequest(req) << std::endl << std::endl;
	
	std::cout << "_________________________________________" << std::endl;
	std::cout << "RESULT" << std::endl
	<< "- - - - - - - - - - - -" << std::endl;
	std::cout << "METHOD => " << std::endl << req.getMethod() << std::endl << std::endl;
	std::cout << "PATH => " << std::endl << req.getPath() << std::endl << std::endl;
	std::cout << "VERSION => " << std::endl << req.getVersion() << std::endl << std::endl;
	std::map<std::string, std::string> header = req.getHeader();
	std::cout << "HEADER => " << std::endl;
	for (std::map<std::string, std::string>::iterator it = header.begin(); it != header.end(); it++)
	std::cout << it->first << ": " << it->second << std::endl;
	std::cout << std::endl << "BODY => " << std::endl;
	std::cout << req.getBody() << std::endl;	
	
	Router	router(serv);
	HttpResponse response = router.HandleRequest(req);
	std::cout << "response result: \n\n" << response.ToString() << std::endl;
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}
