#ifndef STATIC_HANDLER_HPP
#define STATIC_HANDLER_HPP

#include "http_request.hpp"
#include "http_response.hpp"
#include "router.hpp"

struct FormPart {

	std::map<std::string, std::string>	headers;
	std::string 				body;

};

struct FormData {

	std::vector<FormPart>		form_parts;
	std::string 			boundary;
	std::string 			end_bound;
	std::string 			start_bound;
	std::vector<std::string>	tokens;

};

struct RouteInfo;

class 	StaticHandler {

private:

public:
	StaticHandler();
	~StaticHandler();

	static HttpResponse	BuildStatic(
		const HttpRequest & req, RouteInfo & info);

	static HttpResponse	BuildGet(
		const HttpRequest & req, RouteInfo & info);
	static HttpResponse	BuildDelete(
		const HttpRequest & req, RouteInfo & info);
	static HttpResponse	BuildPost(
		const HttpRequest & req, RouteInfo & info);

	static HttpResponse	BuildAutoindex(RouteInfo & info);
};



#endif
