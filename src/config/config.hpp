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
	std::string			base_location;
	std::vector<std::string>	root;
	std::vector<std::string>	cgi;
	std::vector<std::string>	methods;
	std::vector<std::string>	index;
	bool				autoindex;
	std::vector<std::string>	redirect;
	bool						upload_enabled;
};

struct	ServerConfig {

};

class Config
{
private:
	std::vector<ListenInfo>			listens_info_;
	std::string				root_;
	std::map<std::string, LocationConfig>	locations_;
	std::map<int, std::string>		error_pages_;
	std::string				server_name_;
	size_t					max_body_size_;

public:
	Config();
	Config(const Config & src);
	~Config();

	Config &	operator=(const Config & rhs);

	void	add_listen_info(ListenInfo listen);
	void	set_root(std::string root);
	void	add_location(std::pair<std::string, LocationConfig> location);
	void	add_error_page(int code, std::string path);
	void	set_max_body_size(size_t size);
	void	add_error_page(std::pair<int, std::string> page);

	const std::string&	root() const;
	size_t			max_body_size() const;

	const std::vector<ListenInfo> &		listens_info() const;
	std::map<std::string, LocationConfig>	locations() const;	
	static std::vector<ListenInfo>		listens_info(std::vector<Config> configs);
	const std::string &			server_name() const;
};

#endif
