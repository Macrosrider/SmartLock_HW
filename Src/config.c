//
// Created by macros on 27.03.20.
//

#include <config.h>

volatile uint32_t msTicks;

void Delay(uint32_t dlyTicks)
{
    uint32_t curTicks;

    curTicks = msTicks;
    while ((msTicks - curTicks) < dlyTicks);
}

void SysTick_Handler (void)
{
    msTicks++;
}
