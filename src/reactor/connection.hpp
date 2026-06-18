#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "epoll.hpp"
#include "event_handler.hpp"
#include "../http_protocol/http_request.hpp"
#include "../http_protocol/http_response.hpp"
#include "../http_protocol/router.hpp"
#include "listen.hpp"
#include <ctime>
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
	size_t			write_off_;
	time_t			last_activity_;

	Epoll &			epoll_;
	const Listen &		listen_;

	HttpParser		parser_;
	HttpRequest		request_;
	Router			router_;
	HttpResponse		response_;

	Connection();
	Connection(Connection const & src);
	
	Connection &	operator=(Connection const & rhs);

	void		HandleRequest();

	static const int	kReadBufferSize = 4096;
	static const int	kTimeoutSecs = 60;

public:
	Connection(int fd, const Listen & listen, Epoll & epoll);

	int	HandleEvent(uint32_t events);
	int	CheckTimeout(time_t now);

	int	get_fd() const;

	~Connection();
};

#endif
