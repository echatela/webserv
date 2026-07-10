#include "http_parser.hpp"
#include "http_request.hpp"
#include <cctype>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <stdlib.h>
#include <string>

HttpParser::HttpParser()
	: buf_(), content_length_(0), headers_parsed_(false), chunked_(false)
	, bad_request_(false), scan_pos_(0), decoded_body_()
	, buf_size_(0), buf_size_without_body_(0), buf_size_with_body_(0)
	, flag_(0),  has_body_(false) {
}

HttpParser::~HttpParser() {
}

static std::string	FindHeader(const std::string& buf, size_t header_end,
	const std::string& name) {

	std::string	hay = buf.substr(0, header_end);
	std::string	low = hay;

	for (size_t i = 0; i < low.size(); i++)
		low[i] = std::tolower(low[i]);

	size_t	pos = low.find("\r\n" + name + ":");
	if (pos == std::string::npos)
		return "";
	pos += 2 + name.size() + 1;

	size_t	eol = hay.find("\r\n", pos);
	if (eol == std::string::npos)
		eol = hay.size();
	std::string	value = hay.substr(pos, eol - pos);

	size_t	b = value.find_first_not_of(" \t");
	size_t	e = value.find_last_not_of(" \t");
	if (b == std::string::npos)
		return "";
	return value.substr(b, e - b + 1);
}

int HttpParser::Add(const char* buf, size_t n) {

	if (n == 0)
		return flag_;
	buf_size_ += n;
	buf_.append(buf, n);

	size_t header_end = buf_.find("\r\n\r\n");
	if (header_end == std::string::npos)
		return (flag_ = false);

	if (!headers_parsed_) {

		std::string	te
			= FindHeader(buf_, header_end, "transfer-encoding");
		for (size_t i = 0; i < te.size(); i++)
			te[i] = std::tolower(te[i]);
		chunked_ = (te.find("chunked") != std::string::npos);

		if (!chunked_) {
			
			std::string	cl = FindHeader(
				buf_, header_end, "content-length");
			if (cl.empty())
				content_length_ = 0;
			else {
				char*	end;
				errno = 0;
				unsigned long	v
					= std::strtoul(cl.c_str(), &end, 10);
				if (*end != '\0' || errno == ERANGE)
					bad_request_ = true;
				content_length_ = static_cast<size_t>(v);
			}
		}
		headers_parsed_ = true;
	}

	if (bad_request_)
		return (flag_ = true);

	if (chunked_)
		flag_ = ScanChunks(header_end + 4);
	else
		flag_ = (buf_size_ >= header_end + 4 + content_length_);

	return flag_;
}

int HttpParser::ParseRequest(HttpRequest & req) const
{
	int error;

	size_t pos = buf_.find("\r\n");
	error = req.ParseRequestLine(buf_.substr(0, pos));
	if (error != NO_ERROR) {
		req.set_error(error);
		return error;
	}

	size_t end_header = buf_.find("\r\n\r\n");
	if (end_header == std::string::npos) {
		req.set_error(BAD_REQUEST);
		return BAD_REQUEST;
	}
	error = req.ParseHeader(buf_.substr(pos + 2, end_header - pos - 2));
	if (error != NO_ERROR) {
		req.set_error(error);
		return error;
	}

	error = req.ParseBody(chunked_
		? decoded_body_
		: buf_.substr(end_header + 4, content_length_));
	req.set_error(error);
	return error;
}

bool	HttpParser::ScanChunks(size_t body_start) {

	if (scan_pos_ == 0)
		scan_pos_ = body_start;

	while (true) {
		size_t	line_end = buf_.find("\r\n", scan_pos_);
		if (line_end == std::string::npos)
			return false;

		std::string	size_line
			= buf_.substr(scan_pos_, line_end - scan_pos_);
		size_t		semi = size_line.find(";");
		if (semi != std::string::npos)
			size_line = size_line.substr(0, semi);

		char*		end;
		errno = 0;
		unsigned long	size = std::strtoul(size_line.c_str(), &end, 16);
		if (end == size_line.c_str() || errno == ERANGE) {
			bad_request_ = true;
			return false;
		}

		size_t	data_start = line_end + 2;

		if (size == 0)
			return (buf_.find("\r\n\r\n", line_end)
				!= std::string::npos);

		if (buf_.size() < data_start + size + 2)
			return false;

		decoded_body_.append(buf_, data_start, size);
		scan_pos_ = data_start + size + 2;
	}
}

int			HttpParser::flag() const { return flag_; }
const std::string&	HttpParser::buf() const { return buf_; }
bool			HttpParser::bad_request() const { return bad_request_; }
