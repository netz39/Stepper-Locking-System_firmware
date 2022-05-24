#include "core/fault_handler.h"
#include "core/BuildConfiguration.hpp"
#include <stdint.h>

#if IS_EMBEDDED_BUILD()

/* The fault handler implementation calls a function called
prvGetRegistersFromStack(). */
void __attribute__((aligned(4))) faultHandler(void)
{
    __asm volatile(" mrs r0, msp                                            \n"
                   " mov r1, lr                                             \n"
                   " mov r2, #4                                             \n"
                   " tst r1, r2                                             \n"
                   " beq prvGetRegistersFromStack                           \n"
                   " mrs r0, psp                                            \n"
                   " ldr r1, =prvGetRegistersFromStack                      \n"
                   " bx r1");
}

extern "C" [[noreturn]] void prvGetRegistersFromStack(uint32_t *pulFaultStackAddress)
{
    /* These are volatile to try and prevent the compiler/linker optimising them
    away as the variables never actually get used.  If the debugger won't show the
    values of the variables, make them global my moving their declaration outside
    of this function. */

    /* Debugging steps: Convert the content of 'pc' to hex and search for the
    address in the builds .list. That should get you at least somewhere.
    If the address doesn't exist, your firmware tried to access an illegal memory location.
    If the address only contains 0xA5, you ran into a FreeRTOS stack overflow. 0xA5 is the
    detection pattern it uses internally.*/
    volatile uint32_t r0;
    volatile uint32_t r1;
    volatile uint32_t r2;
    volatile uint32_t r3;
    volatile uint32_t r12;
    volatile uint32_t lr;  /* Link register. Function the firmware is executing */
    volatile uint32_t pc;  /* Program counter. Current line being executed */
    volatile uint32_t psr; /* Program status register. */

    r0 = pulFaultStackAddress[0];
    r1 = pulFaultStackAddress[1];
    r2 = pulFaultStackAddress[2];
    r3 = pulFaultStackAddress[3];

    r12 = pulFaultStackAddress[4];
    lr = pulFaultStackAddress[5];
    pc = pulFaultStackAddress[6];
    psr = pulFaultStackAddress[7];

    (void)r0;
    (void)r1;
    (void)r2;
    (void)r3;
    (void)r12;
    (void)lr;
    (void)pc;
    (void)psr;

    if constexpr (core::BuildConfiguration::IsDebugBuild)
    {
        __asm("bkpt");
    }

    for (;;)
        ;
}

#else

void faultHandler(void)
{
}

#endif