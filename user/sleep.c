#include "kernel/types.h"
#include "user/user.h"

/**
1. Create child
2. pause child
3. wait for child to return
4. return program
*/

int main(int argc, const char* argv[])
{
    // Error Handling to always have one input into sleep
    if(argc == 1){
        exit(-1);
    }

    // Convert argument to sleep into int
    int n = atoi(argv[1]);

    sleep(n);

    exit(0);
}