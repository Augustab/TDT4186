#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    while (1)
    {
        int seconds;
        printf("Enter seconds to sleep or 0 to exit:\n");
        scanf( "%d", &seconds);
        if (seconds < 1)
        {
            printf("Exiting...\n");
            exit(0);
        }
        printf("Waiting for %d seconds\n", seconds);
        sleep(seconds);
        printf("\aBEEEP BEEEP BEEEEEEEP\n");
    }

}