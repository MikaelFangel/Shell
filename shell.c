#define _GNU_SOURCE

#include "shell.h" // ""'s to makes the compiler look for the header file in the same directory <>'s would not.

int main(void) {
    char *line = NULL; // Let getline do the heap allocation
    size_t len = 0;
    ssize_t nread;

    welcomeMsg();

    for(;;) { // Alternative while true loop :)

        // Get current working directory
        char *working_dir = getcwd(NULL, 0);

        // Print user's path and username
        printf("%s@%s -> ", getlogin(), working_dir);

        // Clean up
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
            char *args[nread];
            char delim[] = " \n";
            char openBlock[] = {"\""};
            char closeBlock[] = {"\""};
            int argc = 1;

            // Tokenize string
            args[0] = strtok_advanced(line, delim, openBlock, closeBlock); 

            // if exit has been inputted, exit program
            if(strcmp(args[0], "exit") == 0) break; 

            // Read the tokens into args and keep track of number of arguments
            while((args[argc++] = strtok_advanced(NULL, delim, openBlock, closeBlock)) != NULL);
            args[argc] = NULL;

            // Pass the parser the arguments
            parser(argc, args);
        }
    }

    free(line);
    exit(EXIT_SUCCESS);
}

/*
* Advanced strtok:
* Tokenizes input on delim if not in a block surrounded by openBlock and closeBlock
*/
char *strtok_advanced (char *input, char *delim, char *openBlock, char *closeBlock) {
    // Static variables preserve their value out of their scope too
    static char *token = NULL;
    char *lead = NULL;
    char *block = NULL;
    int iBlock = 0;
    int iBlockIndex = 0;

    // when calling the function on input the first time
    if(input != NULL) {
        token = input;
        lead = input;
    }
    else {
        // when function is called with "NULL" as input, it continues from prev state
        // this works like strtok()
        lead = token;
        // if token from prev call was null byte
        if(*token == '\0') {
            lead = NULL;
        }
    }

    while(*token != '\0') {
        // If already in block, don't tokenize on delimiter
        if(iBlock) {
            if(closeBlock[iBlockIndex] == *token) iBlock = 0;
            token++;
            continue;
        }

        // If the token occurs in openBlock
        if((block = strchr(openBlock, *token)) != NULL) {
            iBlock = 1;
            // subtract address of openBlock from address of block to find the index
            // of the character that maches the token
            iBlockIndex = block - openBlock;
            token++;
            continue;
        }

        // If token occurs in the delimiter
        if(strchr(delim, *token) != NULL) {
            *token = '\0';
            token++;
            break;
        }
        token++;
    }
    return lead;
}


/*
   Parses the argv array to determine if is should start
   a new process or i should pipe the processes.
 */
void parser(int argc, char *argv[]) {
    int containsPipe = 0;
    char **nextargv[argc];
    nextargv[0] = &argv[0];
    char his[100];

    //addHistory(argv);
    
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

/*
 * Add string to a .shell_history file to enable Shell Command history
 */
void addHistory(char *argv[]) {
    // Open file or create if it does not excists
    FILE *fptr;
    fptr = fopen(".shell_history", "a");

    // For each argument write it to the file and add space.
    for (int i = 0; argv[i] != NULL; i++) {
       fputs(argv[i], fptr);
       fputs(" ", fptr);
    }
    // Add newline to the end of the command
    fputs("\n", fptr);
    fclose(fptr);
}

/*
 * Read last line from .shell_history file and add it to a character array
 */
void readHistory(char* buf) {
    // Open file
    FILE *fptr;
    fptr = fopen(".shell_history", "r");

    while(1) {
        // Continue reading file line-by-line until EOF, and save each line to the array. 
        // When the file ends the last line is saved in the array
        if (fgets(buf, sizeof(buf), fptr) == NULL) {
            break;
        }
    }
    fclose(fptr);
}

void newProcess(char* argv[]){
    // Handle the process after it's process ID
    switch(fork()){          // Failed
        case -1:
            fprintf(stderr, "fork failed\n");
            exit(EXIT_FAILURE);

        case 0:             // Child
            // Get the PATH environment variable and null terminate it
            char *env[2];
            env[0] = getenv("PATH");
            env[1] = NULL;

            // Overwrite program image
            execvpe(argv[0], argv, env);

            // Print if execvp fails because then command is not found
            puts("Command not found...");
            exit(EXIT_FAILURE);

        default:            // Parent
            waitpid(-1, NULL, 0);
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
    
    for(int i = 0; i < count; i++) 
        waitpid(-1, NULL, 0);
}

void changeDir(char* path) {
    const int max_path_buff = 4096;
    bool relative = false;

    // If no path is provided default to HOME
    if (path == NULL){
        chdir(getenv("HOME"));
        return;
    }

    // Check if the path is relative to the home path
    if (path[0] == '~'){
        char* homepath = getenv("HOME");
        char* homepathCopy = malloc(max_path_buff * sizeof(char)); // IMPORTANT TO NOT MODIFY WHAT IS ON THE ENV VARIABLE POINTER
        strcpy(homepathCopy, homepath);

        // Remove tilde from path
        char substr[max_path_buff];
        strncpy(substr, &path[1], max_path_buff);

        // Concate the string
        strcat(homepathCopy, substr);
        strcpy(path, homepathCopy); // Insert into path

        // Free memory
        free(homepathCopy);
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
