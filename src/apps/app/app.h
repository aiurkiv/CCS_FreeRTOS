#ifndef APP_H_
#define APP_H_

#include "stdint.h"

void app_initialize(uint32_t sysclk_hz);

void app_task(void *pvParameters);

#endif
