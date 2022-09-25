#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>

void parser(int argc, char *argv[]);
void newProcess(char* argv[]);
void pipeProcesses(char *argvfrom[], char *argvto[]);
void pipeLine(char **args[], int count);
void changeDir(char* path);
void welcomeMsg();

int main(void) {
    char *line = NULL; // Let getline do the heap allocation
    size_t len = 0;
    ssize_t nread;
    int count = 20;
    int fd[count][2];

    welcomeMsg();

    for(;;) { // Alternative while true loop :)

        char *working_dir = getcwd(NULL, 0);
        printf("%s@%s -> ", getlogin(), working_dir);
        fflush(stdout);
        free(working_dir);

        // Read input
        nread = getline(&line, &len, stdin);

        // Check if there was an error reading the line and free the line pointer if so
        if(nread == -1) {
            free(line);             // free line even upon failure
            exit(EXIT_FAILURE);
        } 
        else if(nread > 1) {
            char *args[nread], delim[] = " \n";
            int argc = 1;

            args[0] = strtok(line, delim);
            if(strcmp(args[0], "exit") == 0) break; 

            // Read the tokens into args and keep track of number of arguments
            while((args[argc++] = strtok(NULL, delim)) != NULL);
            args[argc] = NULL;

            parser(argc, args);
        }
    }

    free(line);
    exit(EXIT_SUCCESS);
}

/*
   Parses the argv array to determine if is should start
   a new process or i should pipe the processes.
 */
void parser(int argc, char *argv[]) {
    int containsPipe = 0;
    char **nextargv[argc];
    nextargv[0] = &argv[0];

    // Looks if any tokens contains the pipe operator
    for(int i = 0; argv[i] != NULL; i++) {
        if(strstr(argv[i], "|") != NULL) {
            argv[i] = NULL;
            nextargv[++containsPipe] = &argv[i + 1]; 
        }
    }

    // Does correct action depending on type of input
    if(!containsPipe)
        // If command first token is cd
        if (strcmp(argv[0], "cd") == 0)
            // Then use inbuild function to change directory
            if (argc > 1)
                changeDir(argv[1]);
            else 
                changeDir(NULL);

        else // Start new process   
            newProcess(argv);  

    else // Piping
        pipeLine(nextargv, containsPipe + 1);
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
        execvp(argv[0], argv);

        // Print if execvp fails because then command is not found
        puts("Command not found...");
        exit(EXIT_FAILURE);
    }
}

void pipeLine(char **args[], int count) {
    int fd[count][2];
    for(int i = 0; i < count; i++) {
        pipe(fd[i]);
    }

    for(int i = 0; i < count; i++) {
        // printf("%s count: %d\n", *args[i], i);
        switch(fork()) {
            case -1:
                // Error occured
                exit(EXIT_FAILURE);

            case 0:
                // If it is not the first command
                if(i > 0) 
                    dup2(fd[i][0], 0);

                // If it is not the last command
                if(i < count - 1) 
                    dup2(fd[i][1], 1);

                for(int j = 0; j < count; j++) {
                     close(fd[j][0]);
                     close(fd[j][1]);
                }

                
                char *argv[3];
                if(i == 0) {
                    argv[0] = "ls";
                    argv[1] = "-l";
                    argv[2] = NULL;
                } else {
                    argv[0] = "wc";
                    argv[1] = "-l";
                    argv[2] = NULL;
                }

                execvp(argv[0], argv);

                // execvp(*args[i], args[i]);
        }
    }
    for(int i = 0; i < count; i++) {
        close(fd[i][0]);
        close(fd[i][1]);
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

void changeDir(char* path) {
    const int max_path_buff = 4096;
    bool relative = false;

    // If no path is provided default to HOME
    if (path == NULL){
        char home[2] = "~";
        changeDir(home);
        return;
    }

    // Check if the path is relative to the home path
    if (path[0] == '~'){
        char* homepath = getenv("HOME");

        // Remove tilde from path
        char substr[max_path_buff];
        strncpy(substr, &path[1], max_path_buff);

        // Concate the string
        strcat(homepath, substr);
        strcpy(path, homepath); // Insert into path
    }
    // Else check if the path provided is relative or absolute
    else if (path[0] != '/') {
        relative = true;
    }

    int returnCode;
    if (!relative) { // if absolute
        returnCode = chdir(path); // Change directory
    }
    else {  // else it's relative
        char cwd[max_path_buff];

        // Get current working directory to concat the new full path
        getcwd(cwd, sizeof(cwd)); 

        // Add '\' between cwd and the relative path
        strcat(cwd, "/"); 

        // Add the inputed path to the cwd + /
        strcat(cwd, path);

        returnCode = chdir(cwd); // Change directory
    }

    if (returnCode == -1){ // Error
        printf("Unknown path!\n");
    }
}

void welcomeMsg() { 
    puts("                    _"); 
    puts("                   | |");
    puts("     ___  _ __  ___| |__");
    puts("    / _ \\| '_ \\/ __| '_ \\");
    puts("    |(_) | |_) \\__ \\ | | |");
    puts("    \\___/| .__/|___/_| |_|");
    puts("         | |");
    puts("         |_|");
    puts("");
}
