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
long bandwith;

// Handler for SIGUSR1-signal.
void sig_handler(int signum){
    // As it is called once every second, not nescessary for now to divide by elapsed time ( 1 sec ) - Might want to change this for higher accuracy/ error handling" 

    printf("Bandwidth (B/s): %ld \n", bandwith);
    
    bandwith = 0;
    
    alarm(1);
}

// Handler for SIGUSR1-signal.
void sigusr_handler(int signum){
    printf("Current accumulated received bytes: %ld \n", received);
}

int run(size_t block_size){
    // Initializing file descriptor for child and parent.
    int number_of_bytes, fd_child, fd_parent;
    // Initializing name for the named pipe
    char named_pipe[15];
    sprintf(named_pipe, "%d", getpid());
    strcat(named_pipe, "named_pipe");
    // Initializing pid
    pid_t childpid;
    // Filling string of length block size with data
    char *str = malloc(block_size);
    memset(str, 'a', block_size-1);
    str[block_size] = '\0';
    // Initializing readbuffer with data.
    char *readbuffer = malloc(block_size);
    memset(readbuffer, 'd', block_size-1);
    readbuffer[block_size] = '\0';
    // Binding the two signals to the correct handler.
    signal(SIGALRM, sig_handler);
    signal(SIGUSR1, sigusr_handler);

    // Initializing named pipe with error-handling
    if (mkfifo(named_pipe, 0666) != 0){
            printf("pid %d \n", getpid());
            perror("PIPE");
            exit(1);
        }
       
    // Forking with error-handling.
    if((childpid = fork()) == -1){
        perror("Fork");
        exit(1);
    }

    if(childpid == 0){
        // Child-prosess opens pipe for writing only
        fd_child = open(named_pipe, O_WRONLY);
        while(1){
            // Writing str to fd_child (named pipe)
            if (write(fd_child, str, (strlen(str) + 1)) == -1){
                printf("Error %d", errno);
                exit(1);
            }
        }
        // Closing file descriptor
        close(fd_child);
        exit(0);
    }
    else{
        printf("PARENTPROCESS PID %d \n", getpid());
        // Parent-process opens pipe only for reading.
        fd_parent = open(named_pipe, O_RDONLY);
        alarm(1); // Setting first alarm
        while (1){
            // Reading content of fd_parent (named pipe) into readbuffer with error-handling.
            number_of_bytes = read(fd_parent, readbuffer, strlen(readbuffer) + 1);
            if (number_of_bytes == -1) {
                printf("Error %d", errno);     
                exit(1);
            }
            received += number_of_bytes;
            bandwith += number_of_bytes;
            //printf("Cummulative bytes is %d and string is %s \n", received, readbuffer);
        }
        // Closing file descriptor
        close(fd_parent);
        exit(0);
    }
}



// Main takes block size as argument.
int main( int argc, char *argv[] ) {
    // If no argument is given this is the standard size.
    size_t block_size = 1000;

    if (argc > 2) {
        printf("Too many arguments. Program takes only one argument!\n");
        exit(EXIT_FAILURE);
    }

    else if ( argc < 2) {
        printf("No argument given. Using standard block size of 1000 bytes.\n");
    }
    
    else {
        //Checking that input is a valid number  
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