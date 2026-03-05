#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "initialization.h"
#include "definitions.h"
#include "apps/app_led_blink/app_led_blink.h"
#include "pinout/pinout.h"
#include "buttons/buttons.h"
#include "drv_uart.h"
#include "uartstdio.h"
#include "apps/app/app.h"

//*****************************************************************************
// void system_initialization(void)
// Inicia hardware e drives
//*****************************************************************************
void system_initialization(void)
{
    uint32_t g_ui32SysClock;
    // Garanto que todas as interrupções estão desabilitadas
    MAP_IntMasterDisable();

    // Configura o clock do sistema
    // Datasheet pg 253
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240), 120000000);
                                             
    // Configura as GPIOs conforme módulo EK-TM4C129E
    // ethernet = false, usb = false
    // Ver função para mais informações
    Pinout_Init();
    Buttons_Init(g_ui32SysClock);
    DRV_Uart_Init(g_ui32SysClock);

    app_initialize(g_ui32SysClock);
    
    // Habilita a interrupção global
    MAP_IntMasterEnable();
}

