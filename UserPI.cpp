/*
 * UserPI.cpp
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#include <sys/stat.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>

#include "UserPI.h"
#include "helpers.h"

#define TRANSFORM_PORT 20
#define MAX_SOCK 10

int UserPI::do_retr(string localPath, string remotePath) {
	if (this->transferSockfd == -1) { //transfer socket is not init yet
		this->initTransferConnection();
	}
	this->dtp.getFile(localPath);
	//TODO;
	return 0;
}

int UserPI::initTransferConnection() {
//	//TODO:only support IPV4 yet
//	struct addrinfo hints, *res, *res0;
//	int error;
//	memset(&hints, 0, sizeof(hints));
//	hints.ai_socktype = SOCK_STREAM;
//	hints.ai_flags = AI_PASSIVE;
//	error = getaddrinfo(NULL, int2str(TRANSFORM_PORT).c_str(), &hints, &res0);
//	if (error){
//		printf("get addr localhost:%s error\n", int2str(TRANSFORM_PORT).c_str());
//		return 1;
//	}
//	int smax = 0;
//	int sockmax = -1;
//	for (res = res0; res && smax < MAXSO)
	int listenfd, connfd;
	struct sockaddr_in userAddr, serverAddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&userAddr, 0, sizeof(userAddr));
	userAddr.sin_family = AF_INET;
	userAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	userAddr.sin_port = htons(TRANSFORM_PORT);
	bind(listenfd, (struct sockaddr*) &userAddr, sizeof(userAddr));
	listen(listenfd, 100);
	socklen_t servlen;
	servlen = sizeof(serverAddr);
	connfd = accept(listenfd, (struct sockaddr*)&serverAddr, &servlen);
	this->transferSockfd = connfd;
	//TODO:
	return 0;
}

int UserPI::do_list(string remotePath) {
	//TODO:
	return 0;
}

void UserPI::setTelnetSockfd(int telnetSockfd) {
	this->telnetSockfd = telnetSockfd;
}

UserPI::UserPI() {
	this->telnetSockfd = -1;
	this->transferSockfd = -1;
}

