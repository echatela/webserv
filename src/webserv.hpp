#pragma once

#include <string>
#include <vector>

namespace webserv {

namespace utils {
std::string		IntToStr(int value);
std::vector<char *>	ToCStrArray(std::vector<std::string> & v);
}
namespace fd {
int	SetNonBlock(int fd);
int	SetCloExec(int fd);
}
}
