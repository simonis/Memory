#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

#include <sys/mman.h>

#define NR_ALLOCATIONS 256

static int ps;

int main(int argc, char **argv) {

  ps = getpagesize();
  fprintf(stdout, "pagesize = %d\n", ps);

  getchar();

  malloc(ps);

  fprintf(stdout, "malloced %d bytes\n", ps);
  getchar();

  void* buf[NR_ALLOCATIONS];

  for (int i = 0; i < NR_ALLOCATIONS; i++) {
    buf[i] = malloc(ps);
  }

  fprintf(stdout, "malloced %d times %d bytes (total %d bytes or %d kb)\n", NR_ALLOCATIONS, ps, NR_ALLOCATIONS * ps, (NR_ALLOCATIONS * ps) / 1024);
  getchar();

  for (int i = 0; i < NR_ALLOCATIONS; i++) {
    free(buf[NR_ALLOCATIONS - i - 1]);
  }

  fprintf(stdout, "freed all previously allocated bytes\n");
  getchar();

  for (int i = 0; i < NR_ALLOCATIONS; i++) {
    buf[i] = malloc(ps);
  }

  fprintf(stdout, "malloced %d times %d bytes and freed first %d allocations\n", NR_ALLOCATIONS, ps, NR_ALLOCATIONS - 1);
  for (int i = 1; i < NR_ALLOCATIONS; i++) {
    free(buf[NR_ALLOCATIONS - i - 1]);
  }

  getchar();

}
