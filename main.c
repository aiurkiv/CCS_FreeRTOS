//*****************************************************************************
//
// freertos -> Projeto de firmware
// - Kit: EK-TM4C129EXL
// - TivaWare 2.2.0.295
//
// Desenvolvimento de firmware utilizando aplicações com freertos, Tivaware e periféricos do kit.
// Mapa de configuração:
// GPIOF.
// -> portas 0 e 4 - Output para leds
//
// GPION.
//
// 
//
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

#include "FreeRTOS.h"
#include "task.h"
#include "definitions.h"
#include "initialization.h"
#include "tasks_work.h"
#include "uartstdio.h"


//*****************************************************************************
// System clock rate in Hz.
//*****************************************************************************


//*****************************************************************************
// The error routine that is called if the driver library encounters an error.
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


int main(void)
{
    
    system_initialization();
    UARTprintf("System OK\n");

    tasks_configuration();
    UARTprintf("Tasks configuration OK\n");

    tasks_start();

    UARTprintf("System error finished\n");
}
