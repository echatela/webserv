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

std::string webserv::utils::AddrToString(const struct sockaddr_in& addr)
{
	const unsigned char* b =
		reinterpret_cast<const unsigned char*>(&addr.sin_addr.s_addr);
	std::ostringstream	oss;
	oss << static_cast<int>(b[0]) << '.'
		<< static_cast<int>(b[1]) << '.'
		<< static_cast<int>(b[2]) << '.'
		<< static_cast<int>(b[3]);
	return oss.str();
}

std::vector<char *>	webserv::utils::ToCStrArray(
	std::vector<std::string> & v)
{
	std::vector<char *>	out;

	for (size_t i = 0; i < v.size(); i++)
		out.push_back(const_cast<char *>(v[i].c_str()));
	out.push_back(NULL);
	return out;
}

std::string	webserv::utils::Basename(const std::string & path)
{
	std::string::size_type	pos = path.find_last_of('/');
	if (pos == std::string::npos)
		return path;
	return path.substr(pos + 1);
}

std::string	webserv::utils::Basedir(const std::string & path)
{
	std::string::size_type	pos = path.find_last_of('/');
	if (pos == std::string::npos)
		return "./";
	return path.substr(0, pos);
}
