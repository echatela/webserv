#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
{
}

HttpRequest::HttpRequest(HttpRequest const & src)
{
	*this = src;
}

HttpRequest::~HttpRequest()
{
}

HttpRequest &	HttpRequest::operator=(HttpRequest const & rhs)
{
	if (this != &rhs)
	{
		// Copy attributes here
	}
	return (*this);
}
