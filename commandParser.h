/*
 * CommandParser.h
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#ifndef COMMANDPARSER_H_
#define COMMANDPARSER_H_


#include "command.h"
class CommandParser{
public:
	static Command parseCommand(string com);
};

#endif /* COMMANDPARSER_H_ */
