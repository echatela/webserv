#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "event_handler.hpp"
#include <stdint.h>
#include <sys/types.h>
#include <ctime>
#include <string>

class ConnHandler;
class Epoll;

class CgiHandler : public EventHandler
{
private:
	pid_t		pid_;
	int		stdout_fd_;
	ConnHandler*	conn_;
	Epoll &		epoll_;
	std::string	output_buf_;

	static const int	kTimeoutSecs = 10;
	time_t		start_time_;

	static const int	kReadBufferSize = 4096;

	CgiHandler();
	CgiHandler(const CgiHandler & src);
	CgiHandler &	operator=(const CgiHandler & rhs);

public:
	CgiHandler(pid_t pid, int stdout_fd, ConnHandler& conn,
	    Epoll & reactor);

	int	HandleEvent(uint32_t events);
	int	CheckTimeout(time_t now);
	void	Detach();

	~CgiHandler();
};

#endif
