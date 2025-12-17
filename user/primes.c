#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// primes pseudo code
// p = get a number from left neighbor
// print p
// loop:
//     n = get a number from left neighbor
//     if (p does not divide n)
//         send n to right neighbor

// recursive function
void sieve(int fd){
    // read process of child
    // read all of the numbers 
    // create pipes for parent and child
    int fds[2];

    int status;

    int received = 1;
    int flag = 0;
         
    // read the first number that comes in
    int n = read(fd, &received, sizeof(received));

    if (n == 0) {
        close(fd);
        exit(0);
    }

    int prime = received;
    printf("prime %d\n", prime);

    int pid; 
         
    while(1){

        // read from parent
        n = read(fd, &received, sizeof(received));
        if(n == 0){
            // Close read from parent
            close(fd);
            break;
        }

        // write to child
        if(received % prime != 0){
            if(!flag){
                // create pipe
                pipe(fds);

                // create child process
                pid = fork();

                if(pid == 0){
                    // child process
                    // Close the write end of pipe 
                    close(fds[1]); 
                    // Close read from parent
                    close(fd);

                    sieve(fds[0]);
                } else {
                    flag = 1;
                    // Close the read end of pipe
                    close(fds[0]);
                }
            }

            write(fds[1], &received, sizeof(received));
        }
    }

    if(flag){
        // Close the write end of pipe
        close(fds[1]); 
    }
    wait(&status);

    exit(0);
}

// Base function
int main(){

    int pid; 

    // fds[0] is read
    // fds[1] is write

    int fds[2];
     
    // initial fork creation
    pipe(fds);
    pid = fork();
     
    if (pid == 0) {
        close(fds[1]);
        sieve(fds[0]);        
    } else {
        // Generate 2 to 280 
        close(fds[0]);
        for(int i = 2; i <= 280; i++){
            // parent process
            write(fds[1], &i, sizeof(i));
        } 
        close(fds[1]);
        wait(0);
    }

    return exit(0);
}

