#pragma once

#include <string>
#include <iostream>

class httpRequest;

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

		int get_flag() const;
		std::string get_buf() const;

		int ParseRequest(httpRequest& req) const;
		
	private :
		httpParser(const httpParser& other);
		httpParser& operator=(const httpParser& other);

		std::string buf_;
		size_t buf_size_;
		size_t buf_size_without_body_;
		size_t buf_size_with_body_;
		int flag_;
		int content_length_;
};