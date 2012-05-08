/*
 * Server.h
 *
 *  Created on: May 5, 2012
 *      Author: randy
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "ServerPI.h"
#include "helpers.h"
#include <vector>
#include <boost/thread/mutex.hpp>

using namespace std;

class Server{
private:
	int listenSocked;
	ConnectionStatus status;
	boost::mutex* listenMutex;
	vector<ServerPI*> pis;
	void initThread();
public:
	void run();
	~Server();
};


#endif /* SERVER_H_ */
