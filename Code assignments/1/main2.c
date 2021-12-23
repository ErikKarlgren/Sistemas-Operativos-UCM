#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv){
	int nbytes = (argc == 1) ? 1 : atoi(argv[1]);
	char buffer[INT_MAX];
	FILE* fdin;

	if((fdin = fopen("entrada.txt", "r")) != NULL) {
		FILE* fdout = fopen("salida.txt", "w");
		int bytes_read;

		// Reads as many bytes (since a 'char' has a size of 1 byte)
		// as the value of 'nbytes' from 'fdin' (input file) and
		// writes them into the 'buffer' as long as there are bytes
		// left to be read.

		while((bytes_read = fread(buffer, sizeof(char), nbytes, fdin))!= 0) {
			// Writes to 'fdout' as many bytes that have been
			// read from the last fread call.
			fwrite(buffer, sizeof(char), bytes_read, fdout);
		}
		fclose(fdout);
	}
	else {
		printf("File \"entrada.txt\" was not found");
		error(1);
	}
	fclose(fdin);
	return 0;
}
