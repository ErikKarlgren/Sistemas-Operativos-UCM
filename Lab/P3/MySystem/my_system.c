#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

#define MY_SYSTEM
#define BASH_PATH "/bin/bash"

int my_system(const char *command)
{
	int status;
	int pid = fork();

	if (pid == 0)
	{
		// child process
		// printf("Command: %s -c %s\n", BASH_PATH, command);
		status = execl(BASH_PATH, "bash", "-c", command, (char*) NULL);
		// child should be dead after 'execl'
		// but if execl() has an error...
		exit(-1);
	}
	else if (pid > 0)
	{
		// parent process
		wait(&status);
	}
	else
	{
		// error on fork
		exit(-2);
	}
	return status;
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <command>\n", argv[0]);
		exit(1);
	}

#ifdef MY_SYSTEM
	return my_system(argv[1]);
#else
	return system(argv[1]);
#endif
}
