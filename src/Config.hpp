#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <netinet/in.h>
#include <string>
#include <vector>
#include <sys/socket.h>

struct ListenInfo {
	std::string		host;
	std::string		port;
	struct sockaddr_in	address;
};

class Config
{
private:
	std::vector<ListenInfo>	_listensInfo;

public:
	Config();
	Config(const Config & src);
	~Config();

	Config &	operator=(const Config & rhs);

	const std::vector<ListenInfo> &	getListensInfo() const;
};

#endif
