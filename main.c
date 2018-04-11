#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/fcntl.h>

#include "file_system_controller.h"
#include "ext4.h"


#define STDOUT STDOUT_FILENO
#define LENGTH 100

int main(int argc, char **argv) {
	char aux[LENGTH];
	int fs;
	enum Format format;

	if (argc == 1) {
		sprintf(aux, "Wrong format. Use:\n\t./ragnarok <op> <name>\n");
		write(STDOUT, aux, strlen(aux));
		return EXIT_FAILURE;
	}

	fs = open(argv[1], O_RDONLY);
	if (fs <= 0) {
		sprintf(aux, "The file doesn't exist.\n");
		write(STDOUT_FILENO, aux, strlen(aux));
		return EXIT_FAILURE;
	}

	format = getFormat(fs);

	if (format == EXT4) {
		printExt4(extractExt4(fs));
	}

	close(fs);

	return EXIT_SUCCESS;
}