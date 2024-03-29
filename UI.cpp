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
	return this->userPI.do_retr(getCmd.getArg(0), getCmd.getArg(1));
}

int UI::handleOpenCmd(Command openCmd) {
	assert(openCmd.getType() == CommandType_Open);
	int port = atoi(openCmd.getArg(1).c_str());
	string addr = openCmd.getArg(0);
	return this->userPI.do_open(addr, port);
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
	this->userPI.do_list(dirCmd.getArg(0));
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
		case CommandType_Exit:
			handleExitCmd(cmd);
			break;
		case CommandType_Cd:
			handleCdCmd(cmd);
			break;
		case CommandType_Put:
			handlePutCmd(cmd);
			break;
		default:
			//printf("commnd %s is not supported yet\n", cmdInput.c_str());
			this->userPI.telnetSend(cmdInput);
			break;
		}
	}
}

int UI::handleExitCmd(Command exitCmd)
{
	this->userPI.do_close();
	return 0;
}

int UI::handleCdCmd(Command cdCmd)
{
	this->userPI.do_cwd(cdCmd.getArg(0));
}

int UI::handlePutCmd(Command putCmd)
{
	this->userPI.do_stor(putCmd.getArg(0));
}

UI::UI() {
}

