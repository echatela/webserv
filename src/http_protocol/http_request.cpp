#include "http_request.hpp"

HttpRequest::HttpRequest() : error_(NO_ERROR) { }

HttpRequest::~HttpRequest() { }

HttpRequest::HttpRequest(const HttpRequest& other)
: method_(other.method_), path_(other.path_), version_(other.version_), header_(other.header_), body_(other.body_), error_(other.error_)
{
}

HttpRequest& HttpRequest::operator=(const HttpRequest& other)
{
	if (this != &other)
	{
		this->method_ = other.method_;
		this->path_ = other.path_;
		this->version_ = other.version_;
		this->header_ = other.header_;
		this->body_ = other.body_;
		this->error_ = other.error_;
	}
	return *this;
}

const std::string HttpRequest::get_method() const
{
	return method_;
}

const std::string HttpRequest::get_path() const
{
	return path_;
}

const std::string HttpRequest::get_version() const
{
	return version_;
}

const std::map<std::string, std::string> HttpRequest::get_header() const
{
	return header_;
}

const std::string HttpRequest::get_body() const
{
	return body_;
}

int HttpRequest::get_error() const
{
	return error_;
}

void HttpRequest::set_error(int error)
{
	error_ = error;
}

int HttpRequest::ParseMethod(const std::string& method)
{
	if (method != "GET" && method != "POST" && method != "DELETE")
		return METHOD_NOT_ALLOWED;
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
	if (version != "HTTP/1.1")
		return HTTP_VERSION_NOT_SUPPORTED;
	if (version.empty())
		return BAD_REQUEST;
	return NO_ERROR;
}

int HttpRequest::ParseRequestLine(std::string requestline)
{
	std::istringstream iss(requestline);

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

int HttpRequest::ParseHeader(std::string header)
{
	std::istringstream iss(header);
	std::string line;

	while (std::getline(iss, line))
	{
		size_t pos = line.find(":");
		if (pos == std::string::npos)
			return BAD_HEADER;
		std::string index = line.substr(0, pos);
		std::string value = line.substr(pos + 1);
		this->header_[index] = value;
	}
	if (this->header_["Host"].empty())
		return BAD_HEADER;
	return NO_ERROR;
}

int HttpRequest::ParseBody(std::string body)
{
	this->body_ = body;
	return NO_ERROR;
}
