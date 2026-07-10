#include "static_handler.hpp"

#include "http_response.hpp"
#include "router.hpp"
#include "webserv.hpp"

#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>

HttpResponse 	static_handler::BuildStatic(
	const HttpRequest & req, RouteInfo & info) {

	if (req.method() == "GET")
		return BuildGet(req, info);
	else if (req.method() == "DELETE")
		return BuildDelete(req, info);
	else
		return BuildPost(req, info);
	return Router::ErrorResponse(kMethodNotAllowed, info.config);	
}

static void	FillBody(std::string & body, RouteInfo & info) {

	std::ifstream	file(info.file_path.c_str(),
		std::ios::in | std::ios::binary);

	if (!file.is_open()) {
		info.status_code = kForbidden;
		return;
	}

	std::ostringstream	ss;
	ss << file.rdbuf();
	body = ss.str();
	file.close();
}

static std::string	ValueFromExtension(const std::string & extension) {

	std::map<std::string, std::string> types;
	types[".html"] = "text/html";
	types[".css"] = "text/css";	
	types[".js"] = "application/javascript";	
	types[".png"] = "image/png";	
	types[".jpg"] = "image/jpeg";	
	types[".gif"] = "image/gif";

	std::map<std::string, std::string>::const_iterator it
		= types.find(extension);
	if (it == types.end())
		return "application/octet-stream";
	return it->second;
}

static void	AddContentType(HttpResponse & current, const RouteInfo &info) {

	size_t		dot = info.file_path.find_last_of('.');
	std::string	extension;
	
	if (dot != std::string::npos)
		extension = info.file_path.substr(dot, info.file_path.size());
	current.set_header("Content-Type", ValueFromExtension(extension));
}

static void	AddContentLength(HttpResponse & current, std::string body) {

	current.set_header("Content-Length", webserv::utils::IntToStr(body.size()));
}

HttpResponse	static_handler::BuildAutoindex(RouteInfo & info) {

	HttpResponse	response;

	DIR	*dir = opendir(info.file_path.c_str());
	if (dir == NULL)
		return Router::ErrorResponse(kForbidden, info.config);

	std::string uri = info.uri;
	if (uri.empty() || uri[uri.size() - 1] != '/')
		uri.append("/");

	std::string	body;
	body += "<html>\r\n<head><title>Index of " + uri + "</title></head>\r\n";
	body += "<body>\r\n<h1>Index of " + uri + "</h1>\r\n<ul>\r\n";

	struct dirent	*entry;
	while ((entry = readdir(dir)) != NULL) {
		std::string	name = entry->d_name;
		if (name == ".")
			continue;
		body += "<li><a href=\"" + uri + name + "\">" + name
			+ "</a></li>\r\n";
	}
	closedir(dir);

	body += "</ul>\r\n</body>\r\n</html>\r\n";

	response.set_status(kOk);
	response.set_reason_phrase();
	response.set_version("HTTP/1.1");
	response.set_header("Content-Type", "text/html");
	AddContentLength(response, body);
	response.set_body(body);
	return response;
}

HttpResponse 	static_handler::BuildGet(
	const HttpRequest & req, RouteInfo & info) {

	HttpResponse	response;
	std::string 	body;
	(void)req;

	if (info.is_directory == true) {
		std::vector<std::string> candidates = info.location.index;
		if (candidates.empty())
			candidates.push_back("index.html");

		bool	found = false;
		for (size_t i = 0; i < candidates.size(); ++i) {
			std::string	path(info.file_path);
			path.append(candidates[i]);
			if (webserv::utils::StatCheck(path)) {
				info.file_path = path;
				found = true;
				break;
			}
		} 

		if (!found) {
			if (info.location.autoindex)
				return BuildAutoindex(info);
			return Router::ErrorResponse(kForbidden, info.config);
		}
	}
	FillBody(body, info);
	if (info.status_code != kOk)
		return Router::ErrorResponse(info.status_code, info.config);
	AddContentLength(response, body);

	response.set_body(body);
	response.set_status(kOk);
	response.set_reason_phrase();
	response.set_version("HTTP/1.1");

	AddContentType(response, info);
	return response;
}

static std::string	Filename(const std::string & path) {

	std::string	filename;
	size_t 		slash_pos = path.find_last_of('/');

	return (path.substr(slash_pos, path.length()));

}

static std::string	FileDeletedBody(const RouteInfo & info) {

	std::string body;

	body = "<html lang=\"en-US\">\r\n<body>\r\n<h1>File ";
	body += "\"" + Filename(info.file_path) + "\"";
	body += " deleted.</h1>\r\n";
	body += "</body>\r\n</html>\r\n";
	return body;
}

HttpResponse 	static_handler::BuildDelete(
	const HttpRequest & req, RouteInfo & info) {

	HttpResponse	response;
	(void)req;

	if (info.is_directory)
		return Router::ErrorResponse(kForbidden, info.config);
	if (std::remove(info.file_path.c_str()) != 0)
		return Router::ErrorResponse(kForbidden, info.config);

	response.set_status(kOk);
	response.set_reason_phrase();
	response.set_version("HTTP/1.1");
	response.set_header("Content-Type", "text/html");
	response.set_body(FileDeletedBody(info));
	AddContentLength(response, response.body());
	return response;
}

static void	InsertHeader(std::string token, FormPart & form_part) {

	size_t sep = token.find(':');

	if (sep == std::string::npos)
		return;

	std::string	key = token.substr(0, sep);
	size_t		vstart = sep + 1;

	while (vstart < token.size()
		&& (token[vstart] == ' ' || token[vstart] == '\t'))
		vstart++;

	form_part.headers[key] = token.substr(vstart);
}

static FormData	ParseMultipart(const std::string & body,
	const std::string & content_type, RouteInfo & info) {

	FormData	data;
	size_t 		sep = content_type.find("boundary=");

	if (sep == std::string::npos) {
		info.status_code = kBadRequest;
		return data;
	}
	
	std::string	raw = content_type.substr(sep + strlen("boundary="));
	size_t		semi = raw.find(';');

	if (semi != std::string::npos)
		raw = raw.substr(0, semi);
	while (!raw.empty() && (raw[raw.size() - 1] == '\r'
		|| raw[raw.size() - 1] == '\n' || raw[raw.size() - 1] == ' '))
		raw.erase(raw.size() - 1);
	if (raw.size() >= 2 && raw[0] == '"' && raw[raw.size() - 1] == '"')
		raw = raw.substr(1, raw.size() - 2);
	if (raw.empty()) {
		info.status_code = kBadRequest;
		return data;
	}

	data.start_bound = "--" + raw;

	size_t	pos = body.find(data.start_bound);
	if (pos == std::string::npos) {
		info.status_code = kBadRequest;
		return data;
	}
	pos += data.start_bound.size();

	while (pos + 2 <= body.size())
	{
		if (body.compare(pos, 2, "--") == 0) // final delim
			return data;
		if (body.compare(pos, 2, "\r\n") != 0)
			break;
		pos += 2;

		size_t	headers_end = body.find("\r\n\r\n", pos);
		if (headers_end == std::string::npos)
			break;

		FormPart	part;
		size_t		line = pos;

		while (line < headers_end)
		{
			size_t	eol = body.find("\r\n", line);
			if (eol == std::string::npos || eol > headers_end)
				eol = headers_end;
			InsertHeader(body.substr(line, eol - line), part);
			line = eol + 2;
		}
		
		size_t	body_start = headers_end + 4;
		size_t	next = body.find("\r\n" + data.start_bound, body_start);
		if (next == std::string::npos)
			break;

		part.body = body.substr(body_start, next - body_start);
		data.form_parts.push_back(part);

		pos = next + 2 + data.start_bound.size();
	}
	info.status_code = kBadRequest;	// ended with break, wrong body
	return data;
}

static std::string 	GetFilename(const FormPart & part) {

	std::map<std::string, std::string>::const_iterator it
		= part.headers.find("Content-Disposition");

	if (it == part.headers.end())
		return "none";
	
	const std::string&	content_disp = it->second;
	size_t			filename_sep = content_disp.find("filename=");

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

static std::string	SanitizeFilename(const std::string& name)
{
	size_t		slash = name.find_last_of("/\\");
	std::string	base = (slash == std::string::npos)
		? name : name.substr(slash + 1);

	if (base.empty() || base == "." || base == "..")
		return "";
	return base;
}

static void	HandleMultipart(const FormData & data, RouteInfo & info)
{
	bool	wrote = false;

	for (size_t i = 0; i < data.form_parts.size(); i++)
	{
		std::string	filename = GetFilename(data.form_parts[i]);

		if (filename == "none")
			continue;

		filename = SanitizeFilename(filename);
		if (filename.empty()) {
			info.status_code = kBadRequest;
			return ;
		}

		std::string	dir = info.location.upload_path.empty()
			? info.file_path
			: info.location.upload_path;
		std::string	path = dir + "/" + filename;
		std::ofstream	new_file(path.c_str(),
			 std::ios::out | std::ios::binary | std::ios::trunc);
		if (!new_file.is_open()) {
			info.status_code = kForbidden;
			return;
		}
		if (!data.form_parts[i].body.empty())
			new_file.write(data.form_parts[i].body.data(),
				data.form_parts[i].body.size());
		new_file.close();
		wrote = true;
	}

	if (!wrote)
		info.status_code = kBadRequest;
}

static std::string	MultiPartSuccessfullBody()
{
	std::string body;

	body = "<html lang=\"en-US\">\r\n<body>\r\n<h1>File uploaded</h1>\r\n";
	body += "</body>\r\n</html>\r\n";
	return body;
}

HttpResponse 	static_handler::BuildPost(const HttpRequest & req, RouteInfo & info)
{
	std::map<std::string, std::string>	req_headers = req.headers();
	std::string 				content_type;

	if (info.location.upload_enabled != true)
		return Router::ErrorResponse(kForbidden, info.config);
	try {
		content_type = req_headers.at("Content-Type");
	}
	catch (std::exception& e) {
		return Router::ErrorResponse(kBadRequest, info.config);
	}
	if (content_type.compare(0, 19,"multipart/form-data") == 0)
	{
		FormData data = ParseMultipart(req.body(), content_type, info);
		if (info.status_code != kOk)
			return Router::ErrorResponse(info.status_code, info.config);

		HandleMultipart(data, info);
		if (info.status_code != kOk)
			return Router::ErrorResponse(info.status_code, info.config); 
		HttpResponse response;

		response.set_status(info.status_code);
		response.set_reason_phrase();
		response.set_version("HTTP/1.1");
		response.set_header("Content-Type", "text/html");
		response.set_body(MultiPartSuccessfullBody());		
		AddContentLength(response, response.body());

		return response;
	}
	return Router::ErrorResponse(kNotImplemented, info.config);
}
