#include "webserv.hpp"
#include <fcntl.h>
#include <sstream>

int	webserv::fd::SetNonBlock(int fd)
{
	int flags = fcntl(fd, F_get_fL, 0);
	if (flags == -1)
		return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int	webserv::fd::SetCloExec(int fd)
{
	int flags = fcntl(fd, F_get_fD, 0);
	if (flags == -1)
		return -1;
	return fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
}

std::string	webserv::utils::IntToStr(int value)
{
	std::ostringstream s;
	s << value;
	return (s.str());
}
