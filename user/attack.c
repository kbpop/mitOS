#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int
main(int argc, char *argv[])
{
  // your code here. You should write the secret to fd 2 using write

  char secret[40];

  read(1, secret, 40);

  // secret = p->trapframe->ra;
  printf("secret: %s", secret);

  // (e.g., write(2, secret, 8)
  write(2, &secret[32], 8);

  exit(0);
}
