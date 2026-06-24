#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <sys/stat.h>
#include <stdlib.h>
#include <limits.h>
#include <map>
#include <iostream>

#include "http_response.hpp"
#include "http_request.hpp"
#include "../config/config.hpp"
#include "route_result.hpp"
#include "route_result.hpp"

struct HeaderValue {
	std::string							directive;
	std::map <std::string, std::string> parameters;
};

struct FormParts {
	
	std::map< std::string, HeaderValue > 	headers;
	std::string								body;
};


class RouteResult;

class HttpRequest;

class Router {

public:
	Router(Config config);
	Router();
	~Router();

	RouteResult	HandleRequest(HttpRequest & req);
	HttpResponse	CgiResponse(const std::string output);
	void		set_config(const Config & config);

private:
	Config	config_;

	bool	IsCgi(HttpRequest& req);
	CgiPlan	MakeCgiPlan(HttpRequest& req);

	void	AddContentType(HttpResponse & current, std::string filepath, int *status_code);
	void	AddContentLength(HttpResponse & current, std::string filepath);


	RouteResult	HandleGet(HttpRequest & req);
	void			FillBody(std::string& body, std::string path, int *status_code);

	RouteResult	HandleDelete(HttpRequest & req);
	std::string		MakeFileDeletedBody(std::string path);

	RouteResult	HandlePost(HttpRequest & req);
	void			ParsePostBody(HttpRequest & req);


	HttpResponse	BuildErrorResponse(int status_code);

	void			FindLocationRoot(std::string & root, HttpRequest & req);
	std::string		AddRoot(HttpRequest & req, int *status_code);
	void			CheckAndSetPath(std::string& path, HttpRequest & req, int *status_code);
	bool			IsPathResolved(std::string& path, int *status_code);
	std::string		Filename(std::string path);
	// 
};


#endif

// TODO renforcer les verifications pour la requete GET et cleaner tout le bazar
	// voir pour l'imbrication des classes qui peut etre amelioree
	// letsgo pour la suite ! 
