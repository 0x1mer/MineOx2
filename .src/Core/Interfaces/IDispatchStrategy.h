#pragma once

#include <vector>

#include "IWorker.h"
#include "TaskType.h"

struct IDispatchStrategy
{
    virtual ~IDispatchStrategy() = default;

    virtual IWorker& SelectWorker(
        const std::vector<std::unique_ptr<IWorker>>& workers,
        TaskType type
    ) = 0;
};
