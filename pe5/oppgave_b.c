#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <time.h>

long received = 0;
long double bandwith;

struct timespec start, stop;

void sig_handler(int signum){
    // As it is called once every second, not nescessary for now to divide by elapsed time ( 1 sec ) - Might want to change this for higher accuracy/ error handling" 
    int check = clock_gettime(CLOCK_MONOTONIC_RAW, &stop);
    long double timedelta;

    if ( check == -1) {
        perror("Error with timing. Using standard time of 1 sec.");
        timedelta = 1;
    }
    else {
        timedelta = ( stop.tv_sec - start.tv_sec ) + ( stop.tv_nsec - start.tv_nsec )/1000000000 ;
    }

    printf("Bandwidth (B/s): %LF\n", bandwith);
    
    bandwith = 0;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    
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

    if ( pipe(fd) != 0 ){
        perror( "Error creating pipe. " );
        exit( EXIT_FAILURE );
    }
       
    if( ( childpid = fork() ) == -1){
        perror( "Error during forking. " );
        exit( EXIT_FAILURE );
    }

    if( childpid == 0 ){
        /* Child process closes up input side of pipe */
        close( fd[0] );
        while( 1 ){
            number_of_bytes = write( fd[1], str, ( strlen( str ) + 1 ));
            if ( number_of_bytes == -1 ) {
                printf( "Error %d", errno );
                exit( EXIT_FAILURE );
            }
        }
        exit(0);
    }
    else
    {
        /* Parent process closes up output side of pipe */
        printf( "PARENTPROCESS PID %d \n", getpid() );
        close( fd[1] );
        alarm( 1 ); //first alarm
        // Setting start timestamp
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        while ( 1 ){
            number_of_bytes = read( fd[0], readbuffer, strlen( readbuffer ) + 1 );
            if ( number_of_bytes == -1 ) {
                printf( "Error %d", errno );     
                exit( EXIT_FAILURE );
            }
            received += number_of_bytes;
            bandwith += number_of_bytes;
            //printf("Cummulative bytes is %d and string is %s \n", received, readbuffer);
        }
        exit( EXIT_SUCCESS );
    }
}



int main( int argc, char *argv[] ) {
    
    int block_size = 1000;

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
        block_size = atoi( argv[1] );  
    }

    printf("___Benchmark___\n");
    printf("Blocksize: %d\n\n", block_size);

    run(block_size);
}