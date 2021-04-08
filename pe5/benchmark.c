#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


int main(int block_size){
    int fd[2], number_of_bytes, received;
    pid_t childpid;
    char string[block_size];
    char readbuffer[block_size];

    pipe(fd);

    if((childpid = fork()) == -1){
        perror("Fork");
        exit(1);
    }

    if(childpid == 0)
    {
            /* Child process closes up input side of pipe */
            close(fd[0]);
            while (1){
                write(fd[1], string, (strlen(string) + 1));
            }
            exit(0);
    }
    else
    {
            /* Parent process closes up output side of pipe */
            close(fd[1]);
            received = 0;
            while (1){
                number_of_bytes = read(fd[0], readbuffer, sizeof(readbuffer));
                received += number_of_bytes;
                printf("Cummulative bytes is %d and string is %s", number_of_bytes, readbuffer);
            }
            exit(0);
    }
}