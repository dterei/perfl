#ifndef WORKLOAD_H
#define WORKLOAD_H

/**
 * work.h - fake CPU work generation.
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

#endif /* WORKLOAD_H */
