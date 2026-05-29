#pragma once

namespace webserv {

namespace fd {
int	SetNonBlock(int fd);
int	SetCloExec(int fd);
}
}
