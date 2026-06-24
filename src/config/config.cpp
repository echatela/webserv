#include "config.hpp"
#include <cstring>

Config::Config()
{
	// servers_info_ = parser.Parse();
}

Config::Config(Config const & src)
{
	*this = src;
}

Config::~Config()
{
}

const Config &	Config::operator=(const Config & rhs)
{
	if (this != &rhs)
	{
		listens_info_ = rhs.listens_info_;
		root_ = rhs.root_;
		locations_ = rhs.locations_;
	}
	return (*this);
}



void				Config::add_listen_info(ListenInfo listen) {
	listens_info_.push_back(listen);
}

void				Config::set_root(std::string root) {
	root_ = root;
}


void				Config::add_location(std::pair<std::string, LocationConfig> location) {
	locations_.insert(location);
}

void				Config::set_max_body_size(int size) {
	max_body_size_ = size;
}

// const std::vector<ServerConfig> & Config::servers_info() const {
	// 	return servers_info_;
	// }
	
std::string				Config::root() {
	return (root_);
}

std::map<std::string, LocationConfig>	Config::get_locations() const {
	return (locations_);
}

const std::vector<ListenInfo> &Config::listens_info() const
{
	return listens_info_;
}

std::vector<ListenInfo> Config::listens_info(std::vector<Config> configs)
{
	std::vector<ListenInfo> listens;
	for (size_t i = 0; i < configs.size(); i++)
	{
		std::vector<ListenInfo> cur_listens = configs[i].listens_info_;
		for (size_t j = 0; j < cur_listens.size(); j++)
		{
			listens.push_back(cur_listens[j]);
		}
	}
	return listens;
}

const std::string &	Config::server_name() const { return server_name_; }
