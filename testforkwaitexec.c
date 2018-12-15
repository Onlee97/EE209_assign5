#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	pid_t iPid;

	for (;;) {
		fflush(NULL);

		iPid = fork();
		if (iPid == -1) {
			perror(argv[0]);
			return EXIT_FAILURE;
		}
		
		if (iPid == 0) {
			char *apcArgv[2];
			apcArgv[0] = "date";
			apcArgv[1] = NULL;

			execvp(apcArgv[0], apcArgv);
			perror(argv[0]);
			exit(EXIT_FAILURE);
		}

		iPid = wait(NULL);
		if (iPid == -1) {
			perror(argv[0]);
			return EXIT_FAILURE;
		}

		sleep(3);
	}
	/* Never should reach this point. */

	return 0;
}
