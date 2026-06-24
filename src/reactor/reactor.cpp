#include "reactor.hpp"
#include "event_handler.hpp"
#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstddef>
#include <exception>
#include <stdexcept>
#include <sys/epoll.h>
#include <vector>
#include <iostream>

Reactor::Reactor(std::vector<Config> const & configs)
: configs_(configs) //configs_ -> vecteur de serveur config
{
	for (size_t i = 0; i < configs_.size(); ++i) {
		const std::vector<ListenInfo> &cur_listens
			= configs_[i].listens_info();
		for (size_t j = 0; j < cur_listens.size(); ++j) {
			ListenHandler *listen = NULL;

			try {
				listen = new ListenHandler(cur_listens[j],
			epoll_, *this, configs_[i]);
				handlers_.push_back(listen);
			} catch (std::exception&) {
				delete listen;
				throw;
			}
		}
	}
}

void	Reactor::Run()
{
	signal(SIGPIPE, SIG_IGN);
	while (true) {
		int	n = epoll_.Wait(kEpollTimeoutMs);
		Dispatch(n);
		CheckTimeouts();
		CloseHandlers();
	}
}

void	Reactor::Dispatch(int n)
{
	if (n < 0) {
		if (errno == EINTR)
			throw std::runtime_error("epoll_wait() failed");
		return;
	}

	const struct epoll_event	*ev = epoll_.events();

	for (int i = 0; i < n; i++) {
		EventHandler	*handler
			= static_cast<EventHandler*>(ev[i].data.ptr);
		if (closed_.find(handler) == closed_.end())
			if (handler->HandleEvent(ev[i].events) == kClose)
				closed_.insert(handler);
	}
}

void	Reactor::CloseHandlers()
{
	std::set<EventHandler*>::iterator	cev_it;
	std::vector<EventHandler*>::iterator	ev_it;

	for (cev_it = closed_.begin(); cev_it != closed_.end(); ++cev_it) {
		EventHandler *handler = *cev_it;
		ev_it = std::find(handlers_.begin(),
		    handlers_.end(), *cev_it);
		if (ev_it != handlers_.end()) {
			handlers_.erase(ev_it);
			delete handler;
		}
	}
	closed_.clear();
}

void	Reactor::AddEventHandler(EventHandler * ev)
{
	try {
		handlers_.push_back(ev);
	} catch (std::exception &) {
		delete ev;
		throw;
	}
}

void	Reactor::CheckTimeouts()
{
	time_t	now = time(NULL);

	for (size_t i = 0; i < handlers_.size(); ++i) {
		if (closed_.find(handlers_[i]) == closed_.end())
			if (handlers_[i]->CheckTimeout(now) == kClose)
				closed_.insert(handlers_[i]);
	}
}

Reactor::~Reactor()
{
	for (size_t i = 0; i < handlers_.size(); i++)
		delete handlers_[i];
}
