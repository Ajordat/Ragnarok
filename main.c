#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define STDOUT STDOUT_FILENO
#define LENGTH 100

int main(int argc, char **argv) {
	char aux[LENGTH];


	if (argc == 1){
		sprintf(aux, "Wrong format. Use:\n\t./ragnarok <op> <name>\n");
		write(STDOUT, aux, strlen(aux));
	}


	return 0;
}