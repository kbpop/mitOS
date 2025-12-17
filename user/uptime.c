#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(){
    int time = uptime();
    printf("Uptime Ticks: %d\n", time);
    exit(0);
}