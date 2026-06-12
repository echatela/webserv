#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <netinet/in.h>
#include <string>
#include <vector>
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
	ListenInfo					listen_info;
	std::string					root;
	std::vector<LocationConfig>	locations;
};

class Config
{
private:
	std::vector<ServerConfig>	servers_info_;

public:
	Config(ConfigParser & config_parser);
	Config(const Config & src);
	~Config();

	const Config &	operator=(const Config & rhs);

	std::vector<ListenInfo>				get_listens_info() const;
	const std::vector<ServerConfig> & 	get_servers_info() const;
};

#endif
