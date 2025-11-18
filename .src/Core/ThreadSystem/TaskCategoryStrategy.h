#pragma once

#include "IDispatchStrategy.h"
#include "IWorker.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <atomic>

constexpr size_t LIGHT_FOR_4  = 1;
constexpr size_t LIGHT_FOR_8  = 2;
constexpr size_t LIGHT_FOR_15 = 4;

struct TaskCategoryStrategy : IDispatchStrategy
{
    std::atomic<size_t> ioRR{0};
    std::atomic<size_t> lightRR{0};
    std::atomic<size_t> heavyRR{0};

    inline size_t rr(std::atomic<size_t> &counter,
                     size_t begin,
                     size_t count)
    {
        // count гарантированно > 0
        size_t i = counter.fetch_add(1, std::memory_order_relaxed);
        return begin + (i % count);
    }

    IWorker& SelectWorker(
        const std::vector<std::unique_ptr<IWorker>> &workers,
        TaskType type) override
    {
        size_t workerCount = workers.size();
        if (workerCount < 4)
            throw std::runtime_error("Must be at least 4 workers");

        size_t ioWorkers = 1;

        size_t lightWorkers =
            (workerCount >= 15) ? LIGHT_FOR_15 :
            (workerCount >= 8)  ? LIGHT_FOR_8 :
            (workerCount == 4)  ? LIGHT_FOR_4 : 0;

        size_t heavyWorkers = workerCount - ioWorkers - lightWorkers;

        size_t ioBegin    = 0;
        size_t lightBegin = ioBegin + ioWorkers;
        size_t heavyBegin = lightBegin + lightWorkers;

        switch (type)
        {
            case TaskType::IO:
                return *workers[ rr(ioRR, ioBegin, ioWorkers) ];
            case TaskType::Light:
                return *workers[ rr(lightRR, lightBegin, lightWorkers) ];
            case TaskType::Heavy:
                return *workers[ rr(heavyRR, heavyBegin, heavyWorkers) ];
        }

        return *workers[0];
    }
};