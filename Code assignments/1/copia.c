#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>


#define ENTRADA "entrada.txt"
#define SALIDA "salida.txt"

int main( void )
{
  char buffer;

  int fdin = open( ENTRADA, O_RDONLY );
  int fdout = open( SALIDA, O_WRONLY | O_CREAT | O_TRUNC, 0777 );

  while( read( fdin, &buffer, sizeof( char ) /* 1 */ ) != 0 ) {
    write( fdout, &buffer, sizeof( char ) /* 1 */ );
  }

  close( fdin );
  close( fdout );

  return 0;
}
