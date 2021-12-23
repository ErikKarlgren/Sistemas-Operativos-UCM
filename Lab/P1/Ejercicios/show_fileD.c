#include <err.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  FILE *file = NULL;
  int n_bytes = (argc == 3) ? atoi(argv[2]) : 1;
  int bytes_read;
  char *c = malloc(sizeof(char) * n_bytes);

  if (argc != 2 && argc != 3) {
    fprintf(stderr, "Usage: %s <file_name> [number_of_bytes]\n", argv[0]);
    exit(1);
  }
  /*Open file*/
  if ((file = fopen(argv[1], "r")) == NULL)
    err(2, "The input file %s could not be opened", argv[1]);

  /*Read file byte by byte*/
  // while ((c = getc(file)) != EOF) {
  while ((bytes_read = fread(c, sizeof(char), n_bytes, file)) != 0) {
    /*Print byte to stdout*/
    fwrite(c, sizeof(char), bytes_read, stdout);
  }
  fclose(file);
  return 0;
}
