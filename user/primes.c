#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define DATASIZE sizeof(int)

#define IN 0
#define OUT 1
#define ERR 2

void
close_pipe(int *fd) {
  close(fd[IN]);
  close(fd[OUT]);
}

void
primes() {
  int n;
  if (read(IN, &n, DATASIZE) != DATASIZE) {
    close(IN);
    return;
  }
  printf("prime %d\n", n);

  int fd[2];
  if (pipe(fd) == -1) {
    fprintf(ERR, "primes: create pipe failed\n");
    return;
  }

  int pid = fork();
  if (pid < 0) {
    fprintf(ERR, "primes: fork failed\n");
    return;
  } else if (pid == 0) {
    close(IN);
    dup(fd[0]);
    close_pipe(fd);
    primes();
  } else {    
    close(OUT);
    dup(fd[1]);
    close_pipe(fd);
    int tmp;
    while (read(IN, &tmp, DATASIZE) == DATASIZE) {
      if (tmp % n == 0) {
        continue;
      }
      if (write(OUT, &tmp, DATASIZE) != DATASIZE) {
        fprintf(ERR, "primes: write to pipe failed\n");
        return;
      }
    }
    close(IN);
    close(OUT);
    wait(0);
  }
}

int
main(int argc, char *argv[]) {
  int fd[2];
  if (pipe(fd) == -1) {
    fprintf(ERR, "primes: create pipe failed\n");
    exit(1);
  }

  int pid = fork();
  if (pid < 0) {
    fprintf(ERR, "primes: fork failed\n");
    exit(1);
  } else if (pid == 0) {
    close(IN);
    dup(fd[IN]);
    close_pipe(fd);
    primes();
  } else {    
    close(OUT);
    dup(fd[OUT]);
    close_pipe(fd);
     for (int i = 2; i <= 35; ++i) {
      if (write(OUT, &i, DATASIZE) != DATASIZE) {
        fprintf(ERR, "primes: write to stdout failed\n");
        exit(1);
      }
    }
    close(OUT);
    wait(0);
  }

  exit(0);
}
