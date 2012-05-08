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
#include <arpa/inet.h>
#include <boost/regex.hpp>

#include "UserPI.h"
#include "helpers.h"
#include "FTPcode.h"
using namespace boost;

#define TRANSFORM_PORT 52969
#define MAX_SOCK 10
#define MAX_LIST_LEN 100000
#define MAX_TELNET_REPLY 100000
#define MAX_TELNET_READ_TIME_US 200000
#define QUIT_WAIT_READ_TIMES 20

int UserPI::do_retr(string remotePath, string localPath) {
	char buffer[MAX_TELNET_REPLY];
	memset(buffer, 0, sizeof(buffer));
	//TODO: activate mode will be supported in alpha 2.0
	this->do_pasv();
	string content = "RETR ";
	content = content + remotePath;
	if (this->telnetSend(content)) {
		return -1;
	}
	this->dtp.setSockfd(this->transferSockfd);
	if (this->dtp.getFile(localPath)) {
		return -1;
	}
	return 0;
}

int UserPI::do_list(string remotePath) {
	//this->listenTransferConnection();
	//this->telnetSend("PORT 127,0,0,1,206,233");
	this->do_pasv();
	string content = "LIST " + remotePath;
	this->telnetSend(content);
	//this->acceptTransferConnection();
	//this->dtp.setSockfd(this->transferSockfd);
	char buffer[MAX_LIST_LEN];
	memset(buffer, 0, sizeof(buffer));
	int len = this->dtp.getFile(buffer);
	if (len > 0) {
		printf("%s\n", buffer);
	} else {
		printf("get list error\n");
		return -1;
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
		return -1;
	}
	printf("Client Send: %s\n", content.c_str());
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
	return this->telnetSend(content);
}

int UserPI::do_pass(string pwd) {
	string content = "PASS ";
	content = content.append(pwd);
	return this->telnetSend(content);
}

int UserPI::do_syst() {
	if (this->telnetSend("SYST")) {
		return 1;
	}
	return 0;
}

int UserPI::do_port() {
	//TODO:do_port will be supported in alpha 2.0
	return 0;
}

int UserPI::do_pasv() {
//	if (this->status.isIPV6) {
//		this->telnetSend("EPSV");
//	} else {
//		this->telnetSend("PASV");
//	}
	this->telnetSend("EPSV");
	char buffer[MAX_TELNET_REPLY];
	memset(buffer, 0, sizeof(buffer));
	int len;
	while ((len = this->telnetRead(buffer, MAX_TELNET_REPLY)) == 0) {
		//keep read from telnet
	}
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
		if (!digit && count != 0) {
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
		this->transferSockfd = s;
		this->dtp.setSockfd(s);
		return 0;
	} else {
		return 1;
	}
}

void UserPI::setServdddr(string addr) {
	this->servAddr = addr;
}

int UserPI::do_open(string addr, int port) {
	struct sockaddr_in6 sin6;
	memset(&sin6, 0, sizeof(sin6));
	struct addrinfo hints, *res, *res0;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	int error = getaddrinfo(addr.c_str(), int2str(port).c_str(), &hints, &res0);
	if (error) {
		printf("get addr %s error\n", addr.c_str());
		return 1;
	}
	char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
	int s = -1;
	for (res = res0; res; res = res->ai_next) {
		error = getnameinfo(res->ai_addr, res->ai_addrlen, hbuf, sizeof(hbuf),
				sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);
		if (error) {
			printf("get addr %s error\n", addr.c_str());
			return 1;
		}
		printf("trying %s : %s...", hbuf, sbuf);
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
		this->telnetSockfd = s;
		this->servAddr = addr;
		this->isClosed = false;
		if (res->ai_protocol == AF_INET) {
			sin6.sin6_family = AF_INET;
			inet_pton(AF_INET, addr.c_str(), (void*) &sin6.sin6_addr);
		} else {
			sin6.sin6_family = AF_INET6;
			inet_pton(AF_INET6, addr.c_str(), (void*) &sin6.sin6_addr);
		}
		sin6.sin6_port = htons(port);
		status.setStatus(sin6);
		if (!this->initUser()) {
			return 0;
		} else {
			return -1;
		}
	} else {
		return -1;
	}
}

int UserPI::initUser() {
	int len;
	char buffer[MAX_TELNET_REPLY];
	memset(buffer, 0, sizeof(buffer));
	while ((len = this->telnetRead(buffer, MAX_TELNET_REPLY)) == 0) {
		//do nothing, keep reading
	}
	printf("extract code %s\n", buffer);
	int code = extractCode(buffer);
	if (code != SERVICE_READY_FOR_NEW_USER) {
		printf("service could not be opened\n");
		return -1;
	}
	printf("%s\n", buffer);
	string input;
	//now request a user name
	printf("Name : ");
	getline(cin, input);
	this->do_user(input);
	while ((len = this->telnetRead(buffer, MAX_TELNET_REPLY)) == 0) {
		//do nothing, keep reading
	}
	printf("%s\n", buffer);
	//now request a password
	printf("password : ");
	getline(cin, input);
	this->do_pass(input);
	while ((len = this->telnetRead(buffer, MAX_TELNET_REPLY)) == 0) {
		//do nothing, keep reading
	}
	printf("%s\n", buffer);
	this->do_syst();
	while ((len = this->telnetRead(buffer, MAX_TELNET_REPLY)) == 0) {
		//do nothing, keep reading
	}
	printf("%s\n", buffer);

	return 0;
}

int UserPI::do_close() {
	this->telnetSend("QUIT");
	char buf[MAX_TELNET_REPLY];
	memset(buf, 0, sizeof(buf));
	for (int i = 0; i < QUIT_WAIT_READ_TIMES; i++) {
		int len = this->telnetRead(buf, MAX_TELNET_REPLY);
		if (len > 0) {
			printf("%s", buf);
			break;
		}
	}
	this->isClosed = true;
	return 0;
}

UserPI::UserPI() {
	this->telnetSockfd = -1;
	this->transferSockfd = -1;
}

int UserPI::do_cwd(string path)
{
	this->telnetSend("CWD " + path);
}

UserPI::~UserPI() {
	if (!this->isClosed) {
		do_close();
	}
}

