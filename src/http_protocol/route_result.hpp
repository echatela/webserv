#ifndef ROUTERESULT_HPP
#define ROUTERESULT_HPP

#include "http_response.hpp"

struct CgiPlan {
	std::string	script_path;   // chemin filesystem absolu du script
	std::string	interpreter;   // ex. /usr/bin/python3  (depuis la config)
	std::string	script_name;   // partie URL = le script   (pour PATH_INFO)
	std::string	path_info;     // le reste de l'URL après le script
	std::string	query_string;  // après le '?'
};

enum {
	kRouteResponse,
	kRouteCgi
};

class RouteResult
{
private:
	RouteResult(const CgiPlan & plan);
	RouteResult(const HttpResponse & response);

	int		type_;
	const CgiPlan		plan_;
	HttpResponse	response_;

public:
	RouteResult(RouteResult const & src);
	~RouteResult();

	static RouteResult	Cgi(const CgiPlan & plan);
	static RouteResult	Response(const HttpResponse & response);

	int		get_type() const;
	HttpResponse	get_response() const;
	CgiPlan		get_plan() const;

	RouteResult &	operator=(RouteResult const & rhs);
};

#endif
