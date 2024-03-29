/*
 * UserDTP.h
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#ifndef USERDTP_H_
#define USERDTP_H_

#include <string>

using namespace std;

//user date transfer
class DTP {
private:
	int sockfd;
public:
	//store the file to a local file
	int getFile(string localPath);
	//store the file to a buffer, this can be used in list command
	//return value is the real size of the file
	int getFile(char* buffer);
	void setSockfd(int sockfd);
	//send a msg
	void sendMsg(string content);
	//send all the data in a buf
	int sendAll(char* buf, int len);
	int sendFile(string path);
};

#endif /* USERDTP_H_ */
