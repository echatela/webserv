#include "reactor.hpp"

Reactor::Reactor()
{
}

Reactor::Reactor(Config const & config)
: config_(config)
{
	const std::vector<ListenInfo>	listensInfo = config_.getListensInfo();

	for (int i = 0; i < listensInfo.size(); i++) {

		Listen	listen(listensInfo[i].address);
		listens_.push_back(listen);
	}
}

Reactor::Reactor(Reactor const & src)
{
	*this = src;
}

Reactor::~Reactor()
{
}

Reactor &	Reactor::operator=(Reactor const & rhs)
{
	if (this != &rhs)
	{
		// Copy attributes here
	}
	return (*this);
}

void	Reactor::Run()
{

}
