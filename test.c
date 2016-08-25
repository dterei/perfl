/**
 * test.c - fake CPU work generation testing.
 *
 * author: David Terei <code@davidterei.com>
 * copyright: Copyright (c) 2016, David Terei
 * license: BSD
 */

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "tsc.h"
#include "work.h"

#define NSEC 1000000000
#define TEST_NS 1000
#define TEST_CYCLES 1337

int main(void)
{
  struct timespec start, finish;
  uint64_t delta, t0, t1, tscerr;
  work *w;


  // Calibration
  fprintf(stdout, "# Calibrating TSC\n");
  tscerr = measure_tsc_overhead();
  fprintf(stdout, "TSC overhead of %ld cycles\n", tscerr);


  // Testing work_run
  fprintf(stdout, "\n# Testing work_run\n");
  fprintf(stdout, "Should run for %ldns\n", TEST_NS);

  w = work_setup(1024);

  clock_gettime(CLOCK_MONOTONIC, &start);
  work_run(w, TEST_NS);
  clock_gettime(CLOCK_MONOTONIC, &finish);

  delta = 0;
  delta += finish.tv_sec * NSEC + finish.tv_nsec;
  delta -= start.tv_sec * NSEC + start.tv_nsec;
  delta /= 1000;
  fprintf(stdout, "Actual run for %ldns\n", delta);


  // Testing wait_cycles
  fprintf(stdout, "\n# Testing wait_cycles\n");
  t0 = bench_start();
  wait_cycles(TEST_CYCLES);
  t1 = bench_end();

  fprintf(stdout, "Should wait for %ld cycles\n", TEST_CYCLES);
  fprintf(stdout, "Actual wait of %ld cycles\n", t1 - t0 - tscerr);

  return 0;
}
