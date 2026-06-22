#ifndef LISTEN_HPP
#define LISTEN_HPP

#include "../config/config.hpp"
#include "epoll.hpp"
#include "event_handler.hpp"
#include <stdint.h>
#include <netinet/in.h>

class Reactor;

class ListenHandler : public EventHandler
{
private:
	int			fd_;
	Epoll &			epoll_;
	Reactor &		reactor_;
	const Config &		config_;
	const ListenInfo &	listen_info_;

	ListenHandler();
	ListenHandler(const ListenHandler & src);
	ListenHandler &		operator=(const ListenHandler & rhs);

public:
	ListenHandler(const ListenInfo & info, Epoll & epoll, Reactor & reactor,
	const Config & config);

	int	HandleEvent(uint32_t events);

	int			get_fd() const;
	Reactor &		get_reactor() const;
	const Config &		get_config() const;
	const ListenInfo &	get_listen_info() const;


	~ListenHandler();
};

#endif

// COMMENTAIRE
