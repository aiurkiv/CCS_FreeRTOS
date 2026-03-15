#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"

#include "buttons.h"

#define BTN_MASK   0x03         // (GPIO_PIN_0 | GPIO_PIN_1)
#define BTN_DETECT_MS   20U     // Tempo para debounce
#define BTN_CONTINUE_MS 400U    // Tempo para identificar que botão continua pressionado e iniciar repeat
#define BTN_REPEAT_MS   100U    // Tempo para repetição do botão

static uint32_t s_sysclk;                         /* Clock do sistema usado pelo timer. */
static service_buttons_callback_t s_event_cb;    /* Callback de eventos dos botoes. */
static void *s_event_ctx;                        /* Contexto do callback. */

static ACT_EVENT btn;                            /* Estado do debounce/repeat dos botoes. */

/*  Timer0A_StartOneShot_ms(uint32_t ms)
    Timer0A está configurado com tempo em 32 bits, one-shot
    Parâmetro:
    - ms: tempo em ms
*/
static inline void Timer0A_StartOneShot_ms(uint32_t ms)
{
    uint32_t ticks = (s_sysclk / 1000U) * ms;  // assume ms <= ~35000 em 120MHz
    MAP_TimerDisable(TIMER0_BASE, TIMER_A);
    MAP_TimerPrescaleSet(TIMER0_BASE, TIMER_A, 0);
    MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, ticks - 1U);
    MAP_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    MAP_TimerEnable(TIMER0_BASE, TIMER_A);
}

/*
 * Timer0AIntHandler(void)
 *
 * ISR do timer usada para debounce e auto-repeat dos botoes. Quando ha
 * uma leitura valida, converte o estado dos botoes em SERVICE_KEYBOARD_EVENT.
 */
void Timer0AIntHandler(void)
{
    static SERVICE_KEYBOARD_EVENT kb_evt;
    // LIMPA a interrupção do timer
    MAP_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Leio os botões da GPIOJ
    uint8_t pressed = MAP_GPIOPinRead(GPIO_PORTJ_BASE, BTN_MASK);
    // Como os botões estão em PULL UP, inverto e uso mascara para pegar apenas os pinos 0 e 1
    pressed = (~pressed) & BTN_MASK;

    kb_evt.ch = 0;
    if(pressed == GPIO_PIN_0)
        kb_evt.type = SERVICE_KEYBOARD_EVT_UP;
    else if(pressed == GPIO_PIN_1)
        kb_evt.type = SERVICE_KEYBOARD_EVT_DOWN;
    else if(pressed == (GPIO_PIN_0 | GPIO_PIN_1))
    {
        kb_evt.type = SERVICE_KEYBOARD_EVT_COMB;
        kb_evt.ch = (char)pressed;
    }
    else
    {
        kb_evt.type = SERVICE_KEYBOARD_EVT_COMB;
        kb_evt.ch = (char)pressed;
    }
        
    // Trato se houver algum botão apertado
    if(pressed)
    {
        // Executa o callback
        if(s_event_cb)
            s_event_cb(&kb_evt, s_event_ctx);
        // Se detectou novo aperto, configuro o timer para 400ms e ativo.
        // Aqui ele irá verificar se o usuário permanece com o botão apertado. Se sim é para re
        if(btn == ACT_EVENT_BTN_DETECT)
        {
            Timer0A_StartOneShot_ms(BTN_CONTINUE_MS);       // Configuro o timer para 400ms
            btn = ACT_EVENT_BTN_VERIFY_REPEAT;
        }
        // Se botão continua apertado, configuro o timer para 200ms e ativo.
        else if(btn == ACT_EVENT_BTN_VERIFY_REPEAT)
        {
            Timer0A_StartOneShot_ms(BTN_REPEAT_MS);       // Configuro o timer para 200ms
            btn = ACT_EVENT_BTN_REPEAT;
        }
        // Se botão continua apertado, apenas ativo o timer (aqui já está configurado em 200ms).
        else if(btn == ACT_EVENT_BTN_REPEAT)
            MAP_TimerEnable(TIMER0_BASE, TIMER_A);
    }
    // Limpa flags de interrupção
    MAP_GPIOIntClear(GPIO_PORTJ_BASE, BTN_MASK);
    // Ativa a interrupção da GPIOJ (ou garanto caso já esteja ativa)
    MAP_GPIOIntEnable(GPIO_PORTJ_BASE, BTN_MASK);
}

/*
 * GPIOJIntHandler(void)
 *
 * ISR da GPIOJ que detecta a transicao inicial e agenda o debounce
 * no Timer0A.
 */
void GPIOJIntHandler(void)
{
    MAP_GPIOIntDisable(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);   // Desabilito as interrupções dos pinos da GPIOJ dos botões
    MAP_GPIOIntClear(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);     // Limpo as flags de interrupção
    Timer0A_StartOneShot_ms(BTN_DETECT_MS);                         // Configuro o timer para 20ms
    btn = ACT_EVENT_BTN_DETECT; // Detectou nova ação no botão
}
/* Registra o callback que recebera os eventos padronizados dos botoes. */
void Buttons_SetEventCallback(service_buttons_callback_t cbk, void *ctx)
{
    s_event_cb  = cbk;
    s_event_ctx = ctx;
}

/* Inicializa o hardware e as interrupcoes do servico de botoes. */
void Buttons_Init(uint32_t sysclk_hz)
{
    s_sysclk = sysclk_hz;
    s_event_cb = 0;
    s_event_ctx = 0;

    ////////////////////////////////////////////////////////
    // Configurações da GPIO para botões (GPIOJ).
    // PJ0 e PJ1 configuro como input PULL-UP + interrupção
    ////////////////////////////////////////////////////////
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);            // Habilito a GPIOJ
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)) {}   // Aguardo finalizar o processo de habilitar

    MAP_GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, BTN_MASK);        // Input
    MAP_GPIOPadConfigSet(GPIO_PORTJ_BASE, BTN_MASK, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);  // PULL-UP, 2mA

    MAP_GPIOIntDisable(GPIO_PORTJ_BASE, BTN_MASK);                      // Garanto que a interrupção está desabilitada
    MAP_GPIOIntClear(GPIO_PORTJ_BASE, BTN_MASK);                        // 
    MAP_GPIOIntTypeSet(GPIO_PORTJ_BASE, BTN_MASK, GPIO_FALLING_EDGE);   // 

    ////////////////////////////////////////////////////////
    // Configurações para o timer0A (32-bit one-shot) com interrupção ONE_SHOT
    ////////////////////////////////////////////////////////
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) {}

    MAP_TimerDisable(TIMER0_BASE, TIMER_A);
    MAP_TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT);
    MAP_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    MAP_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Habilita NVIC *** Deixar prioridade abaixo do FreeRTOS (configMAX_SYSCALL_INTERRUPT_PRIORITY = 5 << 5)
    // *** Quanto número maior == prioridade menor
    MAP_IntPrioritySet(INT_TIMER0A, PRIORITY_INTERRUPT_TIMER_BOUNCING);    // *** Pensar em uma forma mais "global" de definir o valor da prioridade como configKERNEL_INTERRUPT_PRIORITY, porém sem expor o freertos nessa biblioteca
    MAP_IntPrioritySet(INT_GPIOJ,   PRIORITY_INTERRUPT_BUTTONS);

    // Mapeio as funçõs que serão chamadas na interrupção.
    // Essa é a forma mais recente de configurar a função de interrupção da tivaware, porém também é possivel
    // fazer como nas versões anteriores, declarando a função no vetor de interrupções
    // do arquivo 'startup_ccs.c' (lembrar de declarar a função como extern no cabeçalho do arquivo de startup)
    MAP_GPIOIntRegister(GPIO_PORTJ_BASE, GPIOJIntHandler);
    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0AIntHandler);

    MAP_IntEnable(INT_TIMER0A);
    MAP_IntEnable(INT_GPIOJ);
}

/* Habilita o recebimento de interrupcoes dos botoes. */
void Buttons_Enable(void)
{
    MAP_GPIOIntClear(GPIO_PORTJ_BASE, BTN_MASK);
    MAP_GPIOIntEnable(GPIO_PORTJ_BASE, BTN_MASK);
}

/* Desabilita o recebimento de interrupcoes dos botoes. */
void Buttons_Disable(void)
{
    MAP_GPIOIntDisable(GPIO_PORTJ_BASE, BTN_MASK);
}
