#include "http_request.hpp"

int main()
{
	httpParser parser;

	std::string request = "POST /upload HTTP/1.1\r\nHost: localhost\nContent-Length: 11\r\n\r\nHel";
	std::string request2 = "lo Wo";
	std::string request3 = "rld";

	parser.add(request, request.size());
	parser.add(request2, request2.size());
	parser.add(request3, request2.size());

	std::cout << "BUF => " << std::endl
		<< "- - - - - - - - - - - -" << std::endl
		<< parser.get_buf() << std::endl << std::endl;

	std::cout << "_________________________________________" << std::endl;
	httpRequest req;
	std::cout << "CODE PARSING => " << std::endl
		<< "- - - - - - - - - - - -" << std::endl
		<< parser.ParseRequest(req) << std::endl << std::endl;

	std::cout << "_________________________________________" << std::endl;
	std::cout << "RESULT" << std::endl
		<< "- - - - - - - - - - - -" << std::endl;
	std::cout << "METHOD => " << std::endl << req.get_method() << std::endl << std::endl;
	std::cout << "PATH => " << std::endl << req.get_path() << std::endl << std::endl;
	std::cout << "VERSION => " << std::endl << req.get_version() << std::endl << std::endl;
	std::map<std::string, std::string> header = req.get_header();
	std::cout << "HEADER => " << std::endl;
	for (std::map<std::string, std::string>::iterator it = header.begin(); it != header.end(); it++)
		std::cout << it->first << ": " << it->second << std::endl;
	std::cout << std::endl << "BODY => " << std::endl;
	std::cout << req.get_body() << std::endl;	

	return 0;
}