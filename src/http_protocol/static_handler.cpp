#include "static_handler.hpp"
#include <fstream>

StaticHandler::StaticHandler() {}

StaticHandler::~StaticHandler() {}

HttpResponse 	StaticHandler::BuildStatic(HttpRequest & req, RouteInfo & info) {

	if (req.get_method() == "GET")
		return BuildGet(req, info);
	else if (req.get_method() == "DELETE")
		return BuildDelete(req, info);
	else
		return BuildPost(req, info);
	return Router::ErrorResponse(kMethodNotAllowed);	
}

void		FillBody(std::string & body, RouteInfo & info) {

	std::ifstream	file(info.file_path.c_str());
	std::string	line;

	if (!file.is_open())
		info.status_code = kForbidden;
	while (getline(file, line))
		body.append(line);
	file.close();
	if (body.size() == 0)
		info.status_code = kNoContent;

}

std::string	value_from_extension(std::string extension) {

	std::map<std::string, std::string> types;

	types[".html"] = "text/html";
	types[".css"] = "text/css";	
	types[".js"] = "application/javascript";	
	types[".png"] = "image/png";	
	types[".jpg"] = "image/jpeg";	
	types[".gif"] = "image/gif";

	return (types.at(extension));
}

void			AddContentType(HttpResponse & current, 	RouteInfo info) {

	size_t		dot = info.file_path.find_last_of('.');
	std::string	extension;
	std::string content_type;
	
	if (dot != std::string::npos)
		extension = info.file_path.substr(dot, info.file_path.size());
	try {
		current.set_header("Content-Type", value_from_extension(extension));
	}
	catch (std::exception& e)
	{
		info.status_code = kMethodNotAllowed;
	}

}

void		AddContentLength(HttpResponse & current, std::string body) {

	current.set_header("Content-Length", webserv::utils::IntToStr(body.size()));

}

HttpResponse 	StaticHandler::BuildGet(HttpRequest & req, RouteInfo & info) {

	HttpResponse	response;
	std::string 	body;
	(void)req;

	if (info.is_directory == true)
	{
		info.file_path.append("index.html");
	}
	FillBody(body, info);
	if (info.status_code != kOk)
		return 	Router::ErrorResponse(info.status_code);
	AddContentLength(response, body);

	response.set_body(body);
	response.set_status(kOk);
	response.set_reason_phrase();
	response.set_version("HTTP/1.1");

	AddContentType(response, info);
	return response;
}

HttpResponse 	StaticHandler::BuildDelete(HttpRequest & req, RouteInfo & info) {
	(void)req;
	(void)info;
	return Router::ErrorResponse(kMethodNotAllowed);
}

HttpResponse 	StaticHandler::BuildPost(HttpRequest & req, RouteInfo & info) {
	(void)req;
	(void)info;
	return Router::ErrorResponse(kMethodNotAllowed);
}