#include "http_parser.hpp"

int httpParser::add(const std::string buf, size_t n)
{
	if (!buf_.empty())
		buf_ += buf;
	else
		buf_ = buf;
	if (buf.size() >= 4 && buf.compare(buf.size() - 4, 4, "\r\n\r\n") == 0)
		flag_ = true;
	else
		flag_ = false;
}

httpParser::~httpParser() { }

const int httpParser::getFlag() const
{
	return flag_;
}

const std::string& httpParser::getBuf() const
{
	return buf_;
}

