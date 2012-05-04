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

#define TRANSFORM_PORT 11122
#define MAX_SOCK 10
#define MAX_LIST_SIZE 100000

int UserPI::do_retr(string localPath, string remotePath) {
	//TODO: port command is not well supported yet
	this->listenTransferConnection();
	//IP addr : 127.0.0.1
	//port : 11122
	this->telnetSend("PORT 127,0,0,1,43,256\r\n");
	this->acceptTransferConnection();
	this->dtp.setSockfd(this->transferSockfd);
	this->dtp.getFile(localPath);
	return 0;
}

int UserPI::listenTransferConnection() {
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
	struct sockaddr_in userAddr;
	this->transferListenSockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&userAddr, 0, sizeof(userAddr));
	userAddr.sin_family = AF_INET;
	userAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	userAddr.sin_port = htons(TRANSFORM_PORT);
	bind(this->transferListenSockfd, (struct sockaddr*) &userAddr,
			sizeof(userAddr));
	listen(this->transferListenSockfd, 100);
	return 0;
}

int UserPI::acceptTransferConnection() {
	socklen_t servlen;
	servlen = sizeof(serverAddr);
	int connfd = accept(this->transferListenSockfd,
			(struct sockaddr*) &serverAddr, &servlen);
	//TODO:valid code here
	this->transferSockfd = connfd;
	close(this->transferListenSockfd);
	return 0;
}

int UserPI::do_list(string remotePath) {
	this->listenTransferConnection();
	char buffer[MAX_LIST_SIZE];
	this->telnetSend("PORT 127,0,0,1,43,256\r\n");
	this->acceptTransferConnection();
	this->dtp.setSockfd(this->transferSockfd);
	memset(buffer, 0, sizeof(buffer));
	int len = this->dtp.getFile(buffer);
	if (len > 0) {
		printf("%s\n", buffer);
	} else {
		printf("get list error\n");
	}
	return 0;
}

void UserPI::setTelnetSockfd(int telnetSockfd) {
	this->telnetSockfd = telnetSockfd;
}

int UserPI::telnetSend(string content) {
	if (write(this->telnetSockfd, content.c_str(), content.length())) {
		printf("telnet send error\n");
		return 1;
	}
	return 0;
}

UserPI::UserPI() {
	this->telnetSockfd = -1;
	this->transferSockfd = -1;
}

