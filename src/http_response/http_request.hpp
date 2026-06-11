#ifndef HTTP_REQUEST
#define HTTP_REQUEST

#include <iostream>

class HttpRequest {

public:
	std::string	get_method() const;
	std::string	get_path() const;
	std::string	get_header() const;
	std::string	get_body() const;

private:
	std::string method_;
	std::string path_;
	std::string header_;
	std::string body_;

};

#endif
