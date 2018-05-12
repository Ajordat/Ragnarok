//
// Created by alexj on 23/4/2018.
//

#ifndef RAGNAROK_PARAMETERS_H
#define RAGNAROK_PARAMETERS_H


#include <stdlib.h>
#include "utils.h"

#define INFO_CMD        "--info"
#define SEARCH_CMD        "--search"
#define LIST_CMD        "--list"
#define SHOW_CMD        "--show"

#define ACTION_CMD_READ        "-r"
#define ACTION_CMD_WRITE        "-w"
#define ACTION_CMD_HIDE        "-h"
#define ACTION_CMD_SHOW        "-s"
#define ACTION_CMD_DATE        "-d"

#define DATE_INPUT_FORMAT			"%d/%m/%Y"

enum Operation {
	INFO, SEARCH, LIST, SHOW, ACTION
};

enum Action {
	READ_ONLY, WRITE, HIDE, SHOW_HIDDEN, DATE
};



enum Action getAction(char *command);

void checkMainParameters(int argc);

enum Operation getOperation(int argc, char *command);



#endif //RAGNAROK_PARAMETERS_H
