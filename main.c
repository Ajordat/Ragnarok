#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/fcntl.h>

#include "file_system_controller.h"
#include "ext4.h"


#define STDOUT STDOUT_FILENO
#define LENGTH 100

#define INFO_COMMAND "info"

int main(int argc, char **argv) {
	char aux[LENGTH];
	int fs;
	enum Format format;

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

		fs = open(argv[2], O_RDONLY);
		if (fs <= 0) {
			print("The file doesn't exist.\n");
			return EXIT_FAILURE;
		}

		format = getFormat(fs);

		switch (format) {
			case EXT4:
				printExt4(extractExt4(fs));
				break;
			case FAT32:
				break;
			default:
				break;
		}
		close(fs);

	} else {
		print("Unrecognized command.\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}