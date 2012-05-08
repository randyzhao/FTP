/*
 * helpers.h
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#ifndef HELPERS_H_
#define HELPERS_H_
#include <stdio.h>
#include <sys/socket.h>
       #include <netinet/in.h>
#include <sys/types.h>
#include <string>
using namespace std;

extern string int2str(int value);
//extract ftp code from telnet msg
extern int extractCode(string msg);
extern int extractCode(char* buf);
extern string generateMsg(int code, string content);

class ConnectionStatus{
public:
	bool isIPV6;
	in6_addr remoteAddr;
	int remotePort;
public:
	ConnectionStatus(){
		isIPV6 = false;
		remotePort = -1;
	}
	void setStatus(sockaddr_in6 addr){
		if (addr.sin6_family == AF_INET6){
			this->isIPV6 = true;
		}else{
			this->isIPV6 = false;
		}

		remoteAddr = addr.sin6_addr;
		remotePort = ntohs(addr.sin6_port);
	}
};
#endif /* HELPERS_H_ */
