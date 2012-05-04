/*
 * UserDTP.cpp
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#include "UserDTP.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include<sys/types.h>
#include<sys/socket.h>
using namespace std;

#define RECV_BUFFER_LEN 10000
#define RECEVIE_TIME_LIMIT 20

int UserDTP::getFile(string localPath) {
	char* file = new char[RECV_BUFFER_LEN];
	int len;
	if ((len = getFile(file)) > 0) {
		//file receive successfully
		printf("get file successfully\n");
		ofstream fout;
		fout.open(localPath.c_str());
		if (fout.write(file, len)) {
			printf("write file %s successfully\n", localPath.c_str());
		} else {
			printf("write file %s error\n", localPath.c_str());
		}
		fout.close();
	} else {
		printf("get file error\n");
	}
	delete file;
	return 0;
}

int UserDTP::getFile(char *file) {
	memset(file, 0, sizeof(file));
	int totalSize = 0;
	//TODO: time limit for receive
	struct timeval tv;
	tv.tv_sec = RECEVIE_TIME_LIMIT;
	setsockopt(this->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	char* recvbuf = new char[RECV_BUFFER_LEN + 10];
	while (true) { //not finish yet
		memset(recvbuf, 0, sizeof(recvbuf));
		int recvLen;
		if ((recvLen = recvfrom(sockfd, recvbuf, RECV_BUFFER_LEN, 0, NULL, NULL))
				<= 0) {
			break;
		}
		//receive successful
		recvbuf[recvLen] = 0;
		char* filePtr = file + totalSize;
		totalSize += recvLen;
		if (totalSize > RECV_BUFFER_LEN) {
			printf("file is too large\n");
			break;
		}
		strncpy(filePtr, recvbuf, recvLen);
		if (filePtr[totalSize - 1] == EOF) {
			break;
		}
	}
	return totalSize;
}

void UserDTP::setSockfd(int sockfd) {
	this->sockfd = sockfd;
}

