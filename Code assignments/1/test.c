#include <sys/types.h>
// ...

#define ENTRADA "entrada.txt"
#define SALIDA "salida.txt"

int main(void) {
    // usa "man 2 read" para leer
    // sobre read() (llamada al
    // sistema, de ahí el 2)
    char buffer;
    int fdin = open(ENTRADA, O_RDONLY);

    // abrimos en modo escritura, o se crea si no existe, o se
    // 'trunca'? lo que tiene
    int fdout = open(SALIDA, O_WRONLY | O_CREAT | O_TRUNC, 0777);

	// leemos byte a byte mientras se pueda leer de fdin
    while (read(fdin,
                &buffer, // pasamos la dirección de 'buffer'
                sizeof(char) /* 1 byte */) != 0)
	{
        write(fdout, &buffer, sizeof(char));
    }

    close(fdin);
    close(fdout);

    return 0;
}