#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
  if (argc != 1) {
    fprintf(2, "usage: pingpong\n");
    exit(1);
  }

  int child_to_parent[2], parent_to_child[2];
  if (pipe(child_to_parent) == -1) {
    fprintf(2, "pingpong: create pipe failed\n");
    exit(1);
  }
  if (pipe(parent_to_child) == -1) {
    fprintf(2, "pingpong: create pipe failed\n");
    exit(1);
  }

  char c;

  int pid = fork();
  if (pid < 0) {
    fprintf(2, "pingpong: fork failed\n");
    exit(1);
  } else if (pid == 0) {
    close(child_to_parent[0]);
    close(parent_to_child[1]);
    read(parent_to_child[0], &c, 1);
    close(parent_to_child[0]);
    fprintf(1, "%d: received ping\n", getpid());
    if (write(child_to_parent[1], &c, 1) < 1) {
      fprintf(2, "pingpong: child write to pipe failed\n");
    }
    close(child_to_parent[1]);
  } else {
    close(parent_to_child[0]);
    close(child_to_parent[1]);
    if (write(parent_to_child[1], &c, 1) < 1) {
      fprintf(2, "pingpong: parent write to pipe failed\n");
    }
    close(parent_to_child[1]);
    read(child_to_parent[0], &c, 1);
    close(child_to_parent[0]);
    wait(0);
    fprintf(1, "%d: received pong\n", getpid());
  }

  exit(0);
}
