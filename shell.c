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
            char **args = (char**) calloc(nread, sizeof(char**));
            char delim[] = " \n";
            char openBlock[] = {"\""};
            char closeBlock[] = {"\""};
            int argc = 1;

            addHistory(line);

            // Tokenize string
            args[0] = strtok_advanced(line, delim, openBlock, closeBlock); 

            // if exit has been inputted, exit program
            if(strcmp(args[0], "exit") == 0) {
                free(args);
                break;
            } 

            // Read the tokens into args and keep track of number of arguments
            while((args[argc++] = strtok_advanced(NULL, delim, openBlock, closeBlock)) != NULL) {
                if(*args[argc - 1] == '"') {
                    args[argc - 1]++;
                }
            }
            args[argc] = NULL;

            // Pass the parser the arguments
            parser(argc, args);
            free(args);
        }
    }

    free(line);
    exit(EXIT_SUCCESS);
}

/*
* Advanced strtok:
* Tokenizes input on delim if not in a block surrounded by openBlock and closeBlock
* ref: https://stackoverflow.com/questions/26187037/in-c-split-char-on-spaces-with-strtok-function-except-if-between-quotes
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
            if(closeBlock[iBlockIndex] == *token) {
                iBlock = 0;
                *token = '\0';
                token += 2;
                break;
            }
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
 * Parses the argv array to determine if is should start
 * a new process or i should pipe the processes.
 */
void parser(int argc, char *argv[]) {
    // Used to keep track of how many pipes there is in the input string
    int containsPipe = 0;

    // Make an array of pointers to all commands within the string
    // to use when there is pipes present
    char ***nextargv = (char***) calloc (argc / 2, sizeof(char***));
    // Set the first argument (left of pipe)
    nextargv[0] = &argv[0];
    char his[100];

    //addHistory(argv);
    
    /* Looks if any tokens contains the pipe operator
    and populate the nextargv array with locations of commands i */
    for(int i = 0; argv[i] != NULL; i++) {
        if(strstr(argv[i], "|") != NULL) {
            /* Substitue the pipe with a null pointer so that
            all argument vectors send to the pipe function
            arr null terminated i */
            argv[i] = NULL;

            /* Increment the containsPipe before because the first
            space is alread populated */
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

        else if (strcmp(argv[0], "history") == 0)
                readHistory();

        else // Start new process
            newProcess(argv);  

    else // If containsPipe > 0 then call the pipeline function
        pipeLine(nextargv, containsPipe + 1);

    free(nextargv);
}

/*
 * Add string to a .shell_history file to enable Shell Command history
 */
void addHistory(char *argv) {
    // Open file or create if it does not excists
    FILE *fptr;
    char *file;
    char *fileName = "/.shell_history";
    file = malloc(strlen(getenv("HOME")) + strlen(fileName) + 1); // IMPORTANT TO NOT MODIFY WHAT IS ON THE ENV VARIABLE POINTER
    strcpy(file, getenv("HOME"));
    strcat(file, fileName);

    fptr = fopen(file, "a");
    fputs(argv, fptr); 
    fclose(fptr);
    free(file);
}

/*
 * Read last line from .shell_history file and add it to a character array
 */
void readHistory() {
    // Open and read file line-by-line and print content
    FILE *fptr;
    char* file;
    char* fileName = "/.shell_history";
    file = malloc(strlen(getenv("HOME")) + strlen(fileName) + 1); // IMPORTANT TO NOT MODIFY WHAT IS ON THE ENV VARIABLE POINTER
    strcpy(file, getenv("HOME"));
    strcat(file, fileName);

    fptr = fopen(file, "r");
    char* line = NULL;
    size_t len = 0;
    ssize_t nreads;
    ssize_t fpos = 0;

    int i = 1;
    while((nreads = getline(&line, &len, fptr)) > 1) {
        fseek(fptr, (fpos += nreads), SEEK_SET);
        printf("%i\t%s", i, line);
        i++;
    }

    free(line);
    fclose(fptr);
    free(file);
}

/*
 * Start a new process using the execvp function and return 1 if not found
 * the functions takes a array of the command and all its flags and then
 * creates a new child process.
 */
void newProcess(char* argv[]){
    // Handle the process after it's process ID
    switch(fork()){
        case -1:            // Forking failed
            fprintf(stderr, "fork failed\n");
            exit(EXIT_FAILURE);

        case 0:             // Child
            // Overwrite program image
            execvp(argv[0], argv);

            // Print if execvp fails because then command is not found
            puts("Command not found...");

            // End the child process to avoid zombies
            exit(EXIT_FAILURE);

        default:            // Parent
            // Wait for the child process to finish
            waitpid(-1, NULL, 0);
    }
}

/*
 * Redirects the stdin and stdout and pipes the processes so that
 * they can communicate. The functions take and array of arguments
 * where each location in the array are the position of the command
 * to be executed. Each command with flags should end on a null pointer
 */
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
                    dup2(fd[i - 1][0], 0);  // Redirect pipe read-end and stdin

                // If it is not the last command
                if(i < count - 1) 
                    dup2(fd[i][1], 1);      // Redirect pipe write-end and stdout

                // Close file descriptors
                for(int j = 0; j < count; j++) {
                     close(fd[j][0]);
                     close(fd[j][1]);
                }
                
                // Execute the chosen process as child
                execvp(*args[i], args[i]);

                // If reached error in exec has occurred
                exit(EXIT_FAILURE);
        }
    }

    // Close all pipes as the parent process
    for(int i = 0; i < count; i++) {
        close(fd[i][0]);
        close(fd[i][1]);
    }
    
    // Wait for all child process to finish
    for(int i = 0; i < count; i++) 
        waitpid(-1, NULL, 0);
}

/*
 * Change the working directory within the process. The funciton acounts for
 * realtive paths and absolute path. If NULL is given as the argument
 * changeDir change the working directory to the home of the user
 */
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
        strcpy(substr, &path[1]);

        // Concatenate the string
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
        // Get current working directory to concat the new full path
        char *cwd = getcwd(NULL, 0); 
        cwd = realloc(cwd, strlen(cwd) + strlen(path) + 2);

        // Add '\' between cwd and the relative path
        strcat(cwd, "/"); 

        // Add the inputed path to the cwd + /
        strcat(cwd, path);

        returnCode = chdir(cwd); // Change directory
        free(cwd);
    }

    
    if (returnCode == -1){ // Error
        printf("Unknown path!\n");
    }
}

/*
 * Print a welcome message
 */
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
