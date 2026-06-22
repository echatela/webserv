#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <netinet/in.h>
#include <string>
#include <vector>
#include <map>
#include <sys/socket.h>

class ConfigParser;

struct ListenInfo {
	std::string		host;
	std::string		port;
	struct sockaddr_in	address;
};

struct LocationConfig {
	std::string	base_location;
	std::string	root;
};

struct	ServerConfig {

};

class Config
{
private:
	std::vector<ListenInfo>			listens_info_;
	std::string				root_;
	std::map<std::string, LocationConfig>	locations_;
	std::string				server_name_;
	// std::vector<ServerConfig>	servers_info_;

public:
	Config();
	Config(const Config & src);
	~Config();

	const Config &	operator=(const Config & rhs);

	void		add_listen_info(ListenInfo listen);
	void		set_root(std::string root);
	void		add_location(std::pair<std::string, LocationConfig> location);

	std::string			root();
	const std::vector<ListenInfo> &	listens_info() const;
	static std::vector<ListenInfo>	listens_info(std::vector<Config> configs);
	const std::string &		server_name() const;
	// const std::vector<ServerConfig> & 	get_servers_info() const;
};

#endif
