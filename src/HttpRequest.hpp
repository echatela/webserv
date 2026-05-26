#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

class HttpRequest
{
private:

public:
	HttpRequest();
	HttpRequest(HttpRequest const & src);
	~HttpRequest();

	HttpRequest &	operator=(HttpRequest const & rhs);
};

#endif
