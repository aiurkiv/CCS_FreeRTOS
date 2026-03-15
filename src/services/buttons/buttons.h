#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "definitions.h"

typedef enum
{
    ACT_EVENT_BTN_DETECT,           // Detectou botão
    ACT_EVENT_BTN_VERIFY_REPEAT,    // Verifica se há repetição do botão
    ACT_EVENT_BTN_REPEAT,           // auto-repeat botão
} ACT_EVENT;

/* Callback usado para encaminhar o evento padronizado dos botoes. */
typedef void (*service_buttons_callback_t)(const SERVICE_KEYBOARD_EVENT *event, void *ctx);

/* Inicializa o servico dos botoes e o debounce por timer. */
void Buttons_Init(uint32_t sysclk_hz);
/* Registra o callback que recebera os eventos padronizados dos botoes. */
void Buttons_SetEventCallback(service_buttons_callback_t cb, void *ctx);
/* Habilita a interrupcao dos botoes. */
void Buttons_Enable(void);
/* Desabilita a interrupcao dos botoes. */
void Buttons_Disable(void);

#endif
