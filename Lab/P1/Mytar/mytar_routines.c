#include "mytar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char *use;

#define DEBUG 1
/**
 * Print current file position.
 */
void print_cfp(FILE *file)
{
  if (DEBUG)
    printf("File position: %ld\n", ftell(file));
}

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination
 * file nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int copynFile(FILE *origin, FILE *destination, int nBytes)
{
  if (origin == NULL || destination == NULL)
    return -1;
  if (nBytes < 0)
    return -1;

  int bytes_read = 0;

  print_cfp(origin);
  printf("Copying contents...\n");

  for (bytes_read = 0; bytes_read < nBytes; bytes_read++)
  {
    int c;
    if ((c = getc(origin)) == EOF)
      break;
    putc(c, destination);
    printf("%c", c);
  }

  printf("\nBytes copied: %d\n", bytes_read);
  print_cfp(origin);

  return bytes_read;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor
 *
 * The loadstr() function must allocate memory from the heap to store
 * the contents of the string read from the FILE.
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc())
 *
 * Returns: !=NULL if success, NULL if error
 */
char *loadstr(FILE *file)
{
  // Complete the function
  if (file == NULL)
    return NULL;

  // name of file is expected to be short
  char buffer[1024];
  char *trimmed_buffer;
  int n_bytes = 0;
  int ok = 1;

  while (ok == 1)
  {
    char c = getc(file);
    ok = (c == '\0') ? 0 : 1;
    buffer[n_bytes++] = c;
  }

  trimmed_buffer = (char *)malloc(sizeof(char) + (n_bytes + 1));
  strncpy(trimmed_buffer, buffer, n_bytes);
  trimmed_buffer[n_bytes] = '\0';

  return trimmed_buffer;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor
 * nFiles: output parameter. Used to return the number
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores
 * the (name,size) pairs read from the tar file. Upon failure, the function
 * returns NULL.
 */
stHeaderEntry *readHeader(FILE *tarFile, int *nFiles)
{
  if (tarFile == NULL)
    return NULL;

  stHeaderEntry *entry;
  int num_files;

  // equivalent to fseek(tarFile, 0 SEEK_SET)
  rewind(tarFile);
  fread(&num_files, sizeof(int), 1, tarFile);
  print_cfp(tarFile);

  if (num_files <= 0)
  {
    fprintf(stderr, "Numero de ficheros ilegal (%d)\n", num_files);
    exit(1);
  }
  else
  {
    printf("Found %d files\n", num_files);
  }

  entry = (stHeaderEntry *)malloc(num_files * sizeof(stHeaderEntry));

  for (int i = 0; i < num_files; i++)
  {
    char *dir;
    int size;

    dir = loadstr(tarFile);
    print_cfp(tarFile);
    if (dir == NULL)
    {
      fprintf(stderr, "Error when reading directory: %s\n", dir);
      exit(1);
    }
    printf("String loaded: \"%s\" (length: %lu characters)\n", dir, strlen(dir));

    fread(&size, sizeof(int), 1, tarFile);
    print_cfp(tarFile);
    printf("Size of contents: %d\n", size);

    entry[i].name = (char *)malloc(sizeof(char) * (1 + strlen(dir)));
    strcpy(entry[i].name, dir);
    entry[i].size = size;

    free(dir);
  }

  (*nFiles) = num_files;
  return entry;
}

int calculateOffData(int nFiles, char *fileNames[])
{
  int offData = nFiles * sizeof(unsigned int) + sizeof(int);
  for (int i = 0; i < nFiles; i++)
  {
    offData += strlen(fileNames[i]) + 1;
  }
  return offData;
}

void copyToTarFile(int nFiles, char *fileNames[], FILE *file,
                   stHeaderEntry *stEntries, int offData)
{
  if (fileNames == NULL || file == NULL || stEntries == NULL)
  {
    fprintf(stderr, "Error: one argument is null in copyToTarFile()\n");
    exit(1);
  }

  fseek(file, offData, SEEK_SET);
  for (int i = 0; i < nFiles; i++)
  {
    if (fileNames[i] == NULL)
    {
      fprintf(stderr, "Error: filename is null\n");
      exit(1);
    }
    else
    {
      printf("Copying contents of file %s\n", fileNames[i]);
    }

    FILE *origin = fopen(fileNames[i], "r");
    int bytes_copied = copynFile(origin, file, INT_MAX);

    stEntries[i].size = (unsigned int)bytes_copied;
    // Necesitamos 1 byte más para poder copiar el byte de fin de string ('\0')
    // Sin ese byte extra estaríamos escribiendo 1 byte fuera de rango
    stEntries[i].name = (char *)malloc(sizeof(char) * (strlen(fileNames[i]) + 1));
    strcpy(stEntries[i].name, fileNames[i]);

    fclose(origin);
    printf("Copied file %s (size: %u)\n", stEntries[i].name, stEntries[i].size);
  }
}

void writeHeaderEntries(int nFiles, FILE *file, const stHeaderEntry *stEntries)
{
  for (int i = 0; i < nFiles; i++)
  {
    printf("Writing header of file %s (size: %u)\n", stEntries[i].name,
           stEntries[i].size);
    fwrite(stEntries[i].name, strlen(stEntries[i].name) + 1, 1, file);
    fwrite(&stEntries[i].size, sizeof(int), 1, file);
  }
}

/** Creates a tarball archive
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the
 * tarball tarname: name of the tarball archive
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE.
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball
 * archive. At the same time, build the representation of the tarball header in
 * memory. Finally, rewind the file's position indicator, write the number of
 * files as well as the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple
 * sizeof of stHeaderEntry will not work. Bear in mind that, on disk, file names
 * found in (name,size) pairs occupy strlen(name)+1 bytes.
 *
 */
int createTar(int nFiles, char *fileNames[], char tarName[])
{
  if (fileNames == NULL || tarName == NULL || nFiles <= 0)
    return EXIT_FAILURE;

  FILE *file = fopen(tarName, "w+");
  int offData = calculateOffData(nFiles, fileNames);
  stHeaderEntry *stEntries = (stHeaderEntry *)calloc(nFiles, sizeof(stHeaderEntry));

  printf("Number of files: %d\n", nFiles);
  copyToTarFile(nFiles, fileNames, file, stEntries, offData);

  fseek(file, 0, SEEK_SET);
  fwrite(&nFiles, sizeof(int), 1, file);
  writeHeaderEntries(nFiles, file, stEntries);

  fclose(file);

  for (int i = 0; i < nFiles; i++)
    free(stEntries[i].name);
  free(stEntries);
  return EXIT_SUCCESS;
}

void copyFromTarFile(int nFiles, FILE *file, stHeaderEntry *stEntries)
{
  for (int i = 0; i < nFiles; i++)
  {
    FILE *destiny;
    unsigned int bytes_copied;

    if (stEntries[i].name == NULL)
    {
      fprintf(stderr, "Error: filename is NULL\n");
      exit(1);
    }
    else
    {
      printf("Creating file %s...\n", stEntries[i].name);
    }

    destiny = fopen(stEntries[i].name, "w");
    bytes_copied = copynFile(file, destiny, stEntries[i].size);
    fclose(destiny);

    printf("Copied %u bytes\n", bytes_copied);
    if (bytes_copied != stEntries[i].size)
      fprintf(stderr, "WARNING: %u bytes were supposed to be copied, but actually %u bytes were copied\n",
              stEntries[i].size, bytes_copied);
  }
}
/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE.
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the
 * tarball's data section. By using information from the
 * header --number of files and (file name, file size) pairs--, extract files
 * stored in the data section of the tarball.
 *
 */
int extractTar(char tarName[])
{
  int nFiles;
  stHeaderEntry *stEntries;
  FILE *tarSource;

  if (tarName == NULL)
    return EXIT_FAILURE;

  tarSource = fopen(tarName, "r");
  if (tarSource == NULL)
    return EXIT_FAILURE;

  fread(&nFiles, sizeof(int), 1, tarSource);
  stEntries = readHeader(tarSource, &nFiles);
  copyFromTarFile(nFiles, tarSource, stEntries);

  fclose(tarSource);

  for (int i = 0; i < nFiles; i++)
    free(stEntries[i].name);
  free(stEntries);

  return EXIT_SUCCESS;
}
