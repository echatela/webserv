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
	switch (status_code_) {
		case kOk:
			reason_phrase_ = "OK";
			break;
		case kCreated:
			reason_phrase_ = "Created";
			break;
		case kNoContent:
			reason_phrase_ = "No Content";
			break;
		case kBadRequest:
			reason_phrase_ = "Bad Request";
			break;
		case kForbidden:
			reason_phrase_ = "Forbidden";
			break;
		case kNotFound:
			reason_phrase_ = "Not Found";
			break;
		case kMethodNotAllowed:
			reason_phrase_ = "Method Not Allowed";
			break;
		case kPayloadTooLarge:
			reason_phrase_ = "Payload Too Large";
			break;
		case kInternalServerError:
			reason_phrase_ = "Internal Server Error";
			break;
	}
}

std::string		HttpResponse::body() const {
	return body_;
}

int 			HttpResponse::status_code() const {
	return status_code_;
}

std::string		HttpResponse::reason_phrase(int status_code) {
	switch (status_code) {
		case kOk:
			return "OK";
		case kCreated:
			return "Created";
		case kNoContent:
			return "No Content";
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
