#pragma once

#include <functional>

struct ITask
{
    virtual ~ITask() = default;
    virtual void SetErrorCallback(const std::function<void(const std::exception &)> &callback) = 0;
    virtual void operator()() = 0;
};