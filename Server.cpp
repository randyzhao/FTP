/*
 * Server.cpp
 *
 *  Created on: May 5, 2012
 *      Author: randy
 */

#include "Server.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <errno.h>
#include <stdio.h>

using namespace std;

#define IPV6_LISTEN_PORT 21
#define PENDING_CONNECTION_QUEUE_LEN 10

void Server::run() {
	struct sockaddr_in6 sin6, sin6_accept;
	socklen_t sin6_len;
	int s0 = -1, s = -1;
	memset(&sin6, 0, sizeof(sin6));
	sin6.sin6_family = AF_INET6;
	sin6.sin6_port = htons(IPV6_LISTEN_PORT);
	int on = 1;
	if ((s0 = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) == -1){
		printf("init socket error : %s\n", strerror(errno));
		return;
	}
	setsockopt(s0, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (bind(s0, (const struct sockaddr*) &sin6, sizeof(sin6))) {
		printf("bind error : %s\n", strerror(errno));
		return;
	}
	if (listen(s0, PENDING_CONNECTION_QUEUE_LEN)) {
		printf("listen error\n");
		return;
	}
	printf("init server successfully\n");
	char hbuf[NI_MAXHOST];
	for (;;) {
		memset(&hbuf, 0, sizeof(hbuf));
		sin6_len = sizeof(sin6_accept);
		printf("waiting for connection\n");
		s = accept(s0, (struct sockaddr*) &sin6_accept, &sin6_len);
		printf("accept a user\n");
		if (s == -1) {
			printf("accept failed from %s\n", hbuf);
		} else {
			getnameinfo((struct sockaddr*) &sin6_accept, sin6_len, hbuf,
					sizeof(hbuf), NULL, 0, NI_NUMERICHOST);
			printf("accept a connection from %s\n", hbuf);
			//TODO: further operation
		}

	}

}

