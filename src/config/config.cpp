#include "config.hpp"
#include <cstring>
#include "config_parser.hpp"

Config::Config(ConfigParser& parser)
{
	servers_info_ = parser.Parse();
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
		servers_info_ = rhs.servers_info_;
	}
	return (*this);
}


std::vector<ListenInfo> Config::get_listens_info() const
{
	std::vector<ListenInfo> listens_info;

	for (size_t i = 0; i < servers_info_.size(); i++)
	{
		listens_info.push_back(servers_info_[i].listen_info);
	}
	return listens_info;
}


const std::vector<ServerConfig> & Config::get_servers_info() const {
	return servers_info_;
}
