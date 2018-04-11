//
// Created by alexj on 11/4/2018.
//

#ifndef _UTILS_H_
#define _UTILS_H_


#include <unistd.h>
#include <string.h>
#include <stdint-gcc.h>
#include <stdio.h>
#include <time.h>

#define DEBUG 1
#define STDOUT STDOUT_FILENO
#define LENGTH 100
#define DATE_FORMAT "%a %d-%m-%Y %H:%M:%S %Z"	//https://www.tutorialspoint.com/c_standard_library/c_function_strftime.htm


inline void debug(char *);

inline void print(char *);

void printv(char *string, uint32_t v);

char *getDate(char * dest, time_t date);


#endif //RAGNAROK_UTILS_H
