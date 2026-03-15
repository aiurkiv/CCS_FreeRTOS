#include "app.h"
#include "buttons/buttons.h"
#include "display_lcd/display_lcd.h"
#include "keyboard_usb/keyboard_usb_service.h"

#include <stdio.h>
#include <string.h>


static APP_DATA app_data;

/* Limpa todo o buffer de exibicao do LCD. */
static void app_lcd_clear(void)
{
    memset(app_data.lcd, ' ', sizeof(app_data.lcd));
}

/* Escreve uma string em uma linha especifica do buffer do LCD. */
static void app_lcd_write_line(uint8_t row, const char *text)
{
    size_t text_length;

    if((text == NULL) || (row >= APP_LCD_ROWS))
    {
        return;
    }

    text_length = strlen(text);
    if(text_length > APP_LCD_COLS)
    {
        text_length = APP_LCD_COLS;
    }

    memcpy(app_data.lcd[row], text, text_length);
}

/* Atualiza as duas primeiras linhas do LCD com titulo e mensagem. */
static void app_show_message(const char *title, const char *message)
{
    app_lcd_clear();
    app_lcd_write_line(0U, title);
    app_lcd_write_line(1U, message);
    display_lcd_update(app_data.lcd);
}

/* Traduz o tipo de evento para uma string curta de exibicao. */
static const char *app_keyboard_event_to_text(SERVICE_KEYBOARD_EVENT_TYPE type)
{
    switch(type)
    {
        case SERVICE_KEYBOARD_EVT_ENTER:
            return "ENTER";

        case SERVICE_KEYBOARD_EVT_BACKSPACE:
            return "BACKSPACE";

        case SERVICE_KEYBOARD_EVT_TAB:
            return "TAB";

        case SERVICE_KEYBOARD_EVT_SPACE:
            return "SPACE";

        case SERVICE_KEYBOARD_EVT_ESC:
            return "ESC";

        case SERVICE_KEYBOARD_EVT_UP:
            return "UP";

        case SERVICE_KEYBOARD_EVT_DOWN:
            return "DOWN";

        case SERVICE_KEYBOARD_EVT_LEFT:
            return "LEFT";

        case SERVICE_KEYBOARD_EVT_RIGHT:
            return "RIGHT";

        case SERVICE_KEYBOARD_EVT_ATTACH:
            return "ATTACH";

        case SERVICE_KEYBOARD_EVT_DETACH:
            return "DETACH";

        case SERVICE_KEYBOARD_EVT_COMB:
            return "COMB";

        case SERVICE_KEYBOARD_EVT_CHAR:
        default:
            return "CHAR";
    }
}

/* Trata um evento vindo do teclado USB ou dos botoes. */
static void app_handle_keyboard_event(const SERVICE_KEYBOARD_EVENT *event)
{
    char line[APP_LCD_COLS + 1U];

    if(event == NULL)
    {
        return;
    }

    memset(line, 0, sizeof(line));

    if(event->type == SERVICE_KEYBOARD_EVT_CHAR)
    {
        (void)snprintf(line, sizeof(line), "Char: %c", event->ch);
    }
    else if(event->type == SERVICE_KEYBOARD_EVT_COMB)
    {
        (void)snprintf(line, sizeof(line), "Evt: COMB %u",
                       (unsigned int)(unsigned char)event->ch);
    }
    else
    {
        (void)snprintf(line, sizeof(line), "Evt: %s",
                       app_keyboard_event_to_text(event->type));
    }

    app_show_message("Input", line);
    app_data.state = APP_STATE_KEYBOARD_KEY;
}

/*
 * OnKeyboardInputService(...)
 *
 * Callback compartilhado entre o servico de teclado USB e o servico
 * de botoes. Ambos publicam o mesmo tipo de evento na fila unica da app.
 */
static void OnKeyboardInputService(const SERVICE_KEYBOARD_EVENT *event, void *ctx)
{
    BaseType_t woken;

    (void)ctx;

    if((event == NULL) || (app_data.keyboard_event_queue == NULL))
    {
        return;
    }

    if(xTaskGetSchedulerState() != taskSCHEDULER_RUNNING)
    {
        return;
    }

    woken = pdFALSE;
    xQueueSendFromISR(app_data.keyboard_event_queue, event, &woken);
    portYIELD_FROM_ISR(woken);
}

/* Inicializa o estado interno da aplicacao. */
void app_initialize(uint32_t sysclk_hz)
{
    (void)sysclk_hz;

    app_data.state = APP_STATE_INIT;
    app_data.keyboard_event_queue = NULL;
    app_lcd_clear();
}

/* Task principal da aplicacao responsavel por tratar os eventos de entrada. */
void app_task(void *pvParameters)
{
    SERVICE_KEYBOARD_EVENT keyboard_event;
    QueueSetMemberHandle_t activatedMember;
    BaseType_t ok;

    (void)pvParameters;

    if(app_data.keyboard_event_queue == NULL)
    {
        app_data.keyboard_event_queue = xQueueCreate(APP_KEYBOARD_EVENT_QUEUE_LENGTH, sizeof(SERVICE_KEYBOARD_EVENT));
        configASSERT(app_data.keyboard_event_queue != NULL);
    }
    if(app_data.action_set == NULL)
    {
        app_data.action_set = xQueueCreateSet(APP_QUEUE_SET_LEN);
        configASSERT(app_data.action_set != NULL);
    }
    // Adiciona membros ao set. No momento tem somente a fila do teclado / botões
    ok = xQueueAddToSet(app_data.keyboard_event_queue, app_data.action_set);
    configASSERT(ok == pdPASS);

    Buttons_SetEventCallback(OnKeyboardInputService, NULL);
    Buttons_Enable();
    KeyboardUSBService_SetCallback(OnKeyboardInputService, NULL);

    app_show_message("App", "Aguardando evento");
    app_data.state = APP_STATE_PRESENTATION;

    for(;;)
    {
        // Aguarda algum membro do set ficar pronto
        activatedMember = xQueueSelectFromSet(app_data.action_set, portMAX_DELAY);

        if(activatedMember == app_data.keyboard_event_queue)
        {
            if(xQueueReceive(app_data.keyboard_event_queue, &keyboard_event, portMAX_DELAY) == pdPASS)
            {
                app_handle_keyboard_event(&keyboard_event);
            }
        }
    }
}
