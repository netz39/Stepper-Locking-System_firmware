#include "wrappers/Task.hpp"
#include <algorithm>
#include <core/BuildConfiguration.hpp>
#include <core/SafeAssert.h>

namespace util::wrappers
{
size_t Task::taskListIndex{0};
std::array<TaskHandle_t, Task::MaxTasks> Task::taskList{};
EventGroupHandle_t Task::syncEventGroup = xEventGroupCreate();

Task::Task(TaskFunction_t taskCode, const char *name, uint16_t stackDepth, void *parameter,
           UBaseType_t priority)
    : _taskCode(taskCode), _parameter(parameter)
{
    SafeAssert(taskCode != nullptr);
    xTaskCreate(&Task::taskMain, name, stackDepth, reinterpret_cast<void *>(this), priority,
                &_handle);
    SafeAssert(_handle != nullptr);

    registerTask(_handle);
}

void Task::registerTask(TaskHandle_t handle)
{
    if constexpr (core::BuildConfiguration::IsEmbeddedBuild)
    {

        if (taskListIndex == 0)
        {
            std::fill(taskList.begin(), taskList.end(), nullptr);
        }
        SafeAssert(taskListIndex < taskList.size());
        taskList.at(taskListIndex++) = handle;
    }
}

BaseType_t Task::notifyWait(uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit,
                            uint32_t *pulNotificationValue, TickType_t xTicksToWait)
{
    return xTaskNotifyWait(ulBitsToClearOnEntry, ulBitsToClearOnExit, pulNotificationValue,
                           xTicksToWait);
}

BaseType_t Task::notify(uint32_t ulValue, eNotifyAction eAction)
{
    return xTaskNotify(_handle, ulValue, eAction);
}

BaseType_t Task::notifyFromISR(uint32_t ulValue, eNotifyAction eAction,
                               BaseType_t *pxHigherPriorityTaskWoken)
{

    return xTaskNotifyFromISR(_handle, ulValue, eAction, pxHigherPriorityTaskWoken);
}

Task::~Task()
{
    if (_handle != nullptr)
    {
        vTaskDelete(_handle);
    }
}

[[noreturn]] void Task::taskMain(void *instance)
{
    Task *task = reinterpret_cast<Task *>(instance);
    xEventGroupWaitBits(syncEventGroup, AllTasksWaitFlag, pdFALSE, pdFALSE, portMAX_DELAY);
    task->_taskCode(task->_parameter);
    for (;;)
    {
        vTaskDelay(portMAX_DELAY);
    }
}

void Task::notifyGive()
{
    xTaskNotifyGive(_handle);
}

void Task::notifyTake(const TickType_t waittime)
{
    ulTaskNotifyTake(pdTRUE, waittime);
}

void Task::applicationIsReadyStartAllTasks()
{
    xEventGroupSetBits(syncEventGroup, AllTasksWaitFlag);
}

} // namespace util::wrappers