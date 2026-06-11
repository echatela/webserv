#include "router.hpp"
#include "webserv.hpp"

Router::Router() {}

Router::~Router() {}


HttpResponse	Router::BuildErrorResponse(int status) {

	HttpResponse	resp;
	std::string		body;

	resp.set_status(status);
	resp.set_reason_phrase();
	resp.set_version("HTTP/1.1");
	body.append("<html>\r\n<body>\r\n");
	body.append("<h1>");
	body.append(webserv::utils::IntToStr(status) + resp.get_reason_phrase(status));
	body.append("</h1>");
	body.append("</html>\r\n</body>\r\n");
	resp.set_body(body);
	resp.set_header("Content-Type", "text/htmlRequest");
	resp.set_header("Content-Length", webserv::utils::IntToStr(body.size()));
	return (resp);
}


HttpResponse	Router::HandleRequest(HttpRequest& req) {
	if (req.get_method() == "GET")
		return (HandleGet(req));
	if (req.get_method() == "POST")
		return (HandlePost(req));
	return (HandleDelete(req));
}



HttpResponse	Router::HandleGet(HttpRequest& req) {
	HttpResponse 		current;
	std::string			absolute_path;
	struct stat*		info;

	absolute_path = config_.root + req.get_path();
	if (stat(absolute_path.c_str(), info) == -1)
	{
		current.set_status(NOT_FOUND);
		return (BuildErrorResponse(NOT_FOUND));
	}
	// check avec realpath si il est toujours contenu dans le root du server
	std::fstream	file(absolute_path);
	std::string		line;
	std::string		body;

	if (!file.is_open())
	{
		current.set_status(FORBIDDEN);
		return (BuildErrorResponse(FORBIDDEN));
	}
	while (getline(file, line))
		body.append(line);
	if (body.size() == 0)
		return (BuildErrorResponse(NO_CONTENT));
	
	current.set_body(body);
	current.set_status(OK);
	current.set_reason_phrase();
	current.set_version("HTTP/1.1");
	AddContentType(current, req.get_path());
	AddContentLength(current, body);
	return (current);
}

std::string	value_from_extension(std::string extension) {
	std::map<std::string, std::string> types;

	types = { 	{".html", "text/htmlRequest"},
				{".css", "text/css"},	
				{".js", "application/javascript"},	
				{".png", "image/png"},	
				{".jpg", "image/jpeg"},	
				{".gif", "image/gif"}};

	return (types.at(extension));
}

void		Router::AddContentType(HttpResponse & current, std::string filepath) {
	size_t		dot = filepath.find_last_of('.');
	std::string	extension;
	// Header		content_type;
	
	// content_type.key = "Content-Type";
	
	if (dot != std::string::npos)
		extension = filepath.substr(dot, filepath.size());

	// content_type.value = value_from_extension(extension);
	current.set_header("Content-Type", value_from_extension(extension));
}

void		Router::AddContentLength(HttpResponse & current, std::string body) {
	current.set_header("Content-Length", webserv::utils::IntToStr(body.size()));
}

//  stat() permet de recuperer les infos du fichiers demande, utile pour la reponse http
// realpath() -> transforme uri zarbi en chemin absolu
// 
// 


// recuperer le chemin complet vers le fichier 
	// verif qu'il est dans le root (pour les chemins bizarre)
// determiner l'extension pour le content-type
	// si texte -> content-length
	// si image -> content-length aussi ? ou 


// HTTP/1.1 200 OK
// Content-Type: text/html
// Content-Length: 42

// <html><body>Hello World</body></html>
