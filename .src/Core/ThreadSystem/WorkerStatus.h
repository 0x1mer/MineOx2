#pragma once

#include <cstdint>

// Represents the current execution state of a worker thread.
// Stored as uint8_t to keep the enum compact and cache-friendly.
enum class WorkerStatus : uint8_t
{
    // The worker actively processes tasks.
    Running,

    // The worker temporarily stops taking new tasks but remains alive.
    Paused,

    // The worker is shutting down and will exit its main loop.
    Stopped
};