#include "tasks_work.h"

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
#include "uartstdio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "definitions.h"
#include "apps/app_led_blink/app_led_blink.h"
#include "apps/app/app.h"

TaskHandle_t task_app_led_blink;
TaskHandle_t task_app;

void tasks_configuration(void)
{
    task_app_led_blink = NULL;
    task_app = NULL;
}

void tasks_start(void)
{
    (void) xTaskCreate( app_led_blink_task,
        "app_led_blink",
        256,            // 256 words = 1kbyte
        (void*)NULL,
        1,              // Prioridade 1: bem baixa
        &task_app_led_blink
    );

    (void) xTaskCreate( app_task,
        "app",
        1024,           // 1024 words = 4kbytes
        (void*)NULL,
        5,              // Prioridade 5: alta
        &task_app
    );
    vTaskStartScheduler();          // <<< sem isso, nenhuma task roda
}

// Funções abaixo tratam erro nas tasks do freertos.
// Ver FreeRTOSConfig.h
void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    for(;;);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask; (void)pcTaskName;
    taskDISABLE_INTERRUPTS();
    for(;;);
}
