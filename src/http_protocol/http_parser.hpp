#pragma once

#include <string>
#include <iostream>
#include "http_request.hpp"

enum ParseError
{
    NO_ERROR,
    BAD_REQUEST = 400,
    METHOD_NOT_ALLOWED = 405,
    HTTP_VERSION_NOT_SUPPORTED = 505,
	BAD_HEADER,
	OTHER_ERROR = 999,
};

class httpParser
{
	public:
		httpParser();
		~httpParser();

		int add(const std::string buf, size_t n);

		// int getFlag() const;
		// std::string getBuf() const;

		int parseRequest(httpRequest& req) const;
		
	private :
		httpParser(const httpParser& other);
		httpParser& operator=(const httpParser& other);

		std::string buf_;
		int flag_;
};