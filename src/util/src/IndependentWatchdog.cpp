#include "wrappers/IndependentWatchdog.hpp"
#ifdef HAL_IWDG_MODULE_ENABLED
util::IndependentWatchdog::IndependentWatchdog(IWDG_HandleTypeDef &iwdt) : iwdg(iwdt)
{
}
void util::IndependentWatchdog::refresh()
{
    HAL_IWDG_Refresh(&iwdg);
}
#endif