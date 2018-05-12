
#define _XOPEN_SOURCE

#include <stdlib.h>
#include <time.h>
#include "parameters.h"
#include "file_system_controller.h"


#define STDOUT        STDOUT_FILENO
#define LENGTH        100


int main(int argc, char **argv) {
	enum Operation op;
	enum Action action;
	struct tm time;
	char aux[LENGTH];
	list = 0;

	checkMainParameters(argc);
	op = getOperation(argc, argv[1]);


	switch (op) {
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
		case ACTION:
			action = getAction(argv[1]);
			if (action == DATE) {
				strptime(argv[2], DATE_INPUT_FORMAT, &time);
				print(getDate(aux, mktime(&time)));
				actionCommand(action, argv[3], argv[4], (uint32_t)mktime(&time));
			} else
				actionCommand(action, argv[2], argv[3], 0);
			break;
	}

	return EXIT_SUCCESS;
}