#pragma once

#include "ITask.h"

#include <iostream>
#include <exception>
#include <functional>

// Task object designed for minimum overhead.
// Holds a callable and an optional error handler.
// The operator() is fully exception-safe and optimized
// with predictable fast-path branches.
struct Task : ITask
{
    // Main task function. Should preferably be small enough
    // to fit in the small-object buffer of std::function.
    std::function<void()> func;

    // Optional exception callback.
    // If not set, fallback logging is used.
    std::function<void(const std::exception&)> errorCallback;


    // Sets a custom exception handler.
    void SetErrorCallback(const std::function<void(const std::exception&)>& callback) override
    {
        errorCallback = callback;
    }


    // Executes the task with fully isolated error handling.
    // Uses fast-path for the common case where no exception occurs.
    void operator()() override
    {
        try
        {
            func(); // <--- hot path (LIKELY)
        }
        catch (const std::exception& ex)
        {
            // If a handler exists, use it.
            if (LIKELY(errorCallback))
            {
                errorCallback(ex);
            }
            else
            {
                // Fallback: minimal logging
                std::cerr << "[Task exception] " << ex.what() << "\n";
            }
        }
        catch (...)
        {
            // Rare case: non-std exception
            if (LIKELY(errorCallback))
            {
                static const std::runtime_error unknown("Unknown exception");
                errorCallback(unknown);
            }
            else
            {
                std::cerr << "[Task exception] unknown\n";
            }
        }
    }
};