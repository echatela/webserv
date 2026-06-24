// #include "http_request.hpp"
// #include "http_response.hpp"
// #include "router.hpp"
// #include "../config/config_parser.hpp"

// int main()
// {
// 	HttpParser parser;
// 	const char* request = "GET ../prout/test.css HTTP/1.1\r\nHost: localhost\nUser-Agent: Mo";
// 	const char* request2 = "zilla/5.0 0x00 0xFF 0xA3 ";
	
// 	parser.Add(request, strlen(request));
// 	parser.Add(request2, strlen(request2));
	
// 	std::cout << "BUF => " << std::endl
// 	<< "- - - - - - - - - - - -" << std::endl
// 	<< parser.get_buf() << std::endl << std::endl;
	
// 	std::cout << "_________________________________________" << std::endl;
// 	HttpRequest req;
// 	std::cout << "CODE PARSING => " << std::endl
// 	<< "- - - - - - - - - - - -" << std::endl
// 	<< parser.ParseRequest(req) << std::endl << std::endl;
// }
	
// 	std::cout << "_________________________________________" << std::endl;
// 	std::cout << "RESULT" << std::endl
// 	<< "- - - - - - - - - - - -" << std::endl;
// 	std::cout << "METHOD => " << std::endl << req.getMethod() << std::endl << std::endl;
// 	std::cout << "PATH => " << std::endl << req.getPath() << std::endl << std::endl;
// 	std::cout << "VERSION => " << std::endl << req.getVersion() << std::endl << std::endl;
// 	std::map<std::string, std::string> header = req.getHeader();
// 	std::cout << "HEADER => " << std::endl;
// 	for (std::map<std::string, std::string>::iterator it = header.begin(); it != header.end(); it++)
// 	std::cout << it->first << ": " << it->second << std::endl;
// 	std::cout << std::endl << "BODY => " << std::endl;
// 	std::cout << req.getBody() << std::endl;	
	
// 	Router	router(serv);
// 	HttpResponse response = router.HandleRequest(req);
// 	std::cout << "response result: \n\n" << response.ToString() << std::endl;
// 	}
// 	catch (std::exception & e)
// 	{
// 		std::cout << e.what() << std::endl;
// 	}

// 	return 0;
// }
