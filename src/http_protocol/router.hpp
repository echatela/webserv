#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "http_response.hpp"
#include "http_request.hpp"
#include "../config/config.hpp"
#include "route_result.hpp"


struct RouteInfo {

	Config			config; 

	LocationConfig	location;
	bool 		loc_found;
	
	std::string 	uri;
	std::string	file_path;
	std::string	filename;
	std::string 	root;

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

	CgiPlan			MakeCgiPlan(HttpRequest & req);

	RouteResult 		ProcessRequest(HttpRequest & req);
	
	HttpResponse		CgiResponse(const std::string output);
	HttpResponse 		StaticResponse(HttpRequest & req, RouteInfo & info);

	static HttpResponse	RedirectResponse(int code, std::string target);
	HttpResponse		ErrorResponse(int status_code);
	static HttpResponse	ErrorResponse(int status_code, const Config & config);

	HttpResponse 		GetErrorPage(int status_code);
	static HttpResponse	StaticError(int status_code);
	static HttpResponse	ErrorPage(std::string path);

	void			set_config(const Config & config);
	Config 			config() const;

private:
	Config	config_;
	std::map<int, std::string> error_pages_;
	// RouteInfo		route_info_;
};


#endif
