#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/adc.h"

#include "adc_service.h"
#include "definitions.h"

void ADC_SERVICE_Init(uint32_t sysclk_hz)
{
    ////////////////////////////////////////////////////////
    // Configurações para o timer1A (32-bit one-shot) com interrupção ONE_SHOT
    ////////////////////////////////////////////////////////
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) {}

    MAP_TimerDisable(TIMER1_BASE, TIMER_A);
    MAP_TimerConfigure(TIMER1_BASE, TIMER_CFG_ONE_SHOT);
    MAP_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    MAP_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
}
