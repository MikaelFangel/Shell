#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

            char *args[nread], delim[] = " \n";
            int argc = 1;

            args[0] = strtok(line, delim);
            if(strcmp(args[0], "exit") == 0) break; 

            // Read the tokens into args and keep track of number of arguments
            while((args[argc++] = strtok(NULL, delim)) != NULL);
            args[argc] = NULL;
        }
    }

    free(line);
    exit(EXIT_SUCCESS);
}
