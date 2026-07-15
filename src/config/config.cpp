#include "config.hpp"
#include <cstring>
#include <stdexcept>

Config::Config()
: max_body_size_(1048576) {
}

Config::Config(Config const & src) {
	*this = src;
}

Config::~Config()
{
}

Config &	Config::operator=(const Config & rhs) {

	if (this != &rhs) {
		listens_info_ = rhs.listens_info_;
		root_ = rhs.root_;
		locations_ = rhs.locations_;
		error_pages_ = rhs.error_pages_;
		server_names_ = rhs.server_names_;
		max_body_size_ = rhs.max_body_size_;
	}
	return (*this);
}

void	Config::add_listen_info(ListenInfo listen) {
	for (size_t i = 0; i < listens_info_.size(); ++i) {
		if (listens_info_[i].host == listen.host
			&& listens_info_[i].port == listen.port)
			throw std::logic_error("duplicate listen: "
				+ listen.host + ":" + listen.port);
	}
	listens_info_.push_back(listen);
}

void	Config::set_root(std::string root) {
	root_ = root;
}

void	Config::add_location(std::pair<std::string, LocationConfig> location) {
	locations_.insert(location);
}

void	Config::add_error_page(std::pair<int, std::string> page) {
	error_pages_.insert(page);
}

void	Config::set_max_body_size(size_t size) {
	max_body_size_ = size;
}

const std::map<int, std::string>&	Config::error_pages() const {
	return error_pages_;
}

const std::string&		Config::root() const {
	return root_;
}
size_t				Config::max_body_size() const {
	return max_body_size_;
}
std::map<std::string, LocationConfig>	Config::locations() const {
	return locations_;
}

const std::vector<ListenInfo> &Config::listens_info() const {
	return listens_info_;
}

std::vector<ListenInfo> Config::listens_info(std::vector<Config> configs) {
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
void	Config::set_server_names(std::vector<std::string> names) {
	server_names_ = names;
}

const std::vector<std::string> &	Config::server_names() const {
	return server_names_;
}
