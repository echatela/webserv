#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <sys/stat.h>
#include <fstream>
#include <stdlib.h>
#include <limits.h>

#include "../event_handler.hpp"
#include "http_response.hpp"
#include "http_request.hpp"
#include "../config/config.hpp"

class HttpRequest;

class Router {

public:
	Router(ServerConfig config);
	Router();
	~Router();

	HttpResponse	HandleRequest(HttpRequest & req);
	void			set_config(const ServerConfig & config);

private:
	ServerConfig	config_;

	void				AddContentType(HttpResponse & current, std::string filepath);
	void				AddContentLength(HttpResponse & current, std::string filepath);


	HttpResponse		HandleGet(HttpRequest & req);
	std::string			FillBody(std::string path, int *status_code);
	HttpResponse		HandleDelete(HttpRequest & req);
	HttpResponse		HandlePost(HttpRequest & req);
	HttpResponse		BuildErrorResponse(int status_code);

	std::string			set_path(HttpRequest & req, int *status_code);
	
};


#endif

// TODO renforcer les verifications pour la requete GET et cleaner tout le bazar
	// voir pour l'imbrication des classes qui peut etre amelioree
	// letsgo pour la suite ! 
