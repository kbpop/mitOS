#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int thread_create(
    void (*start_routine)(void *, void *), 
    void *arg1, 
    void *arg2
    ){

    void * ptr = malloc(2 * PGSIZE);
    if(ptr == 0) return -1;
    uint64 aligned_stack = (uint64)PGROUNDUP((uint64)ptr);
    *(void **)aligned_stack = ptr;
    int pid = clone(start_routine, arg1, arg2, (void *)aligned_stack);

    return pid;
}

int thread_join(void) {
  void *stack;
  
  int pid = join(&stack);
  if(pid < 0) return -1;

  void *orig_ptr = *(void **)stack;

  free(orig_ptr);
  
  return pid;
}