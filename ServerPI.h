/*
 * ServerPI.h
 *
 *  Created on: May 5, 2012
 *      Author: randy
 */

#ifndef SERVERPI_H_
#define SERVERPI_H_

#include <string>
using namespace std;

class ServerPI{
private:
	int telnetSockfd;
	int transferSockfd;
	int transferListenSockfd;
	int transferListenPort;

	//read message using telnet and store to the buffer
	//return the length of the message
	int telnetRead(char* buffer, int size);
	//send content using telnet protocol
	int telnetSend(string content);
	int do_user();
	int do_pass();
	int do_pasv();
	int do_list();
	//execute dir command and return the result in string
	string dir();
	//begin to listen to a free port to wait for the user
	//return 0 when successfully
	int listenTransferPort();
	int acceptTransferPort();
public:
	void run(int telnetSockfd);
};


#endif /* SERVERPI_H_ */
