#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "event_handler.hpp"
#include <cstdint>
#include <vector>

class Connection : public EventHandler
{
private:
	int			fd_;
	std::vector<char>	buf_;

	Connection();
	Connection(Connection const & src);
	Connection &	operator=(Connection const & rhs);

	static const int	kReadBufferSize = 4096;

public:
	Connection(int fd);

	int	HandleEvent(uint32_t events);

	int	get_fd() const;

	~Connection();
};

#endif
