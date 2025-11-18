#pragma once

#include <vector>
#include <memory>
#include <thread>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <functional>

#include "ITask.h"
#include "IDispatchStrategy.h"
#include "Worker.h"

class ThreadPool
{
public:
    /// Singleton instance
    static ThreadPool& Instance()
    {
        static ThreadPool instance;
        return instance;
    }

    // Forbid copying/moving
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    void Init()
    {
        int count = (countOfWorkers_ != -1)
                        ? countOfWorkers_
                        : CalculateCountOfWorkers();

        std::call_once(initFlag_, &ThreadPool::InitWorkers, this, count);
    }

    void Init(size_t workerQueueSize)
    {
        workerQueueSize_ = workerQueueSize;
        Init();
    }

    void SetStrategy(std::unique_ptr<IDispatchStrategy> s)
    {
        std::unique_lock lock(strategyMutex_);
        strategy_ = std::move(s);
    }

    AddTaskResult AddTask(TaskType type, std::unique_ptr<ITask> task)
    {
        std::shared_lock lock(strategyMutex_);
        if (!strategy_)
        {
            return AddTaskResult::QueueFull(std::move(task));
        }

        IWorker& worker = strategy_->SelectWorker(workers_, type);
        return worker.AddTask(std::move(task));
    }

    void Shutdown()
    {
        // If Init was not called — nothing to stop
        std::call_once(initFlag_, [](){});

        for (auto& w : workers_)
        {
            if (w) w->Stop();
        }

        workers_.clear();
    }

    const std::vector<std::unique_ptr<IWorker>>& GetWorkers() const noexcept
    {
        return workers_;
    }

    void SetWorkerCount(int c) noexcept
    {
        countOfWorkers_ = c;
    }

private:
    ThreadPool() = default;

    ~ThreadPool()
    {
        try { Shutdown(); }
        catch (...) {}
    }

    unsigned short GetCpuThreads() const noexcept
    {
        unsigned short n = std::thread::hardware_concurrency();
        return n == 0 ? 4 : n;
    }

    unsigned short CalculateCountOfWorkers() const noexcept
    {
        auto n = GetCpuThreads();
        return (n > 1) ? static_cast<unsigned short>(n - 1) : 1;
    }

    void InitWorkers(int count)
    {
        if (count < 1)
            throw std::runtime_error("Invalid number of workers");

        workers_.reserve(count);

        for (int i = 0; i < count; ++i)
        {
            workers_.emplace_back(std::unique_ptr<IWorker>(new Worker(workerQueueSize_)));
        }

        unsigned int cpuCount = GetCpuThreads();
        if (cpuCount == 0) cpuCount = 4;

        for (size_t i = 0; i < workers_.size(); ++i)
        {
            size_t core = (i < cpuCount) ? i : (i % cpuCount);
            workers_[i]->SetAffinityIndex(core);
            workers_[i]->Start();
        }

        std::cout << "Workers created: " << workers_.size() << std::endl;
    }

private:
    std::vector<std::unique_ptr<IWorker>> workers_;

    std::unique_ptr<IDispatchStrategy> strategy_;
    mutable std::shared_mutex strategyMutex_;

    short countOfWorkers_ = -1;
    size_t workerQueueSize_ = 4096;

    std::once_flag initFlag_;
};