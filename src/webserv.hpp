#pragma once

#include <iostream>

namespace webserv {

namespace utils {
	std::string		IntToStr(int value);
	unsigned int	ParseUInt(std::string value);
}
namespace fd {
int	SetNonBlock(int fd);
int	SetCloExec(int fd);
}
}
