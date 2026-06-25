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

struct FormPart {

	std::map<std::string, std::string>	headers;
	std::string 						body;

};

struct FormData {

	std::vector<FormPart>		form_parts;
	std::string 				boundary;
	std::vector<std::string>	tokens;

};

// TODO modifier en utlisant CRLF COMME IL SE DOIT !!!!!
void	 	TokenizeForm(std::string body, FormData & data) {

	std::string cur;
	size_t i = 0;
	
	while (i < body.length())
	{
		while (body[i] != '\r')
			cur += body[i++];
		i++;
		if (body[i] == '\n')
		{
			std::cout << "current token: " << cur << '\n';
			data.tokens.push_back(cur);
			cur.clear();
		}
		i++;
	}
}

void 		InsertHeader(std::string token, FormPart & form_part) {

	size_t sep = token.find(':');

	if (sep != std::string::npos)
	{
		std::string key = token.substr(0, sep);
		// std::cout << "////////////KEY ." << key << ".\n";
		std::string value = token.substr(sep + 2);
		// std::cout << "////////////value ." << value << ".\n";
		std::pair<std::string, std::string> header(key, value);
		form_part.headers.insert(header);
	}
}

FormPart 	AddFormPart(FormData & data, size_t *i) {

	std::cout << "in add form part\n";
	FormPart part;

	while (*i < data.tokens.size() && data.tokens[*i] != "\n")
		InsertHeader(data.tokens[*i++], part);
	std::cout << *i << std::endl;
	if (*i < data.tokens.size() && data.tokens[*i] == "\n")
		i++;
	std::cout << *i << std::endl;
	while (*i < data.tokens.size() && data.tokens[*i] != "\n")
		part.body += data.tokens[*i++];
	std::cout << *i << std::endl;

	return part;
} 

FormData	ParseMultipart(std::string body, std::string content_type, RouteInfo & info) {
	(void)body;
	FormData	data;
	size_t 		sep = content_type.find("boundary");

	if ( sep != std::string::npos)
		data.boundary = content_type.substr(sep + 9);
	else
		info.status_code = kBadRequest;
	TokenizeForm(body, data);

	size_t i = 0;
	std::cout << "start parse multipart loop \n";
	while (i < data.tokens.size() && data.tokens[i] != data.boundary + "--")
	{
		if (data.tokens[i] == data.boundary)
		{
			i++;
		}
		// data.form_parts.push_back(AddFormPart(data, &i));
		FormPart part;

		std::cout << i << std::endl;
		while (i < data.tokens.size() && data.tokens[i] != "\r\n")
			InsertHeader(data.tokens[i++], part);
		std::cout << i << std::endl;
		if (i < data.tokens.size() && data.tokens[i] == "\n")
			i++;
		std::cout << i << std::endl;
		while (i < data.tokens.size() && data.tokens[i] != "\n")
			part.body += data.tokens[i++];		
		data.form_parts.push_back(part);
			std::cout << i << std::endl;
		i++;
	}

	std::cout << "/////FOUND BOUNDARY -> " << data.boundary << std::endl;
	return data; 
}

std::string 	GetFilename(FormPart part) {

	std::map<std::string, std::string> headers = part.headers;
	
	// std::cout << "HEADERS\n";
	// for (size_t = 0; i < headers.)
	
	std::string content_disp;
	try {
		// content_disp = headers.at("Content-Disposition");
		content_disp = headers["Content-Disposition"];
		std::cout << "//////content disp " << content_disp << "\n";
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << std::endl;
		return "none";
	}
	std::string filename;
	size_t 		filename_sep = content_disp.find("filename");
	
	if (filename_sep != std::string::npos)
		return content_disp.substr(filename_sep + 10, content_disp.length() - 2);
	return "none";
}

void			HandleMultipart(FormData data, RouteInfo & info) {
	std::cout << "IN HANDLE SIZE " << data.form_parts.size() << "\n";

	for (size_t i = 0; i < data.form_parts.size(); i++)
	{
		std::ofstream	new_file;
		std::string filename = GetFilename(data.form_parts[i]);

		if (filename != "none")
		{
			filename = info.file_path + "/" + filename;
			std::cout << "/////////FILENAME -> " << filename << "\n";
			std::cout << "/////////body -> " << data.form_parts[i].body << "\n";
			new_file.open(filename.c_str());
			new_file << data.form_parts[i].body;
		}
		else
			info.status_code = kBadRequest;
	}
	std::cout << "IN HANDLE MULTIPART, " << info.status_code << "\n";
}

std::string		MultiPartSuccessfullBody() {
	std::string body;

	body = "<html lang=\"en-US\">\r\n<body>\r\n<h1>File uploaded</h1>\r\n";
	body += "</body>\r\n</html>\r\n";
	return body;
}

HttpResponse 	StaticHandler::BuildPost(HttpRequest & req, RouteInfo & info) {
	(void)req;
	// (void)info;

	std::map<std::string, std::string>	req_headers = req.get_header();
	std::string content_type;

	try {
		content_type = req_headers.at("Content-Type");
	}
	catch (std::exception& e) {
		return Router::ErrorResponse(kBadRequest);
	}
	std::cout << "/////CONTENT TYPE -> ." << content_type << std::endl;
	if (content_type.compare(0, 19,"multipart/form-data") == 0)
	{
		HandleMultipart(ParseMultipart(req.get_body(), content_type, info), info);
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
	return Router::ErrorResponse(kBadRequest);

}


// 	// multipart/form-data -> creation de fichier a partir d'une structure .cf TODO_perso
// 	// application/x-www-form-urlencoded -> cas de formulaire (connexion username + mdp par exemple)
// 	// application/json
// 	// application/x-www-form-urlencoded + location cgi-bin par exemple = cgi 

// // same workflow pour la determination du chemin

// // il faut un body 
// // sa taille doit correspondre a content length
