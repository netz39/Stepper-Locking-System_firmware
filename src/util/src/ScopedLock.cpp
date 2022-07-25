#include "helpers/ScopedLock.hpp"

util::ScopedLock::ScopedLock(util::wrappers::IMutex &mutex) : mtx(&mutex)
{
    mtx->lock();
}

util::ScopedLock::~ScopedLock()
{
    if (mtx)
    {
        mtx->unlock();
    }
}
util::ScopedLock::ScopedLock(util::ScopedLock &&other) noexcept
{
    (*this) = std::move(other);
}
util::ScopedLock &util::ScopedLock::operator=(util::ScopedLock &&other) noexcept
{
    mtx = std::exchange(other.mtx, nullptr);
    return *this;
}
