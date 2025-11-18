#pragma once

#include "Task.h"
#include "FutureTask.h"
#include <memory>
#include <functional>
#include <exception>

struct TaskFactory
{
    // --- Создаёт Task ---
    static std::unique_ptr<ITask> MakeTask(std::function<void()> func,
                                           std::function<void(const std::exception&)> errorCallback = nullptr)
    {
        auto task = std::make_unique<Task>();
        task->func = std::move(func);
        task->SetErrorCallback(std::move(errorCallback));
        return task;
    }

    // --- Создаёт FutureTask ---
    static std::unique_ptr<ITask> MakeFutureTask(std::function<void()> func,
                                                 std::function<void(TaskResult)> futureCallback,
                                                 std::function<void(const std::exception&)> errorCallback = nullptr)
    {
        auto task = std::make_unique<FutureTask>();
        task->func = std::move(func);
        task->futureResultCallback = std::move(futureCallback);
        task->SetErrorCallback(std::move(errorCallback));
        return task;
    }
};