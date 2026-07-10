#include "http_request.hpp"
#include "http_parser.hpp"
#include <cstddef>
#include <map>
#include <sstream>
#include <iostream>

HttpRequest::HttpRequest() : error_(NO_ERROR) { }

HttpRequest::~HttpRequest() { }

HttpRequest::HttpRequest(const HttpRequest& other)
: method_(other.method_), path_(other.path_), version_(other.version_), header_(other.header_), body_(other.body_), error_(other.error_)
{
}

HttpRequest& HttpRequest::operator=(const HttpRequest& other) {

	if (this != &other) {

		this->method_ = other.method_;
		this->path_ = other.path_;
		this->version_ = other.version_;
		this->header_ = other.header_;
		this->body_ = other.body_;
		this->error_ = other.error_;
	}
	return *this;
}

int HttpRequest::ParseMethod(const std::string& method)
{
	if (method != "GET" && method != "POST" && method != "DELETE")
		return NOT_IMPLEMENTED;
	return NO_ERROR;
}

int HttpRequest::ParsePath(const std::string& path)
{
	if (path.empty() || path[0] != '/')
		return BAD_REQUEST;
	return NO_ERROR;
}

int HttpRequest::ParseVersion(const std::string& version)
{
	if (version != "HTTP/1.1" && version != "HTTP/1.0")
		return HTTP_VERSION_NOT_SUPPORTED;
	return NO_ERROR;
}

int HttpRequest::ParseRequestLine(std::string requestline)
{
	std::istringstream	iss(requestline);

	std::string method;
	std::string path;
	std::string version;

	if (!(iss >> method >> path >> version))
		return BAD_REQUEST;

	this->method_ = method;
	this->path_ = path;
	this->version_ = version;

	int error[3] = {ParseMethod(this->method_), ParsePath(this->path_), ParseVersion(this->version_)};
	for (int i = 0; i < 3; i++)
	{
		if (error[i] != NO_ERROR)
			return error[i];
	}
	return NO_ERROR;
}

static std::string	CanonKey(std::string key) {
	
	for (size_t i = 0; i < key.size(); i++) {
		if (i == 0 || key[i - 1] == '-')
			key[i] = std::toupper(key[i]);
		else
			key[i] = std::tolower(key[i]);
	}
	return key;
}

int HttpRequest::ParseHeader(std::string header)
{
	std::istringstream iss(header);
	std::string line;

	while (std::getline(iss, line))
	{
		size_t pos = line.find(":");
		if (pos == std::string::npos)
			return BAD_REQUEST;

		std::string index = CanonKey(line.substr(0, pos));
		std::string value = line.substr(pos + 1);
		size_t start = value.find_first_not_of(" \t");
		size_t end = value.find_last_not_of("\t\r\n");
		if (start == std::string::npos || end == std::string::npos)
			value.clear();
		else
			value = value.substr(start, end - start + 1);

		this->header_[index] = value;
	}
	if (this->header_["Host"].empty())
		return BAD_REQUEST;
	return NO_ERROR;
}

int HttpRequest::ParseBody(std::string body)
{
	this->body_ = body;
	return NO_ERROR;
}

const std::string &	HttpRequest::method() const { return method_; }
const std::string &	HttpRequest::path() const { return path_; }
const std::string &	HttpRequest::version() const { return version_; }
const std::map<std::string, std::string> &	HttpRequest::headers() const {
	return header_;
}
const std::string &	HttpRequest::body() const { return body_; }
int HttpRequest::error() const { return error_; }
std::string	HttpRequest::header(std::string key) const {
	std::map<std::string, std::string>::const_iterator it
		= header_.find(CanonKey(key));
	if (it == header_.end())
		return "";
	return it->second;
}
void HttpRequest::set_error(int error) {
	error_ = error;
}
