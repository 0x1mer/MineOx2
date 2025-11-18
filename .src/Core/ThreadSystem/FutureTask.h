#pragma once

#include "ITask.h"

#include <iostream>
#include <exception>

struct TaskResult
{
    bool success;
};

struct FutureTask : ITask
{
    std::function<void()> func;
    std::function<void(const std::exception &)> errorCallback;
    std::function<void(TaskResult)> futureResultCallback;

    void SetErrorCallback(const std::function<void(const std::exception &)> &callback) override
    {
        errorCallback = callback;
    }

    void operator()() override
    {
        TaskResult result;
        try
        {
            func();
            result.success = true;
        }
        catch (const std::exception &ex)
        {
            result.success = false;
            if (errorCallback)
                errorCallback(ex);
            else
                std::cerr << "[Task exception] " << ex.what() << "\n";
        }
        catch (...)
        {
            result.success = false;
            if (errorCallback)
            {
                std::runtime_error unknown("Unknown exception");
                errorCallback(unknown);
            }
            else
            {
                std::cerr << "[Task exception] unknown\n";
            }
        }
        if (futureResultCallback)
            futureResultCallback(result);
        else
            std::cerr << "[Task exception] unknown\n";
    }
};