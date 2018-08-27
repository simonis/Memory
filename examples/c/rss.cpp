#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>

#include <sys/mman.h>

#define NR_PAGES 64

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

  char *map;
  if ((map = (char*)mmap((void*)0x800000, ps * NR_PAGES, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) err(1, "mmap");

  fprintf(stdout, "mmaped %d bytes (%dkb, %d pages) at %p with PROT_READ | PROT_WRITE\n", ps * NR_PAGES, (ps * NR_PAGES) / 1024, NR_PAGES, map);
  rss(map, ps * NR_PAGES);
  getchar();

  for (int i = 0; i < NR_PAGES; i++) {
    map[i*ps] = 42; // touch page
  }
  fprintf(stdout, "Touched all pages\n");
  fprintf(stdout, "map[i] = %d (with 'map == %p' and 'i from [0, %d) * %d')\n", map[0], map, NR_PAGES, ps);
  rss(map, ps * NR_PAGES);
  getchar();

  if ((map = (char*)mmap(map, ps * (NR_PAGES / 4), PROT_NONE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) err(1, "mmap");
  fprintf(stdout, "%p - %p : PROT_NONE\n", map, map + ps * (NR_PAGES / 4));
  fprintf(stdout, "Remapped first %d pages with PROT_NONE\n", NR_PAGES / 4);
  rss(map, ps * NR_PAGES);
  getchar();

  for (int i = NR_PAGES / 4; i < NR_PAGES / 2; i++) {
    char *tmp_map;
    if ((tmp_map = (char*)mmap(&map[i*ps], ps, PROT_NONE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) err(1, "mmap");
    fprintf(stdout, " %p - %p : remapped with PROT_NONE\n", tmp_map, tmp_map + ps);
  }
  fprintf(stdout, "Remapped page %d to page %d with PROT_NONE\n", NR_PAGES / 4, (NR_PAGES / 2) - 1);
  rss(map, ps * NR_PAGES);
  getchar();

  if (mmap(map, ps, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0) == MAP_FAILED) err(1, "mmap");
  fprintf(stdout, "Remapped first page at %p to PROT_READ|PROT_WRITE|PROT_EXEC\n", map);
  rss(map, ps * NR_PAGES);
  getchar();

  fprintf(stdout, "map[0] == *%p == %d\n", map, map[0]);
  rss(map, ps * NR_PAGES);
  fprintf(stdout, "NOTICE: after reading from the remapped page, 'mincore()' reports the page as resident while 'pmap' still reports it as non-resident!\n");
  getchar();

  map[0] = 0;
  fprintf(stdout, "Assigning 0 to map[0] == *%p\n", &map[0]);
  fprintf(stdout, "map[0] == *%p == %d\n", map, map[0]);
  rss(map, ps * NR_PAGES);
  getchar();

  map[0] = 99;
  fprintf(stdout, "Assigning 99 to map[0] == *%p\n", &map[0]);
  fprintf(stdout, "map[0] == *%p == %d\n", map, map[0]);
  rss(map, ps * NR_PAGES);
  getchar();

  if (madvise(&map[ps * (NR_PAGES * 3) / 4], ps * 2, MADV_DONTNEED) == -1) err(1, "madvise");
  fprintf(stdout, "madvised two page at %p to MADV_DONTNEED\n", &map[ps * (NR_PAGES * 3) / 4]);
  rss(map, ps * NR_PAGES);
  getchar();

  fprintf(stdout, "map[%d] == *%p == %d\n", ps * (NR_PAGES * 3) / 4, &map[ps * (NR_PAGES * 3) / 4], map[ps * (NR_PAGES * 3) / 4]);
  rss(map, ps * NR_PAGES);
  fprintf(stdout, "NOTICE: after reading from the MADV_DONTNEED page, 'mincore()' reports the page as resident while 'pmap' still reports it as non-resident!\n");
  getchar();

  map[ps * (NR_PAGES * 3) / 4] = 99;
  fprintf(stdout, "Assigning 99 to map[%d] == *%p\n", ps * (NR_PAGES * 3) / 4, &map[ps * (NR_PAGES * 3) / 4]);
  fprintf(stdout, "map[%d] == *%p == %d\n", ps * (NR_PAGES * 3) / 4, &map[ps * (NR_PAGES * 3) / 4], map[ps * (NR_PAGES * 3) / 4]);
  rss(map, ps * NR_PAGES);
  getchar();

  if (mprotect(&map[ps * NR_PAGES - 2 * ps], ps * 2, PROT_NONE) == -1) err(1, "mprotect");
  fprintf(stdout, "mprotected two page at %p to PROT_NONE\n", &map[ps * NR_PAGES - 2 * ps]);
  rss(map, ps * NR_PAGES);
  getchar();

  if (mprotect(&map[ps * NR_PAGES - 2 * ps], ps * 2, PROT_READ | PROT_WRITE) == -1) err(1, "mprotect");
  fprintf(stdout, "mprotected two page at %p to PROT_READ | PROT_WRITE\n", &map[ps * NR_PAGES - 2 * ps]);
  rss(map, ps * NR_PAGES);
  getchar();

  fprintf(stdout, "map[%d] == *%p == %d\n", ps * NR_PAGES - 2 * ps, &map[ps * NR_PAGES - 2 * ps], map[ps * NR_PAGES - 2 * ps]);
  rss(map, ps * NR_PAGES);
  getchar();

}
