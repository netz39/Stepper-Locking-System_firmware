#pragma once

#include "FreeRTOS.h"
#include "event_groups.h"

namespace sync
{
constexpr EventBits_t NodeStarted = 1 << 1;
constexpr EventBits_t ConfigurationLoaded = 1 << 2;

void waitForOne(EventBits_t events);
void waitForAll(EventBits_t events);

void signal(EventBits_t events);
} // namespace sync
