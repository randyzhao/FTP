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


int UserDTP::getFile(int size, string localPath)
{
	char* file = new char[size + 10];
	memset(file, 0, sizeof(file));
	int totalSize = 0;
	//TODO: time limit for receive
	struct timeval tv;
	tv.tv_sec = RECEVIE_TIME_LIMIT;
	setsockopt(this->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	char* recvbuf = new char[RECV_BUFFER_LEN + 10];
	while (totalSize < size){//not finish yet
		memset(recvbuf, 0, sizeof(recvbuf));
		int recvLen;
		if ((recvLen = recvfrom(sockfd, recvbuf, RECV_BUFFER_LEN, 0, NULL, NULL)) < 0){
			printf("receive from server error\n");
			break;
		}
		if (recvLen == 0){
			printf("waiting for server time up\n");
			break;
		}
		//receive successful
		recvbuf[recvLen] = 0;
		char* filePtr = file + totalSize;
		strncpy(filePtr, recvbuf, recvLen);
	}
	if (totalSize == size){
		//file receive successfully
		printf("get file successfully\n");
		ofstream fout;
		fout.open(localPath.c_str());
		if (fout.write(file, size)){
			printf("write file %s successfully\n", localPath.c_str());
		}else{
			printf("write file %s error\n", localPath.c_str());
		}
		fout.close();
	}else{
		printf("get file error\n");
	}
	delete recvbuf;
	delete file;
	return 0;
}

void UserDTP::setSockfd(int sockfd){
	this->sockfd = sockfd;
}


