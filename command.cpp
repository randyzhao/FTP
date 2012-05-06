/*
 * Command.cpp
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#include "command.h"
#include <cassert>

string Command::getArg(int id)
{
	string temp;
	//check for the size of args
	if (this->args.size() > (unsigned)id){
		return this->args[id];
	}else{
		return temp;
	}
}



CommandType Command::getType() const
{
	return this->type;
}

void Command::setType(CommandType type)
{
	this->type = type;
}

void Command::addParam(string arg)
{
	this->args.push_back(arg);
}








