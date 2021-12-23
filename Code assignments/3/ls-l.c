#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUF 256

int main(int argc, char **argv) {
  DIR *dirp;
  struct dirent *dp;
  char buf[MAX_BUF];

  /* print current directory */
  getcwd(buf, MAX_BUF);
  printf("Current directory: %s\n", buf);

  if (argc == 1)
    dirp = opendir(".");
  else
    dirp = opendir(argv[1]);

  if (dirp == NULL)
    fprintf(stderr, "Cannot open %s\n", argc == 1 ? buf : argv[1]);
  else {
    /* reads entry by entry */
    printf("Size\t I-node\t Permissions\t Type\t Date\t Name\n");
    while ((dp = readdir(dirp)) != NULL) {
      struct stat *st;
      if (stat(dp->d_name, st) != 0) {
        printf("%ld\t %ld\t %s\t %s\t %s\t %s\n", st->st_size, st->st_ino,
               "permissions", /*st->st_mode*/ "type", "date", dp->d_name);
        printf("%s\n", dp->d_name);
      }
    }
    closedir(dirp);
  }
  return 0;
}
