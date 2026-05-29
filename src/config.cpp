#include "config.hpp"

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

Config &	Config::operator=(const Config & rhs)
{
	if (this != &rhs)
	{
		listens_info_ = rhs.listens_info_;
	}
	return (*this);
}
