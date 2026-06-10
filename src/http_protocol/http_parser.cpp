#include "http_parser.hpp"

httpParser::httpParser() : buf_(), flag_(0) { }

httpParser::~httpParser() { }

int httpParser::add(const std::string buf, size_t n)
{
	if (n == 0)
		return flag_;
	if (!buf_.empty())
		buf_ += buf;
	else
		buf_ = buf;
	if (buf.size() >= 4 && buf.compare(buf.size() - 4, 4, "\r\n\r\n") == 0)
		flag_ = true;
	else
		flag_ = false;
	return flag_;
}

// int httpParser::getFlag() const
// {
// 	return flag_;
// }

// std::string httpParser::getBuf() const
// {
// 	return buf_;
// }

int httpParser::parseRequest(httpRequest & req) const
{
	size_t pos = this->buf_.find("\r\n");
	std::string requestLine = this->buf_.substr(0, pos);
	req.parseRequestLine(requestLine);
	size_t startHeader = pos +2;
	size_t endHeader = this->buf_.find("\r\n\r\n");
	if (endHeader == std::string::npos)
		return OTHER_ERROR;
	std::string header = this->buf_.substr(startHeader, endHeader - startHeader);
	req.parseHeader(header);
	// std::string body = httpParser.get
	// parseBody(httpParser, req);
}
