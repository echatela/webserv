#include "http_parser.hpp"
#include "http_request.hpp"
#include <stdlib.h>

HttpParser::HttpParser()
: buf_(), buf_size_(0), buf_size_without_body_(0), buf_size_with_body_(0),
	flag_(0), content_length_(0), has_body_(false) { }

HttpParser::~HttpParser() { }

int HttpParser::Add(const char* buf, size_t n)
{
	if (n == 0)
		return flag_;
	buf_size_ += n;
	buf_.append(buf, n);

	size_t header_end = buf_.find("\r\n\r\n");
	if (header_end == std::string::npos)
	{
		flag_ = false;
		return flag_;
	}

	if (content_length_ == 0)
	{
		size_t startCL = this->buf_.find("Content-Length:");
		if (startCL == std::string::npos)
			flag_ = true;
		else
		{
			size_t value_start = startCL + 15;
			while (value_start < this->buf_.size() && (this->buf_[value_start] == ' ' || this->buf_[value_start] == '\t'))
				++value_start;
			size_t endCL = this->buf_.find("\r\n", value_start);
			std::string content_length_str = this->buf_.substr(value_start, endCL - value_start);
			content_length_ = atoi(content_length_str.c_str());
		}
	}
	if (content_length_ == 0)
		flag_ = true;
	else
	{
		buf_size_with_body_ = header_end;
		buf_size_without_body_ = buf_size_with_body_ + 4 + content_length_;
		if (buf_size_ >= buf_size_without_body_)
			flag_ = true;
		else
			flag_ = false;
	}
	return flag_;
}

int HttpParser::flag() const
{
	return flag_;
}

std::string HttpParser::buf() const
{
	return buf_;
}

int HttpParser::ParseRequest(HttpRequest & req) const
{
	int error = 0;

	size_t pos = this->buf_.find("\r\n");
	std::string requestLine = this->buf_.substr(0, pos);
	error = req.ParseRequestLine(requestLine);

	if (error != NO_ERROR)
		return error;

	size_t startHeader = pos + 2;
	size_t endHeader = this->buf_.find("\r\n\r\n");
	if (endHeader == std::string::npos)
		return OTHER_ERROR;
	std::string header = this->buf_.substr(startHeader, endHeader - startHeader);
	error = req.ParseHeader(header);

	if (error != NO_ERROR)
		return error;

	size_t startBody = endHeader + 4;
	std::string body = this->buf_.substr(startBody);
	error = req.ParseBody(body);
	req.set_error(error);
	return error;
}

