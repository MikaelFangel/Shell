#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// This is the shell program. 

int newProcess(int argc, char* argv[]);

int main() {

    // OBS! Please note that the length of the char array is one more than what it contains
    // argc should only have the number of entries in the array, not the actual size of the argv array.
    char* argv[4] = {"ls", "-la", "/"};
    int argc = 3;

    newProcess(argc, argv);

    return 0;
}   


int newProcess(int argc, char* argv[]){
    int pid = fork();

    if (pid < 0) {          // Failed
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (pid > 0) {   // Parent
        printf("Parent, pid: %i\n", pid);

    } else {                // Child
        printf("Child, pid: %i\n", pid);

        // Construct path string
        char baseStr[] = "/bin/";
        char* fullStr = malloc(sizeof(baseStr)/sizeof(baseStr[0]) + sizeof(argv[0])/sizeof(argv[0]));
        fullStr = strcat(baseStr, argv[0]);

        // Execute and print error if we get error code back
        int back;
        if (argc > 1)
            back = execvp(fullStr, argv);
        else if (argc == 1)
            back = execlp(fullStr, "", NULL);

        printf("failed to exec, error: %i", back);
    }
}
