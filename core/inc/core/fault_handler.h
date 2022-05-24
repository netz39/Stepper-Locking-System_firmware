#pragma once
#include "core/BuildConfigurationTypes.h"

/**
 * @brief Walks up the stack and looks for the last executed code's address.
 * Used to retrieve from where the chip hardfaults. Do not use outside of
 * crash handlers. __attribute__((naked)) is going to shoot you into the foot.
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif

#if IS_EMBEDDED_BUILD()
    void faultHandler(void) __attribute__((naked));
#else
void faultHandler(void);
#endif

#ifdef __cplusplus
}
#endif
