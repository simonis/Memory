#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

static int ps;

int main(int argc, char **argv) {

  ps = getpagesize();
  fprintf(stdout, "pagesize = %d\n", ps);

  int fd;
  const char *file = argc > 1 ? argv[1] : "test.txt";
  if ((fd = open(file, O_RDONLY)) == -1) err(1, "open");

  char buf[512];
  int count;
  if ((count = read(fd, buf, sizeof(buf))) == -1) err(1, "read");

  fprintf(stdout, "Opened %s for reading and read first %d bytes from it\n", file, count);
  getchar();

  for (int i=1; i < 1024; i++) {
    if ((count += read(fd, buf, sizeof(buf))) == -1) err(1, "read");
  }
  fprintf(stdout, "Read first %d (%dkb) bytes from it\n", count, count / 1024);
  getchar();

}
