#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
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

            // Execute wc with the output from the child process
            execlp("wc", "wc", "-l", NULL);

            // Exit with failure if reached
            exit(1);

        default:
            switch(fork()) {
                case 0:
                    // Close the read end of the pipe
                    close(fd[0]);

                    // Redirect write end to stdout 
                    dup2(fd[1], 1);

                    // Close file descriptor for the write end
                    close(fd[1]);

                    // Execute ls and write to stdin
                    execlp("ls", "ls","-l", NULL);
                    
                    // Exit with failure
                    exit(1);

                default:
                    // Close file descriptors
                    close(fd[0]);
                    close(fd[1]);

                    // Wait for two processes to finish
                    waitpid(-1, NULL, 0);
                    waitpid(-1, NULL, 0);
                }
    }

    return 0;
}
