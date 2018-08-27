#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/time.h>

static int ps;

static void rss(char* addr, size_t len) {
  unsigned char pages[len / ps];
  if (mincore(addr, len, pages) == -1) {
    if (errno = ENOMEM) {
      fprintf(stdout, "[%p - %p] contains unmapped pages\n", addr, addr + len);
      return;
    }
    err(1, "mincore");
  }
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

static void print_stack_attrs(pthread_t tid) {
  pthread_attr_t attr;
  if (pthread_getattr_np(tid, &attr)) err(1, "pthread_getattr_np");

  size_t stack_size, guard_size;
  void *stack_addr;
  if (pthread_attr_getstack(&attr, &stack_addr, &stack_size)) err(1, "pthread_attr_getstack");
  if (pthread_attr_getguardsize(&attr, &guard_size)) err(1, "pthread_attr_getguardsize");
  fprintf(stdout, "  Stack x%lx [%p - %p), size %ld (%ldkb), guard size %ld (%ldkb)\n",
          tid, stack_addr, (char*)stack_addr + stack_size, stack_size, stack_size / 1024, guard_size, guard_size / 1024);

  rss((char*)stack_addr, stack_size);
}

static void* thread_start(void *arg) {
  getchar();
  if (arg != NULL) {
    overflow((char*)arg);
    fprintf(stdout, "--> after overflow\n");
    getchar();

    pthread_attr_t attr;
    if (pthread_getattr_np(pthread_self(), &attr)) err(1, "pthread_getattr_np");
    size_t stack_size;
    void *stack_addr;
    if (pthread_attr_getstack(&attr, &stack_addr, &stack_size)) err(1, "pthread_attr_getstack");
    if (madvise(stack_addr, ((char*)&stack_addr - ps) - (char*)stack_addr, MADV_DONTNEED) == -1) err(1, "madvise");
    fprintf(stdout, "--> MADV_DONTNEED unused stack\n");
    getchar();

    overflow((char*)arg);
    fprintf(stdout, "--> after second overflow\n");
    getchar();
    fprintf(stdout, "--> exiting thread\n");
    // Notice: the glibc pthread implementation issues an madvise(MADV_DONTNEED) call for the unsued part of a stack
    // after the thread exits. This removes the corresponing memory pageds from the RSS and looks as follows:
    // --> exiting thread
    // [pid  6588] madvise(0x800000, 1024000, MADV_DONTNEED) = 0
    //  > /lib/x86_64-linux-gnu/libc-2.19.so(madvise+0x7) [0xf85e7]
    //  > /lib/x86_64-linux-gnu/libpthread-2.19.so(start_thread+0x181) [0x8241]
    //  > /lib/x86_64-linux-gnu/libc-2.19.so(clone+0x6d) [0xfdffd]
    //
    // See advise_stack_range(): https://code.woboq.org/userspace/glibc/nptl/allocatestack.c.html#advise_stack_range
    // called from start_thread() at: https://code.woboq.org/userspace/glibc/nptl/pthread_create.c.html#552
  }
}

int main(int argc, char **argv) {

  ps = getpagesize();
  fprintf(stdout, "pagesize = %d\n", ps);

  rlimit rlim;
  getrlimit(RLIMIT_STACK, &rlim);
  fprintf(stdout, "soft limit = %ld (%ldkb)\n", rlim.rlim_cur, rlim.rlim_cur / 1024);
  fprintf(stdout, "hard limit = %ld%s\n", rlim.rlim_max, (rlim.rlim_max == -1) ? " (unlimited)" : "");

  fprintf(stdout, "Attributes of the main (primordial) thread:\n");
  print_stack_attrs(pthread_self());

  getchar();

  pthread_t tid_default;
  if (pthread_create(&tid_default, NULL, &thread_start, NULL)) err(1, "pthread_create");
  fprintf(stdout, "Created thread x%lx with default attributes:\n", tid_default);
  print_stack_attrs(tid_default);
  if (pthread_join(tid_default, NULL))  err(1, "pthread_join");

  getchar();

  pthread_attr_t attr;
  if (pthread_attr_init(&attr))  err(1, "pthread_attr_init");

  size_t size = 1024 * 1024;
  char *stack_addr;

  if ((stack_addr = (char*)mmap((void*)0x800000, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) err(1, "mmap");
  fprintf(stdout, "--> after mmap\n");
  getchar();
  if (pthread_attr_setstack(&attr, stack_addr, size)) err(1, "pthread_attr_setstack");

  pthread_t tid_custom;
  if (pthread_create(&tid_custom, &attr, &thread_start, stack_addr)) err(1, "pthread_create");
  fprintf(stdout, "Created thread x%lx with custom attributes: (addr = %p, size = %ld)\n", tid_custom, stack_addr, size);
  print_stack_attrs(tid_custom);
  if (pthread_join(tid_custom, NULL))  err(1, "pthread_join");

  getchar();
}
