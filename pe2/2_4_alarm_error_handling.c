#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>


int main() {
    int pid;
    int wp;
    int status;
    printf("In parent: PID %d\n", getpid());
    // man page of getpid(2): These functions are always successful
    long seconds;
    char in_string[32] = {};
    char *pointer;
    while(1)
    {
        // we have left out error handling for printf since it rarely fails and piazza told us it was ok
        printf("Enter seconds to count down (Enter 0 quit):\n");
        scanf("%s", in_string);
        // Error handling for input from scanf
        int i = 0;
        while (in_string[i] != '\0')
        {
            if (!isdigit(in_string[i])) // Bad input
            // isdigit is used to check user input, the function will not fail
            {
                perror("Input is not an integer. Exiting\n");
                exit(EXIT_FAILURE); // No reason to test exit()
            }
            i++;
        }
        seconds = strtol(in_string, &pointer, 10);
        
        if (seconds < 1)
        {
            printf("Exiting...\n");
            exit(0);
        }
        // Terminating Zombies with waitpid(2)
        wp = waitpid(-1, &status, WNOHANG);
        while (wp > 0)
        {
            printf("Terminated Child Process: %d\n",wp);
            wp = waitpid(-1, &status, WNOHANG);
        }

        pid = fork(); // Forking a child process

        if (pid > 0) // Parent process          
        {
            printf("Created a child Process %d, which is waiting for %ld seconds\n", pid, seconds);
        }
        else if (pid == 0) // Child process
        {
            int sleep_left = sleep(seconds); // sleep returns 0 if counting finished 
            sleep(sleep_left); // can only handle one interrupt per alarm. 
            // we could also implement a while-loop to check for correct sleep, but piazza adviced us not to do so!
            printf("\aProcess %d: BEEEP BEEEP BEEEEEEEP\n", getpid()); // we have left out error handling for printf since it rarely fails. 
            exit(0); // If vfork() use _exit()
        }
        else // Forking failure
        {
            perror("Fork Error");
            exit(EXIT_FAILURE);
        }
    }
}