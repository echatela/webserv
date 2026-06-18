#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "event_handler.hpp"
#include "../http_protocol/http_request.hpp"
#include "../http_protocol/router.hpp"
#include <ctime>
#include <stdint.h>
#include <vector>

class CgiHandler;
class Listen;
class Epoll;

enum { kReading, kWriting, kCgi };

class Connection : public EventHandler
{
	int			fd_;
	int			state_;

	std::vector<char>	write_buf_;
	size_t			write_off_;
	time_t			last_activity_;

	Epoll &			epoll_;
	const Listen &		listen_;
	CgiHandler*		cgi_;

	HttpParser		parser_;
	HttpRequest		request_;
	Router			router_;

	Connection();
	Connection(Connection const & src);
	
	Connection &	operator=(const Connection & rhs);

	void		HandleRequest();
	void		StartCgi(const CgiPlan & plan);

	static const int	kReadBufferSize = 4096;
	static const int	kTimeoutSecs = 60;

public:
	Connection(int fd, const Listen & listen, Epoll & epoll);

	int	HandleEvent(uint32_t events);
	int	CheckTimeout(time_t now);
	void	OnCgiDone(const std::string& output);

	int	get_fd() const;

	~Connection();
};

#endif
