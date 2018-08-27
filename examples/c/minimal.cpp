#include <err.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/time.h>

static int ps;

// Find the virtual memory area that contains addr (from hotspot/os/linux/os_linux.cpp)
static bool find_vma(char* addr, char** vma_low, char** vma_high) {
  FILE *fp = fopen("/proc/self/maps", "r");
  if (fp) {
    char *low, *high;
    while (!feof(fp)) {
      if (fscanf(fp, "%p-%p", &low, &high) == 2) {
        if (low <= addr && addr < high) {
          if (vma_low)  *vma_low  = low;
          if (vma_high) *vma_high = high;
          fclose(fp);
          return true;
        }
      }
      for (;;) {
        int ch = fgetc(fp);
        if (ch == EOF || ch == (int)'\n') break;
      }
    }
    fclose(fp);
  }
  return false;
}

static void rss(char* addr, size_t len) {
  unsigned char pages[len / ps];
  if (mincore(addr, len, pages) == -1) err(1, "mincore");
  fprintf(stdout, "[");
  for (int i = 0; i < len / ps; i++) {
    fprintf(stdout, "%s%c", i ? " " : "", (pages[i] & 0x1) ? 'x' : 'o');
  }
  fprintf(stdout, "]\n");
}

static char *experimental_low;
static void overflow(char *bottom) {
  char buf[1024];
  if ((char*)&buf - bottom > 1024 + 48) {
    overflow(bottom);
  }
  else {
    experimental_low = (char*)&buf;
  }
}

int main(int argc, char **argv) {

  ps = getpagesize();
  fprintf(stdout, "pagesize = %d\n", ps);

  rlimit rlim;
  getrlimit(RLIMIT_STACK, &rlim);
  fprintf(stdout, "soft limit = %ld\n", rlim.rlim_cur);
  fprintf(stdout, "hard limit = %ld\n", rlim.rlim_max);

  fprintf(stdout, "&argc = %p\n", &argc);
  fprintf(stdout, "environ = %p\n", environ);

  char *low, *high;
  find_vma((char*)&argc, &low, &high);
  fprintf(stdout, "primordial stack (current) [ %p - %p ]\n", low, high);
  char * real_low = high - rlim.rlim_cur;
  // The primordial thread is growable up to "ulimit -s"
  fprintf(stdout, "primordial stack (maximal) [ %p - %p ]\n", real_low, high);

  rss(low, high - low);

  getchar();

  // The following will crash
  //real_low[0] = 'A';

  overflow(real_low);
  fprintf(stdout, "experimetal low = %p\n", experimental_low);

  getchar();

  real_low[0] = 'A';

  getchar();

  rss(real_low, high - real_low);

  getchar();
}
