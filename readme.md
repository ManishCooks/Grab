# Grab Allocator Experiments

This repository contains a high-performance custom memory allocator prototype and benchmark experiments to measure and resolve multi-threaded lock contention.

## What the experiment does

`main.cpp` runs a stress test to evaluate memory allocation scaling:
- Takes input: number of concurrent threads.
- Creates `n` threads.
- Each thread performs `100,000` allocations of `16` bytes using the custom allocator.
- Measures total elapsed time and prints it in milliseconds.

## Experiments & Benchmarks

### 1. Global Lock Contention (Baseline)
The initial design used a standard `std::mutex` to ensure thread safety across a shared memory pool. The benchmark below demonstrates severe lock contention as threads scale, forcing the CPU cores to sit idle while waiting for lock acquisition.

![Lock Contention Experiment](expt.png)

### 2. Thread-Local Storage (Zero-Lock Scalability)
To eliminate the global lock bottleneck, the allocator was redesigned using C++ `thread_local` storage. This provides every thread with its own isolated memory pool, entirely bypassing the need for mutexes. The benchmark below shows massive performance gains, achieving lock-free concurrency even at 1024 threads with zero lock contention.

![TLS Experiment](TLS_Expt.png)

## Allocator Architecture

The allocator implemented in `grab.hpp` features:
- **Segregated Free Lists:** Achieves strictly $O(1)$ allocation and deallocation times using array-based size classes.
- **Memory Mapping:** Reserves memory pools directly from the OS using `mmap`.
- **Payload-Based Sizing:** Rounds block requests to 8-byte multiples. This intentionally trades minor internal fragmentation for strict memory alignment, simplified math, and ultra-low latency.
- **Concurrency Model:** Evolved from global mutex locking to lock-free, isolated memory pools per thread via `thread_local`.

## Files in this repo

- `main.cpp` — benchmark driver and timing logic.
- `grab.hpp` — custom memory allocator implementation.

## Build and run

Compile with a C++17 compiler and pthread support, then run:

```bash
g++ -O2 -std=c++17 main.cpp -o build/grab_test -pthread
./build/grab_test