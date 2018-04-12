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

inline void print(char *string) {
	write(STDOUT, string, strlen(string));
}

void printv(char *string, uint32_t v) {
	char aux[LENGTH];
	memset(aux, '\0', LENGTH);
	sprintf(aux, "%s%d", string, v);
	write(STDOUT, aux, strlen(aux));
}

char *getDate(char * dest, time_t date) {
	struct tm ts;

	ts = *localtime(&date);
	strftime(dest, LENGTH, DATE_FORMAT, &ts);
	return dest;
}
