#include "event_handler.hpp"

EventHandler::EventHandler()
{
}

int	EventHandler::CheckTimeout(time_t now)
{
	(void)now;
	return kKeep;
}

EventHandler::~EventHandler()
{
}
