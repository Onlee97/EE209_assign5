#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	char *apcArgv[2];

	printf("testexec process (%d)\n", (int)getpid());

	apcArgv[0] = "./hello";
	apcArgv[1] = NULL;

	execvp(apcArgv[0], apcArgv);

	/* Should not be reachable if execvp() was successful. */
	perror(argv[0]);

	return EXIT_FAILURE;
}
