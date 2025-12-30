/*******************************************************************************************************************
 * Objective:  Adapt the program "03-UltrasonicRuler-MbedOS" written with Mbed Studio and its dependencies, to work
 * with CubeIDE. This would be a solid proof that I can work more complex projects with non-IDE dependencies involved.
 *******************************************************************************************************************
 * Remarks: 	- The drivers used in the previous application were tightly tied to Mbed OS. I need now to find something
 * 				  more "generic" (which should be easier to find).
 * 				- Put f on constants if you want float math (0.01715f, 343.0f, etc.). Prefer float over double on STM32F4:
 * 				  the F401 has a single-precision FPU (it accelerates float, not double). [Right-click project → Properties,
 * 				  C/C++ Build → Settings → Tool Settings, MCU GCC Linker → Miscellaneous, In "Other flags" or "Linker flags"
 * 				  box, add: -u _printf_float, Apply → OK, Clean and rebuild].
 * 				- The use of a a timer handle allows microsecond-level delays (for ultrasonic sensor timing - very relevant
 * 				  for my application!), and non-blocking delays (better than HAL_Delay() which blocks everything). This
 * 				  needs to be set from CubeMX. (This timer is not exploited to professional use, since it could also
 * 				  improve the precision of my TRIGGER, but I decided to go for the simpler and understandable approach)
 *******************************************************************************************************************
 * Conclusion: The project works but proves that the use of CubeIDE requires professional knowledge in the field. It is not
 * trivial at all to include dependencies from other developers, considering the CubeMX configurations involved in the process.
 * It may be possible to try to replicate the .ioc files that are provided in the repositories of the dependencies, and skip
 * some of this struggle. However, operating with this IDE requires more studying. A great step forward would be reading the
 * "Mastering STM32" by Carmine Noviello, covering HAL, I2C, SPI, UART peripherals, timers, and ADC configurations.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student, HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "app.h"
#include "main.h"
#include "lcd1602a.h"
#include <stdio.h>

/* --- Pin renaming --- */
// ULTRASONIC_TRIG in pin PA9 (D8) then I need to choose port A, pin 8
#define ULTRASONIC_TRIG_GPIO_Port  GPIOA
#define ULTRASONIC_TRIG_Pin        GPIO_PIN_9
// ULTRASONIC_ECHO in pin PA8 (D7) following the structure of my library (timer handle binds me)
#define ULTRASONIC_ECHO_GPIO_Port  GPIOA
#define ULTRASONIC_ECHO_Pin        GPIO_PIN_8

/* --- Variables --- */
// I2C port setted in CubeMX. This way, a handle is generated for me to use
extern I2C_HandleTypeDef hi2c1;  // Reference the handle from main.c
extern TIM_HandleTypeDef htim1;  // References the timer handle

LCD1602A lcd;
float distance;
char buffer[16];

/* --- Function definitions --- */
float measure_distance_cm();
void test_i2c_scan(void);

// One-time initializations to run before app_loop
void app_init(void)
{
	// Test I2C communication first
	test_i2c_scan();

	// LCD initialization through libraries
	LCD_Init(&lcd, &hi2c1);           // Connect LCD to I2C1
	LCD_SetBacklight(&lcd, 1);        // Turn on backlight
	LCD_ClearDisplay(&lcd);           // Clear screen
	LCD_SetCursor(&lcd, 0, 0);        // Position cursor at row 0, col 0
	LCD_WriteString(&lcd, "Ready!");  // Test message

	// Timer initialization
	HAL_TIM_Base_Start(&htim1);  // Start in basic mode for counting
}

// Main loop
void app_loop(void)
{
	distance = measure_distance_cm();

	LCD_ClearDisplay(&lcd);
	HAL_Delay(5);
	LCD_SetCursor(&lcd, 0, 0);
	LCD_WriteString(&lcd, "Ultrasonic App!");

	LCD_SetCursor(&lcd, 1, 0);

	if (distance >= 0.0f) {
		sprintf(buffer, "Dist: %0.1f cm", distance);
		LCD_WriteString(&lcd, buffer); // I would like to have display that says "Dist: (distance) cm"

		// Debugging
		printf("Distance: %d cm\n", (int)distance);
	}

	BSP_LED_On(LED2);
	HAL_Delay(50);
	BSP_LED_Off(LED2);

	HAL_Delay(500);
}

// Helper function
float measure_distance_cm() {
	// Ensure trigger is low
	HAL_GPIO_WritePin(ULTRASONIC_TRIG_GPIO_Port, ULTRASONIC_TRIG_Pin, GPIO_PIN_RESET);
	HAL_Delay(1); // ms

	// Send 10 us pulse on TRIG
	HAL_GPIO_WritePin(ULTRASONIC_TRIG_GPIO_Port, ULTRASONIC_TRIG_Pin, GPIO_PIN_SET);
	delay(10);	 // us
	HAL_GPIO_WritePin(ULTRASONIC_TRIG_GPIO_Port, ULTRASONIC_TRIG_Pin, GPIO_PIN_RESET);

	// Wait for ECHO to go high (start of pulse), with timeout
	uint32_t timeout_us = 30000;   // 30 ms ~ ~5 m range
	while (HAL_GPIO_ReadPin(ULTRASONIC_ECHO_GPIO_Port, ULTRASONIC_ECHO_Pin) == GPIO_PIN_RESET) {
		if (--timeout_us == 0) {
			return -1.0f;    // No echo received
		}
	}

	// ECHO went high - start measuring pulse width
	uint32_t pulse_start = 0;
	uint32_t pulse_end = 0;

	// Measure how long ECHO stays high
	__HAL_TIM_SET_COUNTER(&htim1, 0);  // Reset timer counter
	pulse_start = __HAL_TIM_GET_COUNTER(&htim1);

	timeout_us = 30000;
	while (HAL_GPIO_ReadPin(ULTRASONIC_ECHO_GPIO_Port, ULTRASONIC_ECHO_Pin) == GPIO_PIN_SET) {
		if (--timeout_us == 0) {
			return -1.0f;    // Pulse too long / out of range
		}
	}

	pulse_end = __HAL_TIM_GET_COUNTER(&htim1);

	// Calculate pulse duration in microseconds
	uint32_t pulse_width_us = pulse_end - pulse_start;

	// Distance in cm: distance = (time * speed_of_sound) / 2
	// speed of sound ≈ 343 m/s → 0.01715 cm/μs
	float distance_cm = pulse_width_us * 0.01715f;

	return distance_cm;
}

// Add this function in app.c after measure_distance_cm()
void test_i2c_scan(void) {
    HAL_StatusTypeDef result;

    // Try common LCD addresses
    uint8_t addresses[] = {0x27 << 1, 0x3F << 1};

    for (int i = 0; i < 2; i++) {
        result = HAL_I2C_IsDeviceReady(&hi2c1, addresses[i], 1, 100);

        if (result == HAL_OK) {
            // Device found - blink LED rapidly
            for (int j = 0; j < 10; j++) {
                BSP_LED_On(LED2);
                HAL_Delay(50);
                BSP_LED_Off(LED2);
                HAL_Delay(50);
            }
            return;
        }
    }

    // No device found - solid LED for 2 sec
    BSP_LED_On(LED2);
    HAL_Delay(2000);
    BSP_LED_Off(LED2);
}
