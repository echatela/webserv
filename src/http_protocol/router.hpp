#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <iostream>

#include "http_response.hpp"
#include "http_request.hpp"
#include "../config/config.hpp"
#include "route_result.hpp"
#include "route_resolve.hpp"
#include "../webserv.hpp"
#include "static_handler.hpp"


struct RouteInfo {
	LocationConfig	location;
	
	std::string 	uri;
	std::string		file_path;
	std::string		filename;

	bool		exists;
	bool		is_directory;
	bool		is_cgi;

	int 		status_code;
};

class Router {

public:
	Router(Config config);
	Router();
	~Router();


	// bool	IsCgi(HttpRequest & req);

	// void	FillRouteInfo(HttpRequest & req);

	CgiPlan		MakeCgiPlan(HttpRequest & req);

	RouteResult 	ProcessRequest(HttpRequest & req);
	
	HttpResponse	CgiResponse(const std::string output);
	HttpResponse 	StaticResponse(HttpRequest & req, RouteInfo & info);
	static HttpResponse 	ErrorResponse(int status_code);

	void			set_config(Config & config);

private:
	Config	config_;
	// RouteInfo		route_info_;
};


#endif