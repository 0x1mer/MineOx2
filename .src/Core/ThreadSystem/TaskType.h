#pragma once

// Defines categories of tasks to allow the dispatcher
// to route work to the most suitable worker threads.
// Categorization improves cache locality, load balancing,
// and reduces cross-thread interference.
enum class TaskType
{
    // I/O-bound tasks (disk, network, file reads, streaming, etc.)
    // Typically low CPU usage but may block on system calls.
    IO,

    // Very small, fast, CPU-light operations.
    // Usually executed in batches for maximum throughput.
    Light,

    // CPU-intensive operations that take significantly more time.
    // Heavy tasks are typically isolated to avoid blocking others.
    Heavy,
};