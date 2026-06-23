#pragma once

#include <string>
#include <vector>
#include <netinet/in.h>

namespace webserv {

namespace utils {
std::string			IntToStr(int value);
std::vector<char *>	ToCStrArray(std::vector<std::string> & v);
std::string 		AddrToString(const struct sockaddr_in& addr);
std::string		Basename(const std::string & path);
std::string		Basedir(const std::string & path);
	unsigned int	ParseUInt(std::string value);
}
namespace fd {
int	SetNonBlock(int fd);
int	SetCloExec(int fd);
}
}
