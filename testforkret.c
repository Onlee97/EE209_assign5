#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	pid_t iPid;

	printf("Parent process (%d)\n", (int)getpid());
	fflush(NULL);

	iPid = fork();
	if (iPid == -1) {
		perror(argv[0]);
		return EXIT_FAILURE;
	}
	
	if (iPid == 0) {
		printf("Child process (%d)\n", (int)getpid());
	} else {
		printf("Parent process (%d)\n", (int)getpid());
	}
	printf("Parent and child processes (%d)\n", (int)getpid());

	return 0;
}
