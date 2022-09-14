#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// This is the shell program. 

int main() {

    // Create child process and execute the command
    int pid = fork();
    if (pid < 0){
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (pid == 0) {
        printf("Child, pid: %i\n", pid);
        int back = execlp("/bin/ls","",NULL);
        printf("failed to execv, error: %i", back);
    } else {
        printf("Parent, pid: %i\n", pid);
    }

    return 0;
}   
