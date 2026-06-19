#include "http_request.hpp"
#include "router.hpp"
#include "../webserv.hpp"
#include <fstream>
#include <cerrno>
#include <string.h>

Router::Router(Config config) : config_(config) {}

Router::Router() {}

Router::~Router() {}

void			Router::set_config(const Config & config) { config_ = config; }

std::string		FirstDir(std::string path) {

	std::string first_dir;
	size_t		first_slash = path.find('/');

	if (first_slash != std::string::npos)
		return (path.substr(1, first_slash));
	return path.substr(1, path.length());

}

// checks existing folder in given location, root is none if not found
void			Router::FindLocationRoot(std::string & root, HttpRequest & req) {

	size_t			first_dir = req.get_path().substr(1, req.get_path().size()).find_first_of('/');
	std::string 	base;
	LocationConfig	location;

	std::cout << "::::::::::::::::req path" << req.get_path() << '\n';

	try {
	
		if (first_dir == std::string::npos)
		{
			base = req.get_path();
		}
		else
		{
			base = req.get_path().substr(0, first_dir + 1);
		}
	
			location = this->config_.get_locations().at(base);
		root = location.root;
	}
	catch (std::exception& e)
	{
		root = "none";
	}
}

bool			Router::IsPathResolved(std::string& path, int *status_code) {

	char		resolved_path[PATH_MAX];
	char*		res = realpath(path.c_str(), resolved_path);
	struct stat	info;

	if (res == NULL)
	{
		*status_code = NOT_FOUND;
		return false;
	}
	if (stat(path.c_str(), &info) == -1)
	{
		*status_code = NOT_FOUND;
		return false;
	}	
	return true;
}

std::string		Router::Filename(std::string path) {

	std::string	filename;
	size_t 		slash_pos = path.find_last_of('/');

	return (path.substr(slash_pos, path.length()));

}

void		Router::CheckAndSetPath(std::string& path, HttpRequest & req, int *status_code) {

	std::string		request_path = req.get_path();
	std::string		loc_root;

	FindLocationRoot(loc_root, req);
	if (loc_root != "none")
	{
		loc_root.replace(0, 1, config_.get_root());
		path = loc_root + Filename(request_path);
		IsPathResolved(path, status_code);
	}
	else
	{
		path = config_.get_root() + request_path;
		IsPathResolved(path, status_code);
	}

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

void		Router::AddContentType(HttpResponse & current, std::string filepath) {

	size_t		dot = filepath.find_last_of('.');
	std::string	extension;
	
	if (dot != std::string::npos)
		extension = filepath.substr(dot, filepath.size());
	std::cout << "" << '\n';
	current.set_header("Content-Type", value_from_extension(extension));

}

void		Router::AddContentLength(HttpResponse & current, std::string body) {

	current.set_header("Content-Length", webserv::utils::IntToStr(body.size()));

}

HttpResponse	Router::BuildErrorResponse(int status) {

	HttpResponse	resp;
	std::string	body;

	resp.set_status(status);
	resp.set_reason_phrase();
	resp.set_version("HTTP/1.1");
	body.append("<html>\r\n<body>\r\n");
	body.append("<h1>");
	body.append(webserv::utils::IntToStr(status) + " " + resp.get_reason_phrase(status));
	body.append("</h1>\r\n");
	body.append("</html>\r\n</body>");
	resp.set_body(body);
	resp.set_header("Content-Type", "text/html");
	resp.set_header("Content-Length", webserv::utils::IntToStr(body.size()));
	return (resp);

}




HttpResponse	Router::HandleRequest(HttpRequest& req) {
	
	std::cout << "in handlerequest\n";
	if (req.get_method() == "GET")
		return (HandleGet(req));
	if (req.get_method() == "DELETE")
		return (HandleDelete(req));
	if (req.get_method() == "POST")
	{
		// std::cout << "\n=== POST REQUEST DEBUG ===" << std::endl;
		// std::cout << "Method: " << req.get_method() << std::endl;
		// std::cout << "Path: " << req.get_path() << std::endl;
		// std::cout << "Version: " << req.get_version() << std::endl;
		// std::cout << "Body: " << req.get_body() << std::endl;
		// std::cout << "=== END POST REQUEST DEBUG ===" << std::endl;
		return HandlePost(req);
		// return (BuildErrorResponse(METHOD_NOT_ALLOWED));
	}
	return (BuildErrorResponse(METHOD_NOT_ALLOWED));
}


void		Router::FillBody(std::string& body, std::string filepath, int* status_code) {

	std::ifstream	file(filepath.c_str());
	std::string	line;

	if (!file.is_open())
		*status_code = FORBIDDEN;
	while (getline(file, line))
		body.append(line);
	file.close();
	if (body.size() == 0)
		*status_code = NO_CONTENT;

}

HttpResponse	Router::HandleGet(HttpRequest& req) {
	
	int 		status_code = NO_ERROR;
	std::string	path;
	std::string	body;
	HttpResponse response;

	std::cout << "request path = " << req.get_path() << '\n';
	CheckAndSetPath(path, req, &status_code);
	if (status_code != NO_ERROR)
		return BuildErrorResponse(status_code);

	FillBody(body, path, &status_code);
	if (status_code != NO_ERROR)
		return BuildErrorResponse(status_code);
	
	response.set_body(body);
	response.set_status(OK);
	response.set_reason_phrase();
	response.set_version("HTTP/1.1");

	AddContentType(response, path);
	AddContentLength(response, body);

	return response;

	// find path
		// location root ?
			// oui -> remplace par root
				// check realpath
			// non -> ajout root server au debut
				// check realpath
		// resolved path
}

std::string		Router::MakeFileDeletedBody(std::string path) {
	std::string body;

	body = "<html lang=\"en-US\">\r\n<body>\r\n<h1>File ";
	body += "\"" + Filename(path) + "\"";
	body += " deleted.</h1>\r\n";
	body += "</body>\r\n</html>\r\n";
	return body;
}


HttpResponse	Router::HandleDelete(HttpRequest& req) {

	int				status_code = NO_ERROR;
	std::string		path;
	HttpResponse	response;

	CheckAndSetPath(path, req, &status_code);
	if (status_code != NO_ERROR)
		return BuildErrorResponse(status_code);

	status_code = std::remove(path.c_str());
	if (status_code != NO_ERROR)
		return BuildErrorResponse(METHOD_NOT_ALLOWED);

	response.set_status(OK);
	response.set_reason_phrase();
	response.set_version("HTTP/1.1");
	AddContentType(response, path);
	response.set_body(MakeFileDeletedBody(path));
	AddContentLength(response, response.get_body());
	
	return response;
}

std::string		PostType(std::string content_type) {

	std::string post_type;
	size_t		sep = content_type.find(';');

	if (sep != std::string::npos)				// TODO
		return content_type.substr(1, sep - 1); // on commence a 1 pour enlever l'espace = nul, pas clean ni stable
												// sep - 1 pour le ';' -> idem

	return "none";
}

std::string		Delimiter(std::string content_type) {

	size_t boundary_start = content_type.find("boundary");
	std::string	boundary;
	
	std::cout << "/////////////bound start -> " << boundary_start << '\n';
	if (boundary_start != std::string::npos)
	{
		std::cout << "////////////found delim " << content_type.substr(boundary_start + 8, content_type.length()) << '\n';
		return content_type.substr(boundary_start + 9, content_type.length());
	}


	// sep = boundary.find('=');
	// if (sep != std::string::npos)
	// 	return boundary.substr(sep, boundary.length());
	return "none";	
}


void			Router::ParsePostBody(HttpRequest& req) {

	std::string 			delim;
	std::vector<FormParts>	form_parts;
	int						i;
	std::string				body = req.get_body();

	delim = Delimiter(req.get_header().at("Content-Type"));
	
	while (delim[i] == body[i])
		i++;
	
	size_t block_start = body.find(delim);
	if (block_start != std::string::npos)
		

}

HttpResponse	Router::HandlePost(HttpRequest& req) {
	
	// int 		status_code = NO_ERROR;
	std::string	root;
	HttpResponse response;
	

	std::string		post_type;
	std::string		delim;
	
	std::cout << "////////////////////in handle post \n";
	FindLocationRoot(root, req);
	if (root == "none")
	return BuildErrorResponse(BAD_REQUEST);
	std::cout << "/////////" << req.get_header().at("Content-Type") << std::endl;
	post_type = PostType(req.get_header().at("Content-Type"));
	std::cout << "////////////////post_type ." << post_type << '\n';
	if (post_type == "multipart/form-data")

	std::cout << "////////////////delim " << delim << '\n';
	std::cout << "////////////////body is: " << req.get_body() << "/////////////////";

	return response;
}

// HttpResponse	Router::HandlePost(HttpRequest& req) {

// 	int status_code = NO_ERROR;

	// multipart/form-data -> creation de fichier a partir d'une structure .cf TODO_perso
	// application/x-www-form-urlencoded -> cas de formulaire (connexion username + mdp par exemple)
	// application/json
	// application/x-www-form-urlencoded + location cgi-bin par exemple = cgi 

// same workflow pour la determination du chemin

// il faut un body 
// sa taille doit correspondre a content length

// }

std::vector<Token>	ConfigLexer::Tokenize() {

	std::vector<Token> tokens;
	std::string		current_str;
	size_t i = 0;
	while (i < file_content_.size())
	{
		while (i < file_content_.size() 
			&& !std::isspace(file_content_[i])
			&& !isSpecial(file_content_[i]))
			current_str += file_content_[i++];
		if (!current_str.empty())
		{
			tokens.push_back(MakeToken(current_str));
			current_str.clear();
		}
		if (isSpecial(file_content_[i]))
		{
			current_str += file_content_[i];
			if (!current_str.empty())
			{
				tokens.push_back(MakeToken(current_str));
				current_str.clear();
			}
		}
		i++;
	}
	
	// for (size_t i = 0; i < tokens.size(); i++)
	// {
	// 	std::cout << "token type: " << tokens[i]._type << "token content: " << tokens[i]._content << std::endl;
	// }
	return (tokens);
}

