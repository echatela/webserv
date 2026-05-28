#ifndef LISTEN_HPP
#define LISTEN_HPP

#include <netinet/in.h>
class Listen
{
private:
	int	_fd;

public:
	Listen(sockaddr_in addr);
	Listen(const Listen & src);
	~Listen();

	Listen &	operator=(const Listen & rhs);
};

#endif
