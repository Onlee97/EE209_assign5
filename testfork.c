#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	pid_t iRet;

	printf("Parent process (%d)\n", (int)getpid());
	fflush(NULL);

	iRet = fork();
	if (iRet == -1) {
		perror(argv[0]);
		return EXIT_FAILURE;
	}

	printf("Parent and child processes (%d)\n", (int)getpid());

	return 0;
}
