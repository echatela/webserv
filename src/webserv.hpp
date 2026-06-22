#pragma once

#include <string>
#include <vector>
#include <netinet/in.h>

namespace webserv {

namespace utils {
std::string		IntToStr(int value);
std::vector<char *>	ToCStrArray(std::vector<std::string> & v);
std::string 		AddrToString(const struct sockaddr_in& addr);
}
namespace fd {
int	SetNonBlock(int fd);
int	SetCloExec(int fd);
}
}
