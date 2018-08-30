#include <stdio.h>
#include <sys/time.h>

int main(int argc, char **argv) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  printf("seconds: %ld, microseconds: %ld\n", tv.tv_sec, tv.tv_usec);
  getchar();
}
