#ifndef CGI_IN_HANDLER_HPP
#define CGI_IN_HANDLER_HPP

#include "event_handler.hpp"
#include <cstddef>
#include <string>
#include <vector>

class Epoll;

class CgiInHandler : public EventHandler
{
	int			stdin_fd_;
	Epoll &			epoll_;
	std::vector<char>	write_buf_;
	size_t			write_off_;

	CgiInHandler();
	CgiInHandler(const CgiInHandler & src);
	CgiInHandler &	operator=(const CgiInHandler & rhs);

public:
	CgiInHandler(int stdin_fd, Epoll & epoll,
	      const std::string & body);

	int HandleEvent(uint32_t events);

	~CgiInHandler();
};

#endif
