#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "epoll.hpp"
#include "event_handler.hpp"
#include "http_protocol/http_request.hpp"
#include "http_protocol/http_response.hpp"
#include "http_protocol/router.hpp"
#include "config/config_parser.hpp"
#include "listen.hpp"
#include <stdint.h>
#include <vector>

enum {
	kReading,
	kWriting
};

class Connection : public EventHandler
{
private:
	int			fd_;
	int			state_;

	std::vector<char>	write_buf_;
	size_t				write_off_;

	Epoll &				epoll_;
	const Listen &		listen_;
	HttpParser			parser_;
	HttpRequest			request_;
	Router				router_;
	HttpResponse		response_;

	Connection();
	Connection(Connection const & src);
	
	Connection &	operator=(Connection const & rhs);

	static const int	kReadBufferSize = 4096;

public:
	Connection(int fd, const Listen & listen, Epoll & epoll);

	int	HandleEvent(uint32_t events);

	int	get_fd() const;

	~Connection();
};

#endif
