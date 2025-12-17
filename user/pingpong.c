#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(){

    // 1. Parents sends byte to child
    // 2. child prints received ping
    // 3. child returns byte
    // 4. parents reads byte
    // 5. parents prints received pong


    int pid; 
    int fds[2];
    char buf[1];

    pipe(fds);

    char c = 'K';

    pid = fork();
    if (pid == 0) {

        // child reads byte
        read(fds[1], buf, sizeof(buf));

        // if(n == c){
            int pi = getpid();
            printf("%d: received ping\n", pi);
            write(fds[1], buf, c);
        // }

    } else {

        // parent sends byte to child
        write(fds[0], buf, c);
 
        // parent reads byte 
        read(fds[0], buf, sizeof(buf));
        // if(n == c){
            int pi = getpid();
            printf("%d: received pong\n", pi);
        // }
    }

    return exit(0);
}