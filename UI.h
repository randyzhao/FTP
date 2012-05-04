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
#include <sys/socket.h>
#include <netdb.h>
using namespace std;

//user-interface
class UI{
private:
	bool isConnected;
	int sockfd;
	int handleGetCmd(Command getCmd);
	int handleOpenCmd(Command openCmd);
	int handleDirCmd(Command dirCmd);
	int initConnection(string addr, int port);
public:
	void run();
	UI();
};


#endif /* UI_H_ */