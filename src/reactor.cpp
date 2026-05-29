#include "reactor.hpp"

Reactor::Reactor()
{
}

Reactor::Reactor(Config const & config)
: config_(config)
{
	const std::vector<ListenInfo>	listens_info = config_.get_listens_info();

	for (int i = 0; i < listens_info.size(); i++) {

		Listen	listen(listens_info[i].address);
		listens_.push_back(listen);
	}
}

Reactor::~Reactor()
{
}

void	Reactor::Run()
{

}
