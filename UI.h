/*
 * UI.h
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#ifndef UI_H_
#define UI_H_

#include <string>
#include "command.h"
using namespace std;

//user-interface
class UI{
private:
	bool isConnected;
	int sockfd;
	struct addrinfo serverAddr;
	int handleGetCmd(Command getCmd);
	int handleOpenCmd(Command openCmd);
	int handleDirCmd(Command dirCmd);
	int initConnection(string addr, int port);
public:
	void run();
	UI();
};


#endif /* UI_H_ */
