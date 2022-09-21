#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int newProcess(int argc, char* argv[]);

int main(void) {
    char *line = NULL;          // Let getline do the heap allocation
    size_t len = 0;
    ssize_t nread;

    for(;;) {
        nread = getline(&line, &len, stdin);

        // Check if there was an error reading the line and free the line pointer if so
        if(nread == -1) {
            free(line);             // free line even upon failure
            exit(EXIT_FAILURE);
        } else if(nread > 1) {

            char *token, *args[nread], delim[] = " \n";
            int argc = 1;

            args[0] = strtok(line, delim);
            if(strcmp(args[0], "exit") == 0) break; 

            // Read the tokens into args and keep track of number of arguments
            while((args[argc++] = strtok(NULL, delim)) != NULL);
            args[argc] = NULL;

            newProcess(argc, args);
        }
    }

    free(line);
    exit(EXIT_SUCCESS);
}

int newProcess(int argc, char* argv[]){
    // Fork Process
    int pid = fork();

    // Handle the process after it's process ID
    // OBS! all prints, can be removed or commented out
    if (pid < 0) {          // Failed
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (pid > 0) {   // Parent
        printf("Parent, pid: %i\n", pid);

    } else {                // Child
        printf("Child, pid: %i\n", pid);

        // Construct path string from input
        char baseStr[] = "/bin/";
        char* fullStr = malloc(sizeof(baseStr)/sizeof(baseStr[0]) + sizeof(argv[0])/sizeof(argv[0]));
        fullStr = strcat(baseStr, argv[0]);

        // Execute and print error if we get error code back
        int back;
        if (argc > 1) // If the command has associated arguments
            back = execvp(fullStr, argv);

        else if (argc == 1) // If there is no arguments alongside the initial command
            back = execlp(fullStr, "", NULL);

        // Should only ever be executed if exec fails. Else the image has been overwritten.
        printf("failed to exec, error: %i", back);
    }
}
