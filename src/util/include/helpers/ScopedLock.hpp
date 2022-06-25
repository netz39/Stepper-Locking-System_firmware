#pragma once
#include "wrappers/IMutex.hpp"

namespace util
{
class ScopedLock
{
public:
    explicit ScopedLock(wrappers::IMutex &mutex);
    ~ScopedLock();

    ScopedLock(const ScopedLock &) = delete;
    ScopedLock(ScopedLock &&) = delete;
    void operator=(const ScopedLock &) = delete;
    void operator=(ScopedLock &&) = delete;

private:
    wrappers::IMutex &m_mutex;
};
} // namespace util