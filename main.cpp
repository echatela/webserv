#include "epoll.hpp"
#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>


int	main()
{
	int	epfd = epoll_create1(0);

	struct epoll_event	ev;
	int	sock = socket(AF_INET, SOCK_STREAM, 0);
	ev.events = EPOLLIN;
	ev.data.fd = sock;

	epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev);

	struct epoll_event	events[10];

	while (1) {

		int	n = epoll_wait(epfd, events, 10, -1);

		for (int i = 0; i < n; i++) {
			int	fd = events[i].data.fd;

			if (events[i].events & EPOLLIN) {
				std::cout << "FD: " << fd
					<< " is ready to read!\n";
			}
		}
	}

	return 0;
}
