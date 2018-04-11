//
// Created by alexj on 11/4/2018.
//

#include <stdio.h>
#include "utils.h"


inline void debug(char *string) {
#if DEBUG
	write(STDOUT, string, strlen(string));
#endif
}

inline void print(char *string) {
	write(STDOUT, string, strlen(string));
}

inline void printv(char *string, uint32_t v) {
	char aux[LENGTH];
	sprintf(aux, "%s%d", string, v);
	write(STDOUT, aux, strlen(aux));
}