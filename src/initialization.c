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
#include "utils/utils.h"

// Drivers
#include "pinout/pinout.h"
#include "drv_uart.h"
#include "uartstdio.h"
#include "display_lcd/display_lcd.h"
// Serviços
#include "buttons/buttons.h"
#include "adc/adc_service.h"
#include "keyboard_usb/keyboard_usb_service.h"
// Apps
#include "apps/app/app.h"
#include "apps/app_led_blink/app_led_blink.h"

#include <string.h>

//*****************************************************************************
// void system_initialization(void)
// Inicia hardware e drives
//*****************************************************************************
void system_initialization(void)
{
    uint32_t g_ui32SysClock;

    //static const char lcd_boot_msg[] = "Polenta frita";
    //uint8_t lcd[4][20];

    // Garanto que todas as interrupções estão desabilitadas
    MAP_IntMasterDisable();

    // Configura o clock do sistema
    // Datasheet pg 253
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240), 120000000);

    // Configura 'utils'. Biblioteca com funções gerais
    utils_init(g_ui32SysClock);
    // Configura drivers
    Pinout_Init();
    DRV_Uart_Init(g_ui32SysClock);
    display_lcd_init();

    //memset(lcd, ' ', sizeof(lcd));
    //memcpy(lcd[0], lcd_boot_msg, sizeof(lcd_boot_msg) - 1U);
    //memcpy(lcd[1], lcd_boot_msg, sizeof(lcd_boot_msg) - 1U);
    //memcpy(lcd[2], lcd_boot_msg, sizeof(lcd_boot_msg) - 1U);
    //memcpy(lcd[3], lcd_boot_msg, sizeof(lcd_boot_msg) - 1U);
    //display_lcd_update(lcd);

    // Configura serviços
    Buttons_Init(g_ui32SysClock);
    ADC_SERVICE_Init(g_ui32SysClock);
    KeyboardUSBService_Init(g_ui32SysClock);
    // Configura apps
    app_initialize(g_ui32SysClock);
    
    // Habilita a interrupção global
    MAP_IntMasterEnable();
}

