#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "event_handler.hpp"
#include <cstdint>

class Connection : public EventHandler
{
private:
	int	fd_;

	Connection(Connection const & src);
	Connection &	operator=(Connection const & rhs);

public:
	Connection();

	int	HandleEvent(uint32_t events);

	~Connection();
};

#endif
