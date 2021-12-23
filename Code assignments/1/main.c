#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv){
	char buffer;
	int fdin = open("entrada.txt", O_RDONLY);
	int fdout = open("salida.txt", O_CREAT | O_WRONLY | O_TRUNC, 0777);
	int nbytes = (argc == 1) ? 1 : atoi(argv[1]);

	while(read(fdin, &buffer, nbytes) != 0) {
		write(fdout, &buffer, nbytes);
	}
	close(fdin);
	close(fdout);
	return 0;
}
