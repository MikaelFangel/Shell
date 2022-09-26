#define _GNU_SOURCE

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
void addHistory(char *argv[]);
void readHistory();

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

void addHistory(char *argv[]) {
    FILE *fptr;
    fptr = fopen(".shell_history", "a");

    if(fptr == NULL) {
        printf("Error adding to history");
    }

    for (int i = 0; argv[i] != NULL; i++) {
       fputs(argv[i], fptr);
       fputs(" ", fptr);
    }
    fputs("\n", fptr);

    fclose(fptr);
}

void readHistory() {
    FILE *fptr;
    char c;

    fptr = fopen(".shell_history", "r");
    int i = 0;
    while((c = fgetc(fptr)) != '\n') {
        fseek(fptr, i, SEEK_END);
        printf("%c", c);
        printf("%i", i);
        i = i - 1;
    } 
    printf("Something...");
    fclose(fptr);
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
        char *env[2];
        env[0] = getenv("PATH");
        env[1] = NULL;
        execvpe(argv[0], argv, env);

        // Print if execvp fails because then command is not found
        puts("Command not found...");
        exit(EXIT_FAILURE);
    }
}

void pipeLine(char **args[], int count) {
    // Create all pipes needed
    int fd[count][2];
    for(int i = 0; i < count; i++) {
        pipe(fd[i]);
    }

    for(int i = 0; i < count; i++) {
        switch(fork()) {
            case -1:
                // Error occured
                exit(EXIT_FAILURE);

            case 0:
                // If it is not the first command
                if(i > 0) 
                    dup2(fd[i - 1][0], 0);

                // If it is not the last command
                if(i < count - 1) 
                    dup2(fd[i][1], 1);

                // Close file descriptors
                for(int j = 0; j < count; j++) {
                     close(fd[j][0]);
                     close(fd[j][1]);
                }
                
                // Execute the chosen process as child
                char *env[2];
                env[0] = getenv("PATH");
                env[1] = NULL;
                execvpe(*args[i], args[i], env);

                // If reached error in exec has occurred
                exit(EXIT_FAILURE);
        }
    }

    // Close all pipes as the parent process
    for(int i = 0; i < count; i++) {
        close(fd[i][0]);
        close(fd[i][1]);
    }
    
    for(int i = 0; i < count - 1; i++) 
        waitpid(-1, NULL, 0);
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
