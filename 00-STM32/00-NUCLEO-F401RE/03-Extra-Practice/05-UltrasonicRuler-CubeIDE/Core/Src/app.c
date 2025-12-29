/*******************************************************************************************************************
 * Objective:  Adapt the program "03-UltrasonicRuler-MbedOS" written with Mbed Studio and its dependencies, to work
 * with CubeIDE. This would be a solid proof that I can work more complex projects with non-IDE dependencies involved.
 *******************************************************************************************************************
 * Remarks: 	- The drivers used in the previous application were tightly tied to Mbed OS. I need now to find something
 * 				  more "generic" (which should be easier to find).
 * 				- Put f on constants if you want float math (0.01715f, 343.0f, etc.). Prefer float over double on STM32F4:
 * 				  the F401 has a single-precision FPU (it accelerates float, not double).
 * 				- The use of a a timer handle allows microsecond-level delays (for ultrasonic sensor timing - very relevant
 * 				  for my application!), and non-blocking delays (better than HAL_Delay() which blocks everything)
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student, HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "app.h"
#include "main.h"

/* --- Pin renaming --- */
// ULTRASONIC_TRIG in pin PA8 (D7) then I need to choose port A, pin 8
#define ULTRASONIC_TRIG_GPIO_Port  GPIOA
#define ULTRASONIC_TRIG_Pin        GPIO_PIN_8
// ULTRASONIC_ECHO in pin PA9 (D8)
#define ULTRASONIC_ECHO_GPIO_Port  GPIOA
#define ULTRASONIC_ECHO_Pin        GPIO_PIN_9

/* --- Variables --- */
// I2C port setted in CubeMX. This way, a handle is generated for me to use
extern I2C_HandleTypeDef hi2c1;  // Reference the handle from main.c

LCD1602A lcd;

/* --- Function definitions --- */
float measure_distance_cm();

// One-time initializations to run before app_loop
void app_init(void)
{

}

// Main loop
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

// Helper function
float measure_distance_cm() {

}
