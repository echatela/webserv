#include "route_result.hpp"
#include "http_response.hpp"

RouteResult::RouteResult(const CgiPlan & plan)
: type_(kRouteCgi), plan_(plan) { }

RouteResult::RouteResult(const HttpResponse & response)
: type_(kRouteResponse), response_(response) { }

RouteResult::RouteResult(RouteResult const & src)
: type_(src.type_), plan_(src.plan_), response_(src.response_) { }

int		RouteResult::get_type() const { return type_; }

HttpResponse	RouteResult::get_response() const { return response_; }

CgiPlan		RouteResult::get_plan() const { return plan_; }

RouteResult::~RouteResult()
{ }

RouteResult &	RouteResult::operator=(RouteResult const & rhs)
{ return (*this); }
