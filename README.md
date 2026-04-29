# Concurrent Rate Limiter

Thread-safe rate limiter in C++17 with Fixed Window and Sliding Window algorithms.

## Build

    mkdir build && cd build
    cmake ..
    make -j4
    cp ../config.json .

## Run stress test

    ./stress_test

## Run interactive CLI

    ./rate_limiter
    # Type client_id and press Enter
    # Ctrl+C to quit

## Algorithms

- Fixed Window: O(1) per request, simple, burst possible at window boundary
- Sliding Window Log: deque of timestamps per client, accurate, no burst edge case

## Design

- IRateLimiter interface: swap algorithms without changing service layer
- Per-client limiter instances: client_A lock never blocks client_B
- std::mutex per limiter: every check is a write, shared_mutex gives no benefit
- Atomic counters for stats: no lock needed for read-heavy summary
- Config from JSON: per-client limits, unknown clients get default

## Thread Safety

Run with ThreadSanitizer to verify zero races:

    cmake -DCMAKE_CXX_FLAGS="-fsanitize=thread -g" -B build_tsan
    cmake --build build_tsan -j4
