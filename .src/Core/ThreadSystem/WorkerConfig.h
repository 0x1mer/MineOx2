#pragma once

// Typical CPU cache line size on x86/x64. Used for padding structures to avoid
// false sharing between worker threads.
constexpr std::size_t CACHE_LINE_SIZE = 64;

// Number of busy-spin iterations before the worker yields execution.
// A higher value reduces context switches but increases CPU usage.
static constexpr int WORKER_SPIN_TRIES = 256;

// Number of std::this_thread::yield() calls before the worker falls back to
// a longer sleep. Helps balance responsiveness vs CPU burn.
static constexpr int WORKER_YIELD_TRIES = 64;

// Number of tasks to dequeue at once (batch processing).  
// Larger batches reduce synchronization overhead significantly.
static constexpr std::size_t WORKER_BATCH_SIZE = 32;

// Special value meaning "no CPU affinity".  
// When set, the thread runs on any available core.
static constexpr std::size_t NO_AFFINITY = SIZE_MAX;

// Worker statistics toggle.
// 0 — disabled (recommended in production for performance)
// 1 — enabled (useful for debugging and performance tests)
#define WORKER_ENABLE_STATS 1
