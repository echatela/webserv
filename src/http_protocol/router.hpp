#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <sys/stat.h>
#include <stdlib.h>
#include <limits.h>
#include <map>

#include "http_response.hpp"
#include "http_request.hpp"
#include "../config/config.hpp"

class HttpRequest;

class Router {

public:
	Router(Config config);
	Router();
	~Router();

	HttpResponse	HandleRequest(HttpRequest & req);
	void		set_config(const Config & config);

private:
	Config	config_;

	void	AddContentType(HttpResponse & current, std::string filepath);
	void	AddContentLength(HttpResponse & current, std::string filepath);


	HttpResponse	HandleGet(HttpRequest & req);
	void			FillBody(std::string& body, std::string path, int *status_code);

	HttpResponse	HandleDelete(HttpRequest & req);
	std::string		MakeFileDeletedBody(std::string path);

	HttpResponse	HandlePost(HttpRequest & req);

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
