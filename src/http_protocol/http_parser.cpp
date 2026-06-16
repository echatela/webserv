#include "http_parser.hpp"
#include "http_request.hpp"

HttpParser::HttpParser() : 
buf_(), buf_size_(0), buf_size_without_body_(0), buf_size_with_body_(0), flag_(0), content_length_(0) { }

HttpParser::~HttpParser() { }

int HttpParser::add(const std::string buf, size_t n)
{
	if (n == 0)
		return flag_;
	buf_size_ += n;
	if (!buf_.empty())
		buf_ += buf;
	else
		buf_ = buf;
	if (n >= 4 && buf.compare(n - 4, 4, "\r\n\r\n") == 0)
	{
		size_t startCL = buf.find("Content-Lenght");
		if (startCL == std::string::npos)
			flag_ = true;
		else
		{
			size_t endCL = this->buf_.find("\r\n");
			std::string content_length_str = this->buf_.substr(startCL + 15, endCL - startCL - 15);
			content_length_ = std::stoi(content_length_str);
			if (content_length_ == 0)
				flag_ = true;
			else
				flag_ = false;
		}
	}
	else if (content_length_ != 0)
	{
		buf_size_with_body_ = this->buf_.find("\r\n\r\n");
		buf_size_without_body_ = buf_size_with_body_ + 4 + content_length_;
		if (buf_size_ >= buf_size_without_body_)
			flag_ = true;
		else
			flag_ = false;
	}
	else
		flag_ = false;
	return flag_;
}

int HttpParser::get_flag() const
{
	return flag_;
}

std::string HttpParser::get_buf() const
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

