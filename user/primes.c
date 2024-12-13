#include "kernel/types.h"
#include "user/user.h"

int natural_proc();
int sieve_proc(int rfd, int prime);

int main(int argc, char *argv[]) {
  int rfd = natural_proc();
  int prime;
  while (read(rfd, &prime, sizeof(int))) {
    printf("prime %d\n", prime);
    rfd = sieve_proc(rfd, prime);
  }
  while (wait(0) >= 0);
  exit(0);
}

int natural_proc() {
  int fds[2];
  pipe(fds);
  if (!fork()) {
    close(fds[0]);
    for (int i = 2; i <= 280; i++) {
      write(fds[1], &i, sizeof(int));
    }
    close(fds[1]);
    exit(0);
  }

  close(fds[1]);
  return fds[0];
}

int sieve_proc(int rfd, int prime) {
  int fds[2];
  int num;
  pipe(fds);
  if (!fork()) {
    close(fds[0]);
    while (read(rfd, &num, sizeof(int))) {
      if (num % prime != 0) {
        write(fds[1], &num, sizeof(int));
      }
    }
    close(fds[1]);
    exit(0);
  }

  close(rfd);
  close(fds[1]);
  return fds[0];
}
