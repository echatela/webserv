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

void httpRequest::setMethod(const std::string& method)
{
	method_ = method;
}

void httpRequest::setPath(const std::string& path)
{
	path_ = path;
}

void httpRequest::setVersion(const std::string& version)
{
	version_ = version;
}

void httpRequest::setHeader(const std::map<std::string, std::string>& header)
{
	header_ = header;
}

void httpRequest::setBody(const std::string& body)
{
	body_ = body;
}

const std::string httpRequest::getMethod() const
{
	return method_;
}

const std::string httpRequest::getPath() const
{
	return path_;
}

const std::string httpRequest::getVersion() const
{
	return version_;
}

const std::map<std::string, std::string> httpRequest::getHeader() const
{
	return header_;
}

const std::string httpRequest::getBody() const
{
	return body_;
}