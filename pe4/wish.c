#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_INPUT_LEN 256 // Maximum input length + 2 (2 because of ending "\n" and \0)

int main() {
    char *input = malloc(MAX_INPUT_LEN);
    char *params[64];
    if (input == NULL) {
        printf("No memory\n");
        return 1;
    }
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    char delims[] = " \t\n"; // Tokens split by space or tab character
    int pid;

    while(1) { // TODO: find an exit condition for the loop
        printf("wish:<%s>$ ", cwd);
        fgets(input, MAX_INPUT_LEN, stdin); // fgets returns 0 on error TODO: error handling

        char *command = strtok(input, delims);
        printf("Command: %s\n", command);
        char *param = strtok(NULL, delims);
        int i = 0;
        while(param != NULL) {
            if (!strcmp(param, "<") || !strcmp(param, ">")){
                break;
            }
            printf("Parameter token: %s\n", param);
            params[i] = param;
            i++;
            param = strtok(NULL, delims);
        }

        // Redirections
        char *in_file;
        char *out_file;
        int has_redirected_input = 0;
        int has_redirected_output = 0;
        if (param != NULL && strcmp(param, "<") == 0) {
            has_redirected_input = 1;
            in_file = strtok(NULL, delims);
            param = strtok(NULL, delims);
        }
        if (param != NULL && strcmp(param, ">") == 0) {
            has_redirected_output = 1;
            out_file = strtok(NULL, delims);
            param = strtok(NULL, delims);
        }
        if (param != NULL && strcmp(param, "<") == 0) {
            if (has_redirected_input) {
                printf("Attempted to redirect input twice, exiting\n");
                exit(1);
            }
            in_file = strtok(NULL, delims);
            param = strtok(NULL, delims);
        }
        printf("Input file path: %s\n", in_file);
        printf("Output file path: %s\n", out_file);


        //Execution

        pid = fork();
        if (pid > 0) //Parent process          
        {
            printf("Created a child Process %d\n", pid);
            wait(NULL);
        }
        else if (pid == 0) //Child process
        {
            char *argv[i+2];
            argv[0] = command;
            for(int c=0; c<i; c++) {
                argv[c+1] = params[c];
            }
            argv[i+1] = NULL;

            if (execvp(command, argv) < 0) {
                printf("An error has occured. Error type: %d\n", errno);
            exit(0);
            }
        }
    }
}
