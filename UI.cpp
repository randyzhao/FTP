/*
 * UI.cpp
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#include "UI.h"

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cassert>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "command.h"
#include "commandParser.h"
#include "helpers.h"

#define MAX_TELNET_REPLY 100000

using namespace std;

int UI::handleGetCmd(Command getCmd) {
	assert(getCmd.getType() == CommandType_Get);
	this->userPI.do_retr(getCmd.getParam("localPath").getStringValue(),
			getCmd.getParam("remotePath").getStringValue());
	return 0;
	//TODO;
}

int UI::handleOpenCmd(Command openCmd) {
	assert(openCmd.getType() == CommandType_Open);
	int port = openCmd.getParam("port").getIntValue();
	string addr = openCmd.getParam("addr").getStringValue();
	if (initConnection(addr, port)) {
		printf("Could not connect to %s:%d\n", addr.c_str(), port);
		return -1;
	}
	int len;
	char buffer[MAX_TELNET_REPLY];
	memset(buffer, 0, sizeof(buffer));
	while ((len = userPI.telnetRead(buffer, MAX_TELNET_REPLY)) == 0) {
		//do nothing, keep reading
	}
	//TODO: valid for 220
	printf("%s\n", buffer);
	string input;
	//now request a user name
	printf("Name : ");
	getline(cin, input);
	this->userPI.do_user(input);
	while ((len = this->userPI.telnetRead(buffer, MAX_TELNET_REPLY)) == 0) {
		//do nothing, keep reading
	}
	printf("%s\n", buffer);
	//now request a password
	printf("password : ");
	getline(cin, input);
	this->userPI.do_pass(input);
	while ((len = this->userPI.telnetRead(buffer, MAX_TELNET_REPLY)) == 0) {
		//do nothing, keep reading
	}
	printf("%s\n", buffer);
	this->userPI.do_syst();
	while ((len = this->userPI.telnetRead(buffer, MAX_TELNET_REPLY)) == 0) {
		//do nothing, keep reading
	}
	printf("%s\n", buffer);

	//TODO: no valid
	return 0;
}

extern string int2str(int value);

int UI::initConnection(string addr, int port) {
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
		this->sockfd = s;
		userPI.setTelnetSockfd(s);
		userPI.setServdddr(addr);
		//into pasv mode
		return 0;
	} else {
		return 1;
	}
}

int UI::handleDirCmd(Command dirCmd) {
	this->userPI.do_list("test");
	//TODO:
	return 0;
}

void UI::run() {
	char reply[MAX_TELNET_REPLY];
	while (true) {
		memset(reply, 0, sizeof(reply));
		if (this->userPI.telnetRead(reply, MAX_TELNET_REPLY) > 0) {
			printf("%s\n", reply);
		}
		string cmdInput;
		printf("ftp > ");
		getline(cin, cmdInput);
		Command cmd = CommandParser::parseCommand(cmdInput);
		if (cmd.getType() == CommandType_Exit) {
			return;
		}

		switch (cmd.getType()) {
		case CommandType_Get:
			handleGetCmd(cmd);
			break;
		case CommandType_Open:
			handleOpenCmd(cmd);
			break;
		case CommandType_Dir:
			handleDirCmd(cmd);
			break;
		default:
			//printf("commnd %s is not supported yet\n", cmdInput.c_str());
			this->userPI.telnetSend(cmdInput);
			break;
		}
	}
}

UI::UI() {
}

