#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// This is the shell program. 

int main() {

    char* args[3] = {"ls", "-l"};

    // Create child process and execute the command
    int pid = fork();

    if (pid < 0){
        fprintf(stderr, "fork failed\n");
        exit(1);

    } else if (pid == 0) {
        printf("Child, pid: %i\n", pid);

        // Construct the string
        char baseStr[] = "/bin/";
        char* fullStr = malloc(sizeof(baseStr)/sizeof(baseStr[0]) + sizeof(args[0])/sizeof(args[0]));
        fullStr = strcat(baseStr, args[0]);

        // Execute and print error if we get error code back
        int back = execlp(fullStr, args[1], NULL);
        printf("failed to execv, error: %i", back);

    } else {
        printf("Parent, pid: %i\n", pid);
    }

    return 0;
}   
