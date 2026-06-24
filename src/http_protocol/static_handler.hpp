#ifndef STATIC_HANDLER_HPP
#define STATIC_HANDLER_HPP

#include "http_request.hpp"
#include "http_response.hpp"
#include "router.hpp"

struct RouteInfo;

class 	StaticHandler {

private:

public:

	StaticHandler();
	~StaticHandler();

	static HttpResponse	BuildStatic(HttpRequest & req, RouteInfo & info);

	static HttpResponse	BuildGet(HttpRequest & req, RouteInfo & info);
	static HttpResponse	BuildDelete(HttpRequest & req, RouteInfo & info);
	static HttpResponse	BuildPost(HttpRequest & req, RouteInfo & info);


};



#endif