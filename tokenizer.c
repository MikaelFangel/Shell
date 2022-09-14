#include <stdio.h>
#include <string.h>

int input(char *, char *);
char* parse(char *);

int main(void) {
    char command[256] = {0}, buf[256] = {0};
    input(command, buf);
    char * result;
    result = parse(command);

    while(result != NULL) {
        printf("%s", result);
        result = strtok(NULL, " ");
    }
    return 0; 
}

int input(char *command, char *buf) {

    fgets(command, sizeof(buf), stdin);
    return 0;
}

char *parse(char *commands) {
    return strtok(commands, " ");
}
