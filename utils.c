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

void debugv(const char *string, long v) {
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

char *getFat32Date(char *dest, struct tm date) {
	sprintf(dest, FAT32_DATE_FORMAT, date.tm_mday, date.tm_mon, date.tm_year, date.tm_hour, date.tm_min,
			date.tm_sec);
	return dest;
}

uint32_t getChecksumString(const char *string, uint32_t size) {
	int bit;
	uint32_t checksum = 0;

	while (size--)
		for (bit = 0; bit < 8; bit++)
			checksum += (string[size] >> bit) & 0x01;

	return checksum;
}

uint8_t getChecksumValue(uint64_t value) {
	int bit;
	uint8_t checksum = 0;

	for (bit = 0; bit < 64; bit++)
		checksum += (value >> bit) & 0x01;

	return checksum;
}

void printMemory(int fs, int size) {
	off_t offset = getBase(fs);
	uint8_t byte;
	int i;

	for (i = 0; i < size; i++) {
		read(fs, &byte, sizeof(uint8_t));
		printv("Byte", (uint64_t) i);
		print(": ");
		printByte(byte);
		println();
	}

	recoverBase(fs, offset);
}

void listFile(char *name) {
	int i;

//	if (name[0] == '.') return;

	if (depth)
		print("|");

	for (i = 0; i < depth; i++) {
		print("  ");
	}
	print("|-");
	print(name);
	println();
}