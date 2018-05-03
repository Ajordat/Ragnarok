//
// Created by alexj on 23/4/2018.
//

#ifndef RAGNAROK_PARAMETERS_H
#define RAGNAROK_PARAMETERS_H

#include <stdlib.h>
#include "utils.h"

#define INFO_COMMAND		"--info"
#define SEARCH_COMMAND		"--search"
#define LIST_COMMAND		"--list"
#define SHOW_COMMAND		"--show"


enum Operation {
	INFO, SEARCH, LIST, SHOW
};


void checkMainParameters(int argc) {
	if (argc < 3 || argc > 4) {
		print("Wrong format. Use:\n\t./ragnarok <op> <name>\n");
		exit(EXIT_FAILURE);
	}
}

enum Operation getOperation(int argc, char *command) {
	char aux[LENGTH];


	if (!strcasecmp(command, INFO_COMMAND)) {
		if (argc != 3) {
			sprintf(aux, "Wrong format. Use:\n\t./ragnarok %s <file_system>\n", INFO_COMMAND);
			write(STDOUT, aux, strlen(aux));
			exit(EXIT_FAILURE);
		}
		return INFO;

	} else if (!strcasecmp(command, SEARCH_COMMAND)) {
		if (argc != 4) {
			sprintf(aux, "Wrong format. Use:\n\t./ragnarok %s <file> <file_system>\n", SEARCH_COMMAND);
			write(STDOUT, aux, strlen(aux));
			exit(EXIT_FAILURE);
		}
		return SEARCH;

	} else if (!strcasecmp(command, LIST_COMMAND)) {
		if (argc != 3) {
			sprintf(aux, "Wrong format. Use:\n\t./ragnarok %s <file_system>\n", LIST_COMMAND);
			write(STDOUT, aux, strlen(aux));
			exit(EXIT_FAILURE);
		}
		return LIST;

	} else if (!strcasecmp(command, SHOW_COMMAND)) {
		if (argc != 4) {
			sprintf(aux, "Wrong format. Use:\n\t./ragnarok %s <file> <file_system>\n", SHOW_COMMAND);
			write(STDOUT, aux, strlen(aux));
			exit(EXIT_FAILURE);
		}
		return SHOW;

	}else {
		print("Unrecognized command.\n");
		exit(EXIT_FAILURE);
	}
}


#endif //RAGNAROK_PARAMETERS_H
