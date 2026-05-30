#ifndef REACTOR_HPP
#define REACTOR_HPP

#include "epoll.hpp"
#include "config.hpp"
#include "event_handler.hpp"
#include <set>
#include <vector>
#include "listen.hpp"

class Reactor
{
private:
	const Config			config_;

	Epoll				epoll_;
	std::vector<EventHandler*>	handlers_;

	std::set<EventHandler*>		closed_;

	void	Dispatch(int n);
	void	CloseHandlers();

	Reactor();
	Reactor(const Reactor & src);
	Reactor &	operator=(const Reactor & rhs);

public:
	Reactor(const Config & config);

	void	Run();
	void	AddEventHandler(EventHandler * ev);

	~Reactor();
};

#endif
