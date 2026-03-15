#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/adc.h"

#include "adc_service.h"
#include "definitions.h"

static uint32_t s_sysclk;

void amostra(void);

void ADC_SERVICE_Init(uint32_t sysclk_hz)
{   
    s_sysclk = sysclk_hz;
    ////////////////////////////////////////////////////////
    // Configurações para o timer1A (32-bit periodico) com interrupção
    ////////////////////////////////////////////////////////
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1)) {}

    MAP_TimerDisable(TIMER1_BASE, TIMER_A);
    //MAP_TimerConfigure(TIMER1_BASE, TIMER_CFG_ONE_SHOT);
    MAP_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    MAP_TimerLoadSet(TIMER1_BASE, TIMER_A, (s_sysclk / 1000) * 40); // 40ms
    MAP_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    MAP_TimerControlTrigger(TIMER1_BASE, TIMER_A, true);
    MAP_TimerEnable(TIMER1_BASE, TIMER_A);
    // Como o timer vai apenas disparar o adc, não precisa acionar a interrupção

    ////////////////////////////////////////////////////////
    // Configurações para ADC0 no GPIOE4
    ////////////////////////////////////////////////////////
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    // Aguarda os periféricos iniciarem
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)) {}
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)) {}
    // Configuro GPIOE pino 4 para leitura ADC (input analogic)
    MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4);
    MAP_ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_TIMER, 1);
    MAP_ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);
    MAP_ADCSequenceEnable(ADC0_BASE, 3);
    MAP_ADCIntEnable(ADC0_BASE, 3);
    ADCIntRegister(ADC0_BASE, 3, amostra);
    MAP_IntPrioritySet(INT_ADC0SS3, PRIORITY_INTERRUPT_ADC0);   // prioridade da interrupção
    MAP_IntEnable(INT_ADC0SS3);
}

void amostra(void)
{
    uint32_t s;
    MAP_ADCIntClear(ADC0_BASE, 3);
    ADCSequenceDataGet(ADC0_BASE, 3, &s);
    if(s > 2048)
        MAP_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4);  // liga
    else
        MAP_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);           // desliga
}
