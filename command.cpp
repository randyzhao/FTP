/*
 * Command.cpp
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#include "command.h"
#include <cassert>


double CommandParam::getDoubleValue() const
{
	assert(type == ParamType_Double);
    return doubleValue;
}

int CommandParam::getIntValue() const
{
	assert(type == ParamType_Int);
    return intValue;
}

string CommandParam::getName() const
{

    return name;
}

string CommandParam::getStringValue() const
{
	assert(type == ParamType_String);
    return stringValue;
}

ParamType CommandParam::getType() const
{
    return type;
}

void CommandParam::setDoubleValue(double doubleValue)
{
	assert(type == ParamType_Double);
    this->doubleValue = doubleValue;
}

void CommandParam::setIntValue(int intValue)
{
	assert(type == ParamType_Int);
    this->intValue = intValue;
}

void CommandParam::setName(string name)
{
    this->name = name;
}

void CommandParam::setStringValue(string stringValue)
{
	assert(type == ParamType_String);
    this->stringValue = stringValue;
}

void CommandParam::setType(ParamType type)
{
    this->type = type;
}

CommandType Command::getType() const
{
    return type;
}

void Command::addParam(CommandParam param)
{
	this->params[param.getName()] = param;
}

void Command::setType(CommandType type)
{
    this->type = type;
}

CommandParam Command::getParam(string name)
{
    if(params.find(name) == params.end()){
        CommandParam param;
        param.setType(ParamType_Error);
        return param;
    }else{ //param name found
        return params[name];
    }
}






