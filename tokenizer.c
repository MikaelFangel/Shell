#include <stdio.h>
#include <string.h>

int main(void) {
    char command[256] = {0}, buf[256] = {0};
    fgets(command, sizeof(buf), stdin);

    char *token;
    char *args[256]; 
    char delim[] = " \n";
    args[0] = strtok(command, delim);

    int i = 1;
    while((args[i++] = strtok(NULL, delim)) != NULL);

    return 0;
}
