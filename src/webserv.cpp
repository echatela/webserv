#include "webserv.hpp"
#include <fcntl.h>
#include <sstream>
#include <limits.h>
#include <stdlib.h>
#include <cerrno>

int	webserv::fd::SetNonBlock(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int	webserv::fd::SetCloExec(int fd)
{
	int flags = fcntl(fd, F_GETFD, 0);
	if (flags == -1)
		return -1;
	return fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
}

std::string		webserv::utils::IntToStr(int value)
{
	std::ostringstream s;
	s << value;
	return (s.str());
}

unsigned int	webserv::utils::ParseUInt(std::string value)
{
	char*	end;
	long	val = std::strtol(value.c_str(), &end, 10);

	if (errno == ERANGE || val < 0 || val > INT_MAX)
		throw std::logic_error("Int value overflows: " + value);
		
	return (static_cast<unsigned int>(val));
}
