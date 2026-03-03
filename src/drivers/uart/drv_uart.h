#ifndef __DRV_UART_H__
#define __DRV_UART_H__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

void DRV_Uart_Init(uint32_t sysclk_hz);

#ifdef __cplusplus
}
#endif

#endif // __DRV_UART_H__
