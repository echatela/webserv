#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "event_handler.hpp"
#include "../http_protocol/http_request.hpp"
#include "../http_protocol/router.hpp"
#include <ctime>
#include <stdint.h>
#include <vector>

class Reactor;
class CgiHandler;
class ListenHandler;
class Epoll;

enum { kReading, kWriting, kCgi };

class ConnHandler : public EventHandler
{
	int			fd_;
	int			state_;
	sockaddr_in		addr_;

	std::vector<char>	write_buf_;
	size_t			write_off_;
	time_t			last_activity_;

	Epoll &			epoll_;
	Reactor &		reactor_;
	const ListenHandler &	listen_;
	CgiHandler*		cgi_;

	HttpParser		parser_;
	HttpRequest		request_;
	Router			router_;

	ConnHandler();
	ConnHandler(ConnHandler const & src);
	
	ConnHandler &	operator=(const ConnHandler & rhs);

	void				HandleRequest();
	void				StartCgi(const CgiPlan & plan);
	std::vector<std::string>	BuildCgiEnv(const CgiPlan & plan) const;

	static const int	kReadBufferSize = 4096;
	static const int	kTimeoutSecs = 60;

public:
	ConnHandler(sockaddr_in addr ,int fd, const ListenHandler & listen,
	     Epoll & epoll, Reactor & reactor);

	int	HandleEvent(uint32_t events);
	int	CheckTimeout(time_t now);
	void	OnCgiDone(const std::string& output);

	int	fd() const;

	~ConnHandler();
};

#endif
