#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
int received;
int bandwith;

void sig_handler(int signum){
    static int timer;
    timer += 1;
    printf("Bandwidthmannen %d \n", bandwith);
    bandwith = 0;
    alarm(1);
}

int run(int block_size){
    int fd[2], number_of_bytes;
    pid_t childpid;
    char *str = malloc(block_size);
    memset(str, 'a', block_size-1);
    str[block_size] = '\0';
    char *readbuffer = malloc(block_size);
    memset(readbuffer, 'd', block_size-1);
    readbuffer[block_size] = '\0';
    signal(SIGALRM, sig_handler);


    if (pipe(fd) != 0){
        perror("PIPE");
        exit(1);
    }
       
    if((childpid = fork()) == -1){
        perror("Fork");
        exit(1);
    }

    if(childpid == 0){
        /* Child process closes up input side of pipe */
        close(fd[0]);
        int j = 0;
        //printf("len %ld \n", strlen(str) + 1);
        while(j<2000000){
            if (write(fd[1], str, (strlen(str) + 1)) == -1){
                printf("Error %d", errno);
                exit(1);
            }
            j += 1;
        }
        exit(0);
    }
    else
    {
        /* Parent process closes up output side of pipe */
        close(fd[1]);
        received = 0;
        int i = 0;
        alarm(1); //first alarm
        //printf("len %ld \n", strlen(readbuffer) + 1);
        while (i<2000000){
            number_of_bytes = read(fd[0], readbuffer, strlen(readbuffer) + 1);
            if (number_of_bytes == -1) {
                printf("Error %d", errno);     
                exit(1);
            }
            received += number_of_bytes;
            bandwith += number_of_bytes;
            i += 1;
            //printf("Cummulative bytes is %d and string is %s \n", received, readbuffer);
        }
        exit(0);
    }
}



int main(){
    run(8193);
}