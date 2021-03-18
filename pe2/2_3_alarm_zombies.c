#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pid;
    int wp;
    int status;
    printf("In parent: PID %d PPID %d\n", getpid(), getppid());
    int seconds;
    while(1)
    {
        printf("Enter seconds to count down (Enter 0 quit):\n");
        scanf("%d", &seconds);

        if (seconds < 1)
        {
            printf("Exiting...");
            exit(0);
        }
        wp = waitpid(-1, &status, WNOHANG); // Terminating Zombies with waitpid(2)
        while (wp > 0)
        {    
            printf("Terminated Child Process: %d\n",wp);
            wp = waitpid(-1, &status, WNOHANG);
        }
        pid = fork(); //Forking a child process

        if (pid > 0) //Parent process          
        {
            printf("Created a child Process %d, which is waiting for %d seconds\n", pid, seconds);
        }
        else if (pid == 0) //Child process
        {
            sleep(seconds);
            printf("\aProcess %d: BEEEP BEEEP BEEEEEEEP\n", getpid());
            exit(0); //If vfork() use _exit()
        }
    }
}