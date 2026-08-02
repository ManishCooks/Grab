# Grab Allocator Experiments

This repository contains a small custom allocator prototype and a benchmark-style experiment to measure multi-threaded allocation time.

## What the experiment does

`main.cpp` runs a simple stress test:

- Takes input: number of threads.
- Creates `n` threads.
- Each thread performs `100000` allocations of `16` bytes using the custom allocator.
- Measures total elapsed time and prints it in milliseconds.

## Lock Contention Experiment

The following screenshot shows the lock contention experiment results obtained while benchmarking the allocator under multiple threads.

![Lock Contention Experiment](expt.png)

## Allocator notes

The allocator in `grab.hpp`:

- Reserves a large memory pool with `mmap` (currently `1 GB` in `main.cpp`).
- Uses segregated free lists with 8-byte size classes.
- Stores payload size in a small header before returned memory.
- Uses a mutex for thread safety in `allocate`/`deallocate`.

## Files read in this repo

- `main.cpp` — benchmark driver and timing logic.
- `grab.hpp` — allocator implementation.

## Build and run (example)

Compile with a C++ compiler and pthread support, then run:

```bash
g++ -O2 -std=c++17 main.cpp -o build/grab_test -pthread
./build/grab_test