/*
 * UserPI.h
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#ifndef USERPI_H_
#define USERPI_H_

#include <string>
#include <netdb.h>
#include "UserDTP.h"
using namespace std;

//user protocol interpreter

class UserPI {
private:
	int telnetSockfd;
	int transferSockfd;
	int transferListenSockfd;
	//this is the read address of server
	addrinfo serverAddr;
	UserDTP dtp;
	int listenTransferConnection();
	int acceptTransferConnection();
public:
	//retrive file from remote
	int do_retr(string localPath, string remotePath);
	int do_list(string remotePath);
	void setTelnetSockfd(int telnetSockfd);
	//read message using telnet and store to the buffer
	//return the length of the message
	int telnetRead(char* buffer, int size);
	//send content using telnet protocol
	int telnetSend(string content);
	UserPI();
};

#endif /* USERPI_H_ */
