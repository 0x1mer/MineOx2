#pragma once

#include <memory>

#include "WorkerStatus.h"
#include "AddTaskResult.h"

struct ITask;

struct IWorker
{
    virtual ~IWorker() = default;

    virtual void Pause() = 0;
    virtual void Resume() = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;

    virtual AddTaskResult AddTask(std::unique_ptr<ITask> t) = 0;

    virtual void SetAffinityIndex(size_t idx) = 0;

    virtual size_t GetQueueSize();
    virtual WorkerStatus GetStatus() const = 0;
    virtual uint64_t GetExecutedTasks() const = 0;
};