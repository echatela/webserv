#include "http_response.hpp"
#include <sstream>
#include <string>

HttpResponse::HttpResponse() : status_code_(0), reason_phrase_("OK"), body_(""), version_("HTTP/1.1") {}

HttpResponse::HttpResponse(const HttpResponse &src)
	: status_code_(src.status_code_), reason_phrase_(src.reason_phrase_),
	  body_(src.body_), headers_(src.headers_), version_(src.version_) {}

HttpResponse &HttpResponse::operator=(const HttpResponse &rhs) {
	if (this != &rhs) {
		status_code_ = rhs.status_code_;
		reason_phrase_ = rhs.reason_phrase_;
		body_ = rhs.body_;
		headers_ = rhs.headers_;
		version_ = rhs.version_;
	}
	return *this;
}

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
	reason_phrase_ = reason_phrase(status_code_);
}

std::string		HttpResponse::body() const { return body_; }
int 			HttpResponse::status_code() const { return status_code_; }

std::string		HttpResponse::reason_phrase(int status_code) {
	switch (status_code) {
		case kOk:
			return "OK";
		case kCreated:
			return "Created";
		case kNoContent:
			return "No Content";
		case kMovedPermanently:
			return "Moved Permanently";
		case kFound:
			return "Found";
		case kBadRequest:
			return "Bad Request";
		case kForbidden:
			return "Forbidden";
		case kNotFound:
			return "Not Found";
		case kMethodNotAllowed:
			return "Method Not Allowed";
		case kPayloadTooLarge:
			return "Payload Too Large";
		case kInternalServerError:
			return "Internal Server Error";
		case kNotImplemented:
			return "Not Implemented";
		case kBadGateway:
			return "Bad Gateway";
		case kGatewayTimeout:
			return "Gateway Timeout";
		case kHttpVersionNotSupported:
			return "HTTP Version Not Supported";
	}
	return "Internal Server Error";
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
