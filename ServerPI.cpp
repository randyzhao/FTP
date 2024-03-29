/*
 * ServerPI.cpp
 *
 *  Created on: May 5, 2012
 *      Author: randy
 */

#include "ServerPI.h"
#include "DTP.h"
#include "FTPcode.h"
#include "file_list.h"

#include <string.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <boost/algorithm/string.hpp>
#include <vector>

#define MAX_TELNET_REPLY 100000
#define MAX_TELNET_READ_TIME_US 200000
#define PENDING_CONNECTION_QUEUE_LEN 10
#define MAX_FILE_SIZE 10000000
#define USER_TIME_OUT_TIMES 2000

using namespace std;

int ServerPI::do_user() {
	this->telnetSend("331 Please specify the password\n");
	return 0;
}

int ServerPI::do_pass() {
	this->telnetSend("230 Login successfully");
	return 0;
}

int ServerPI::do_pasv(bool isIPV6/* = true */) {
	if (this->listenTransferPort()) {
		printf("listen transferport error\n");
		return -1;

	}
	char buf[MAX_TELNET_REPLY];
	memset(&buf, 0, sizeof(buf));
	if (isIPV6) {
		sprintf(buf, "229 Entering extended passive mode (|||%d|).",
				this->transferListenPort);
	} else {
		//not IPV6
		int highPort = this->transferListenPort / 256;
		int lowPort = this->transferListenPort % 256;
		inet_ntop(AF_INET, &status.remoteAddr, buf, sizeof(buf));
		vector<string> temp;
		string addr = buf;
		boost::split(temp, addr, boost::is_any_of(".,"));
		//printf("addr is %s\n", buf);
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "229 Entering passive mode (%s,%s,%s,%s,%d,%d).",
				temp[0].c_str(), temp[1].c_str(), temp[2].c_str(),
				temp[3].c_str(), highPort, lowPort);
	}
	this->inPassive = true;
	string content = buf;
	printf("msg is %s\n", content.c_str());

	return this->telnetSend(content);
}

int ServerPI::telnetSend(string content) {
	string temp = content;
	temp = temp.append("\r\n");
	if (write(this->telnetSockfd, temp.c_str(), temp.length()) < 0) {
		printf("telnet send error\n");
		return -1;
	}
	return 0;
}

int ServerPI::telnetRead(char *buffer, int size) {
	memset(buffer, 0, sizeof(buffer));
	char tempBuffer[MAX_TELNET_REPLY];
	memset(tempBuffer, 0, sizeof(tempBuffer));
	int totalLen = 0;
	int currentLen = 0;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = MAX_TELNET_READ_TIME_US;
	setsockopt(this->telnetSockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	while ((currentLen = read(this->telnetSockfd, tempBuffer, MAX_TELNET_REPLY))
			> 0) {
		if (currentLen + totalLen < size) {
			strncpy(buffer + totalLen, tempBuffer, currentLen);
		} else {
			break;
		}
		totalLen += currentLen;
		memset(tempBuffer, 0, sizeof(tempBuffer));
	}
	buffer[totalLen] = 0;
	return totalLen;
}

int ServerPI::listenTransferPort() {
	struct sockaddr_in6 sin6;
	memset(&sin6, 0, sizeof(sin6));
	sin6.sin6_family = AF_INET6;
	sin6.sin6_port = htons(0);
	int s0 = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (s0 == -1) {
		printf("init transfer socket error : %s\n", strerror(errno));
		return -1;
	}
	int on = 1;
	if (setsockopt(s0, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) {
		printf("setsockopt error : %s\n", strerror(errno));
	}
	if (bind(s0, (const struct sockaddr*) &sin6, sizeof(sin6))) {
		printf("bind error : %s\n", strerror(errno));
		return -1;
	}
	if (listen(s0, PENDING_CONNECTION_QUEUE_LEN)) {
		printf("listen error\n");
		return -1;
	}
	//now get port
	struct sockaddr_in6 sa;
	memset(&sa, 0, sizeof(sa));
	socklen_t salen = sizeof(sa);
	if (getsockname(s0, (struct sockaddr*) &sa, &salen) == -1) {
		printf("getsockname error\n");
		return -1;
	}
	int port = ntohs(sa.sin6_port);
	printf("listen to transfer connection on port %d\n", port);
	this->transferListenSockfd = s0;
	this->transferListenPort = port;
	return 0;
}

int ServerPI::acceptTransferPort() {
	int s0 = this->transferListenSockfd;
	struct sockaddr_in6 sin6_accept;
	socklen_t sin6_len = sizeof(sin6_accept);
	char hbuf[NI_MAXHOST];
	memset(&hbuf, 0, sizeof(hbuf));
	int s;
	//TODO: not valid user's address. It may not come from the user
	while ((s = accept(s0, (struct sockaddr*) &sin6_accept, &sin6_len)) < 0) {
		printf("accpet error: %s\n", strerror(errno));
	}
	getnameinfo((struct sockaddr*) &sin6_accept, sin6_len, hbuf, sizeof(hbuf),
			NULL, 0, NI_NUMERICHOST);
	printf("transfer connection build from : %s\n", hbuf);
	close(this->transferListenSockfd);
	this->transferListenSockfd = -1;
	this->transferSockfd = s;
	this->dtp.setSockfd(s);
	return 0;
}

int ServerPI::do_list() {
	if (!this->inPassive) {
		this->telnetSend(CANNOT_OPEN_DATA_CONNECTION_MSG);
		return -1;
	}
	if (this->acceptTransferPort()) {
		return -1;
	}
	string content = this->dir();
	this->telnetSend("150 Here comes the directory listing");
	this->dtp.sendMsg(content);
	close(this->transferSockfd);
	this->dtp.setSockfd(-1);
	this->inPassive = false;
	return 0;
}

string ServerPI::dir() {
	string cmd = "ls -l";
	FILE* result = popen(cmd.c_str(), "r");
	char buf[MAX_TELNET_REPLY];
	memset(buf, 0, sizeof(buf));
	long maxl = MAX_TELNET_REPLY;
	fread(buf, 1, maxl, result);
	string r = buf;
	return r;

//	char buf[256];
//	char temp[10];
//	getcwd(buf, 256);
//	int fd[2];
//	pipe(fd);
//	file_list(fd[1], buf, temp);
//	char content[MAX_TELNET_REPLY];
//	read(fd[0], content, MAX_TELNET_REPLY);
//	printf("filelist: %s\n", content);
//	string out = content;
//	return out;
}

ServerPI::ServerPI(int listenSockfd, boost::mutex* listenMutex) {
	this->listenMutex = listenMutex;
	this->listenSockfd = listenSockfd;
}

void ServerPI::begin() {
	printf("new thread begin\n");
	char hbuf[NI_MAXHOST];
	memset(hbuf, 0, sizeof(hbuf));
	for (;;) { //thread should always be running

		struct sockaddr_in6 sin6_accept;
		memset(&sin6_accept, 0, sizeof(sin6_accept));
		socklen_t sin6_len = sizeof(sin6_accept);
		int sock;
		//try to accept
		for (;;) {
			{
				boost::mutex::scoped_lock lock(*(this->listenMutex));
				sock = accept(this->listenSockfd,
						(struct sockaddr*) &sin6_accept, &sin6_len);
			}
			printf("accept a user\n");
			if (sock == -1) {
				printf("accept failed from %s\n", hbuf);
			} else {
				getnameinfo((struct sockaddr*) &sin6_accept, sin6_len, hbuf,
						sizeof(hbuf), NULL, 0, NI_NUMERICHOST);
				printf("accepPt a connection from %s\n", hbuf);
				this->telnetSockfd = sock;
				status.setStatus(sin6_accept);
				break;
			}

		}
		this->run();
	}
}

void ServerPI::requestDispacher(string cmd) {
	vector<string> splitVec;
	boost::split(splitVec, cmd, boost::algorithm::is_space(),
			boost::algorithm::token_compress_on);
	string type = splitVec[0];
	if (type == "USER") {
		this->do_user();
	} else if (type == "PASS") {
		this->do_pass();
	} else if (type == "EPSV" || type == "PASV") {
		this->do_pasv(type == "EPSV");
	} else if (type == "LIST") {
		this->do_list();
	} else if (type == "SYST") {
		this->do_syst();
	} else if (type == "RETR") {
		if (splitVec.size() < 2) {
			this->telnetSend(ARG_SYNTEX_ERROR_MSG);
		} else {
			this->do_retr(splitVec[1]);
		}
	} else if (type == "QUIT") {
		this->do_quit();
	} else if (type == "CWD") {
		if (splitVec.size() < 2) {
			this->telnetSend(ARG_SYNTEX_ERROR_MSG);
		} else {
			this->do_cwd(splitVec[1]);
		}
	} else if (type == "STOR"){
		this->do_stor(splitVec[1]);
	}else {
		printf("not supported yet");
	}

}

int ServerPI::do_syst() {
	string cmd = "uname -a";
	char buf[MAX_TELNET_REPLY];
	memset(buf, 0, sizeof(buf));
	FILE* re = popen(cmd.c_str(), "r");
	long maxl = MAX_TELNET_REPLY;
	fread(buf, 1, maxl, re);
	string sysinfo = buf;
	string content = "215 " + sysinfo;
	this->telnetSend(content);
	return 0;
}

int ServerPI::do_retr(string path) {
	this->acceptTransferPort();
	int result = dtp.sendFile(path);
	if (result == -1) {
		this->telnetSend(OPEN_FILE_ERROR_MSG);
	}
	close(this->transferSockfd);
	dtp.setSockfd(-1);
	this->inPassive = false;
	return result;
}
int ServerPI::do_quit() {
	this->telnetSend(QUIT_MSG);
	close(this->telnetSockfd);
	this->telnetClosed = true;
	return 0;
}

int ServerPI::do_cwd(string path) {
	if (chdir(path.c_str()) != -1) {
		this->telnetSend(DIR_CHANGE_OK_MSG);
		return 0;
	} else {
		this->telnetSend(FILE_REQUEST_ERROR_MSG);
		return -1;
	}
}

int ServerPI::do_argError() {
	return this->telnetSend(ARG_SYNTEX_ERROR_MSG);
}

int ServerPI::do_stor(string fileName)
{
	this->acceptTransferPort();
	this->dtp.getFile(fileName);
	return 0;
}

void ServerPI::run() {
	this->fatalError = false;
	this->telnetClosed = false;
	this->inPassive = false;

	printf("ServerPI : run\n");
	this->telnetSend("220 Randy's FTP alpha 1.0\n");
	char buf[MAX_TELNET_REPLY];
	memset(buf, 0, sizeof(buf));
	struct timeval bt, et;
	gettimeofday(&bt, NULL);
	for (;;) {
		int len = this->telnetRead(buf, MAX_TELNET_REPLY);
		if (len == 0) {
			gettimeofday(&et, NULL);
			if (et.tv_sec - bt.tv_sec > USER_TIME_OUT_TIMES) {
				printf("user time out\n");
				close(this->telnetSockfd);
				return;
			}else{
				continue;
			}
		}
		gettimeofday(&bt, NULL);
		string cmd = buf;
		printf("recv request : %s\n", buf);
		this->requestDispacher(cmd);
		if (this->telnetClosed) {
			printf("client connection closed\n");
			return;
		}
		if (this->fatalError) {
			printf("fatal error\n");
			return;
		}
	}
}

