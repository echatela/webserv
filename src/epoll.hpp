#ifndef EPOLL_HPP
#define EPOLL_HPP

class Epoll
{
private:

public:
	Epoll();
	Epoll(Epoll const & src);
	~Epoll();

	Epoll &	operator=(Epoll const & rhs);
};

#endif
