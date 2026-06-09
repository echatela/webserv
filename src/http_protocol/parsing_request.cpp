#include "http_parser.hpp"
#include "http_request.hpp"
#include <stdexcept>

void findMethod(httpParser& httpParser, httpRequest request)
{
	size_t methodEnd = httpParser.getBuf().find(' ');
	if (methodEnd == std::string::npos)
		throw ("Invalid HTTP request: Method not found");
	request.setMethod(httpParser.getBuf().substr(0, methodEnd));
}

void parseRequest(httpParser& httpParser)
{
	httpRequest request;

	try
	{
		size_t pos = httpParser.getBuf().find("\r\n");
		std::string requestLine = httpParser.getBuf().substr(0, pos);
		findMethod(httpParser, requestLine);
		findPath(httpParser, request);
		findVersion(httpParser, request);
		findHeader(httpParser, request);
		findBody(httpParser, request);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error parsing request: " << e.what() << std::endl;
	}
}