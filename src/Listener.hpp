#ifndef LISTENER_HPP
#define LISTENER_HPP

class Listener
{
private:

public:
	Listener();
	Listener(Listener const & src);
	~Listener();

	Listener &	operator=(Listener const & rhs);
};

#endif
