#include "router.hpp"

#include "http_parser.hpp"
#include "http_response.hpp"
#include "route_result.hpp"
#include "webserv.hpp"
#include "route_resolve.hpp"
#include "static_handler.hpp"

#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

Router::Router(Config config) : config_(config) {}

Router::Router() {}

Router::~Router() {}

void			Router::set_config(Config & config) { config_ = config; }

static std::string	JoinMethods(const std::vector<std::string>& methods) {

	std::string	out;

	for (size_t i = 0; i < methods.size(); i++) {
		out += methods[i];
		if (i + 1 < methods.size())
			out += ", ";
	}
	return out;
}

// si pas d'erreur au moment de la resolution de route,
// envoie la requete soit dans le process cgi soit dans le process classique/statique
RouteResult	Router::ProcessRequest(HttpRequest & req) {

	if (req.error() != NO_ERROR)
		return RouteResult::Response(ErrorResponse(req.error()));

	RouteInfo info = RouteResolve::ResolveRoute(req, config_);

	if (!info.location.redirect.empty())
		return RouteResult::Response(RedirectResponse(
			webserv::utils::ParseUInt(info.location.redirect[0]),
			info.location.redirect[1]));

	if (info.status_code == kMethodNotAllowed) {
		HttpResponse	resp = ErrorResponse(kMethodNotAllowed);
		resp.set_header("Allow", JoinMethods(info.location.methods));
		return RouteResult::Response(resp);
	}
	if (info.status_code != kOk)
		return RouteResult::Response(ErrorResponse(info.status_code));
	if (req.method() == "POST" && info.location.upload_enabled != true)
		return RouteResult::Response(ErrorResponse(kForbidden));
	if (info.is_cgi == true)
		return RouteResult::Cgi(MakeCgiPlan(req));
	return RouteResult::Response(StaticResponse(req, info));

}

// construction objet http_reponse cgi
HttpResponse 	Router::CgiResponse(const std::string output) {

	HttpResponse	response;
	size_t		headers_end = output.find("\r\n\r\n");
	size_t		sep_len = 4;

	if (headers_end == std::string::npos) {
		headers_end = output.find("\n\n");
		sep_len = 2;
	}
	if (headers_end == std::string::npos)
		return ErrorResponse(kBadGateway);

	std::string		head = output.substr(0, headers_end);
	std::string		body = output.substr(headers_end + sep_len);
	std::istringstream	iss(head);
	std::string		line;
	bool			has_type = false;

	response.set_version("HTTP/1.1");
	response.set_status(kOk);

	while (std::getline(iss, line)) {
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);

		size_t	colon = line.find(':');
		if (colon == std::string::npos)
			continue;

		std::string	key = line.substr(0, colon);
		std::string	value = line.substr(colon + 1);
		size_t		b = value.find_first_not_of(" \t");
		value = (b == std::string::npos) ? "" : value.substr(b);

		if (key == "Status") {
			response.set_status(webserv::utils::ParseUInt(
				value.substr(0, value.find(' '))));
			continue;
		}
		if (key == "Content-Length")
			continue;
		if (key == "Content-Type")
			has_type = true;
		response.set_header(key, value);
	}

	if (!has_type)
		response.set_header("Content-Type", "text/html");
	response.set_body(body);
	response.set_header("Content-Length", 
		webserv::utils::IntToStr(response.body().size()));
	response.set_reason_phrase();
	return response;
}

// construction objet http_reponse statique
HttpResponse 	Router::StaticResponse(HttpRequest& req, RouteInfo & info) {
	return static_handler::BuildStatic(req, info);
}

HttpResponse  Router::RedirectResponse(int code, std::string target)
{
      HttpResponse    resp;
      std::string     body;

      resp.set_status(code);
      resp.set_reason_phrase();
      resp.set_version("HTTP/1.1");
      resp.set_header("Location", target);
      body.append("<html>\r\n<body>\r\n<h1>");
      body.append(webserv::utils::IntToStr(code) + " Redirect");
      body.append("</h1>\r\n<a href=\"" + target + "\">" + target + "</a>\r\n");
      body.append("</body>\r\n</html>\r\n");
      resp.set_body(body);
      resp.set_header("Content-Type", "text/html");
      resp.set_header("Content-Length", webserv::utils::IntToStr(body.size()));
      return resp;
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

	sep = path.find('?');
	if (sep != std::string::npos) {
		cgi.query_string = path.substr(sep + 1, path.length());
		path = path.substr(0, sep);
	}

	sep = path.find(".py");
	if (sep != std::string::npos) {
		cgi.script_name = path.substr(0, sep + 3);
		cgi.path_info = path.substr(sep + 3);

		std::string	full_path = cgi.script_name;
		full_path.replace(0, 4, config_.root() + "/cgi-bin");
		if (webserv::utils::StatCheck(full_path))
			cgi.script_path = full_path;
	}
	return cgi;
}
