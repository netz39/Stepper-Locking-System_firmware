#pragma once

#ifdef HAL_IWDG_MODULE_ENABLED
#include <hal_header.h>
namespace util::wrappers
{
class IndependentWatchdog
{
public:
    explicit IndependentWatchdog(IWDG_HandleTypeDef &iwdt);
    void refresh();

private:
    IWDG_HandleTypeDef &iwdg;
};
} // namespace util
#endif