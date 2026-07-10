#ifndef ROUTE_RESOLVE_HPP
#define ROUTE_RESOLVE_HPP

#include <sys/stat.h>

#include "router.hpp"
#include "http_request.hpp"
#include "../config/config.hpp"

struct RouteInfo;

class RouteResolve {

private:

public:

	RouteResolve();
	~RouteResolve();
	
	static RouteInfo 	ResolveRoute(HttpRequest & req,
		Config & config);
	static void		FindLocation(std::string req_path,
		Config & config, LocationConfig & location, bool & found);
	static std::string 	BuildFilesystemPath(std::string uri,
		Config & config, LocationConfig location, bool found);
	static bool		NormalizeUri(const std::string& uri,
		std::string& out);
};

#endif
