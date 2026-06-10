#include "http_request.hpp"

httpRequest::httpRequest() { }

httpRequest::~httpRequest() { }

httpRequest::httpRequest(const httpRequest& other)
: method_(other.method_), path_(other.path_), version_(other.version_), header_(other.header_), body_(other.body_)
{
}

httpRequest& httpRequest::operator=(const httpRequest& other)
{
	if (this != &other)
	{
		this->method_ = other.method_;
		this->path_ = other.path_;
		this->version_ = other.version_;
		this->header_ = other.header_;
		this->body_ = other.body_;
	}
	return *this;
}

// const std::string httpRequest::getMethod() const
// {
// 	return method_;
// }

// const std::string httpRequest::getPath() const
// {
// 	return path_;
// }

// const std::string httpRequest::getVersion() const
// {
// 	return version_;
// }

// const std::map<std::string, std::string> httpRequest::getHeader() const
// {
// 	return header_;
// }

// const std::string httpRequest::getBody() const
// {
// 	return body_;
// }

int httpRequest::parseMethod(const std::string& method)
{
	if (method != "GET" && method != "POST" && method != "DELETE")
		return METHOD_NOT_ALLOWED;
	return NO_ERROR;
}

int httpRequest::parsePath(const std::string& path)
{
	if (path.empty() || path[0] != '/')
		return BAD_REQUEST;
	return NO_ERROR;
}

int httpRequest::parseVersion(const std::string& version)
{
	if (version != "HTTP/1.1")
		return HTTP_VERSION_NOT_SUPPORTED;
	if (version.empty())
		return BAD_REQUEST;
	return NO_ERROR;
}

int httpRequest::parseRequestLine(std::string requestline)
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

	int error[3] = {parseMethod(this->method_), parsePath(this->path_), parseVersion(this->version_)};
	for (int i = 0; i < 3; i++)
	{
		if (error[i] != NO_ERROR)
			return error[i];
	}
	return NO_ERROR;
}

int httpRequest::parseHeader(std::string header)
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
	return NO_ERROR;
}

