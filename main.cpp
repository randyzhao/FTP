/*
 * main.cpp
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#include "UI.h"
#include "Server.h"
#include <stdio.h>

int main(int argc, char** argv) {
	char t = getopt(argc, argv, "sc");
	UI ui;
	Server ser;
	switch (t) {
	case 's':
		ser.run();
		break;
	case 'c':
		//TODO:
		ui.run();
		break;
	default:
		printf("error usage");
		break;
	}
	return 0;
}
