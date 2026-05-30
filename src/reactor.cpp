#include "reactor.hpp"
#include "connection.hpp"
#include "event_handler.hpp"
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

	for (size_t i = 0; i < listens_info.size(); i++) {
		Listen	*listen = NULL;

		try {
			listen = new Listen(
				listens_info[i].address, epoll_, *this);
			listens_.push_back(listen);
		} catch (std::exception&) {
			delete listen;
			for (size_t i = 0; i < listens_.size(); i++)
				delete listens_[i];
			throw;
		}
	}
}

void	Reactor::Run()
{
	while (true) {
		int	n = epoll_.Wait();

		Dispatch(n);

		CleanupCloseHandler();
	}
}

void	Reactor::Dispatch(int n)
{
	if (n < 0) {
		if (errno == EINTR)
		// ERROR handling
			;
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

void	Reactor::CloseHandler()
{
	std::set<EventHandler*>::iterator	evh_it;
	std::vector<Connection*>::iterator	con_it;

	for (evh_it = closed_.begin(); evh_it != closed_.end(); ++evh_it) {
		con_it = std::find(
			connections_.begin(), connections_.end(), *evh_it);
		connections_.erase(con_it);
		delete *evh_it;
	}
	closed_.clear();
}

void	Reactor::AddConnection(int fd, const Listen & listen)
{
	Connection	*conn = NULL;

	try {
		conn = new Connection(fd, listen, epoll_);
		connections_.push_back(conn);
	} catch (std::exception &) {
		delete conn;
		throw;
	}
}

Reactor::~Reactor()
{
	for (size_t i = 0; i < listens_.size(); i++)
		delete listens_[i];
	for (size_t i = 0; i < connections_.size(); i++)
		delete connections_[i];
}
