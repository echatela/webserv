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

int httpParser::getFlag() const
{
	return flag_;
}

std::string httpParser::getBuf() const
{
	return buf_;
}

