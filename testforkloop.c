#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	pid_t iPid;
	int i = 0;

	iPid = fork();
	if (iPid == -1) {
		perror(argv[0]);
		return EXIT_FAILURE;
	}
	if (iPid == 0) {
		for (i = 0; i < 1000; i++)
			printf("Child process (%d): %d\n", (int)getpid(), i);
		exit(0);
	} 
	
	for (i = 0; i < 1000; i++)
		printf("Parent process (%d): %d\n", (int)getpid(), i);

	return 0;
}
