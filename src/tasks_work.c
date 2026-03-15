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
#include "keyboard_usb/keyboard_usb_service.h"

TaskHandle_t task_app_led_blink;
TaskHandle_t task_app;
TaskHandle_t task_keyboard_usb_service;

void tasks_configuration(void)
{
    task_app_led_blink = NULL;
    task_app = NULL;
    task_keyboard_usb_service = NULL;
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

    (void) xTaskCreate( KeyboardUSBService_Task,
        "keyboard_usb",
        2048,           // 1024 words = 4kbytes
        (void*)NULL,
        4,              // Prioridade 4: abaixo da app
        &task_keyboard_usb_service
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
    UARTprintf("Malloc failed hook\r\n");
    MAP_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_0);
    MAP_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_PIN_4);
    taskDISABLE_INTERRUPTS();
    for(;;);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    UARTprintf("Stack overflow hook: %s\r\n",
               (pcTaskName != NULL) ? pcTaskName : "unknown");
    MAP_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_1);
    MAP_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_PIN_0);
    taskDISABLE_INTERRUPTS();
    for(;;);
}
