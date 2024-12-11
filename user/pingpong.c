#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc < 1) {
    fprintf(2, "Usage: sleep secs...\n");
    exit(1);
  }

  int fds1[2], fds2[2];
  char byte;

  if (pipe(fds1) < 0 || pipe(fds2) < 0) {
    fprintf(2, "pingpong: pipe failed\n");
    exit(1);
  }

  if (fork() != 0) {
    // parent
    close(fds1[0]);
    close(fds2[1]);
    write(fds1[1], &byte, 1);
    read(fds2[0], &byte, 1);
    printf("%d: received pong\n", getpid());
    wait(0);
  } else {
    // child
    close(fds1[1]);
    close(fds2[0]);
    read(fds1[0], &byte, 1);
    printf("%d: received ping\n", getpid());
    write(fds2[1], &byte, 1);
  }
  exit(0);
}