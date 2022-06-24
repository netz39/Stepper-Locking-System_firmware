#include "sync.hpp"
#include "FreeRTOS.h"
#include "event_groups.h"

namespace
{
EventGroupHandle_t syncEventGroup = xEventGroupCreate();
} // namespace

namespace sync
{
void waitForOne(EventBits_t events)
{
    (void)xEventGroupWaitBits(syncEventGroup, events, pdFALSE, pdFALSE, portMAX_DELAY);
}

void waitForAll(EventBits_t events)
{
    (void)xEventGroupWaitBits(syncEventGroup, events, pdFALSE, pdTRUE, portMAX_DELAY);
}

void signal(EventBits_t events)
{
    (void)xEventGroupSetBits(syncEventGroup, events);
}
} // namespace sync