#include "Listener.hpp"

Listener::Listener()
{
}

Listener::Listener(Listener const & src)
{
	*this = src;
}

Listener::~Listener()
{
}

Listener &	Listener::operator=(Listener const & rhs)
{
	if (this != &rhs)
	{
		// Copy attributes here
	}
	return (*this);
}
