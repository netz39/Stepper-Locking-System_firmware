#pragma once
#include <cmsis_os2.h>
#include "IFreeRTOSTask.hpp"
#include "ITaskWithMemberFunction.hpp"
#include <FreeRTOS.h>
#include <array>
#include <event_groups.h>
#include <functional>
#include <limits>
#include <task.h>

namespace util::wrappers
{

/// todo introduce start method that must be called after the constructor so FreeRTOS doesn't
///  get a function pointer to a still constructing class. This will cause no issue here as
///  a new task will only execute after the next scheduler run (where the class will always be
///  ready) but despite this, its still technical debt.
class Task : public IFreeRTOSTask
{
public:
    Task(TaskFunction_t taskCode, const char *name, uint16_t stackDepth, void *parameter,
         UBaseType_t priority);

    ~Task() override;

    Task(const Task &) = delete;
    Task(Task && other) noexcept;
    Task &operator=(const Task &) = delete;
    Task &operator=(Task && other) noexcept;

    static constexpr uint32_t ClearAllBits = std::numeric_limits<uint32_t>::max();
    int32_t notifyWait(uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit,
                       uint32_t *pulNotificationValue, uint32_t xTicksToWait) override;
    int32_t notify(uint32_t ulValue, NotifyAction eAction) override;
    int32_t notifyFromISR(uint32_t ulValue, NotifyAction eAction,
                          int32_t *pxHigherPriorityTaskWoken) override;
    void notifyGive() override;
    void notifyTake(uint32_t waittime) override;
    void delay(units::si::Time time) override;

    [[nodiscard]] static constexpr eNotifyAction notifyActionConverter(NotifyAction action);

    static constexpr uint8_t MaxTasks = 16;
    static std::array<TaskHandle_t, MaxTasks> &getAllTaskHandles()
    {
        return taskList;
    }
    static void registerTask(TaskHandle_t);

    static void applicationIsReadyStartAllTasks();

protected:
    TaskHandle_t _handle{nullptr};
    TaskFunction_t _taskCode{nullptr};
    void *_parameter{nullptr};

    [[noreturn]] static void taskMain(void *);

    static std::array<TaskHandle_t, MaxTasks> taskList;
    static size_t taskListIndex;
    static EventGroupHandle_t syncEventGroup;
    static constexpr EventBits_t AllTasksWaitFlag = 1 << 0;
};

/// By inherited from here the child class can start FreeRTOS task in C++ context.
/// There is the virtual function "taskMain" which should be implemented by the child class.
class TaskWithMemberFunctionBase : public Task, public ITaskWithMemberFunction
{
public:
    TaskWithMemberFunctionBase(const char *name, uint16_t stackDepth, UBaseType_t priority)
        : Task(&runTaskStub, name, stackDepth, this, priority){};

    [[noreturn]] void taskMain() override = 0;
    static void runTaskStub(void *parameters)
    {
        static_cast<TaskWithMemberFunctionBase *>(parameters)->taskMain();
    }
};

} // namespace util::wrappers