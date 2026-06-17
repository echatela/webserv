#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <netinet/in.h>
#include <string>
#include <vector>
#include <map>
#include <sys/socket.h>

class ConfigParser;

struct ListenInfo {
	std::string			host;
	std::string			port;
	struct sockaddr_in	address;
};

struct LocationConfig {
	std::string base_location;
	std::string	root;
};

struct	ServerConfig {
};

class Config
{
private:
	std::vector<ListenInfo>					listens_info_;
	std::string								root_;
	std::map<std::string, LocationConfig>	locations_;
	unsigned int							max_body_size_;
	
	// std::vector<ServerConfig>	servers_info_;

public:
	Config();
	Config(const Config & src);
	~Config();

	const Config &	operator=(const Config & rhs);

	void								add_listen_info(ListenInfo listen);
	void								set_root(std::string root);
	void								add_location(std::pair<std::string, LocationConfig> location);
	void								set_max_body_size(int size);

	std::string							get_root();	
	const std::vector<ListenInfo> &					get_listens_info() const;
	std::map<std::string, LocationConfig>				get_locations() const;	
	static std::vector<ListenInfo>		get_listens_info(std::vector<Config> configs);
	// const std::vector<ServerConfig> & 	get_servers_info() const;
};

#endif
