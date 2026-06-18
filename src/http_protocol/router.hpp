#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <sys/stat.h>
#include <stdlib.h>
#include <limits.h>

#include "http_response.hpp"
#include "http_request.hpp"
#include "../config/config.hpp"
#include "route_result.hpp"

class HttpRequest;

class Router {

public:
	Router(Config config);
	Router();
	~Router();

	RouteResult	HandleRequest(HttpRequest & req);
	void		set_config(const Config & config);

private:
	Config	config_;

	void	AddContentType(HttpResponse & current, std::string filepath);
	void	AddContentLength(HttpResponse & current, std::string filepath);


	RouteResult	HandleGet(HttpRequest & req);
	std::string	FillBody(std::string path, int *status_code);

	HttpResponse	HandleDelete(HttpRequest & req);

	HttpResponse	HandlePost(HttpRequest & req);

	HttpResponse	BuildErrorResponse(int status_code);

	std::string	set_path(HttpRequest & req, int *status_code);

	// 
};


#endif

// TODO renforcer les verifications pour la requete GET et cleaner tout le bazar
	// voir pour l'imbrication des classes qui peut etre amelioree
	// letsgo pour la suite ! 
