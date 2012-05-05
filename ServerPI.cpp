/*
 * ServerPI.cpp
 *
 *  Created on: May 5, 2012
 *      Author: randy
 */


#include "ServerPI.h"
#include <string.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>

#define MAX_TELNET_REPLY 100000
#define MAX_TELNET_READ_TIME_US 200000
#define PENDING_CONNECTION_QUEUE_LEN 10

int ServerPI::do_user()
{
	this->telnetSend("331 Please specify the password\n");
	return 0;
}

int ServerPI::do_pass()
{
	this->telnetSend("230 Login successfully");
	return 0;
}

int ServerPI::do_pasv()
{
	//TODO: not valid
	this->listenTransferPort();
	char buf[MAX_TELNET_READ_TIME_US];
	memset(&buf, 0, sizeof(buf));
	//TODO: address is not supported
	sprintf(buf, "229 Entering extended passive mode (|||%d|).", this->transferListenPort);
	string content = buf;
	this->telnetSend(content);
	//TODO:
	return 0;
}

int ServerPI::telnetSend(string content) {
	string temp = content;
	temp = temp.append("\r\n");
	if (write(this->telnetSockfd, temp.c_str(), temp.length()) < 0) {
		printf("telnet send error\n");
		return 1;
	}
	return 0;
}

int ServerPI::telnetRead(char *buffer, int size) {
	memset(buffer, 0, sizeof(buffer));
	char tempBuffer[MAX_TELNET_REPLY];
	memset(tempBuffer, 0, sizeof(tempBuffer));
	int totalLen = 0;
	int currentLen;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = MAX_TELNET_READ_TIME_US;
	setsockopt(this->telnetSockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	while ((currentLen = read(this->telnetSockfd, tempBuffer, MAX_TELNET_REPLY))
			> 0) {
		if (currentLen + totalLen < size) {
			strncpy(buffer + totalLen, tempBuffer, currentLen);
		} else {
			break;
		}
		totalLen += currentLen;
		memset(tempBuffer, 0, sizeof(tempBuffer));
	}
	buffer[totalLen] = 0;
	return totalLen;
}

int ServerPI::listenTransferPort()
{
	//TODO:
	struct sockaddr_in6 sin6;
	memset(&sin6, 0, sizeof(sin6));
	sin6.sin6_family = AF_INET6;
	sin6.sin6_port = htons(0);
	int s0 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s0 == -1){
		printf("init transfer socket error : %s\n", strerror(errno));
		return -1;
	}
	int on = 1;
	if (setsockopt(s0, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))){
		printf("setsockopt error : %s\n", strerror(errno));
	}
	if (bind(s0, (const struct sockaddr*) &sin6, sizeof(sin6))) {
		printf("bind error : %s\n", strerror(errno));
		return -1;
	}
	if (listen(s0, PENDING_CONNECTION_QUEUE_LEN)) {
		printf("listen error\n");
		return -1;
	}
	printf("listen to transfer connection on port %d\n", ntohs(sin6.sin6_port));
	this->transferListenSockfd = s0;
	this->transferListenPort = ntohs(sin6.sin6_port);
	return 0;
}

int ServerPI::acceptTransferPort()
{
	int s0 = this->transferListenSockfd;
	struct sockaddr_in6 sin6_accept;
	socklen_t sin6_len;
	char hbuf[NI_MAXHOST];
	memset(&hbuf, 0, sizeof(hbuf));
	//TODO: not valid user's address. It may not come from the user
	int s = accept(s0, (struct sockaddr*)&sin6_accept, &sin6_len);
	getnameinfo((struct sockaddr*)&sin6_accept, sin6_len, hbuf,
			sizeof(hbuf), NULL, 0, NI_NUMERICHOST);
	printf("transfer connection ok\n");
	close(this->transferListenSockfd);
	this->transferListenSockfd = -1;
	this->transferSockfd = s;
	return 0;
}

int ServerPI::do_list()
{
	//TODO: not valid if PASV mode
	//TODO: not valid accept result
	this->acceptTransferPort();
	string content = this->dir();
	this->telnetSend("150 Here comes the directory listing");
	//TODO: dtp working
}

string ServerPI::dir()
{
	string cmd = "dir";
	FILE* result = popen(cmd.c_str(), "r");
	char buf[MAX_TELNET_REPLY];
	memset(buf, 0, sizeof(buf));
	long maxl = MAX_TELNET_REPLY;
	fread(buf, 1, maxl, result);
	string r = buf;
	return r;
}

void ServerPI::run(int telnetSockfd)
{
	this->telnetSockfd = telnetSockfd;
	printf("ServerPI : run\n");
}

