void runPipedCommands(cmdLine* command, char* userInput) {
    int numPipes = countPipes(userInput);


    int status;
    int i = 0;
    pid_t pid;

    int pipefds[2*numPipes];

    for(i = 0; i < (numPipes); i++){
        if(pipe(pipefds + i*2) < 0) {
            perror("couldn't pipe");
            exit(EXIT_FAILURE);
        }
    }


    int j = 0;
    while(command) {
        pid = fork();
        if(pid == 0) {

            //if not last command
            if(command->next){
                if(dup2(pipefds[j + 1], 1) < 0){
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            //if not first command&& j!= 2*numPipes
            if(j != 0 ){
                if(dup2(pipefds[j-2], 0) < 0){
                    perror(" dup2");///j-2 0 j+1 1
                    exit(EXIT_FAILURE);
                }
            }


            for(i = 0; i < 2*numPipes; i++){
                    close(pipefds[i]);
            }

            if( execvp(*command->arguments, command->arguments) < 0 ){
                    perror(*command->arguments);
                    exit(EXIT_FAILURE);
            }
        } else if(pid < 0){
            perror("error");
            exit(EXIT_FAILURE);
        }

        command = command->next;
        j+=2;
    }
    /**Parent closes the pipes and wait for children*/

    for(i = 0; i < 2 * numPipes; i++){
        close(pipefds[i]);
    }

    for(i = 0; i < numPipes + 1; i++)
        wait(&status);
}

/*THis part is my implementation of pipe, but fail*/

        //Create a array to store the pointer to each child process
    int i = 0;
    int process[pipe_num+1];
    int index = 0;
    process[index++] = 0;
    while (i < len) {
        if (argv[i] == NULL){
            process[index++] = i+1;
        }
        i++;
    }
    
    //Create Pipes
    int fds[2*pipe_num];
    for(i = 0; i < pipe_num; i++){
        if(pipe(fds + i*2) < 0) {
            perror("Couldn't pipe");
            return 0;
        }
    }

    int j = 0;
    int exe_status;
    for (int ip = 0; ip < pipe_num+1; ip++){
/*      printf("process start at %s\n", argv[process[ip]]);
        printf("process: ");
        DynArray_map(token_list, printToken, NULL);
        printf("\n");*/

        //if not first command && j!= 2*numPipes

        fflush(NULL);
        if ((pid = fork()) == 0) { // Run child process
            if(j != 0 ){
                if(dup2(fds[j-2], 0) < 0){
                    perror(" dup2");///j-2 0 j+1 1
                    return 0;
                }
            }

            //if not last command
            if(ip != pipe_num){
                if(dup2(fds[j + 1], 1) < 0){
                    perror("dup2");
                    return 0;
                }
            }

            for(i = 0; i < 2*pipe_num; i++){
                close(fds[i]);
            }

            if (exe_status = (execvp(argv[process[ip]], argv + process[ip])) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }
        //pid = wait(NULL);
        if (exe_status < 0)
            break;
    }
    for(i = 0; i < 2 * pipe_num; i++){
        close(fds[i]);
    }

    for(i = 0; i < pipe_num + 1; i++)
        wait(NULL); 
