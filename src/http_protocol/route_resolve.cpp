#include "route_resolve.hpp"
#include "http_response.hpp"

#include <algorithm>
#include <iostream>
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
		location = config.get_locations().at(base_dir);
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
		
	return  root_dir + uri;

}

bool 			PathResolved(std::string filepath) {
	char	resolved_path[PATH_MAX];
	char*	res;

	res = realpath(filepath.c_str(), resolved_path);
	if (res == NULL)
		return false;
	return true;
}

bool			StatCheck(std::string path) {
	struct stat info;

	if (stat(path.c_str(), &info) == -1)
		return false;
	return true;
}

bool 			EndsWith(char c, std::string str) {
	
	if (str[str.length() - 1] == c)
		return true;
	return false;
	
}

void 		InitRouteInfo(RouteInfo & info) {

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

RouteInfo 	RouteResolve::ResolveRoute(HttpRequest & req, Config & config) {

	RouteInfo info;
	bool location_found = false;
	struct stat stat_info;

	InitRouteInfo(info);

	std::cout << "=================================max_body_size is :" << config.max_body_size();
	if (req.get_body().size() > config.max_body_size())
	{
		info.status_code = kPayloadTooLarge;
		return info;
	}
	FindLocation(req.get_path(), config, info.location, location_found);

	info.uri = req.get_path();
	info.file_path = BuildFilesystemPath(info.uri, config, info.location, location_found);

	if (MethodNotAllowed(req.get_method(), info.location.methods)) {
		info.status_code = kMethodNotAllowed;
		return info;
	}

	if (PathResolved(info.file_path) == false
		|| stat(info.file_path.c_str(), &stat_info) == -1)
		info.status_code = kNotFound;

	if (S_ISDIR(stat_info.st_mode))
		info.is_directory = true;

	if (info.location.base_location == "/cgi")
		info.is_cgi = true;
	info.loc_found = location_found;
	info.root = config.root();
	return info;
}
