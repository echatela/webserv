#pragma once

#include <cstddef>
#include <string>

class HttpRequest;

enum ParseError {
	NO_ERROR = 0,
	BAD_REQUEST = 400,
	NOT_IMPLEMENTED = 501,
	HTTP_VERSION_NOT_SUPPORTED = 505
};

class HttpParser {

public:
	HttpParser();
	~HttpParser();

	int	Add(const char* buf, size_t n);
	int	ParseRequest(HttpRequest& req) const;
	bool	ScanChunks(size_t body_start);

	int			flag() const;
	const std::string&	buf() const;
	bool			bad_request() const;
	void			set_max_body_size(size_t size);
	bool			too_large() const;

	static const size_t	kMaxHeaderSize = 8192;

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
	size_t		max_body_size_;
	bool		too_large_;

	HttpParser(const HttpParser& other);
	HttpParser& operator=(const HttpParser& other);
};
