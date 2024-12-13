#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
  // option & command
  char *cmd = "echo";
  int cmd_idx = 0;
  if (argc > 1) {
    cmd_idx = 1;
  }

  // command's argv
  char *cmd_argv[MAXARG];
  int cmd_argv_idx = 1;
  if (cmd_idx > 0) {
    cmd = argv[cmd_idx];
    for (int i = cmd_idx + 1; i < argc; i++) {
      if (cmd_argv_idx >= MAXARG - 2) {
        fprintf(2, "xargs: too many arguments");
      }
      cmd_argv[cmd_argv_idx++] = argv[i];
    }
  }
  cmd_argv[0] = cmd;

  // readline
  int n;
  int line_idx = 0;
  char buf[64];
  char line[512];
  while ((n = read(0, buf, sizeof(buf)))) {
    if (n < 0) {
      fprintf(2, "xargs: read failed\n");
      exit(1);
    }
    for (int i = 0; i < n; i++) {
      if (buf[i] == '\n') {
        line[line_idx] = 0;
        line_idx = 0;
        cmd_argv[cmd_argv_idx] = line;
        cmd_argv[cmd_argv_idx + 1] = 0;
        if (!fork()) {
          exec(cmd, cmd_argv);
        }
        wait(0);
      } else {
        if (line_idx >= 512) {
          fprintf(2, "xargs: line too long (>512)\n");
          exit(1);
        }
        line[line_idx++] = buf[i];
      }
    }
  }
}