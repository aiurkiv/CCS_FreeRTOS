#ifndef __ADC_SERVICE_H__
#define __ADC_SERVICE_H__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

void ADC_SERVICE_Init(uint32_t sysclk_hz);

#ifdef __cplusplus
}
#endif

#endif // __ADC_SERVICE_H__
