#pragma once

struct ITask;

#include <memory>

enum class AddTaskError
{
    None,
    QueueFull,
};

struct [[nodiscard]] AddTaskResult
{
    AddTaskError error;
    std::unique_ptr<ITask> task;

    explicit operator bool() const noexcept {
        return error == AddTaskError::None;
    }

    static AddTaskResult Ok() noexcept {
        return {AddTaskError::None, nullptr};
    }

    static AddTaskResult QueueFull(std::unique_ptr<ITask> t) noexcept {
        return {AddTaskError::QueueFull, std::move(t)};
    }
};