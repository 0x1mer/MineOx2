#pragma once

#include "IDispatchStrategy.h"
#include "IWorker.h"
#include <limits>
#include <vector>
#include <memory>

struct LoadBalanceStrategy : IDispatchStrategy
{
    IWorker& SelectWorker(
        const std::vector<std::unique_ptr<IWorker>>& workers,
        TaskType type
    ) override
    {
        size_t bestIndex = 0;
        size_t bestLoad = std::numeric_limits<size_t>::max();

        for (size_t i = 0; i < workers.size(); i++)
        {
            size_t load = workers[i]->GetQueueSize();
            if (load < bestLoad)
            {
                bestLoad = load;
                bestIndex = i;
            }
        }

        return *workers[bestIndex];
    }
};