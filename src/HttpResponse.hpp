#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

class HttpResponse
{
private:

public:
	HttpResponse();
	HttpResponse(HttpResponse const & src);
	~HttpResponse();

	HttpResponse &	operator=(HttpResponse const & rhs);
};

#endif
