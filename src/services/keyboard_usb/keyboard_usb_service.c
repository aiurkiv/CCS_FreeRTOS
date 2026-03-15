#include "keyboard_usb_service.h"

#include <stdbool.h>

#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"

#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/udma.h"
#include "driverlib/usb.h"

#include "FreeRTOS.h"
#include "task.h"

#include "usblib/usblib.h"
#include "usblib/usbhid.h"
#include "usblib/host/usbhost.h"
#include "usblib/host/usbhhid.h"
#include "usblib/host/usbhhidkeyboard.h"

/* Memoria usada pelo HCD da pilha USB host. */
#define KEYBOARD_USB_SERVICE_HCD_MEMORY_SIZE       128U
/* Buffer usado pela instancia do driver de teclado HID. */
#define KEYBOARD_USB_SERVICE_DEVICE_MEMORY_SIZE    128U
/* Periodo de processamento da task do servico USB. */
#define KEYBOARD_USB_SERVICE_TASK_PERIOD_MS        10U

/* Estados internos do servico de teclado USB. */
typedef enum
{
    KEYBOARD_USB_STATE_NO_DEVICE = 0,       /* Nenhum dispositivo conectado. */
    KEYBOARD_USB_STATE_KEYBOARD_INIT,       /* Teclado conectado e aguardando init. */
    KEYBOARD_USB_STATE_KEYBOARD_CONNECTED,  /* Teclado inicializado e operacional. */
    KEYBOARD_USB_STATE_KEYBOARD_UPDATE,     /* Atualizacao dos modificadores do teclado. */
    KEYBOARD_USB_STATE_UNKNOWN_DEVICE,      /* Dispositivo USB nao reconhecido. */
    KEYBOARD_USB_STATE_POWER_FAULT          /* Falha de alimentacao na porta USB. */
} KEYBOARD_USB_SERVICE_STATE;

/* Callback do HCD para eventos globais do barramento USB. */
static void KeyboardUSBService_HCDEvents(void *pvData);
/* Callback do driver HID para eventos especificos do teclado. */
static void KeyboardUSBService_KeyboardCallback(tUSBHKeyboard *psKbInstance,
                                                uint32_t ui32Event,
                                                uint32_t ui32MsgParam,
                                                void *pvMsgData);

/* Callback registrado pela aplicacao. */
static service_keyboard_usb_callback_t s_callback;
/* Contexto associado ao callback da aplicacao. */
static void *s_callback_ctx;
/* Estado atual do servico. */
static volatile KEYBOARD_USB_SERVICE_STATE s_state;
/* Clock do sistema informado pela aplicacao. */
static uint32_t s_sysclk_hz;
/* Frequencia da PLL USB usada pelo host controller. */
static uint32_t s_usb_pll_rate;
/* Estado acumulado dos modificadores do teclado. */
static uint32_t s_modifiers;
/* Indica se um teclado valido esta conectado. */
static bool s_keyboard_attached;
/* Instancia do driver HID do teclado. */
static tUSBHKeyboard *s_keyboard_instance;

/* Memoria interna do host controller driver. */
static uint8_t s_hcd_pool[KEYBOARD_USB_SERVICE_HCD_MEMORY_SIZE];
/* Buffer da instancia do teclado HID. */
static uint8_t s_keyboard_buffer[KEYBOARD_USB_SERVICE_DEVICE_MEMORY_SIZE];

/* Driver de eventos genericos do host USB. */
DECLARE_EVENT_DRIVER(g_sKeyboardUSBEventDriver, 0, 0, KeyboardUSBService_HCDEvents);

/* Tabela de drivers de classe registrados no host USB. */
static tUSBHostClassDriver const * const s_host_class_drivers[] =
{
    &g_sUSBHIDClassDriver,
    &g_sKeyboardUSBEventDriver
};

/* Quantidade de drivers de classe registrados. */
static const uint32_t s_num_host_class_drivers =
    sizeof(s_host_class_drivers) / sizeof(tUSBHostClassDriver *);

/* Mapa de traducao de usages HID para teclado US. */
extern const tHIDKeyboardUsageTable g_sUSKeyboardMap;

#if defined(ewarm)
#pragma data_alignment=1024
tDMAControlTable g_sKeyboardUSBDMAControlTable[6];
#elif defined(ccs)
#pragma DATA_ALIGN(g_sKeyboardUSBDMAControlTable, 1024)
tDMAControlTable g_sKeyboardUSBDMAControlTable[6];
#else
tDMAControlTable g_sKeyboardUSBDMAControlTable[6] __attribute__ ((aligned(1024)));
#endif

/*
 * KeyboardUSBService_DispatchEvent(...)
 *
 * Encaminha um evento gerado pelo servico para o callback registrado
 * pela aplicacao.
 */
static void KeyboardUSBService_DispatchEvent(SERVICE_KEYBOARD_EVENT_TYPE type, char ch)
{
    SERVICE_KEYBOARD_EVENT event;

    if(s_callback == NULL)
    {
        return;
    }

    event.type = type;
    event.ch = ch;
    s_callback(&event, s_callback_ctx);
}

/*
 * KeyboardUSBService_InitPins(void)
 *
 * Configura os pinos e perifericos necessarios para operar a USB0
 * em modo host com suporte a uDMA.
 */
static void KeyboardUSBService_InitPins(void)
{
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);

    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)) {}
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD)) {}
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)) {}
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOQ)) {}
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_USB0)) {}
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UDMA)) {}

    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= GPIO_PIN_6;

    MAP_GPIOPinConfigure(GPIO_PD6_USB0EPEN);
    MAP_GPIOPinTypeUSBAnalog(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    MAP_GPIOPinTypeUSBDigital(GPIO_PORTD_BASE, GPIO_PIN_6);
    MAP_GPIOPinTypeUSBAnalog(GPIO_PORTL_BASE, GPIO_PIN_6 | GPIO_PIN_7);
    MAP_GPIOPinTypeGPIOInput(GPIO_PORTQ_BASE, GPIO_PIN_4);

    MAP_IntPrioritySet(INT_USB0, PRIORITY_INTERRUPT_KEYBOARD_USB);

    MAP_uDMAEnable();
    MAP_uDMAControlBaseSet(g_sKeyboardUSBDMAControlTable);
}

/*
 * KeyboardUSBService_BuildEventFromUsage(...)
 *
 * Converte um usage HID recebido do teclado em um evento padronizado
 * para a aplicacao.
 */
static bool KeyboardUSBService_BuildEventFromUsage(uint32_t usage,
                                                   SERVICE_KEYBOARD_EVENT *event)
{
    unsigned char translated_char;

    if(event == NULL)
    {
        return false;
    }

    event->type = SERVICE_KEYBOARD_EVT_CHAR;
    event->ch = 0;

    switch(usage)
    {
        case HID_KEYB_USAGE_ENTER:
            event->type = SERVICE_KEYBOARD_EVT_ENTER;
            return true;

        case HID_KEYB_USAGE_BACKSPACE:
            event->type = SERVICE_KEYBOARD_EVT_BACKSPACE;
            return true;

        case HID_KEYB_USAGE_TAB:
            event->type = SERVICE_KEYBOARD_EVT_TAB;
            return true;

        case HID_KEYB_USAGE_SPACE:
            event->type = SERVICE_KEYBOARD_EVT_SPACE;
            return true;

        case HID_KEYB_USAGE_ESCAPE:
            event->type = SERVICE_KEYBOARD_EVT_ESC;
            return true;

        case HID_KEYB_USAGE_UP_ARROW:
            event->type = SERVICE_KEYBOARD_EVT_UP;
            return true;

        case HID_KEYB_USAGE_DOWN_ARROW:
            event->type = SERVICE_KEYBOARD_EVT_DOWN;
            return true;

        case HID_KEYB_USAGE_LEFT_ARROW:
            event->type = SERVICE_KEYBOARD_EVT_LEFT;
            return true;

        case HID_KEYB_USAGE_RIGHT_ARROW:
            event->type = SERVICE_KEYBOARD_EVT_RIGHT;
            return true;

        default:
            break;
    }

    translated_char = (unsigned char)USBHKeyboardUsageToChar(s_keyboard_instance,
                                                             &g_sUSKeyboardMap,
                                                             (unsigned char)usage);
    if(translated_char == 0U)
    {
        return false;
    }

    event->type = SERVICE_KEYBOARD_EVT_CHAR;
    event->ch = (char)translated_char;
    return true;
}

/*
 * KeyboardUSBService_HCDEvents(...)
 *
 * Trata eventos globais informados pelo host controller, como conexao,
 * desconexao, dispositivo desconhecido e falha de alimentacao.
 */
static void KeyboardUSBService_HCDEvents(void *pvData)
{
    tEventInfo *event_info;

    event_info = (tEventInfo *)pvData;
    if(event_info == NULL)
    {
        return;
    }

    switch(event_info->ui32Event)
    {
        case USB_EVENT_CONNECTED:
            if((USBHCDDevClass(event_info->ui32Instance, 0) == USB_CLASS_HID) &&
               (USBHCDDevProtocol(event_info->ui32Instance, 0) == USB_HID_PROTOCOL_KEYB))
            {
                s_keyboard_attached = true;
                s_state = KEYBOARD_USB_STATE_KEYBOARD_INIT;
                KeyboardUSBService_DispatchEvent(SERVICE_KEYBOARD_EVT_ATTACH, 0);
            }
            break;

        case USB_EVENT_UNKNOWN_CONNECTED:
            s_state = KEYBOARD_USB_STATE_UNKNOWN_DEVICE;
            break;

        case USB_EVENT_DISCONNECTED:
            if(s_keyboard_attached)
            {
                s_keyboard_attached = false;
                KeyboardUSBService_DispatchEvent(SERVICE_KEYBOARD_EVT_DETACH, 0);
            }

            s_state = KEYBOARD_USB_STATE_NO_DEVICE;
            break;

        case USB_EVENT_POWER_FAULT:
            s_keyboard_attached = false;
            s_state = KEYBOARD_USB_STATE_POWER_FAULT;
            KeyboardUSBService_DispatchEvent(SERVICE_KEYBOARD_EVT_DETACH, 0);
            break;

        default:
            break;
    }
}

/*
 * KeyboardUSBService_KeyboardCallback(...)
 *
 * Trata os eventos especificos do teclado HID e converte as teclas
 * pressionadas em eventos para a aplicacao.
 */
static void KeyboardUSBService_KeyboardCallback(tUSBHKeyboard *psKbInstance,
                                                uint32_t ui32Event,
                                                uint32_t ui32MsgParam,
                                                void *pvMsgData)
{
    SERVICE_KEYBOARD_EVENT event;

    (void)psKbInstance;
    (void)pvMsgData;

    switch(ui32Event)
    {
        case USBH_EVENT_HID_KB_PRESS:
            if(ui32MsgParam == HID_KEYB_USAGE_CAPSLOCK)
            {
                s_modifiers ^= HID_KEYB_CAPS_LOCK;
                s_state = KEYBOARD_USB_STATE_KEYBOARD_UPDATE;
                return;
            }

            if(ui32MsgParam == HID_KEYB_USAGE_SCROLLOCK)
            {
                s_modifiers ^= HID_KEYB_SCROLL_LOCK;
                s_state = KEYBOARD_USB_STATE_KEYBOARD_UPDATE;
                return;
            }

            if(ui32MsgParam == HID_KEYB_USAGE_NUMLOCK)
            {
                s_modifiers ^= HID_KEYB_NUM_LOCK;
                s_state = KEYBOARD_USB_STATE_KEYBOARD_UPDATE;
                return;
            }

            if(KeyboardUSBService_BuildEventFromUsage(ui32MsgParam, &event))
            {
                KeyboardUSBService_DispatchEvent(event.type, event.ch);
            }
            break;

        case USBH_EVENT_HID_KB_MOD:
        case USBH_EVENT_HID_KB_REL:
        default:
            break;
    }
}

/*
 * KeyboardUSBService_Init(uint32_t sysclk_hz)
 *
 * Inicializa o servico do teclado USB, registra os drivers de classe
 * e prepara a pilha USB host para operacao.
 */
void KeyboardUSBService_Init(uint32_t sysclk_hz)
{
    s_callback = NULL;
    s_callback_ctx = NULL;
    s_state = KEYBOARD_USB_STATE_NO_DEVICE;
    s_sysclk_hz = sysclk_hz;
    s_usb_pll_rate = 0U;
    s_modifiers = 0U;
    s_keyboard_attached = false;
    s_keyboard_instance = NULL;

    KeyboardUSBService_InitPins();

    USBStackModeSet(0, eUSBModeHost, 0);
    USBHCDRegisterDrivers(0, s_host_class_drivers, s_num_host_class_drivers);

    s_keyboard_instance = USBHKeyboardOpen(KeyboardUSBService_KeyboardCallback,
                                           s_keyboard_buffer,
                                           KEYBOARD_USB_SERVICE_DEVICE_MEMORY_SIZE);
    configASSERT(s_keyboard_instance != NULL);

    USBHCDPowerConfigInit(0, USBHCD_VBUS_AUTO_HIGH | USBHCD_VBUS_FILTER);

    SysCtlVCOGet(SYSCTL_XTAL_25MHZ, &s_usb_pll_rate);
    USBHCDFeatureSet(0, USBLIB_FEATURE_CPUCLK, &s_sysclk_hz);
    USBHCDFeatureSet(0, USBLIB_FEATURE_USBPLL, &s_usb_pll_rate);
    USBHCDInit(0, s_hcd_pool, KEYBOARD_USB_SERVICE_HCD_MEMORY_SIZE);
}

/*
 * KeyboardUSBService_SetCallback(...)
 *
 * Registra o callback da aplicacao que recebera os eventos do teclado USB.
 */
void KeyboardUSBService_SetCallback(service_keyboard_usb_callback_t callback, void *ctx)
{
    s_callback = callback;
    s_callback_ctx = ctx;
}

/*
 * KeyboardUSBService_Task(void *pvParameters)
 *
 * Task periodica responsavel por manter a pilha USB host ativa e
 * executar as transicoes do estado interno do servico.
 */
void KeyboardUSBService_Task(void *pvParameters)
{
    TickType_t last_wake_time;

    (void)pvParameters;

    last_wake_time = xTaskGetTickCount();

    for(;;)
    {
        USBOTGMain(KEYBOARD_USB_SERVICE_TASK_PERIOD_MS);

        switch(s_state)
        {
            case KEYBOARD_USB_STATE_KEYBOARD_INIT:
                USBHKeyboardInit(s_keyboard_instance);
                USBHKeyboardModifierSet(s_keyboard_instance, s_modifiers);
                s_state = KEYBOARD_USB_STATE_KEYBOARD_CONNECTED;
                break;

            case KEYBOARD_USB_STATE_KEYBOARD_UPDATE:
                USBHKeyboardModifierSet(s_keyboard_instance, s_modifiers);
                s_state = KEYBOARD_USB_STATE_KEYBOARD_CONNECTED;
                break;

            case KEYBOARD_USB_STATE_KEYBOARD_CONNECTED:
            case KEYBOARD_USB_STATE_NO_DEVICE:
            case KEYBOARD_USB_STATE_UNKNOWN_DEVICE:
            case KEYBOARD_USB_STATE_POWER_FAULT:
            default:
                break;
        }

        vTaskDelayUntil(&last_wake_time,
                        pdMS_TO_TICKS(KEYBOARD_USB_SERVICE_TASK_PERIOD_MS));
    }
}
