#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void parser(char *argv[]);
void newProcess(char* argv[]);
void pipeProcesses(char *argvfrom[], char *argvto[]);

int main(void) {
    char *line = NULL;          // Let getline do the heap allocation
    size_t len = 0;
    ssize_t nread;

    for(;;) {

        char *working_dir = getcwd(NULL, 0);
        printf("%s@%s -> ", getlogin(), working_dir);
        fflush(stdout);
        free(working_dir);
        
        nread = getline(&line, &len, stdin);

        // Check if there was an error reading the line and free the line pointer if so
        if(nread == -1) {
            free(line);             // free line even upon failure
            exit(EXIT_FAILURE);
        } else if(nread > 1) {

            char *args[nread], delim[] = " \n";
            int argc = 1;

            args[0] = strtok(line, delim);
            if(strcmp(args[0], "exit") == 0) break; 

            // Read the tokens into args and keep track of number of arguments
            while((args[argc++] = strtok(NULL, delim)) != NULL);
            args[argc] = NULL;

            parser(args);
        }
    }

    free(line);

    // Temp demo for piping
    // char *args1[3];
    // char *args2[3];

    // args1[0] = "ls";
    // args1[1] = "-l";
    // args1[2] = NULL;
    // args2[0] = "wc";
    // args2[1] = "-l";
    // args2[2] = NULL;

    // pipeProcesses(args1, args2);
    exit(EXIT_SUCCESS);
}

/*
Parses the argv array to determine if is should start
a new process or i should pipe the processes.
*/
void parser(char *argv[]) {
    int containsPipe = 0;
    for(int i = 0; argv[i] != NULL; i++) {
        if(strstr(argv[i], "|") != NULL)
            containsPipe = 1;
    }

    if(!containsPipe)
        newProcess(argv);
    else // Piping goes here!
        puts("Piping not implemented yet");
}

void newProcess(char* argv[]){
    // Fork Process
    int pid = fork();

    // Handle the process after it's process ID
    if (pid < 0) {          // Failed
        fprintf(stderr, "fork failed\n");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {   // Parent
        waitpid(-1, NULL, 0);

    } else {                // Child
                            // Execute and print error if we get error code back
        int back = execvp(argv[0], argv);

        // Should only ever be executed if exec fails. Else the image has been overwritten.
        printf("failed to exec, error: %d\n", back);
        exit(EXIT_FAILURE);
    }
}

void pipeProcesses(char *argvfrom[], char *argvto[]) {
    int fd[2];
    pipe(fd);

    switch (fork()) {
        case 0:
            // Close the write end of the pipe
            close(fd[1]);

            // Redirect stdin to read end 
            dup2(fd[0], 0);

            // Close the read end
            close(fd[0]);

            // Execute argv 0 with the output from the child process
            execvp(argvto[0], argvto);

            // Exit with failure if reached
            exit(EXIT_FAILURE);

        default:
            switch(fork()) {
                case 0:
                    // Close the read end of the pipe
                    close(fd[0]);

                    // Redirect write end to stdout 
                    dup2(fd[1], 1);

                    // Close file descriptor for the write end
                    close(fd[1]);

                    // Execute argv 0 and write to stdout
                    execvp(argvfrom[0], argvfrom);

                    // Exit with failure if reached
                    exit(EXIT_FAILURE);

                default:
                    // Close file descriptors
                    close(fd[0]);
                    close(fd[1]);

                    // Wait for two processes to finish
                    waitpid(-1, NULL, 0);
                    waitpid(-1, NULL, 0);
            }
    }
}
