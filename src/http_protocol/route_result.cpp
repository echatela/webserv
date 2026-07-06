#include "route_result.hpp"
#include "http_response.hpp"

RouteResult::RouteResult(const CgiPlan & plan)
: type_(kRouteCgi), plan_(plan) { }

RouteResult::RouteResult(const HttpResponse & response)
: type_(kRouteResponse), response_(response) { }

RouteResult::RouteResult(RouteResult const & src)
: type_(src.type_), plan_(src.plan_), response_(src.response_) { }

RouteResult RouteResult::Response(const HttpResponse & response) {
    return RouteResult(response);
} 

RouteResult RouteResult::Cgi(const CgiPlan & plan) {
    return RouteResult(plan);
} 

int		RouteResult::type() const { return type_; }

HttpResponse	RouteResult::response() const { return response_; }

CgiPlan		RouteResult::plan() const { return plan_; }

RouteResult::~RouteResult()
{ }

RouteResult &	RouteResult::operator=(RouteResult const & rhs)
{ 
    if (&rhs != this)
    {
        type_ = rhs.type_;
        // plan_ = rhs.plan_;
        // response = rhs.response_;
    } 
    return (*this); }
