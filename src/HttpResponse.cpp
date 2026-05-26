#include "HttpResponse.hpp"

HttpResponse::HttpResponse()
{
}

HttpResponse::HttpResponse(HttpResponse const & src)
{
	*this = src;
}

HttpResponse::~HttpResponse()
{
}

HttpResponse &	HttpResponse::operator=(HttpResponse const & rhs)
{
	if (this != &rhs)
	{
		// Copy attributes here
	}
	return (*this);
}
