#include "router.hpp"

#include "webserv.hpp"
#include "route_resolve.hpp"
#include "static_handler.hpp"

#include <iostream>

Router::Router(Config config) : config_(config) {}

Router::Router() {}

Router::~Router() {}

void			Router::set_config(Config & config) { config_ = config; }

// si pas d'erreur au moment de la resolution de route,
// envoie la requete soit dans le process cgi soit dans le process classique/statique
RouteResult		Router::ProcessRequest(HttpRequest & req)
{
	RouteInfo info = RouteResolve::ResolveRoute(req, config_);

	std::cout << "==================================" << info.status_code << std::endl;
	if (info.status_code != 200)
		return RouteResult::Response(ErrorResponse(info.status_code));
	if (info.is_cgi == true)
		return RouteResult::Cgi(MakeCgiPlan(req));
	return RouteResult::Response(StaticResponse(req, info));

}

// construction objet http_reponse cgi
HttpResponse 	Router::CgiResponse(const std::string output)
{
	HttpResponse	response;
	std::string	cgi_result = output;

	response.set_version("HTTP/1.1");
	response.set_status(kOk);
	response.set_reason_phrase();

	size_t headers_end = cgi_result.find("\n\n");
	size_t	line_end = cgi_result.find("\n");
	while (line_end != std::string::npos)
	{
		size_t sep = cgi_result.find(':');
		if (sep != std::string::npos)
		{

			response.set_header(cgi_result.substr(0, sep), cgi_result.substr(sep + 2, line_end - 1));
			// response.set_header(cgi_result.substr(0, sep), "text/html");
			cgi_result.erase(0, line_end);
		}
		if (line_end == headers_end)
			break;
		line_end = cgi_result.find("\n");
	}

	int i = 0;
	while (cgi_result[i] == '\n')
		i++;
	cgi_result.substr(i, cgi_result.length());
	response.set_body(cgi_result);
	response.set_header("Content-Length",
		webserv::utils::IntToStr(response.body().size()));
	return response;
}

// construction objet http_reponse statique
HttpResponse 	Router::StaticResponse(HttpRequest& req, RouteInfo & info) {
	return StaticHandler::BuildStatic(req, info);
}

// construction objet http_reponse erreur
HttpResponse 	Router::ErrorResponse(int status_code) {
	
	HttpResponse	resp;
	std::string	body;

	resp.set_status(status_code);
	resp.set_reason_phrase();
	resp.set_version("HTTP/1.1");
	body.append("<html>\r\n<body>\r\n");
	body.append("<h1>");
	body.append(webserv::utils::IntToStr(status_code) + " " + resp.reason_phrase(status_code));
	body.append("</h1>\r\n");
	body.append("</html>\r\n</body>");
	resp.set_body(body);
	resp.set_header("Content-Type", "text/html");
	resp.set_header("Content-Length", webserv::utils::IntToStr(body.size()));
	return (resp);

}

CgiPlan		Router::MakeCgiPlan(HttpRequest & req) {

	CgiPlan cgi;
	std::string path = req.path();
	size_t sep;

	cgi.interpreter = config_.locations().at("/cgi").cgi[1];
	sep = path.find(".py");
	if (sep != std::string::npos)
	{
		cgi.script_name = path.substr(0, sep + 3);
		std::string full_path = cgi.script_name.replace(0, 4, config_.root() + "/cgi-bin");
		char	resolved_path[PATH_MAX];
		char* res = realpath(full_path.c_str(), resolved_path);
		if (res != NULL)
			cgi.script_path = resolved_path;
		path = path.substr(sep + 3, path.length());
	}
	sep = path.find('?');
	if (sep != std::string::npos)
	{
		cgi.path_info = path.substr(0, sep);
		cgi.query_string = path.substr(sep + 1, path.length());
	}

	return cgi;
}
