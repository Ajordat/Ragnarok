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

#define DEBUG 			0
#define STDOUT 			STDOUT_FILENO
#define LENGTH 			100
#define DATE_FORMAT		"%a %d-%m-%Y %H:%M:%S %Z"    //https://www.tutorialspoint.com/c_standard_library/c_function_strftime.htm
#define COLOR_DEBUG		"\x1b[33m"    //YELLOW
#define COLOR_RESET		"\x1b[0m"

#define println()	write(STDOUT,"\n",1)
#if DEBUG
	#define debugln()	write(STDOUT,"\n",1)
#else
	#define debugln()
#endif


void printByte(uint8_t byte);

inline void debug(const char *);

void debugv(const char *string, uint64_t v);

void debugvh(const char *string, uint32_t v);

inline void print(char *);

inline void printc(char character);

void printv(char *string, uint64_t v);

char *getDate(char *dest, time_t date);


#endif //RAGNAROK_UTILS_H
