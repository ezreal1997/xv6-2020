#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(2, "usage: xargs args ...\n");
    exit(1);
  } else if (argc > MAXARG+1) {
    fprintf(2, "xargs: too many args\n");
    exit(1);
  }

  char *args[MAXARG];
  for (int i = 1; i < argc; i++) args[i - 1] = argv[i];

  char buf[512];
  int flag = 1;

  while (flag) {
    int buf_len = 0;
    int arg_pos = 0;
    int args_idx = argc - 1;
    while (1) {
      char c;
      if ((flag = read(0, &c, 1)) == 0) break;
      if (c == ' ' || c == '\n') {
          buf[buf_len++] = 0;
          args[args_idx++] = &buf[arg_pos];
          arg_pos = buf_len;
          if (c == '\n') break;
      }
      else buf[buf_len++] = c;
    }
    if (buf_len == 0) break; // Empty buffer, do nothing.
    args[args_idx] = 0; // Set 0 to the last arg.
    int pid = fork();
    if (pid < 0) {
      fprintf(2, "xargs: fork failed\n");
      exit(1);
    } else if (pid == 0) {
      exec(args[0], args);
    } else {
      wait(0);
    }
  }

  exit(0);
}
