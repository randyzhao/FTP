/*
 * Command.h
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <string>
#include <vector>
using namespace std;


enum CommandType{
	CommandType_Get,
	CommandType_Exit,
	CommandType_Open,
	CommandType_Dir,
	CommandType_Error
};

class Command
{
private:
    CommandType type;
    vector<string> args;
public:
    void setType(CommandType type);
    CommandType getType() const;
    string getArg(int id);
    void addParam(string arg);

};


#endif /* COMMAND_H_ */
