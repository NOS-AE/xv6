#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

const char *fmtname(const char *path) {
  static char buf[DIRSIZ + 1];
  const char *p;

  // Find first character after last slash.
  for (p = path + strlen(path); p >= path && *p != '/'; p--);
  p++;

  // Return blank-padded name.
  int len = strlen(p);
  if (len >= DIRSIZ) return p;
  memmove(buf, p, len);
  buf[len] = 0;
  return buf;
}

void find(const char *path, const char *name) {
  int fd;
  struct stat st;
  struct dirent de;
  char buf[512], *p;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  // printf("%s is %d\n", path, st.type);
  switch (st.type) {
    case T_DEVICE:
    case T_FILE:
      const char *fn = fmtname(path);
      if (!strcmp(fn, name)) {
        printf("%s\n", path);
      }
      break;
    case T_DIR:
      if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
        printf("find: path too long\n");
        break;
      }
      strcpy(buf, path);
      p = buf + strlen(buf);
      *p++ = '/';
      while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (!strcmp(de.name, "..") || !strcmp(de.name, ".")) {
          continue;
        }
        if (de.inum == 0) continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        find(buf, name);
      }
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(2, "usage: find path name\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
