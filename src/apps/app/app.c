#include "app.h"
#include "pinout.h"
#include "uartstdio.h"
#include "buttons/buttons.h"

#include "driverlib/gpio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

QueueHandle_t ACTEventQueue = NULL;
//static uint32_t s_sysclk;

static void OnButtonsDebounced(uint8_t btn_now, void *ctx)
{
    (void)ctx;

    if (ACTEventQueue == NULL) {
        return;
    }

    if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
        return;
    }
    
    BaseType_t woken = pdFALSE;
    xQueueSendFromISR(ACTEventQueue, &btn_now, &woken);    // Envio os botões para a fila via Callback ISR
    portYIELD_FROM_ISR(woken);
}

void app_initialize(uint32_t sysclk_hz)
{
    if (ACTEventQueue == NULL)
    {
        ACTEventQueue = xQueueCreate(8, sizeof(uint8_t));
        configASSERT(ACTEventQueue != NULL);
    }

    //s_sysclk = sysclk_hz;
    Buttons_SetCallback(OnButtonsDebounced, 0);
    Buttons_Enable();
}

void app_task(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(1000));
    UARTprintf("app iniciado\n");
    while(true)
    {
        uint8_t ev_btn;
        // Bloqueia esperando evento de ação
        if (xQueueReceive(ACTEventQueue, &ev_btn, portMAX_DELAY) == pdPASS)
        {
            if(ev_btn == (GPIO_PIN_0 | GPIO_PIN_1))
            {
                UARTprintf("BTN_J0 + BTN_J1\n");
            }
            else if(ev_btn == GPIO_PIN_0)
            {
                UARTprintf("BTN_J0\n");
            }
            else if(ev_btn == GPIO_PIN_1)
            {
                UARTprintf("BTN_J1\n");
            }
        }
    }
}
