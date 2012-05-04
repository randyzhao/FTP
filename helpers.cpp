/*
 * helpers.cpp
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#include "helpers.h"


string int2str(int value){
	char t[256];
	sprintf(t, "%d", value);
	string s = t;
	return s;
}
