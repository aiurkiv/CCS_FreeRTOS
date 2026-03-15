#ifndef KEYBOARD_USB_SERVICE_H_
#define KEYBOARD_USB_SERVICE_H_

#include "definitions.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

// Callback chamado pelo servico quando um novo evento e detectado.
typedef void (*service_keyboard_usb_callback_t)(const SERVICE_KEYBOARD_EVENT *event,
                                                void *ctx);

// Inicia o servico de host USB para o teclado.
void KeyboardUSBService_Init(uint32_t sysclk_hz);

// Registra o callback que recebera os eventos do teclado USB.
void KeyboardUSBService_SetCallback(service_keyboard_usb_callback_t callback, void *ctx);

// Task responsavel por processar periodicamente a pilha USB host.
void KeyboardUSBService_Task(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif
