//
// Created by alexj on 11/4/2018.
//

#ifndef _UTILS_H_
#define _UTILS_H_


#include <unistd.h>
#include <string.h>
#include <stdint-gcc.h>

#define DEBUG 1
#define STDOUT STDOUT_FILENO
#define LENGTH 100


void debug(char *);

inline void print(char *);

inline void printv(char *string, uint32_t v);


#endif //RAGNAROK_UTILS_H
