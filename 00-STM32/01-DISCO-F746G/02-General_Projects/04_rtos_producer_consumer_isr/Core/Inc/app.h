#ifndef APP_H
#define APP_H
#include "cmsis_os.h"

void app_init(void);
void app_rtos_init(void);

void app_loop(void);

uint32_t pseudoRandomValue(uint32_t min, uint32_t max);

#endif
