#include "webserv.hpp"
#include <cstddef>
#include <fcntl.h>
#include <sstream>
#include <vector>

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

std::string	webserv::utils::IntToStr(int value)
{
	std::ostringstream s;
	s << value;
	return (s.str());
}

std::vector<char *>	webserv::utils::ToCstrArray(
	std::vector<std::string> & v)
{
	std::vector<char *>	out;

	for (size_t i = 0; i < v.size(); i++)
		out.push_back(const_cast<char *>(v[i].c_str()));
	out.push_back(NULL);
	return out;
}
