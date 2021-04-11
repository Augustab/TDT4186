#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
long received = 0;
long bandwith;

void sig_handler(int signum){
    printf("Bandwidth: %ld \n", bandwith);
    bandwith = 0;
    alarm(1);
}

void sigusr_handler(int signum){
    printf("Current accumulated received bytes: %ld \n", received);
}

int run(int block_size){
    int number_of_bytes, fd_child, fd_parent;
    char named_pipe[15];
    sprintf(named_pipe, "%d", getpid());
    strcat(named_pipe, "named_pipe");
    pid_t childpid;
    char *str = malloc(block_size);
    memset(str, 'a', block_size-1);
    str[block_size] = '\0';
    char *readbuffer = malloc(block_size);
    memset(readbuffer, 'd', block_size-1);
    readbuffer[block_size] = '\0';
    signal(SIGALRM, sig_handler);
    signal(SIGUSR1, sigusr_handler);

    if (mkfifo(named_pipe, 0666) != 0){
            printf("pid %d \n", getpid());
            perror("PIPE");
            exit(1);
        }
       
    if((childpid = fork()) == -1){
        perror("Fork");
        exit(1);
    }

    if(childpid == 0){
        /* Child process closes up input side of pipe */
        fd_child = open(named_pipe, O_WRONLY);
        int j = 0;
        //printf("len %ld \n", strlen(str) + 1);
        while(j<300000){
            if (write(fd_child, str, (strlen(str) + 1)) == -1){
                printf("Error %d", errno);
                exit(1);
            }
            j += 1;
        }
        close(fd_child);
        exit(0);
    }
    else
    {
        /* Parent process closes up output side of pipe */
        printf("PARENTPROCESS PID %d \n", getpid());
        fd_parent = open(named_pipe, O_RDONLY);
        int i = 0;
        alarm(1); //first alarm
        //printf("len %ld \n", strlen(readbuffer) + 1);
        while (i<300000){
            number_of_bytes = read(fd_parent, readbuffer, strlen(readbuffer) + 1);
            if (number_of_bytes == -1) {
                printf("Error %d", errno);     
                exit(1);
            }
            received += number_of_bytes;
            bandwith += number_of_bytes;
            i += 1;
            //printf("Cummulative bytes is %d and string is %s \n", received, readbuffer);
        }
        close(fd_parent);
        exit(0);
    }
}



int main(){
    run(100000);
}