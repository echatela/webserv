#include "route_resolve.hpp"
#include "http_response.hpp"

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

RouteResolve::RouteResolve() {}

RouteResolve::~RouteResolve() {}


void 	RouteResolve::FindLocation(std::string req_path, Config & config, LocationConfig & location, bool & found) {
	
	std::string path = req_path.substr(1, req_path.length());
	size_t		frst_dir = path.find('/');
	std::string base_dir;

	if (frst_dir != std::string::npos)
		base_dir = req_path.substr(0, frst_dir + 1);
	else
		base_dir = req_path;

	try {
		location = config.locations().at(base_dir);
		found = true;
	}
	catch (std::exception & e) {
		found = false;
	}
}

std::string 	RouteResolve::BuildFilesystemPath(std::string uri, Config & config, LocationConfig location, bool found) {
	std::string root_dir;
	std::string file_system_path;

	if (found && !location.root.empty() && !location.root[0].empty() && !location.base_location.empty())
	{
		std::string root_path = location.root[0];
		root_dir = config.root() + root_path;

		if (uri.length() >= location.base_location.length())
			uri = uri.substr(location.base_location.length());
		else
			uri.clear();
	}
	else
	{
		root_dir = config.root();
	}
	
	size_t mark;

	mark = uri.find('?');
	if (mark != std::string::npos)
		uri = uri.substr(0, mark);	

	return  root_dir + uri;

}

static std::string	StripQuery(const std::string& uri) {

	size_t	mark = uri.find('?');

	if (mark == std::string::npos)
		return uri;
	return uri.substr(0, mark);
}

bool	RouteResolve::NormalizeUri(const std::string& uri, std::string& out) {

	std::vector<std::string>	parts;
	size_t				i = 0;
	bool				trailing;

	trailing = !uri.empty() && uri[uri.size() - 1] == '/';
	if (uri == "." || uri == ".."
		|| (uri.size() >= 2 && uri.compare(uri.size() - 2, 2, "/.") == 0)
		|| (uri.size() >= 3 && uri.compare(uri.size() - 3, 3, "/..") == 0))
		trailing = true;

	while (i < uri.size()) {
		size_t	j = uri.find('/', i);
		if (j == std::string::npos)
			j = uri.size();

		std::string	seg = uri.substr(i, j - i);
		if (seg == "..") {
			if (parts.empty())
				return false;
			parts.pop_back();
		} else if (!seg.empty() && seg != ".")
			parts.push_back(seg);
		i = j + 1;
	}

	out = "/";
	for (size_t k = 0; k < parts.size(); k++) {
		out += parts[k];
		if (k + 1 < parts.size())
			out += "/";
	}
	if (trailing && out != "/")
		out += "/";
	return true;
}

bool	StatCheck(std::string path) {
	struct stat info;

	if (stat(path.c_str(), &info) == -1)
		return false;
	return true;
}

bool	EndsWith(char c, std::string str) {
	
	if (str[str.length() - 1] == c)
		return true;
	return false;
	
}

void	InitRouteInfo(RouteInfo & info) {

	info.exists = false;
	info.file_path = "";
	info.filename = "";
	info.is_cgi = false;
	info.is_directory = false;
	info.status_code = kOk;
	info.uri = "";

}

static bool	MethodNotAllowed(const std::string & method,
	const std::vector<std::string> & methods)
{
	if (methods.empty())
		return false;

	std::vector<std::string>::const_iterator	it
		= std::find(methods.begin(), methods.end(), method);

	if (it == methods.end())
		return true;
	return false;
}

RouteInfo	RouteResolve::ResolveRoute(HttpRequest & req, Config & config) {

	RouteInfo	info;
	bool		location_found = false;
	struct stat	stat_info;
	std::string	clean_uri;

	InitRouteInfo(info);
	info.config = config;
	info.root = config.root();

	if (req.body().size() > config.max_body_size()) {
		info.status_code = kPayloadTooLarge;
		return info;
	}

	if (NormalizeUri(StripQuery(req.path()), clean_uri) == false) {
		info.status_code = kForbidden;
		return info;
	}

	FindLocation(clean_uri, config, info.location, location_found);
	info.loc_found = location_found;
	info.uri = clean_uri;
	info.file_path = BuildFilesystemPath(clean_uri, config,
		info.location, location_found);

	if (MethodNotAllowed(req.method(), info.location.methods)) {
		info.status_code = kMethodNotAllowed;
		return info;
	}

	if (stat(info.file_path.c_str(), &stat_info) == -1) {
		info.status_code = kNotFound;
		return info;
	}

	info.exists = true;
	if (S_ISDIR(stat_info.st_mode))
		info.is_directory = true;
	if (info.location.base_location == "/cgi")
		info.is_cgi = true;
	return info;
}
