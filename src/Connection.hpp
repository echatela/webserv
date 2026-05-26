#ifndef CONNECTION_HPP
#define CONNECTION_HPP

class Connection
{
private:

public:
	Connection();
	Connection(Connection const & src);
	~Connection();

	Connection &	operator=(Connection const & rhs);
};

#endif
