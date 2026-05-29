#include "Reactor.hpp"

Reactor::Reactor()
{
}

Reactor::Reactor(Config const & config)
: _config(config)
{
	const std::vector<ListenInfo>	listensInfo = _config.getListensInfo();

	for (int i = 0; i < listensInfo.size(); i++) {

		Listen	listen(listensInfo[i].address);
		_listens.push_back(listen);
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
