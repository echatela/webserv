#include "reactor.hpp"
#include "event_handler.hpp"
#include <cerrno>
#include <cstddef>
#include <exception>
#include <sys/epoll.h>

Reactor::Reactor(Config const & config)
: config_(config)
{
	const std::vector<ListenInfo>	listens_info = config_.get_listens_info();

	for (size_t i = 0; i < listens_info.size(); i++)
	{
		Listen	*listen = NULL;
		try
		{
			listen = new Listen(listens_info[i].address);
			epoll_.add(listen->get_fd(), EPOLLIN, listen);
			listens_.push_back(listen);
		}
		catch (std::exception&)
		{
			if (listen)
				epoll_.del(listen->get_fd());
			delete listen;

			for (size_t i = 0; i < listens_.size(); i++) {
				epoll_.del(listens_[i]->get_fd());
				delete listens_[i];
			}
			throw;
		}
	}
}

void	Reactor::Run()
{
	while (true)
	{
		int	n = epoll_.wait();

		if (n < 0) {
			if (errno == EINTR)
				continue;
			// ERROR handling
			break;
		}

		const struct epoll_event	*ev = epoll_.get_events();

		for (int i = 0; i < n; i++)
		{
			EventHandler	*handler
				= static_cast<EventHandler*>(ev[i].data.ptr);

			handler->HandleEvent(ev[i].events);
		}
	}
}

Reactor::~Reactor()
{
	for (size_t i = 0; i < listens_.size(); i++) {
		epoll_.del(listens_[i]->get_fd());
		delete listens_[i];
	}
}
