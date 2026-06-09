#include "http_parser.hpp"

int main()
{
	httpParser parser;

	parser.add("GET / HTTP/1.1\r\nHost: example.com\r\n\r\n", 39);

	std::cout << "Buffer: " << parser.getBuf() << std::endl;
	std::cout << "Flag: " << parser.getFlag() << std::endl;

	return 0;
}