#pragma once

#include <iostream>

namespace webserv {

namespace utils {
	std::string	IntToStr(int value);
}
namespace fd {
int	SetNonBlock(int fd);
int	SetCloExec(int fd);
}
}
