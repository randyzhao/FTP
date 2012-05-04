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
class UserDTP{
private:
	int sockfd;
public:
	int getFile(int size, string localPath);
	void setSockfd(int sockfd);
};


#endif /* USERDTP_H_ */
