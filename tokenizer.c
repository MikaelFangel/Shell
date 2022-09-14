#include <stdio.h>
#include <string.h>

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t lineSize = 0;

    lineSize = getline(&line, &len, stdin);
    
    char *token, *args[lineSize], delim[] = " \n";
    args[0] = strtok(line, delim);

    int argc = 1;
    while((args[argc++] = strtok(NULL, delim)) != NULL);

    return 0;
}
