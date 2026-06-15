#include "reactor.hpp"
#include "event_handler.hpp"
#include "http_protocol/http_request.hpp"
#include "http_protocol/http_response.hpp"
#include "http_protocol/router.hpp"
#include "config/config_parser.hpp"
#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <exception>
#include <sys/epoll.h>
#include <vector>

Reactor::Reactor(Config const & config)
: config_(config)
{
	const std::vector<ListenInfo> &	listens_info = config_.get_listens_info();
	const std::vector<ServerConfig> & servers_info = config_.get_servers_info();
	

	for (size_t i = 0; i < listens_info.size(); i++) {
		Listen	*listen = NULL;

		try {
			listen = new Listen(
				listens_info[i].address, epoll_, *this, servers_info[i]);
			handlers_.push_back(listen);
		} catch (std::exception&) {
			delete listen;
			throw;
		}
	}
}

void	Reactor::Run()
{
	while (true) {
		int	n = epoll_.Wait();

		Dispatch(n);

		CloseHandlers();
	}
}

void	Reactor::Dispatch(int n)
{
	if (n < 0) {
		if (errno == EINTR) {}
		// ERROR handling
	}

	const struct epoll_event	*ev = epoll_.get_events();

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
		ev_it = std::find(handlers_.begin(),
		    handlers_.end(), *cev_it);
		handlers_.erase(ev_it);
		// delete *ev_it;
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

Reactor::~Reactor()
{
	for (size_t i = 0; i < handlers_.size(); i++)
		delete handlers_[i];
}
