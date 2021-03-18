#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    int pid;
    printf("In parent: PID %d PPID %d\n", getpid(), getppid());
    int seconds;
    while (1)
    {
        printf("Enter seconds to sleep or 0 to exit:\n");
        scanf( "%d", &seconds);
        if (seconds < 1)
        {
            printf("Exiting...\n");
            exit(0);
        }
        pid = fork();
        if (pid > 0) // In parent process
        {
            printf("Created child process %d, which is waiting for %d seconds\n", pid, seconds);
        }
        else if (pid == 0) // In child process
        {
            sleep(seconds);
            printf("\aProcess %d: BEEEP BEEEP BEEEEEEEP\n", getpid());
            exit(0);
        }
    }
}