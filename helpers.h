/*
 * helpers.h
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#ifndef HELPERS_H_
#define HELPERS_H_

#include <iostream>
#include <string>

using namespace std;

string int2str(int value){
	char t[256];
	sprintf(t, "%d", value);
	string s = t;
	return s;
}


#endif /* HELPERS_H_ */
