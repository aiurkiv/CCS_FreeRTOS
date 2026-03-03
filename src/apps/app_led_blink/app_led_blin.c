#include "app_led_blink.h"
#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "definitions.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

#include "uartstdio.h"


void app_led_blink_task(void *pvParameters)
{
    (void) pvParameters;

    for(;;)
    {
        MAP_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);  // liga
        vTaskDelay(pdMS_TO_TICKS(1000));

        MAP_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0);           // desliga
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
