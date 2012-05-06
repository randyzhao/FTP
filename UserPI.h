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
#include "DTP.h"
using namespace std;

//user protocol interpreter

class UserPI {
private:
	struct sockaddr_storage serverAddr;
	int telnetSockfd;
	int transferSockfd;
	int transferListenSockfd;
	//this is the read address of server
	DTP dtp;
	string servAddr;
	//int listenTransferConnection();
	//int acceptTransferConnection();

	//whether the client send QUIT command to the server
	bool isClosed;
public:
	//PASV in ipv4 and EPSV in ipv6
	int do_pasv();
	//set port for server to connectnc
	//PORT in ipv4 and EPRT in ipv6
	//TODO: now only support ipv6 eprt
	int do_port();
	//retrive file from remote
	int do_retr(string remotePath, string localPath);
	int do_user(string name);
	int do_pass(string pwd);
	int do_syst();
	int do_list(string remotePath);
	int do_open(string addr, int port);
	int do_close();
	int initUser();
	void setTelnetSockfd(int telnetSockfd);
	//read message using telnet and store to the buffer
	//return the length of the message
	int telnetRead(char* buffer, int size);
	//send content using telnet protocol
	int telnetSend(string content);
	void setServdddr(string addr);
	UserPI();
	~UserPI();
};

#endif /* USERPI_H_ */
