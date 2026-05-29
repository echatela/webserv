#include "Connection.hpp"

Connection::Connection()
{
}

Connection::Connection(Connection const & src)
{
	*this = src;
}

Connection::~Connection()
{
}

Connection &	Connection::operator=(Connection const & rhs)
{
	if (this != &rhs)
	{
		// Copy attributes here
	}
	return (*this);
}
