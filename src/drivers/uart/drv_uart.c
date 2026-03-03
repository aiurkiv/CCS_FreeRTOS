#include "drv_uart.h"
#include "definitions.h"

#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

#include "driverlib/uart.h"
#include "uartstdio.h"

static uint32_t s_sysclk;

void DRV_Uart_Init(uint32_t sysclk_hz)
{
    s_sysclk = sysclk_hz;
    ////////////////////////////////////////////////////////
    // Configuração da UART0
    // - PA0 -> RX
    // - PA1 -> TX
    // - UART0, 115200 bits, clock do sistema
    ////////////////////////////////////////////////////////
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {}

    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);    // Habilito o clock para UART0
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)) {}
    UARTStdioConfig(0, 115200, s_sysclk);         // Configuro -> UART0, 115200 bits, clock do sistema (função completa em uartstdio.c)
    ////////////////////////////////////////////////////////
    // Fim da configuração da UART0
    ////////////////////////////////////////////////////////
}
