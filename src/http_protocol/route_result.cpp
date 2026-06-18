#include "route_result.hpp"
#include "http_response.hpp"

RouteResult::RouteResult(const CgiPlan & plan)
: type_(kCgi), plan_(plan) { }

RouteResult::RouteResult(const HttpResponse & response)
: type_(kResponse), response_(response) { }

RouteResult::RouteResult(RouteResult const & src)
: type_(src.type_), plan_(src.plan_), response_(src.response_) { }

RouteResult::~RouteResult()
{ }

RouteResult &	RouteResult::operator=(RouteResult const & rhs)
{ return (*this); }
