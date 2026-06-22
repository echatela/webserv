#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "event_handler.hpp"
#include <stdint.h>
#include <sys/types.h>
#include <ctime>
#include <string>

class ConnHandler;
class Reactor;

class CgiHandler : public EventHandler
{
	pid_t		pid_;
	int		stdout_fd_;
	ConnHandler*	connection_;
	std::string	output_buf_;

	static const int	kTimeoutSecs = 10;
	time_t		start_time_;

public:
	CgiHandler(pid_t pid, int stdout_fd_,
	    ConnHandler& conn, Reactor& reactor);

	int	HandleEvent(uint32_t events);
	int	CheckTimeout(time_t now);
	void	Detach();

	~CgiHandler();
};

#endif
