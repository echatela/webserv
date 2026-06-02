#ifndef EVENT_HANDLER_HPP
#define EVENT_HANDLER_HPP

#include <stdint.h>

enum	HandleReturn { kKeep, kClose };

class EventHandler
{
private:
	EventHandler(EventHandler const & src);
	EventHandler &	operator=(EventHandler const & rhs);

public:
	EventHandler();

	virtual int	HandleEvent(uint32_t events) = 0;

	virtual ~EventHandler();
};

#endif
