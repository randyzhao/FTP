/*
 * UserPI.cpp
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#include "UserPI.h"




int UserPI::do_retr(string localPath, string remotePath)
{
	if (this->transferSockfd == -1){//transfer socket is not init yet
		this->initTransferConnection();
	}

	//TODO;
	return 0;
}

int UserPI::initTransferConnection()
{

	//TODO:

	return 0;
}

int UserPI::do_list(string remotePath)
{
	//TODO:
	return 0;
}

