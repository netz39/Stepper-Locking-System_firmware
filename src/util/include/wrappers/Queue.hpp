#pragma once
#include <FreeRTOS.h>
#include <core/SafeAssert.h>
#include <cstdint>
#include <optional>
#include <queue.h>
#include <utility>

namespace util::wrappers
{
template <typename T>
class Queue
{
public:
    explicit Queue(size_t length) : handle(xQueueCreate(length, sizeof(T)))
    {
        SafeAssert(handle != nullptr);
    };

    ~Queue()
    {
        if (handle != nullptr)
        {
            vQueueDelete(handle);
        }
    }

    Queue(const Queue &other) = delete;
    void operator=(const Queue &other) = delete;

    Queue(Queue &&other) noexcept
    {
        (*this) = std::forward<Queue>(other);
    };

    Queue &operator=(Queue &&other) noexcept
    {
        handle = std::exchange(other.handle, nullptr);
        return *this;
    };

    [[nodiscard]] BaseType_t getMessagesWaiting() const
    {
        return uxQueueMessagesWaiting(handle);
    }
    [[nodiscard]] BaseType_t getMessagesWaitingFromISR() const
    {
        return uxQueueMessagesWaitingFromISR(handle);
    }

    void sendOverwrite(const T &item) const
    {
        xQueueOverwrite(handle, static_cast<const void *>(&item));
    }
    void sendOverwriteFromISR(const T &item, BaseType_t &pxHigherPriorityTaskWoken) const
    {
        xQueueOverwriteFromISR(handle, static_cast<const void *>(&item),
                               &pxHigherPriorityTaskWoken);
    }
    bool send(const T &item, TickType_t ticksToWait) const
    {
        return xQueueSend(handle, static_cast<const void *>(&item), ticksToWait) == pdPASS;
    }
    bool sendFromISR(const T &item, BaseType_t &pxHigherPriorityTaskWoken) const
    {
        return xQueueSendFromISR(handle, static_cast<const void *>(&item),
                                 &pxHigherPriorityTaskWoken) == pdPASS;
    }

    std::optional<T> peek(TickType_t ticksToWait) const
    {
        T data;
        const auto Res = xQueuePeek(handle, static_cast<void *>(&data), ticksToWait);
        if (Res == pdPASS)
        {
            return {data};
        }
        return {};
    }
    std::optional<T> peekFromISR() const
    {
        T data;
        const auto Res = xQueuePeekFromISR(handle, static_cast<void *>(&data));
        if (Res == pdPASS)
        {
            return {data};
        }
        return {};
    }

    std::optional<T> receive(TickType_t ticksToWait) const
    {
        T data;
        const auto Res = xQueueReceive(handle, static_cast<void *>(&data), ticksToWait);
        if (Res == pdPASS)
        {
            return {data};
        }
        return {};
    }
    std::optional<T> receiveFromISR(BaseType_t &pxHigherPriorityTaskWoken) const
    {
        T data;
        const auto Res =
            xQueueReceiveFromISR(handle, static_cast<void *>(&data), &pxHigherPriorityTaskWoken);
        if (Res == pdPASS)
        {
            return {data};
        }
        return {};
    }

    void reset() const
    {
        xQueueReset(handle);
    }

private:
    QueueHandle_t handle{nullptr};
};

} // namespace util::wrappers