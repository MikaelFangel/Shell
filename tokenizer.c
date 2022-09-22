#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void newProcess(char* argv[]);
void pipeProcesses(char *argvfrom[], char *argvto[]);
void picture();

int main(void) {
    char *line = NULL;          // Let getline do the heap allocation
    size_t len = 0;
    ssize_t nread;

    picture();

    for(;;) {

        printf("shell -> ");
        fflush(stdout);
        
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

            newProcess(args);
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

void picture(){
    printf("Welcome\n\n");
    //stolen at https://www.asciiart.eu/computers/computers
    printf("   _______________                        |*\\_/*|________\n");
    printf("  |  ___________  |     .-.     .-.      ||_/-\\_|______  |\n");
    printf("  | |           | |    .****. .****.     | |           | |\n");
    printf("  | |   0   0   | |    .*****.*****.     | |   0   0   | |\n");
    printf("  | |     -     | |     .*********.      | |     -     | |\n");
    printf("  | |   \\___/   | |      .*******.       | |   \\___/   | |\n");
    printf("  | |___     ___| |       .*****.        | |___________| |\n");
    printf("  |_____|\\_/|_____|        .***.         |_______________|\n");
    printf("    _|__|/ \\|_|_.............*.............._|________|_\n");
    printf("   / ********** \\                          / ********** \\\n");
    printf(" /  ************  \\                      /  ************  \\\n");
    printf("--------------------                    --------------------\n");
    printf("\n\n\n");
}
