//
// Created by alexj on 11/4/2018.
//

#include "utils.h"


inline void debug(const char *string) {
#if DEBUG
	write(STDOUT, COLOR_DEBUG, strlen(COLOR_DEBUG));
	write(STDOUT, string, strlen(string));
	write(STDOUT, COLOR_RESET, strlen(COLOR_RESET));
#else
	(void) (string);
#endif
}

void debugv(const char *string, uint64_t v) {
	char aux[LENGTH];

	sprintf(aux, "%s: %lu", string, v);
	debug(aux);
}

void debugvh(const char *string, uint32_t v) {
	char aux[LENGTH];

	sprintf(aux, "%s: 0x%X", string, v);
	debug(aux);
}

inline void print(char *string) {
	write(STDOUT, string, strlen(string));
}

inline void printc(char character) {
	char c[1];
	c[0] = character;
	write(STDOUT, c, 1);
}

void printByte(uint8_t byte) {
	char aux[10];
	memset(aux, '\0', 10);
	if ((byte >= 'a' && byte <= 'z') || (byte >= 'A' && byte <= 'Z') || (byte >= '0' && byte <= '9') || byte == '.' ||
		byte == ' ') {
		sprintf(aux, "%c", byte);
	} else {
		sprintf(aux, "0x%X", byte);
	}
	write(STDOUT, aux, strlen(aux));
}

void printv(char *string, uint64_t v) {
	char aux[LENGTH];
	memset(aux, '\0', LENGTH);
	sprintf(aux, "%s: %lu", string, v);
	write(STDOUT, aux, strlen(aux));
}

char *getDate(char *dest, time_t date) {
	struct tm ts;

	ts = *localtime(&date);
	strftime(dest, LENGTH, DATE_FORMAT, &ts);
	return dest;
}
