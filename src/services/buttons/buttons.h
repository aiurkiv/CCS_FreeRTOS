#ifndef BUTTONS_H_
#define BUTTONS_H_

typedef enum
{
    ACT_EVENT_BTN_DETECT,           // Detectou botão
    ACT_EVENT_BTN_VERIFY_REPEAT,    // Verifica se há repetição do botão
    ACT_EVENT_BTN_REPEAT,           // auto-repeat botão
} ACT_EVENT;

typedef void (*buttonj_cb_t)(uint8_t pressed_mask, void *user_ctx);
// pressed_mask: bits GPIO_PIN_0 / GPIO_PIN_1 (1 = pressionado)

void Buttons_Init(uint32_t sysclk_hz);
void Buttons_SetCallback(buttonj_cb_t cb, void *user_ctx);
void Buttons_Enable(void);
void Buttons_Disable(void);

#endif
