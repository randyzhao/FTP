/*
 * Command.h
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <string>
#include <map>
using namespace std;

enum ParamType{
	ParamType_Int,
	ParamType_String,
	ParamType_Double,
	ParamType_Error
};

class CommandParam
{
private:
    string name;
    int intValue;
    string stringValue;
    double doubleValue;
    ParamType type;
public:
    double getDoubleValue() const;
    int getIntValue() const;
    string getName() const;
    string getStringValue() const;
    ParamType getType() const;
    void setDoubleValue(double doubleValue);
    void setIntValue(int intValue);
    void setName(string name);
    void setStringValue(string stringValue);
    void setType(ParamType type);
};

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
    map<string, CommandParam> params;
public:
    CommandParam getParam(string name);

    void addParam(CommandParam param);
    CommandType getType() const;
    void setType(CommandType type);

};


#endif /* COMMAND_H_ */
