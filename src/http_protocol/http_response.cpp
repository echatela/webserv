#include "http_response.hpp"
#include <sstream>
#include <string>

HttpResponse::HttpResponse() {}

HttpResponse::~HttpResponse() {}

void		HttpResponse::set_body(std::string body) {body_ = body;}

void		HttpResponse::set_header(std::string key, std::string value) {
	Header current;

	current.key = key;
	current.value = value;
	headers_.push_back(current);
}

void		HttpResponse::set_status(int status) {status_code_ = status;}

void		HttpResponse::set_version(std::string version) {version_ = version;}

void		HttpResponse::set_reason_phrase() {
	switch (status_code_) {
		case OK:
			reason_phrase_ = "OK";
			break;
		case CREATED:
			reason_phrase_ = "Created";
			break;
		case NO_CONTENT:
			reason_phrase_ = "No content found";
			break;
		// case BAD_REQUEST:
		// 	reason_phrase_ = "Bad request";
			break;
		case FORBIDDEN:
			reason_phrase_ = "Forbidden";
			break;
		case NOT_FOUND:
			reason_phrase_ = "Not Found";
			break;
		case METHOD_NOT_ALLOWED:
			reason_phrase_ = "Method not allowed";
			break;
		case PAYLOAD_TOO_LARGE:
			reason_phrase_ = "Payload too large";
			break;
		case INTERNAL_SERVER_ERROR:
			reason_phrase_ = "Internal server error";
			break;
	}
}

std::string		HttpResponse::get_body() const {
	return body_;
}

std::string		HttpResponse::get_reason_phrase(int status_code) {
	switch (status_code) {
		case OK:
			return "OK";
		case CREATED:
			return "Created";
		case NO_CONTENT:
			return "No content found";
		// case BAD_REQUEST:
		// 	return "Bad request";
		case FORBIDDEN:
			return "Forbidden";
		case NOT_FOUND:
			return "Not Found";
		// case METHOD_NOT_ALLOWED:
		// 	return "Method not allowed";
		case PAYLOAD_TOO_LARGE:
			return "Payload too large";
		case INTERNAL_SERVER_ERROR:
			return "Internal server error";
	}
	return "CODE NOT VALID";
}


std::string int_to_string(int value) {
	std::ostringstream	s;
	s << value;
	return (s.str());
}

std::string	HttpResponse::ToString() {
	std::string response = version_;
	
	response.append(" " + int_to_string(status_code_) + " " + reason_phrase_);
	response.append("\r\n");
	for (size_t i = 0; i < headers_.size(); i++)
	{
		response.append(headers_[i].key + ": " + headers_[i].value + "\r\n");
	}
	// response.append("\r\n");
	response.append("\r\n");
	response.append(body_);
		// DISCUTABLE -> le header de la requete est parse sous forme de map
		// 
	return response;
}

std::vector<char> 	HttpResponse::ToCharVector() {

	std::string response = this->ToString();
	std::vector<char> vector;
	for (size_t i = 0; i < response.size(); i++)
	{
		vector.push_back(response[i]);
	}
	vector.push_back('\0');
	return (vector);
}


// | Code | Signification         |
// | ---- | --------------------- |
// | 200  | OK                    |
// | 201  | Created               |
// | 204  | No Content            |
// | 400  | Bad Request           |
// | 403  | Forbidden             |
// | 404  | Not Found             |
// | 405  | Method Not Allowed    |
// | 413  | Payload Too Large     |
// | 500  | Internal Server Error |
