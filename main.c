#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "file_system_controller.h"


#define STDOUT STDOUT_FILENO
#define LENGTH 100

#define INFO_COMMAND "--info"


int main(int argc, char **argv) {
	char aux[LENGTH];

	if (argc == 1) {
		print("Wrong format. Use:\n\t./ragnarok <op> <name>\n");
		return EXIT_FAILURE;
	}

	if (!strcasecmp(argv[1], INFO_COMMAND)) {
		if (argc != 3) {
			sprintf(aux, "Wrong format. Use:\n\t./ragnarok %s <name>\n", INFO_COMMAND);
			write(STDOUT, aux, strlen(aux));
			return EXIT_FAILURE;
		}

		infoCommand(argv[2]);

	} else {
		print("Unrecognized command.\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}