#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>

long received = 0;
long double bandwith;


void sig_handler(int signum){
    // As it is called once every second, not nescessary for now to divide by elapsed time ( 1 sec ) - Might want to change this for higher accuracy/ error handling" 

    printf("Bandwidth (B/s): %.0LF \n", bandwith);
    
    bandwith = 0;
    
    alarm(1);
}

void sigusr_handler(int signum){
    printf("Current accumulated received bytes: %ld \n", received);
}

int run(size_t block_size){
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
        while(1){
            if (write(fd_child, str, (strlen(str) + 1)) == -1){
                printf("Error %d", errno);
                exit(1);
            }
        }
        close(fd_child);
        exit(0);
    }
    else
    {
        /* Parent process closes up output side of pipe */
        printf("PARENTPROCESS PID %d \n", getpid());
        fd_parent = open(named_pipe, O_RDONLY);
        alarm(1); //first alarm
        while (1){
            number_of_bytes = read(fd_parent, readbuffer, strlen(readbuffer) + 1);
            //printf("Number of bytes: %d \n", number_of_bytes);
            if (number_of_bytes == -1) {
                printf("Error %d", errno);     
                exit(1);
            }
            received += number_of_bytes;
            bandwith += number_of_bytes;
            //printf("Cummulative bytes is %d and string is %s \n", received, readbuffer);
        }
        close(fd_parent);
        exit(0);
    }
}



int main(int argc, char *argv[] ){

    size_t block_size = 1000;

    if (argc > 2) {
        printf("Too many arguments. Program takes only one argument!\n");
        exit(EXIT_FAILURE);
    }

    else if ( argc < 2) {
        printf("No argument given. Using standard block size of 1000 bytes.\n");
    }
    
    else {
        //Checking input    
        int i = 0;
        while( i < strlen( argv[1] )){
            if (! isdigit( argv[1][i] ) ){
                printf( "Argument must be a valid number!\n" );
                exit( EXIT_FAILURE );
            }
            i++;
        }
        block_size = strtoul( argv[1], NULL, 10 );
    }

    printf("___Benchmark___\n");
    printf("Blocksize: %ld\n\n", block_size);

    run(block_size);
}