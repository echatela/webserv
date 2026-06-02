#include "config.hpp"
#include <cstring>

Config::Config()
{
	// juste pour tester, faudra faire un vrai constructeur apres
	ListenInfo	listen_info;
	memset(&listen_info.address, 0, sizeof(listen_info.address));
	listen_info.address.sin_family = AF_INET;
	listen_info.address.sin_port = htons(8080);
	listen_info.address.sin_addr.s_addr = htonl(INADDR_ANY);

	listens_info_.push_back(listen_info);
}

Config::Config(Config const & src)
{
	*this = src;
}

Config::~Config()
{
}

Config &	Config::operator=(const Config & rhs)
{
	if (this != &rhs)
	{
		listens_info_ = rhs.listens_info_;
	}
	return (*this);
}


const std::vector<ListenInfo> &	Config::get_listens_info() const
{
	return listens_info_;
}
