#pragma once
#include <string>

template <typename T>
class IOptions {
public:
    virtual ~IOptions() = default;

    virtual const T& Value() const = 0;
    virtual const T* operator->() const = 0;
    virtual void Reload(const std::string& filename) = 0;
};