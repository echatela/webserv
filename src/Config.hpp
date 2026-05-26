#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <sys/socket.h>

struct Listen {
	std::string		host;
	struct sockaddr	address;
	uint16_t		port;
};

typedef std::vector<Listen>	Listens;

class Config
{
private:


public:
	Config();
	Config(Config const & src);
	~Config();

	Config &	operator=(Config const & rhs);
};

#endif
