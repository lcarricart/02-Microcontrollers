/*******************************************************************************************************************
 * Objective:  Blink an LED using a semi-professional project structure through the use of CubeMX and CubeIDE.
 *******************************************************************************************************************
 * Context: Developed using CubeIDE. In order to avoid the direct and constant use of the STM32 HAL that would slow
 * down major projects, the official BSP Driver files "stm32f4xx_nucleo.c/h" were included.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student, HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "app.h"
#include "main.h"

void app_init(void)
{
    // any one-time init you want, beyond CubeMX init
}

void app_loop(void)
{
	if (BSP_PB_GetState(BUTTON_USER) == 0)   // pressed (low active) | can also use macro GPIO_PIN_RESET/GPIO_PIN_SET
	{
		// S: ...
		for (int i = 0; i < 3; i++) {
			BSP_LED_On(LED2);
			HAL_Delay(200);
			BSP_LED_Off(LED2);
			HAL_Delay(200);
		}
		HAL_Delay(1000);

		// O: ---
		for (int i = 0; i < 3; i++) {
			BSP_LED_On(LED2);
			HAL_Delay(1000);
			BSP_LED_Off(LED2);
			HAL_Delay(200);
		}
		HAL_Delay(1000);

		// S: ...
		for (int i = 0; i < 3; i++) {
			BSP_LED_On(LED2);
			HAL_Delay(200);
			BSP_LED_Off(LED2);
			HAL_Delay(200);
		}

		// wait for release
		while (BSP_PB_GetState(BUTTON_USER) == 0) {
			HAL_Delay(10);
		}
	}
}
