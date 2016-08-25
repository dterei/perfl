#ifndef WORKLOAD_H
#define WORKLOAD_H

/**
 * work.h - fake CPU work generation.
 *
 * author: David Terei <code@davidterei.com>
 * copyright: Copyright (c) 2016, David Terei
 * license: BSD
 */

#include <stdint.h>

/**
 * A 'work' is just a block of memory we'll touch to simulate a working sets
 * cache footprint.
 */
typedef struct work {
    double iter_per_us;  // how many iterations in a microsecond of work
    int dlines;          // how large is memory workspace
    char mem[];          // the working set
} work;

/* Setup a working set of 'lines' cachelines size. */
work *work_setup(int lines);

/* Run a CPU bound computation over the working set for 'us' microseconds. */
void work_run(work *w, uint64_t us);

/* Busy wait for specified number of cycles. */
static inline void wait_cycles(uint64_t cycles)
{
  asm volatile( "MOV %0, %%rcx\n\t"
                "loop: DEC %%rcx\n\t"
                "CMP $0, %%rcx\n\t"
                "JNZ loop"
                :: "r" (cycles) );
}

/**
 * Alternative: link-list approach.
 *
 * Credit: Ulrich Drepper, http://lwn.net/Articles/252125/
 */
#if 0

/* Set padding so work_elm is cacheline sized. */
#define PADDING 56

struct work_elm {
  struct work_elm* next;
  char pad[PADDING];
};
#endif

#endif /* WORKLOAD_H */
