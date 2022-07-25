#pragma once
#include "wrappers/IMutex.hpp"
#include <utility>

namespace util
{
class ScopedLock
{
public:
    /// empty initialisation, not doing anything
    ScopedLock() = default;

    /// acquires mutex
    explicit ScopedLock(wrappers::IMutex &mutex);

    /// releases mutex
    ~ScopedLock();

    ScopedLock(const ScopedLock &) = delete;
    ScopedLock(ScopedLock &&) noexcept;
    ScopedLock& operator=(const ScopedLock &) = delete;
    ScopedLock& operator=(ScopedLock &&) noexcept;

private:
    wrappers::IMutex* mtx{nullptr};
};
} // namespace util