#include <stdio.h>
#include <unistd.h> 

// This is the shell program. 

char* getInput(){
    return "ls";
}

int main() {

    // Get the input from the fd[1]
    char* input = getInput();

    // Create child process and execute the command
    int pid = fork();
    if (pid > 0) {
        printf("Parent, pid: %i\n", pid);
    }
    else {
        printf("Child, pid: %i\n", pid);

        char* args[2];
        args[0] = "bin/ls";
        args[1] = NULL;

        execv(args[0], args);
        
    }

    return 1;
}   