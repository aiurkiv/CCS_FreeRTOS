#ifndef APP_H_
#define APP_H_

#include "stdint.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

typedef enum
{
    APP_STATE_INIT = 0,
    APP_STATE_PRESENTATION,
    APP_STATE_SHOW_MEMORY,
    APP_STATE_KEYBOARD_KEY,
} APP_STATES;

/* Quantidade de linhas do LCD. */
#define APP_LCD_ROWS                    4U
/* Quantidade de colunas do LCD. */
#define APP_LCD_COLS                    20U
// Tamanho da fila unificada de eventos de entrada.
#define APP_KEYBOARD_EVENT_QUEUE_LENGTH 8U
// Tamanho total do controle de filas
#define APP_QUEUE_SET_LEN (APP_KEYBOARD_EVENT_QUEUE_LENGTH)

typedef struct
{
    APP_STATES state;                               // Estado atual da aplicacao.
    uint8_t lcd[APP_LCD_ROWS][APP_LCD_COLS];        // Buffer usado para escrita no LCD.
    QueueHandle_t keyboard_event_queue;             // Fila unica para teclado USB e botoes.
    QueueSetHandle_t action_set;                    // Controle de filas
} APP_DATA;

void app_initialize(uint32_t sysclk_hz);

void app_task(void *pvParameters);

#endif
