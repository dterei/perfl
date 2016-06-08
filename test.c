#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "work.h"

#define NSEC 1000000000

int main(void)
{
  struct timespec start, finish;
  uint64_t delta;
  work *w;

  w = work_setup(1024);

  clock_gettime(CLOCK_MONOTONIC, &start);
  work_run(w, 1000);
  clock_gettime(CLOCK_MONOTONIC, &finish);

  delta = 0;
  delta += finish.tv_sec * NSEC + finish.tv_nsec;
  delta -= start.tv_sec * NSEC + start.tv_nsec;
  delta /= 1000;
  fprintf(stdout, "Ran for %ldns\n", delta);

  return 0;
}
