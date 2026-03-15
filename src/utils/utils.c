#include "utils.h"

#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

static uint32_t s_sysclk;

void utils_init(uint32_t sysclk_hz)
{
    s_sysclk = sysclk_hz;
}

void delay_us(uint32_t us)
{
    if(us == 0U)
        return;
    MAP_SysCtlDelay((s_sysclk / 3000000U) * us);
}

void delay_ms(uint32_t ms)
{
    if(ms == 0U)
        return;
    MAP_SysCtlDelay((s_sysclk / 1000U) * ms);
}
