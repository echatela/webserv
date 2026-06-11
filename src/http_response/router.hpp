#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <sys/stat.h>
#include <fstream>

#include "../event_handler.hpp"
#include "http_response.hpp"
#include "http_request.hpp"
#include "config/config.hpp"

class Router {

public:
	Router();
	~Router();

	HttpResponse	HandleRequest(HttpRequest & req);

private:
	ServerConfig	config_;

	void				AddContentType(HttpResponse & current, std::string filepath);
	void				AddContentLength(HttpResponse & current, std::string filepath);


	HttpResponse		HandleGet(HttpRequest & req);
	HttpResponse		HandleDelete(HttpRequest & req);
	HttpResponse		HandlePost(HttpRequest & req);
	HttpResponse		BuildErrorResponse(int status_code);
	
};


#endif
