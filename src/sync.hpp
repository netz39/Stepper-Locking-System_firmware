#pragma once

#include "FreeRTOS.h"
#include "event_groups.h"

namespace sync
{
// todo Refactor Firmware in such a way, that this is not required anymore
constexpr EventBits_t NodeStarted = 1 << 1;
constexpr EventBits_t ConfigurationLoaded = 1 << 2;
constexpr EventBits_t StateMachineStarted = 1 << 3;

void waitForOne(EventBits_t events);
void waitForAll(EventBits_t events);

void signal(EventBits_t events);
} // namespace sync
