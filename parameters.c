
#include "parameters.h"


int isActionCommand(char *command) {

	return !strcmp(command, ACTION_CMD_READ) || !strcmp(command, ACTION_CMD_WRITE) ||
		   !strcmp(command, ACTION_CMD_HIDE) || !strcmp(command, ACTION_CMD_SHOW) || !strcmp(command, ACTION_CMD_DATE);
}

enum Action getAction(char *command) {

	if (!strcmp(command, ACTION_CMD_READ))
		return READ_ONLY;
	if (!strcmp(command, ACTION_CMD_WRITE))
		return WRITE;
	if (!strcmp(command, ACTION_CMD_HIDE))
		return HIDE;
	if (!strcmp(command, ACTION_CMD_SHOW))
		return SHOW_HIDDEN;
	return DATE;
}

void checkMainParameters(int argc) {
	if (argc < 3 || argc > 5) {
		print("Wrong format. Use:\n\t./ragnarok <op> <name>\n");
		exit(EXIT_FAILURE);
	}
}

enum Operation getOperation(int argc, char *command) {
	char aux[LENGTH];
	char action_name[10];
	enum Action action;

	if (!strcasecmp(command, INFO_CMD)) {
		if (argc != 3) {
			sprintf(aux, "Wrong format. Use:\n\t./ragnarok %s <file_system>\n", INFO_CMD);
			write(STDOUT, aux, strlen(aux));
			exit(EXIT_FAILURE);
		}
		return INFO;

	} else if (!strcasecmp(command, SEARCH_CMD)) {
		if (argc != 4) {
			sprintf(aux, "Wrong format. Use:\n\t./ragnarok %s <file> <file_system>\n", SEARCH_CMD);
			write(STDOUT, aux, strlen(aux));
			exit(EXIT_FAILURE);
		}
		return SEARCH;

	} else if (!strcasecmp(command, LIST_CMD)) {
		if (argc != 3) {
			sprintf(aux, "Wrong format. Use:\n\t./ragnarok %s <file_system>\n", LIST_CMD);
			write(STDOUT, aux, strlen(aux));
			exit(EXIT_FAILURE);
		}
		return LIST;

	} else if (!strcasecmp(command, SHOW_CMD)) {
		if (argc != 4) {
			sprintf(aux, "Wrong format. Use:\n\t./ragnarok %s <file> <file_system>\n", SHOW_CMD);
			write(STDOUT, aux, strlen(aux));
			exit(EXIT_FAILURE);
		}
		return SHOW;
	} else if (isActionCommand(command)) {
		action = getAction(command);

		if (action == DATE) {
			if (argc != 5) {
				sprintf(aux, "Wrong format. Use:\n\t./ragnarok %s <sec_since_epoch> <file> <file_system>\nWhere <sec_since_epoch> must have format ddmmyyyy\n",
						ACTION_CMD_DATE);
				write(STDOUT, aux, strlen(aux));
				exit(EXIT_FAILURE);
			}
		} else if (argc != 4) {
			memset(action_name, '\0', 10);
			switch (action) {
				case READ_ONLY:
					strcpy(action_name, ACTION_CMD_READ);
				case WRITE:
					strcpy(action_name, ACTION_CMD_WRITE);
				case HIDE:
					strcpy(action_name, ACTION_CMD_HIDE);
				case SHOW_HIDDEN:
					strcpy(action_name, ACTION_CMD_SHOW);
				case DATE:
					break;
			}

			sprintf(aux, "Wrong format. Use:\n\t./ragnarok %s <file> <file_system>\n", action_name);
			write(STDOUT, aux, strlen(aux));
			exit(EXIT_FAILURE);
		}
		return ACTION;
	} else {
		print("Unrecognized command.\n");
		exit(EXIT_FAILURE);
	}
}

