#include "dynarray.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <inttypes.h>

#define MAXLINE		1024
#define MAXARGS		128

int file_opened = 1;

void eval(char *cmdline);
int is_space(char c);
int is_newline(char c);
int is_char(char c);
int is_pipe(char c);
int is_dquote(char c);
static int lexLine(const char *cmdline, DynArray_T token_list);
static void printToken(void *pvItem, void *pvExtra); //for debugging
char * makeToken(char *tokenValue); 
int synLine(DynArray_T token_list);


void eval(char *cmdline) {
	pid_t pid;
	int pipe_num = 0; // Number of pipe using for implement pipe
	DynArray_T token_list;
	token_list = DynArray_new(0);
      if (token_list == NULL)
  	    {
        	fprintf(stderr, "Cannot allocate memory\n");
         	exit(EXIT_FAILURE);
      	}
      	if (!lexLine(cmdline, token_list))
      	{
          	return;
      	}

      	if ((pipe_num = synLine(token_list)) == -1) 
      	{
      		return;	
      	}
      	printf("pipe_num: %d\n", pipe_num);
      	if (DynArray_getLength(token_list) == 0)
      		return;
      	//DynArray_map(process_list, printProcess, NULL);
		//printf("length process: %d\n", DynArray_getLength(process_list));

      	/*if (argv[0] == NULL) //Ignore empty lines
      		return;*/
      	//Print out token list (for debugging)
      	printf("run cmd: ");
      	DynArray_map(token_list, printToken, NULL);
      	printf("\n");
		
      	/*fflush(NULL);
      	if ((pid = fork()) == 0) { // Run child process
          if (execvp(argv[0], argv) < 0) {
             printf("%s: Command not found.\n", argv[0]);
             exit(0);
          }
       	}
       	pid = wait(NULL);*/
 
    DynArray_free(token_list);
}

int synLine(DynArray_T token_list)
/* Syntaxically analyze the array of token. Return number of pipe if 
   successful, or -1 (FALSE) otherwise. Change the pipe token to NULL
   to indicate end of a process

/* synLine() uses a DFA approach.  It "reads" its characters from
   token_list. */
{	
	char *argv[MAXARGS];
	int i = 0;
	int pipe_num = 0;
	int len = DynArray_getLength(token_list);
	DynArray_toArray(token_list, (void **)argv);

	enum synState {STATE_START, STATE_ARGV, STATE_PIPE};
	enum synState sState = STATE_START;
	
	while(i < len) {
		switch(sState) {
			case STATE_START:
				if (argv[i] == NULL) {
					//printf("null\n");
					return pipe_num;
				}
				//Pipe special character is saved as "space+|" in token to distingush from pipe
				//character within ""
				else if (strcmp(argv[i]," |") == 0) {
					fprintf(stderr, "Pipe or redirection destination not specified\n");
				}
				else if (*argv[i]){
					//printf("state start %s \n", argv[i]);
					//process[iProIndex++] = argv[i];
					//iProIndex++;
					sState = STATE_ARGV;
				}
				break;

			case STATE_ARGV:
				//printf("%s ", argv[i]);
				if (argv[i] == NULL) {
					//DynArray_add(process_list, process);
					return pipe_num;
				}
				//Pipe special character is saved as "space+|" in token to distingush from pipe
				//character within ""
				else if (strcmp(argv[i]," |") == 0) {
					//process[iProIndex++] = NULL;
					DynArray_set(token_list, i, NULL);
					//printf("add\n");
					//iProIndex = 0;
					//process[MAXARGS] = NULL;
					pipe_num++;
					sState = STATE_PIPE;
				}
				else if (*argv[i]) {
					//printf("state argv %s \n", argv[i]);
					//process[iProIndex++] = argv[i];
				}
				break;

			case STATE_PIPE:
				if (argv[i] == NULL) {
					fprintf(stderr, "Pipe or redirection destination not specified\n");
					return -1;
				}
				//Pipe special character is saved as "space+|" in token to distingush from pipe
				//character within ""
				else if (strcmp(argv[i]," |") == 0) {
					fprintf(stderr, "Pipe or redirection destination not specified\n");
					return -1;
				}
				else if (*argv[i]) {
					//printf("state argv %s \n", argv[i]);
					//process[iProIndex++] = argv[i];
					sState = STATE_ARGV;
				}
				break;
		}
/*		char *pipe;
		strcpy(pipe, "|");
		char *pipe2;
		strcpy(pipe, "123");
		printf("%s ", argv[i]);
		printf("%s ", pipe);
		//if (*(argv + i) == "|") {
		int j = 0;
		if (*(argv + i) != NULL)
			j = strcmp(argv[i], pipe);
		printf("%d\n", j);*/
		/*if (!strcmp(argv[i], pipe)){
				printf("null\n");
					//return 1;
		}*/
		i++;
	}
	printf("\n");
	return 1;
}

static int lexLine(const char *cmdline, DynArray_T token_list)
/* Lexically analyze string cmdLine.  Populate token_list with the
   tokens that cmdline contains.  Return 1 (TRUE) if successful, or
   0 (FALSE) otherwise.  In the latter case, token_list may contain
   tokens that were discovered before the error. The caller owns the
   tokens placed in token_list. */

/* lexLine() uses a DFA approach.  It "reads" its characters from
   cmdLine. */
{
	assert(cmdline != NULL);
	assert(token_list != NULL);

	enum LexState {STATE_START, STATE_CHAR, STATE_QUOTE};

	enum LexState eState = STATE_START;

	int iTokenIndex = 0;
	int iLineIndex = 0;
	char value[MAXLINE];
	char array[MAXLINE]; //Hold a local copy of cmdline
	char c;
	strcpy(array, cmdline);
	//printf("cmdline: %s\n", array);


	for(;;) {
		c = array[iLineIndex++];
		switch(eState) {

			case STATE_START:
				if (is_space(c))
					eState = STATE_START;
				else if (is_newline(c)) {
					DynArray_add(token_list, NULL);
					return 1;
				}
				else if (is_char(c)) {
					value[iTokenIndex++] = c;
					eState = STATE_CHAR;
				}
				else if (is_pipe(c)) {
					// Pipe that is not within "" is saved as "space + |"
					char *token = makeToken(" |"); 
					if (! DynArray_add(token_list, token))
               		{
                  		fprintf(stderr, "Cannot allocate memory\n");
                  		return 0;
               		}
				}
				else if (is_dquote(c))
					eState = STATE_QUOTE;
				else {
               		fprintf(stderr, "Invalid line\n");
               		return 0;
            	}
            	break;

            case STATE_CHAR:
            	if (is_space(c) || is_newline(c) || is_pipe(c)) {
            		value[iTokenIndex++] = '\0';
            		char *token = makeToken(value);
            		if (! DynArray_add(token_list, token))
    				{	
    					fprintf(stderr, "Cannot allocate memory\n");
    					return 0;
    				}
               		iTokenIndex = 0;
               		eState = STATE_START;

    				if (is_pipe(c)) {
    					// Pipe that is not within "" is saved as "space + |"
    					char *token = makeToken(" |");
						if (! DynArray_add(token_list, token))
               			{
                  			fprintf(stderr, "Cannot allocate memory\n");
                  			return 0;
               			}
    				}
               		if (is_newline(c)) {
            			DynArray_add(token_list, NULL);
            			return 1;
               		}
            	}
            	else if (is_dquote(c)) {
            		eState = STATE_QUOTE;
            	}
            	else if (is_char(c)) {
            		value[iTokenIndex++] = c;
            	}
            	else {
               		fprintf(stderr, "Invalid line\n");
               		return 0;
            	}
            	break;

            case STATE_QUOTE:
            	if (is_newline(c)) {
            		fprintf(stderr, "ERROR - unmatched quote");
            	}
            	else if (is_dquote(c)) {
            		eState = STATE_CHAR;
            	}
            	else if (is_char(c) || is_space(c) || is_pipe(c)) {
            		value[iTokenIndex++] = c;
            	}
            	else {
               		fprintf(stderr, "Invalid line\n");
               		return 0;
            	}
            	break;
		}

   }
}

char* makeToken(char *tokenValue) 
{
	char *token = (char*)malloc(strlen(tokenValue) + 1);
	if (token == NULL)
   	{
      free(token);
      return NULL;
   	}
	strcpy(token, tokenValue);
	return token;
}

static void printToken(void *pvItem, void *pvExtra)
{
   printf("%s, ", (char *)pvItem);
}

int is_space(char c) {
	if (isspace(c) && c != '\n' && c != EOF)
		return 1;
	return 0;
}

int is_newline(char c) {
	if (c == '\n' || c == EOF)
		return 1;
	return 0;
}

int is_char(char c) {
	if (!isspace(c) && c != '|' && c != '"')
		return 1;
	return 0;
}

int is_pipe(char c) {
	if (c == '|')
		return 1;
	return 0;
}

int is_dquote(char c) {
	if (c == '"')
		return 1;
	return 0;
}

int main() {
	FILE *file;
	if ((file = fopen("/mnt/home/20186466/.ishrc","r")) == NULL) {
		file_opened = 0;
		perror("Could not open .ishrc file\n");
	}
	char cmdline[MAXLINE];

	while (1) {
        // Read command line if file is not found//
		if (file_opened) {
			if ((fgets(cmdline, MAXLINE, file) == NULL) && ferror(stdin))
            	fprintf(stderr, "fgets error");
			else {
				printf("%% %s", cmdline);
        		fflush(stdout);
        	}
        	if (feof(file)) {
        		fflush(stdout);
        		file_opened = 0;
        	}	
		}
		else {
        	printf("%% ");
        	fflush(stdout);
        	if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
            	fprintf(stderr, "fgets error");
        
        	if (feof(stdin)) { // End of file (ctrl-d)
            	fflush(stdout);
            	exit(0);
        	}
    	}
    	eval(cmdline);
        fflush(stdout);
        fflush(stdout);
    }

    exit(0); /* control never reaches here */
}