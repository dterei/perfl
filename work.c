/**
 * work.c - fake CPU work generation.
 *
 * author: David Terei <code@davidterei.com>
 * copyright: Copyright (c) 2016, David Terei
 * license: BSD
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "work.h"

#ifdef __GNUC__
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

#define CLOCK_CALIBRATE_N 1000
#define WORK_CALIBRATE_N 100000000
#define CACHELINE 64
#define NSEC 1000000000

/**
 * Measure overhead of calling clock, used to improve accuracy of
 * 'work_calibrate'.
 */
static uint64_t __clock_overhead(void)
{
    struct timespec start, finish;
    int ret, i;
    uint64_t delta;

    ret = clock_gettime(CLOCK_MONOTONIC, &start);
    if (ret == -1) {
        perror("clock_gettime() start");
        abort();
    }

    for (i = 0; i < CLOCK_CALIBRATE_N; i++) {
        ret = clock_gettime(CLOCK_MONOTONIC, &finish);
        if (ret == -1) {
            perror("clock_gettime() loop");
            abort();
        }
    }

    ret = clock_gettime(CLOCK_MONOTONIC, &finish);
    if (ret == -1) {
        perror("clock_gettime() finish");
        abort();
    }

    delta = 0;
    delta += finish.tv_sec * NSEC + finish.tv_nsec;
    delta -= start.tv_sec * NSEC + start.tv_nsec;
    delta /= CLOCK_CALIBRATE_N;

    return delta;
}

/**
 * Primitive PRNG using crc32q instruction. Don't really need "random" numbers,
 * just a way to touch different cache lines.
 */
static inline uint64_t __mm_crc32_u64(uint64_t crc, uint64_t val)
{
    asm("crc32q %1, %0" : "+r"(crc) : "rm"(val));
    return crc;
}

/**
 * Model a memory work by writing to n random bytes in some malloc'd memory.
 * Run for n iterations.
 */
static void NOINLINE __work_run(work *w, int n)
{
    int i;

    for (i = 0; i < n; i++) {
        w->mem[__mm_crc32_u64(0xDEADBEEF, i) % w->dlines] = 0x0A;
    }
}

/**
 * Public interface to '__work_run'. Run for specified number of microseconds.
 */
void work_run(work *w, uint64_t us)
{
    int n = (int)(w->iter_per_us * (double)us);
    __work_run(w, n);
}

/**
 * Figure out the mapping of iterations to microseconds for work.
 */
static void __work_calibrate(work *w)
{
    int ret;
    struct timespec start, finish;
    uint64_t start_ns, finish_ns, clock_err;
    double delta_us;

    clock_err = __clock_overhead() * 2;

    ret = clock_gettime(CLOCK_MONOTONIC, &start);
    if (ret == -1) {
        perror("clock_gettime() start");
        abort();
    }

    __work_run(w, WORK_CALIBRATE_N);

    ret = clock_gettime(CLOCK_MONOTONIC, &finish);
    if (ret == -1) {
        perror("clock_gettime() finish");
        abort();
    }

    start_ns = start.tv_sec * NSEC + start.tv_nsec;
    finish_ns = finish.tv_sec * NSEC + finish.tv_nsec;
    if (start_ns > finish_ns) {
        fprintf(stderr, "clock not monotonic?\n");
        abort();
    }

    delta_us = (double)(finish_ns - start_ns - clock_err) / 1000;
    w->iter_per_us = (double)WORK_CALIBRATE_N / delta_us;
}

static work *_work_alloc(int dlines)
{
    work *w;
    assert(dlines > 0);

    w = (work *)malloc(sizeof(work) + sizeof(char) * dlines);
    w->dlines = dlines;

    return w;
}

/**
 * Setup a working memory set of 'lines' cacheline size.
 */
work *work_setup(int lines)
{
    work *w;

    w = _work_alloc(lines * CACHELINE);
    if (!w) {
        fprintf(stderr, "work_setup: not enough memory");
        abort();
    }

    __work_calibrate(w);
    return w;
}
