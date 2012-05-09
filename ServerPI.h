/*
 * ServerPI.h
 *
 *  Created on: May 5, 2012
 *      Author: randy
 */

#ifndef SERVERPI_H_
#define SERVERPI_H_
#include <string>
#include <boost/thread/mutex.hpp>
#include "DTP.h"
#include "helpers.h"
using namespace std;

class ServerPI{
private:
	boost::mutex* listenMutex;
	DTP dtp;
	ConnectionStatus status;
	bool fatalError;
	bool telnetClosed;
	//the service is in passive mode and ready for connect
	bool inPassive;
	//the socket while in begin()
	int listenSockfd;
	int telnetSockfd;
	int transferSockfd;
	int transferListenSockfd;
	int transferListenPort;

	//read message using telnet and store to the buffer
	//return the length of the message
	int telnetRead(char* buffer, int size);
	//send content using telnet protocol
	int telnetSend(string content);
	int do_argError();
	int do_user();
	int do_pass();
	int do_pasv(bool isIPV6 = true);
	int do_list();
	int do_stor(string fileName);
	int do_syst();
	int do_quit();
	int do_cwd(string path);
	int do_retr(string path);
	//execute dir command and return the result in string

	//request dispatcher
	//called whenever read a request from telnet
	void requestDispacher(string cmd);
	string dir();
	//begin to listen to a free port to wait for the user
	//return 0 when successfully
	int listenTransferPort();
	int acceptTransferPort();
public:
	ServerPI(int listenSockfd, boost::mutex* listenMutex);
	//init and try to accept a client
	void begin();
	void run();
};


#endif /* SERVERPI_H_ */
