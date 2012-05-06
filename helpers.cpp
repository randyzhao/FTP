/*
 * helpers.cpp
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#include "helpers.h"
#include <vector>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
using namespace std;

string int2str(int value){
	char t[256];
	sprintf(t, "%d", value);
	string s = t;
	return s;
}
int extractCode(char* buf){
	string content = buf;
	return extractCode(content);
}
int extractCode(string msg){
	vector<int> code;
	boost::trim(msg);
	const char* m = msg.c_str();
	for (unsigned int i = 0; i < msg.size(); i++){
		if (m[i] <= 57 || m[i] >= 48){
			code.push_back(m[i] - 48);
		}else{
			break;
		}
	}
	int temp = 1;
	int count = 0;
	for (int i = code.size() - 1; i >= 0; i--){
		count += temp * code[i];
		temp *= 10;
	}
	return count;
}

extern string generateMsg(int code, string content)
{
	string c= boost::lexical_cast<string>(code);
	c =  c + content;
	return c;

}


