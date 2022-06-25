#include "helpers/ScopedLock.hpp"

util::ScopedLock::ScopedLock(util::wrappers::IMutex &mutex)
    : //
      m_mutex(mutex)
{
    m_mutex.lock();
}
util::ScopedLock::~ScopedLock()
{
    m_mutex.unlock();
}
