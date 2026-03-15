#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

// Observações ao utilizar interrupção + FreeRTOS
// 1 - Quanto maior o valor, menor a prioridade
// 2 - As interrupções que utilizam eventos com notificação (...fromISR como fila, notify, etc) devem ter prioridade entre PRIORITY_KERNEL_INTERRUPT e PRIORITY_MAX_SYSCALL_INTERRUPT
// 3 - É possível utilizar interrupções com prioridade acima de PRIORITY_MAX_SYSCALL_INTERRUPT, porém elas não devem chamar funções de eventos
// 4 - É possível utilizar interrupções com a mesma prioridade do kernel do FreeRTOS (PRIORITY_MAX_SYSCALL_INTERRUPT), porém é aconselhavel evitar
#define PRIORITY_KERNEL_INTERRUPT           ( 7 << 5 )  /* Priority 7, or 0xE0 as only the top three bits are implemented.  This is the lowest priority. */
#define PRIORITY_MAX_SYSCALL_INTERRUPT      ( 5 << 5 )  /* Priority 5, or 0xA0 as only the top three bits are implemented. */
#define PRIORITY_INTERRUPT_BUTTONS          ( 6 << 5 ) 
#define PRIORITY_INTERRUPT_TIMER_BOUNCING   ( 7 << 5 ) 
#define PRIORITY_INTERRUPT_ADC0             ( 3 << 5 )
#define PRIORITY_INTERRUPT_KEYBOARD_USB     ( 6 << 5 )

// Tipos de eventos gerados pelo servico de teclado/botões
// Os botões geram os mesmos tipos de evento do teclado
// SERVICE_KEYBOARD_EVT_COMB é evento de combinação de botões, assim os bits de 'char ch' podem representar a combinação de até 8 botões pressionados ao mesmo tempo
typedef enum
{
    SERVICE_KEYBOARD_EVT_CHAR = 0,      // Caractere ASCII traduzido.
    SERVICE_KEYBOARD_EVT_ENTER,         // Tecla Enter.
    SERVICE_KEYBOARD_EVT_BACKSPACE,     // Tecla Backspace.
    SERVICE_KEYBOARD_EVT_TAB,           // Tecla Tab.
    SERVICE_KEYBOARD_EVT_SPACE,         // Tecla espaco.
    SERVICE_KEYBOARD_EVT_ESC,           // Tecla Escape.
    SERVICE_KEYBOARD_EVT_UP,            // Seta para cima.
    SERVICE_KEYBOARD_EVT_DOWN,          // Seta para baixo.
    SERVICE_KEYBOARD_EVT_LEFT,          // Seta para esquerda.
    SERVICE_KEYBOARD_EVT_RIGHT,         // Seta para direita.
    SERVICE_KEYBOARD_EVT_ATTACH,        // Teclado conectado.
    SERVICE_KEYBOARD_EVT_DETACH,        // Teclado desconectado.
    SERVICE_KEYBOARD_EVT_COMB           // Combinação de botões.
} SERVICE_KEYBOARD_EVENT_TYPE;

// Estrutura enviada para a aplicacao com o evento do teclado USB.
typedef struct
{
    SERVICE_KEYBOARD_EVENT_TYPE type;   // Tipo do evento detectado.
    char ch;                            // Caractere valido em EVT_CHAR e mascara de botoes em EVT_COMB.
} SERVICE_KEYBOARD_EVENT;

#ifdef __cplusplus
extern "C"
{
#endif



#ifdef __cplusplus
}
#endif

#endif // __DEFINITIONS_H__
