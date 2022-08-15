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

static const unsigned int NR_PAGES = 64;
static int ps;

static void rss(char* addr, size_t len) {
  unsigned char pages[len / ps];
  if (mincore(addr, len, pages) == -1) err(1, "mincore");
  fprintf(stdout, "[");
  for (int i = 0; i < len / ps; i++) {
    fprintf(stdout, "%s%c", i ? " " : "", (pages[i] & 0x1) ? 'x' : 'o');
  }
  fprintf(stdout, "]\n");
}

int main(int argc, char **argv) {

  ps = getpagesize();
  fprintf(stdout, "pagesize = %d\n", ps);

  int fd;
  const char *file = argc > 1 ? argv[1] : "test.txt";
  if ((fd = open(file, O_RDONLY)) == -1) {
    fprintf(stdout, "\nCreate a large enough input file:\n"
            "(e.g. `for i in {1..65536}; do echo -n '01234567' >> %s; done`)\n\n", file);
    err(1, "%s", file);
  }
  fprintf(stdout, "Opened %s for reading\n", file);

  fprintf(stdout, "\nRun `echo 1 > /proc/sys/vm/drop_caches` to clean the page cache\n");
  getchar();

  char *map;
  const int size = ps * NR_PAGES;
  if ((map = (char*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) err(1, "mmap");;
  fprintf(stdout, "mmaped first %dkb of %s for reading to %p\n", size / 1024, file, map);
  rss(map, ps * NR_PAGES);
  getchar();

  fprintf(stdout, "Value at map[%d] = %c\n", 0, map[0]);
  rss(map, ps * NR_PAGES);
  getchar();

  char buf[512];
  int count;
  if ((count = read(fd, buf, sizeof(buf))) == -1) err(1, "read");

  fprintf(stdout, "Reading first %d bytes from %s with 'read() to buf (buf[0]=%c, file pos=%ld)'\n", count, file, buf[0], lseek(fd, 0, SEEK_CUR));
  rss(map, ps * NR_PAGES);
  getchar();

  for (int i=1; i < ps * NR_PAGES / 4 / 512; i++) {
    if ((count += read(fd, buf, sizeof(buf))) == -1) err(1, "read");
  }
  fprintf(stdout, "Reading up to %d (%dkb) bytes from %s (file pos=%ld)\n", count, count / 1024, file, lseek(fd, 0, SEEK_CUR));
  rss(map, ps * NR_PAGES);
  getchar();

  if ((count = read(fd, buf, sizeof(buf))) == -1) err(1, "read");
  fprintf(stdout, "Reading %d more bytes from %s (file pos=%ld)\n", count,  file, lseek(fd, 0, SEEK_CUR));
  rss(map, ps * NR_PAGES);
  getchar();

  fprintf(stdout, "Value at map[%d] = %c\n", 1024 * 64 + 1, map[1024 * 64 + 1]);
  rss(map, ps * NR_PAGES);
  getchar();

}
