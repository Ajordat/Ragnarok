#include <stdlib.h>

#include "file_system_controller.h"
#include "parameters.h"


#define STDOUT		STDOUT_FILENO
#define LENGTH		100



int main(int argc, char **argv) {
	enum Operation op;
	list = 0;

	checkMainParameters(argc);
	op = getOperation(argc, argv[1]);

	switch (op){
		case INFO:
			infoCommand(argv[2]);
			break;
		case SEARCH:
			search = 1;
			list = 1;
			searchCommand(argv[2], argv[3]);
			break;
		case LIST:
			list = 1;
			searchCommand("", argv[2]);
			break;
		case SHOW:
			show = 1;
			searchCommand(argv[2], argv[3]);
			break;
	}

	return EXIT_SUCCESS;
}