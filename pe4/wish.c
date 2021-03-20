#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>


#define MAX_INPUT_LEN 256 // Maximum input length + 2 (2 because of ending "\n" and \0)

int main() {
    char *input = malloc(MAX_INPUT_LEN);
    char *params[64];
    if (input == NULL) {
        printf("No memory\n");
        return 1;
    }
    char cwd[PATH_MAX];
    //getcwd(cwd, sizeof(cwd));
    char delims[] = " \t\n"; // Tokens split by space or tab character
    int pid;

    while(1) { // TODO: find an exit condition for the loop
        getcwd(cwd, sizeof(cwd));   // Updating CWD
        printf("\033[1;31mwish:<%s>$ \033[0m", cwd);
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
        char *in_file = NULL;
        char *out_file = NULL;
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
        
        //printf("%s\n", command);
        //printf("Params: %s\n", params[0]);
        
        // Task D - Internal Commands
        if (strcmp(command, "cd")==0  || strcmp(command, "CD")==0) {
            if (chdir(params[0])!=0) {
                printf("Error CD-ing, %d\n", errno);
            }               
        }
        else if (strcmp(command, "exit")==0 || strcmp(command, "EXIT")==0)
        {
            printf("Exiting...\n");
            exit(EXIT_SUCCESS);
        }
        else if (strcmp(command, "wish")==0 || strcmp(command, "WISH")==0)
        {
            
        }
        
        else 
        {
            pid = fork();
            if (pid > 0) //Parent process
            {
                printf("Created a child Process %d\n", pid);
                wait(NULL);
            }
            else if (pid == 0) //Child process
            {
                char *argv[i + 2];
                argv[0] = command;
                for (int c = 0; c < i; c++)
                {
                    argv[c + 1] = params[c];
                }
                argv[i + 1] = NULL;

                if (in_file != NULL)
                {
                    int input = open(in_file, O_RDONLY);
                    if (input == -1)
                    {
                        printf("Error: InputFile can not be read or doesn't exist\n");
                        exit(EXIT_FAILURE);
                    }
                    else
                    {
                        //printf("HER ER INPUT\n");
                        dup2(input, 0);
                        close(input);
                    }
                }

                if (out_file != NULL)
                {
                    //printf("INNE HER\n");
                    int output = open(out_file, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR); // FLAGS FOUND HERE: http://www.cs.loyola.edu/~jglenn/702/S2005/Examples/dup2.html
                    printf("ERROR: %d", errno);
                    if (output == -1)
                    {
                        printf("ERROR: Outfile cannot be opened - Showing Result in shell instead\n");
                        close(output);
                    }
                    else
                    {
                        //printf("HER ER OUTPUT\n");
                        dup2(output, 1);
                        close(output);
                    }
                }

                if (execvp(command, argv) < 0)
                {
                    printf("An error has occured. Error type: %d\n", errno);
                    //Exit failure
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}

