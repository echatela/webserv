#ifndef ROUTERESULT_HPP
#define ROUTERESULT_HPP

#include "http_response.hpp"

struct CgiPlan
{

};

enum {
	kResponse,
	kCgi
};

class RouteResult
{
private:
	RouteResult(const CgiPlan & plan);
	RouteResult(const HttpResponse & response);

	int			type_;
	const CgiPlan		plan_;
	const HttpResponse	response_;

public:
	RouteResult(RouteResult const & src);
	~RouteResult();

	static RouteResult	Cgi(const CgiPlan & plan);
	static RouteResult	Response(const HttpResponse & response);

	RouteResult &	operator=(RouteResult const & rhs);
};

#endif
