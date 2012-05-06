/*
 * UserDTP.cpp
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#include "DTP.h"
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <stdio.h>
#include <cstring>
#include<sys/types.h>
#include <errno.h>
#include<sys/socket.h>
using namespace std;

#define RECV_BUFFER_LEN 10000
#define RECEVIE_TIME_LIMIT 20
#define MAX_FILE_SIZE 1000000
int DTP::getFile(string localPath) {
	char* file = new char[RECV_BUFFER_LEN];
	int len;
	if ((len = getFile(file)) >= 0) {
		//file receive successfully
		printf("get file successfully\n");
		ofstream fout;
//		fout.open("./sample3");
//		if (!fout.write(file, len)) {
//			printf("write file %s successfully\n", localPath.c_str());
//		} else {
//			printf("write file %s error\n", localPath.c_str());
//		}
//		fout.close();
		FILE* f;
		f = fopen(localPath.c_str(), "w");
		if (f == NULL) {
			printf("open file %s error\n", localPath.c_str());
			return -1;
		} else {
			if (fputs(file, f) == EOF) {
				printf("write file %s error\n", localPath.c_str());
			} else {
				printf("write file %s successfully\n", localPath.c_str());
			}
			fclose(f);
		}
	} else {
		printf("get file error\n");
	}
	delete file;
	return 0;
}

int DTP::getFile(char *file) {
	memset(file, 0, sizeof(file));
	int totalSize = 0;
	//TODO: time limit for receive
	struct timeval tv;
	tv.tv_sec = RECEVIE_TIME_LIMIT;
	setsockopt(this->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	char* recvbuf = new char[RECV_BUFFER_LEN + 10];while
(	true) { //not finish yet
		memset(recvbuf, 0, sizeof(recvbuf));
		int recvLen = read(sockfd, recvbuf, RECV_BUFFER_LEN);
		if (recvLen <= 0){
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

void DTP::setSockfd(int sockfd) {
	this->sockfd = sockfd;
}

void DTP::sendMsg(string content) {
	int len = write(this->sockfd, content.c_str(), strlen(content.c_str()));
	if (len < 0) {
		printf("socket error: %s\n", strerror(errno));
	} else {
		printf("send msg length = %d, content:\n %s\n", len, content.c_str());
	}
}

int DTP::sendFile(string path)
{
	int fd = open(path.c_str(), O_RDONLY);
	char* buf = new char[MAX_FILE_SIZE];
	memset(buf, 0, sizeof(buf));
	int size = read(fd, buf, MAX_FILE_SIZE);
	if (size <= 0){
		printf("send file %s error: %s\n", path.c_str(), strerror(errno));
		return 1;
	}
	int len = write(this->sockfd, buf, size);
	if (len < 0) {
		printf("socket error: %s\n", strerror(errno));
	} else {
		printf("send file %s successfully\n", path.c_str());
	}
}



