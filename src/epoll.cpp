#include "Epoll.hpp"

Epoll::Epoll()
{
}

Epoll::Epoll(Epoll const & src)
{
	*this = src;
}

Epoll::~Epoll()
{
}

Epoll &	Epoll::operator=(Epoll const & rhs)
{
	if (this != &rhs)
	{
		// Copy attributes here
	}
	return (*this);
}
