#include "Config.hpp"

Config::Config()
{
}

Config::Config(Config const & src)
{
	*this = src;
}

Config::~Config()
{
}

Config &	Config::operator=(Config const & rhs)
{
	if (this != &rhs)
	{
		// Copy attributes here
	}
	return (*this);
}
