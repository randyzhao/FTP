/*
 * CommandParser.cpp
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#include "commandParser.h"
#include "command.h"
#include "boost/algorithm/string.hpp"
#include <stdio.h>
#include <vector>

using namespace std;

#define TELNET_PORT "21"

Command CommandParser::parseCommand(string com) {
	//using boost to split the string
	Command cmd;
	cmd.setType(CommandType_Error);
	vector<string> splitVec;
	boost::split(splitVec, com, boost::algorithm::is_space(),
			boost::algorithm::token_compress_on);
	if (splitVec[0] == "get") { //get command
		if (splitVec.size() < 3) {
			printf("arguments for get command is too low\n");
		} else {
			cmd.setType(CommandType_Get);
			cmd.addParam(splitVec[1]);
			cmd.addParam(splitVec[2]);
		}
	} else if (splitVec[0] == "open") { //open command
		if (splitVec.size() < 2) {
			printf("arguments for open command is too low\n");
		} else {
			cmd.setType(CommandType_Open);
			cmd.addParam(splitVec[1]);
			if (splitVec.size() >= 3){
				cmd.addParam(splitVec[2]);
			}else{
				//add default port
				cmd.addParam(TELNET_PORT);
			}

		}
	} else if (splitVec[0] == "dir") {
		//TODO:dir commands are not supported directory yet
		cmd.setType(CommandType_Dir);
	} else {
		printf("command %s is not supported yet\n", splitVec[0].c_str());
	}
	return cmd;
	//TODO:
}

