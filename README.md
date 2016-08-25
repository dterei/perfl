# Latency Perf

How do we evaluate performance latency?

## Work

Fake workload - busy spin.

## Tips

The first time a loop of code executes, you'll be paying the cost to load it
into the cache. Assuming fits into L1 instruction cache, expect to see first
iteration cost noticeably more than subsequent iterations.

