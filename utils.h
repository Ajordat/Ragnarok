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

#define DEBUG                1
#define HIDDEN_FILES        1
#define STDOUT                STDOUT_FILENO
#define LENGTH                100
#define DATE_FORMAT            "%a %d-%m-%Y %H:%M:%S %Z"    //https://www.tutorialspoint.com/c_standard_library/c_function_strftime.htm
#define FAT32_DATE_FORMAT    "%d-%d-%d %d:%d:%d"        //"%d-%m-%Y %H:%M:%S"
#define COLOR_DEBUG            "\x1b[33m"    //YELLOW
#define COLOR_RESET            "\x1b[0m"

#define println()    write(STDOUT,"\n",1)
#if DEBUG
#define debugln()    write(STDOUT,"\n",1)
#else
#define debugln()
#endif
#define getBase(fs) (lseek((fs), 0, SEEK_CUR))
#define recoverBase(fs, offset) (lseek((fs), (offset), SEEK_SET))


int list;
int depth;
int search;
int show;

void printByte(uint8_t byte);

inline void debug(const char *);

void debugv(const char *string, long v);

void debugvh(const char *string, uint32_t v);

inline void print(char *);

inline void printc(char character);

void printv(char *string, uint64_t v);

char *getDate(char *dest, time_t date);

char *getFat32Date(char *dest, struct tm date);

void printMemory(int fs, int size);

void listFile(char *name);

int byteInString(uint8_t byte, const char *string);



#endif //RAGNAROK_UTILS_H
