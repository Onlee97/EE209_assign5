#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	pid_t iPid;
	int i = 0;
	int iStatus;

	iPid = fork();
	if (iPid == 0) {
		for (i = 0; i < 1000; i++)
			printf("Child process (%d): %d\n", (int)getpid(), i);
		exit(0);
	} 

	iPid = wait(&iStatus);
	if (iPid == -1) {
		perror(argv[0]);
		return EXIT_FAILURE;
	}

	printf("Child process terminated with status %d.\n",
		   WEXITSTATUS(iStatus));

	for (i = 0; i < 1000; i++)
		printf("Parent process (%d): %d\n", (int)getpid(), i);

	return 0;
}
