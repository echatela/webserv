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
	std::string 			start_bound;
};

struct RouteInfo;

namespace static_handler {

HttpResponse	BuildStatic(const HttpRequest & req, RouteInfo & info);

HttpResponse	BuildGet(const HttpRequest & req, RouteInfo & info);
HttpResponse	BuildDelete(const HttpRequest & req, RouteInfo & info);
HttpResponse	BuildPost(const HttpRequest & req, RouteInfo & info);

HttpResponse	BuildAutoindex(RouteInfo & info);

};



#endif
