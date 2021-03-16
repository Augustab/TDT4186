#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#define MAX_INPUT_LEN 256 // Maximum input length + 2 (2 because of ending \n and NULL)

int main() {
    char *input = malloc(MAX_INPUT_LEN);
    if (input == NULL) {
        printf("No memory\n");
        return 1;
    }
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    printf("Current working dir: %s\n", cwd);
    char delims[] = " \t\n"; // Tokens split by space or tab character
    
    while(1) { // TODO: find an exit condition for the loop
        printf("wish:<%s>$ ", cwd);
        fgets(input, MAX_INPUT_LEN, stdin); // fgets returns 0 on error TODO: error handling
        printf("Whole input string:\n%s", input);
        
        char *command = strtok(input, delims);
        printf("Command: %s\n", command);
        char *param = strtok(NULL, delims);
        while (strcmp(param, "<") && strcmp(param, ">") && param != NULL ) {
            printf("Parameter token: |%s|\n", param);
            param = strtok(NULL, delims);
        }
        
        // Redirections
        char *in_file;
        char *out_file;
        int has_redirected_input = 0;
        int has_redirected_output = 0;
        if (strcmp(param, "<") == 0) {
            has_redirected_input = 1;
            in_file = strtok(NULL, delims);
            param = strtok(NULL, delims);
        }
        if (strcmp(param, ">") == 0) {
            has_redirected_output = 1;
            out_file = strtok(NULL, delims);
            param = strtok(NULL, delims);
        }
        if (strcmp(param, "<") == 0) {
            if (has_redirected_input) {
                printf("Attempted to redirect input twice, exiting\n");
                exit(1);
            }
            in_file = strtok(NULL, delims);
            param = strtok(NULL, delims);
        }
        printf("Input file path: %s\n", in_file);
        printf("Output file path: %s\n", out_file);
        // if (strcmp(param, ">") == 0) {
        //     if (has_redirected_output) {
        //         printf("Attempted to redirect input twice, exiting\n");
        //         exit(1);
        //     }
        
        // }
    }
}
