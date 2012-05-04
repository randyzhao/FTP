/*
 * main.cpp
 *
 *  Created on: May 4, 2012
 *      Author: randy
 */

#include "UI.h"
#include <stdio.h>

int main(int argc, char** argv) {
	char t = getopt(argc, argv, "sc");
	UI ui;
	switch (t) {
	case 's':
		//TODO: server
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
