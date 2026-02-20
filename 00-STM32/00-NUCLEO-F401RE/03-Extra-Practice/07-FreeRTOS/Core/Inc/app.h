#ifndef APP_H
#define APP_H
#include "cmsis_os.h"

void app_init(void);
void app_rtos_init(void);  // Create tasks (LED_Blink, LED_Button) & semaphores

void app_loop(void);

#endif
