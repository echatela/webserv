#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "epoll.hpp"
#include "event_handler.hpp"
#include "listen.hpp"
#include <cstdint>
#include <vector>

enum {
	kReading,
	kWriting
};

class Connection : public EventHandler
{
private:
	int			fd_;
	int			state_;

	std::vector<char>	write_buf_;
	int			write_off_;

	const Listen &		listen_;
	Epoll &			epoll_;
	// HttpRequestParser &	parser_

	Connection();
	Connection(Connection const & src);
	Connection &	operator=(Connection const & rhs);

	static const int	kReadBufferSize = 4096;

public:
	Connection(int fd, const Listen & listen, Epoll & epoll);

	int	HandleEvent(uint32_t events);

	int	get_fd() const;

	~Connection();
};

#endif
