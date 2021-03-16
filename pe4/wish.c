#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COM_LEN 256 // Maximum command length + 1

int main() {
    char *command = malloc(MAX_COM_LEN);
    if (command == NULL) {
        printf("No memory\n");
        return 1;
    }

    while(1) { // TODO: find an exit condition for the loop
        printf("wish:<CURRENT_PATH_HERE>$ ");
        fgets(command, MAX_COM_LEN, stdin);
        printf("%s", command);

    }
}
