/*
 * UserDTP.cpp
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#include "DTP.h"
#include "cache.h"

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
#define MAX_FILE_SIZE 10000000
#define MAX_SEND_SIZE 10000
int DTP::getFile(string localPath) {
	char* file = new char[MAX_FILE_SIZE];
	int len;
	if ((len = getFile(file)) > 0) {
		//file receive successfully
		printf("get file successfully\n");
		ofstream fout;
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
	struct timeval tv;
	tv.tv_sec = RECEVIE_TIME_LIMIT;
	setsockopt(this->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	char* recvbuf = new char[RECV_BUFFER_LEN + 10];while(true) { //not finish yet
		memset(recvbuf, 0, sizeof(recvbuf));
		int recvLen = read(sockfd, recvbuf, RECV_BUFFER_LEN);
		if (recvLen <= 0) {
			break;
		}
		//receive successful
		recvbuf[recvLen] = 0;
		char* filePtr = file + totalSize;
		totalSize += recvLen;
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

int DTP::sendAll(char *buf, int len) {
	int totalSend = 0;
	while (totalSend < len) {
		int tempSend = write(this->sockfd, buf + totalSend, len - totalSend);
		if (tempSend < 0) {
			return -1;
		}
		totalSend += tempSend;
	}
	return 0;
}

int DTP::sendFile(string path) {
	int fd = open(path.c_str(), O_RDONLY);
	if (fd == -1) {
		printf("file %s not exist\n", path.c_str());
		return -1;
	}
	CacheEntry entry;
	if (cacheGet(path, entry) == 0) {
		//get a cache
		if (sendAll(entry.buf, entry.size) < 0) {
			printf("send file %s error: %s\n", path.c_str(), strerror(errno));
			return -1;
		}
		return 0;
	}
	//not from a cache
	char* buf = new char[MAX_FILE_SIZE];
	memset(buf, 0, sizeof(buf));
	int totalSize = 0;
	int size = 0;
	int readTimes = 0;
	while ((size = read(fd, buf, MAX_FILE_SIZE)) > 0) {
		readTimes++;
		totalSize += size;
		if (sendAll(buf, size) < 0) {
			printf("send file %s error: %s\n", path.c_str(), strerror(errno));
			close(fd);
			delete buf;
			return -1;
		}
	}
	if (size < 0) {
		printf("read file %s error: %s\n", path.c_str(), strerror(errno));
		close(fd);
		delete buf;
		return -1;
	}
	if (readTimes == 1) {
		//small file, should cache
		cacheAdd(path, buf, totalSize);
	}
	close(fd);
	return 0;
}

