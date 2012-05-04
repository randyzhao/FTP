/*
 * CommandParser.cpp
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#include "commandParser.h"
#include "command.h"
#include "boost/algorithm/string.hpp"
#include <vector>

using namespace std;

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
			CommandParam remotePathParam;
			remotePathParam.setType(ParamType_String);
			remotePathParam.setName("remotePath");
			remotePathParam.setStringValue(splitVec[1]);
			cmd.addParam(remotePathParam);

			CommandParam localPathParam;
			localPathParam.setType(ParamType_String);
			localPathParam.setName("localPath");
			localPathParam.setStringValue(splitVec[2]);
			cmd.addParam(localPathParam);
		}
	} else if (splitVec[0] == "open") { //open command
		if (splitVec.size() < 2) {
			printf("arguments for open command is too low\n");
		} else {
			cmd.setType(CommandType_Open);
			CommandParam serverAddrParam;
			serverAddrParam.setType(ParamType_String);
			serverAddrParam.setName("serverAddr");
			serverAddrParam.setStringValue(splitVec[1]);
			cmd.addParam(serverAddrParam);
		}
	} else{
		printf("command %s is not supported yet\n", splitVec[0].c_str());
	}
	return cmd;
	//TODO:
}

