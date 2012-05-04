
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
	//this is the read address of server
	addrinfo serverAddr;
	UserDTP dtp;
	int initTransferConnection();
public:
	//retrive file from remote
	UserPI(int telnetSockfd);
	int do_retr(string localPath, string remotePath);
	int do_list(string remotePath);
};

#endif /* USERPI_H_ */
