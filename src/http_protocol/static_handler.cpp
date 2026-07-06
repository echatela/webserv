#include "static_handler.hpp"

#include "http_response.hpp"
#include "webserv.hpp"

#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>

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

	// std::cout << req.get_header("CONNECTION") << req.get_header("USer-agent") << std::endl;
	HttpResponse	response;
	std::string 	body;
	(void)req;

	if (info.is_directory == true) {
		if (!info.location.index.empty()) {
			size_t	i;
			for (i = 0; i < info.location.index.size(); ++i) {
				std::string path(info.file_path);
				path.append(info.location.index[i]);
				if (webserv::utils::StatCheck(path)) {
					info.file_path = path;
					break;
				}
			}
			if (i == info.location.index.size())
				return Router::ErrorResponse(kNotFound);
		} else {
			info.file_path.append("index.html");
		}
	}
	FillBody(body, info);
	if (info.status_code != kOk)
		return Router::ErrorResponse(info.status_code);
	AddContentLength(response, body);

	response.set_body(body);
	response.set_status(kOk);
	response.set_reason_phrase();
	response.set_version("HTTP/1.1");

	AddContentType(response, info);
	return response;
}

std::string		Filename(std::string path) {

	std::string	filename;
	size_t 		slash_pos = path.find_last_of('/');

	return (path.substr(slash_pos, path.length()));

}

std::string		FileDeletedBody(RouteInfo info) {
	std::string body;

	body = "<html lang=\"en-US\">\r\n<body>\r\n<h1>File ";
	body += "\"" + Filename(info.file_path) + "\"";
	body += " deleted.</h1>\r\n";
	body += "</body>\r\n</html>\r\n";
	return body;
}

HttpResponse 	StaticHandler::BuildDelete(HttpRequest & req, RouteInfo & info) {
	
	HttpResponse	response;

	info.status_code = std::remove(info.file_path.c_str());
	if (info.status_code != 0)
		return Router::ErrorResponse(kMethodNotAllowed);

	response.set_status(kOk);
	response.set_reason_phrase();
	response.set_version("HTTP/1.1");
	AddContentType(response, info);
	if (info.status_code != 0)
		return Router::ErrorResponse(info.status_code);

	response.set_body(FileDeletedBody(info));
	AddContentLength(response, response.get_body());

	(void)req;
	return response;
}

void TokenizeForm(const std::string& body, FormData& data)
{
    size_t start = 0;
    size_t pos;

    while ((pos = body.find("\r\n", start)) != std::string::npos)
    {
        std::string token = body.substr(start, pos - start);
        data.tokens.push_back(token);

        start = pos + 2;
    }

    if (start < body.size())
    {
        std::string token = body.substr(start);
        data.tokens.push_back(token);
    }

}

void 		InsertHeader(std::string token, FormPart & form_part) {

	size_t sep = token.find(':');

	if (sep != std::string::npos)
	{
		std::string key = token.substr(0, sep);
		std::string value = token.substr(sep + 2);
		std::pair<std::string, std::string> header(key, value);
		form_part.headers.insert(header);
	}
}

FormData	ParseMultipart(std::string body, std::string content_type, RouteInfo & info) {
	FormData	data;
	size_t 		sep = content_type.find("boundary=");

	if ( sep != std::string::npos)
	{
		std::string raw = content_type.substr(sep + strlen("boundary="));
		
		if (raw[raw.size() - 1] == 13)
			raw = raw.substr(0, raw.size() - 1);

		if (!raw.empty() && raw[0] == '"')
			raw = raw.substr(1, raw.size() - 2);

		data.start_bound = "--" + raw;
		data.end_bound = "--" + raw + "--";
	}
	else
		info.status_code = kBadRequest;

	TokenizeForm(body, data);

	size_t i = 0;

	while (i < data.tokens.size() && data.tokens[i] != data.boundary + "--")
	{
		if (data.tokens[i] == data.start_bound)
		{
			i++;
		}
		FormPart part;

		while (i < data.tokens.size() && data.tokens[i] != "")
			InsertHeader(data.tokens[i++], part);
		if (i < data.tokens.size() && data.tokens[i] == "")
			i++;
		while (i < data.tokens.size())
		{
			if (data.tokens[i] == data.end_bound)
				break;
			part.body += data.tokens[i];
			i++;	
		}
		data.form_parts.push_back(part);
		i++;
	}

	return data; 
}

std::string 	GetFilename(FormPart part) {

	std::map<std::string, std::string> headers = part.headers;
	
	std::string content_disp;
	try {
		content_disp = headers["Content-Disposition"];
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << std::endl;
		return "none";
	}
	std::string filename;
	size_t 		filename_sep = content_disp.find("filename=");
	
	if (filename_sep == std::string::npos)
		return "none";

	size_t start = filename_sep + strlen("filename=");
	if (start < content_disp.size() && content_disp[start] == '"')
		start++;

	size_t end = content_disp.find('"', start);	
	if (end == std::string::npos)
		end = content_disp.find(';', start);
	if (end == std::string::npos)
		end = content_disp.length();
	
	return content_disp.substr(start, end - start);
}

void			HandleMultipart(FormData data, RouteInfo & info) {

	for (size_t i = 0; i < data.form_parts.size(); i++)
	{
		std::ofstream	new_file;
		std::string filename = GetFilename(data.form_parts[i]);

		if (filename != "none")
		{
			filename = info.file_path + "/" + filename;
			new_file.open(filename.c_str());
			new_file << data.form_parts[i].body;
		}
		else
			info.status_code = kBadRequest;
	}
}

std::string		MultiPartSuccessfullBody() {
	std::string body;

	body = "<html lang=\"en-US\">\r\n<body>\r\n<h1>File uploaded</h1>\r\n";
	body += "</body>\r\n</html>\r\n";
	return body;
}

HttpResponse 	StaticHandler::BuildPost(HttpRequest & req, RouteInfo & info) {

	std::map<std::string, std::string>	req_headers = req.get_headers();
	std::string 						content_type;

	try {
		content_type = req_headers.at("Content-Type");
	}
	catch (std::exception& e) {
		return Router::ErrorResponse(kBadRequest);
	}
	if (content_type.compare(0, 19,"multipart/form-data") == 0)
	{
		FormData data = ParseMultipart(req.get_body(), content_type, info);

		HandleMultipart(data, info);
		if (info.status_code != kOk)
			return Router::ErrorResponse(info.status_code); 
		HttpResponse response;

		response.set_status(info.status_code);
		response.set_reason_phrase();
		response.set_version("HTTP/1.1");
		response.set_header("Content-Type", "text/html");
		response.set_body(MultiPartSuccessfullBody());		
		AddContentLength(response, response.get_body());

		return response;
	}
	return Router::ErrorResponse(kMethodNotAllowed);
}


// 	// multipart/form-data -> creation de fichier a partir d'une structure .cf TODO_perso
// 	// application/x-www-form-urlencoded -> cas de formulaire (connexion username + mdp par exemple)
// 	// application/json
// 	// application/x-www-form-urlencoded + location cgi-bin par exemple = cgi 

// // same workflow pour la determination du chemin

// // il faut un body 
// // sa taille doit correspondre a content length
