#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

/**
xargs pseudo code:
1. Read all of the output as input
2. Read the command given
3. Fork and Execute for line of output the command
4. Wait until all finish
*/
int main(int argc, char *argv[])
{
    char buf[512];

    // Read previous input
    // loop for n arguments
    while(1){

        char c;
        int i = 0;
        while (read(0, &c, 1) == 1) {

            // If \n this is one line of input
            if (c == '\n') {
                buf[i] = 0;
                break;
            }

        buf[i++] = c;
        }

        // EOF reached
        if(i == 0)
            break;

        // Add null
        buf[i] = 0;


        // Moves arguments into correct place
        char *new_argv[MAXARG];

        int k;
        // Loop through the xargs arguments
        for(k = 1; k < argc; k++){
            new_argv[k-1] = argv[k];
        }
        // append line input to end of arguments
        new_argv[k-1] = buf;

        // Null addition
        new_argv[k] = 0;

        // Execution cycle
        int status;
        int pid = fork();

        // split processe
        if(pid == 0){
            // child
            exec(argv[1], new_argv);
        } else {
            // parent
            wait(&status);
        }
    }
    
    exit(0);
}