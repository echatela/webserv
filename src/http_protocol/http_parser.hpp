#pragma once

#include <cstddef>
#include <string>

class HttpRequest;

enum ParseError {
	NO_ERROR,
	BAD_REQUEST = 400,
	METHOD_NOT_ALLOWED = 405,
	HTTP_VERSION_NOT_SUPPORTED = 505,
	BAD_HEADER,
	OTHER_ERROR = 999,
};

class HttpParser
{
public:
	HttpParser();
	~HttpParser();

	int	Add(const char* buf, size_t n);
	int	ParseRequest(HttpRequest& req) const;
	bool	ScanChunks(size_t body_start);

	int			flag() const;
	const std::string&	buf() const;
	bool			bad_request() const;

private :
	std::string	buf_;
	size_t		content_length_;
	bool		headers_parsed_;
	bool		chunked_;
	bool		bad_request_;
	size_t		scan_pos_;
	std::string	decoded_body_;
	size_t		buf_size_;
	size_t		buf_size_without_body_;
	size_t		buf_size_with_body_;
	int		flag_;
	bool		has_body_;

	HttpParser(const HttpParser& other);
	HttpParser& operator=(const HttpParser& other);
};
