#include "connection.hpp"
#include <cstdint>
#include <unistd.h>

Connection::Connection()
{
}

int	Connection::HandleEvent(uint32_t events)
{

}

Connection::~Connection()
{
	close(fd_);
}
