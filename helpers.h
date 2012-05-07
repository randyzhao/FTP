/*
 * helpers.h
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#ifndef HELPERS_H_
#define HELPERS_H_
#include <stdio.h>
#include <string>
using namespace std;

extern string int2str(int value);
//extract ftp code from telnet msg
extern int extractCode(string msg);
extern int extractCode(char* buf);
extern string generateMsg(int code, string content);

class ConnectionStatus{
	bool isIPV6;
	int localPort;
	int remotePort;

};
#endif /* HELPERS_H_ */
