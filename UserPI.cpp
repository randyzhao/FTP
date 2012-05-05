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
#include <boost/regex.hpp>

#include "UserPI.h"
#include "helpers.h"

using namespace boost;

#define TRANSFORM_PORT 52969
#define MAX_SOCK 10
#define MAX_LIST_LEN 100000
#define MAX_TELNET_REPLY 100000
#define MAX_TELNET_READ_TIME_US 200000

int UserPI::do_retr(string localPath, string remotePath) {
	//TODO: port command is not well supported yet
	char buffer[MAX_TELNET_REPLY];
	memset(buffer, 0, sizeof(buffer));
	this->listenTransferConnection();
	//IP addr : 127.0.0.1
	//port : 11122
	this->telnetSend("PORT 127,0,0,1,206,233");
	this->telnetRead(buffer, MAX_TELNET_REPLY);
	printf("%s\n", buffer);
	string content = "RETR ";
	content = content + remotePath;
	this->telnetSend(content);
	this->acceptTransferConnection();
	this->dtp.setSockfd(this->transferSockfd);
	this->dtp.getFile(localPath);
	this->telnetRead(buffer, MAX_TELNET_REPLY);
	printf("%s\n", buffer);
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
	int connfd = -1;
	while ((connfd = accept(this->transferListenSockfd,
			(struct sockaddr*) &serverAddr, &servlen)) < 0) {
		//do nothing
	}
	printf("accept transfer connection successfully\n");
	//TODO:valid code here
	this->transferSockfd = connfd;
	close(this->transferListenSockfd);
	return 0;
}

int UserPI::do_list(string remotePath) {
	//this->listenTransferConnection();
	//this->telnetSend("PORT 127,0,0,1,206,233");
	this->do_pasv();
	this->telnetSend("LIST");
	//this->acceptTransferConnection();
	//this->dtp.setSockfd(this->transferSockfd);
	char buffer[MAX_LIST_LEN];
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
	string temp = content;
	temp = temp.append("\r\n");
	if (write(this->telnetSockfd, temp.c_str(), temp.length()) < 0) {
		printf("telnet send error\n");
		return 1;
	}
	return 0;
}

int UserPI::telnetRead(char *buffer, int size) {
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

int UserPI::do_user(string name) {
	string content = "USER ";
	content = content.append(name);
	this->telnetSend(content);
	//TODO: no valid
	return 0;
}

int UserPI::do_pass(string pwd) {
	string content = "PASS ";
	content = content.append(pwd);
	this->telnetSend(content);
	//TODO: no valid
	return 0;
}

int UserPI::do_syst() {
	this->telnetSend("SYST");
	//TODO: not valid
	return 1;
}

int UserPI::do_port() {
	//TODO:
	return 0;
}

int UserPI::do_pasv() {
	//TODO: now only support epsv in ipv6
	this->telnetSend("EPSV");
	char buffer[MAX_TELNET_REPLY];
	memset(buffer, 0, sizeof(buffer));
	int len = this->telnetRead(buffer, MAX_TELNET_REPLY);
	int count = 0;
	int temp = 1;
	printf("get pasv reply %s\n", buffer);
	for (int i = len - 1; i >= 0; i--) {
		bool digit = false;
		if (buffer[i] >= 48 && buffer[i] <= 57) { //a digit
			digit = true;
			count += temp * (buffer[i] - 48);
			temp *= 10;
		}
		if (!digit && count != 0){
			break;
		}
	}
	//now connect the port
	struct addrinfo hints, *res, *res0;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	int error = getaddrinfo(this->servAddr.c_str(), int2str(count).c_str(),
			&hints, &res0);
	if (error) {
		printf("get addr %s error\n", this->servAddr.c_str());
		return 1;
	}
	char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
	int s = -1;
	for (res = res0; res; res = res->ai_next) {
		error = getnameinfo(res->ai_addr, res->ai_addrlen, hbuf, sizeof(hbuf),
				sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);
		if (error) {
			printf("get addr %s error\n", this->servAddr.c_str());
			return 1;
		}
		printf("transfer connection trying %s : %s...", hbuf, sbuf);
		s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (s < 0) {
			printf("failed\n");
			continue;
		}
		if (connect(s, res->ai_addr, res->ai_addrlen) < 0) {
			close(s);
			s = -1;
			printf("failed\n");
			continue;
		}
		//successfully
		printf("successfully\n");
		break;
	}
	if (s != -1) {
		struct sockaddr_storage servAddr;
		memset(&servAddr, 0, sizeof(servAddr));
		servAddr.ss_family = res->ai_family;
		this->transferSockfd = s;
		return 0;
	} else {
		return 1;
	}
}

void UserPI::setServdddr(string addr) {
	this->servAddr = addr;
}

UserPI::UserPI() {
	this->telnetSockfd = -1;
	this->transferSockfd = -1;
}

