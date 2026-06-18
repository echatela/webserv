#ifndef REACTOR_HPP
#define REACTOR_HPP

#include "epoll.hpp"
#include "../config/config.hpp"
#include "event_handler.hpp"
#include <set>
#include <vector>
#include "listen.hpp"

class Reactor
{
private:
	const std::vector<Config>	configs_;

	Epoll				epoll_;
	std::vector<EventHandler*>	handlers_;

	std::set<EventHandler*>		closed_;

	void	Dispatch(int n);
	void	CloseHandlers();
	void	CheckTimeouts();

	static const int	kEpollTimeoutMs =  5000;

	Reactor();
	Reactor(const Reactor & src);
	Reactor &	operator=(const Reactor & rhs);

public:
	Reactor(const std::vector<Config> & configs);

	void	Run();
	void	AddEventHandler(EventHandler * ev);

	~Reactor();
};

#endif
