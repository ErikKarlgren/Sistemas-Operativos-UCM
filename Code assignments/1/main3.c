// ENTREGA FINAL: por fin me ha funcionado! :D

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define ENTRADA "entrada.txt"
#define SALIDA "salida.txt"
 
int main(int argc, char** argv)
{
  // Number of bytes to read each step.
  int nbytes = (argc == 1) ? 1: atoi(argv[1]);
  // Actual number of bytes read after a system call to 'read()'
  int read_bytes;
  // Buffer that can store as many characters as bytes specified by
  // 'nbytes'.
  char* buffer = malloc(sizeof(char) * nbytes);

  // You should check if 'buffer' is NULL; maybe you couldn't have allocated memory, you know?
  
 
  int fdin = open( ENTRADA, O_RDONLY );
  int fdout = open( SALIDA, O_WRONLY | O_CREAT | O_TRUNC, 0777 );
 
  // We read at most as many bytes as specified by 'nbytes' each step (could
  // be less if file doesn't have enough bytes left to be read), and then 
  // write them into 'salida.txt'.
  while ((read_bytes = read(fdin, buffer, sizeof(char) * nbytes)) != 0 ) {
    write( fdout, buffer, sizeof(char) * read_bytes);
  }
 
  close(fdin);
  close(fdout);
  free(buffer);
 
  return 0;
}
 
