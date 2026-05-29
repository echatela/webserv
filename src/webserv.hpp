#pragma once

namespace webserv {

namespace fd {
int	setNonblock(int fd);
int	setCloexec(int fd);
}
}
