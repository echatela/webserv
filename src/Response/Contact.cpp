#include "Contact.hpp"

// namespace
// {
// 	std::string escape_html(const std::string &value)
// 	{
// 		std::string escaped;

// 		for (size_t i = 0; i < value.size(); ++i)
// 		{
// 			if (value[i] == '&')
// 				escaped.append("&amp;");
// 			else if (value[i] == '<')
// 				escaped.append("&lt;");
// 			else if (value[i] == '>')
// 				escaped.append("&gt;");
// 			else if (value[i] == '"')
// 				escaped.append("&quot;");
// 			else
// 				escaped.push_back(value[i]);
// 		}
// 		return (escaped);
// 	}

// 	HttpResponse build_response(const std::string &body)
// 	{
// 		HttpResponse response;

// 		response.set_status(OK);
// 		response.set_version("HTTP/1.1");
// 		response.set_reason_phrase();
// 		response.set_header("Content-Type", "text/html");
// 		response.set_header("Content-Length", webserv::utils::IntToStr(body.size()));
// 		response.set_body(body);
// 		return (response);
// 	}

// 	std::string build_contact_page(const std::string &name, const std::string &phone, const std::string &img)
// 	{
// 		std::string body;

// 		body.append("<!doctype html>\n<html lang=\"en\">\n<head>\n");
// 		body.append("\t<meta charset=\"utf-8\">\n");
// 		body.append("\t<title>Contact</title>\n");
// 		body.append("\t<link rel=\"stylesheet\" href=\"style.css\">\n");
// 		body.append("</head>\n<body>\n<div class=\"menu contact-page\">\n");
	
// 		body.append("\t<div class=\"contact-card\">\n");
// 		body.append("\t\t<img class=\"contact-photo\" src=\"");
// 		body.append(escape_html(img));
// 		body.append("\" alt=\"Contact photo\">\n");
// 		body.append("\t\t<p class=\"contact-phone\">Phone: ");
// 		body.append(escape_html(phone));
// 		body.append("</p>\n");
// 		body.append("\t</div>\n");
// 		body.append("\t<a href=\"add.html\"><button class=\"links\">Back</button></a>\n");
// 		body.append("\t<form action=\"/contact/delete\" method=\"post\">\n");
// 		body.append("\t\t<button class=\"save\" type=\"submit\">Delete</button>\n");
// 		body.append("\t</form>\n");
// 		body.append("</div>\n</body>\n</html>\n");
// 		return (body);
// 	}

// 	std::string build_add_page()
// 	{
// 		std::string body;

// 		body.append("<!doctype html>\n<html lang=\"en\">\n<head>\n");
// 		body.append("\t<meta charset=\"utf-8\">\n");
// 		body.append("\t<title>Add Contact</title>\n");
// 		body.append("\t<link rel=\"stylesheet\" href=\"style.css\">\n");
// 		body.append("</head>\n<body>\n<div class=\"menu\">\n");
// 		body.append("\t<h1>Add Contact</h1>\n");
// 		body.append("\t<form action=\"/contact\" method=\"post\">\n");
// 		body.append("\t\t<input type=\"text\" name=\"name\" placeholder=\"Name\"><br>\n");
// 		body.append("\t\t<input type=\"text\" name=\"phone\" placeholder=\"Phone\"><br>\n");
// 		body.append("\t\t<input type=\"text\" name=\"img\" placeholder=\"Photo URL\"><br>\n");
// 		body.append("\t\t<button class=\"save\" type=\"submit\">Save</button>\n");
// 		body.append("\t</form>\n");
// 		body.append("\t<a href=\"index.html\"><button class=\"links\">Back</button></a>\n");
// 		body.append("</div>\n</body>\n</html>\n");
// 		return (body);
// 	}
// }

Contact::Contact(std::string name, std::string phone, std::string img)
: name_(name), phone_(phone), img_(img)
{
}

Contact::~Contact()
{
}

std::string Contact::get_name() const
{
	return (name_);
}

std::string Contact::get_phone() const
{
	return (phone_);
}

std::string Contact::get_img() const
{
	return (img_);
}

std::string Contact::HandlePostResponse(HttpResponse &response)
{
	(void)response;
	std::string response_body = "<!doctype html>\n<html lang=\"en\">\n<head>\n";
	response_body += "\t<meta charset=\"utf-8\">\n";
	response_body += "\t<title>View Contact</title>\n";
	response_body += "\t<link rel=\"stylesheet\" href=\"style.css\">\n";
	response_body += "</head>\n";
	response_body += "<body>\n";
	response_body += "\t<div class=\"menu\">\n";
	response_body += "\t	<h1>";
	response_body += name_;
	response_body += "</h1>\n\n";
	response_body += "\t	<div class=\"contact-card\">\n";
	response_body += "\t\t\t<img class=\"contact-photo\" src=\"";
	response_body += img_;
	response_body += "\">\n";
	response_body += "\t		<p class=\"contact-phone\">";
	response_body += phone_;
	response_body += "</p>\n";
	response_body += "\t	<button class=\"save\" type=\"submit\">Delete</button>\n\n";
	response_body += "\t	<div class=\"contact-actions\" style=\"margin-top: 50px;\">\n";
	response_body += "\t		<a href=\"index.html\"><button class=\"links\">Back</button></a>\n";
	response_body += "\t	</div>\n";
	response_body += "\t</div>\n";
	response_body += "</body>\n";
	response_body += "</html>\n";

	return (response_body);
}