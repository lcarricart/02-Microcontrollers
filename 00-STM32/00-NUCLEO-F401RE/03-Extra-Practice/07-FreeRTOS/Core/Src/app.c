/*******************************************************************************************************************
 * Objective:  Create a simple FreeRTOS application to better understand its syntax and flow.
 *******************************************************************************************************************
 * Remarks: The print does not happen in the regular console where I see "flashing successfull". I must open a specific
 * console that is also built-in called "Command Shell Console" (default parameters are fine for it).
 *******************************************************************************************************************
 * Conclusion: The project is not finished. I created an LED printing task but the button task does not work (probably
 * because of interrupts or so.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student, HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "app.h"
#include "main.h"
#include <stdio.h>

// Button in pin PC13 then I need to choose port C, pin 13
#define BUTTON_Port  GPIOC
#define BUTTON_Pin   GPIO_PIN_13

// FreeRTOS objects declaration
osThreadId_t task1Handle;
osThreadId_t task2Handle;
osSemaphoreId_t buttonSemaphoreHandle;

// Tasks prototypes
void Task1_Print_LEDBlink(void *argument);
void Task2_Print_ButtonPressed(void *argument);

// Helper functions
void delay_me(int);

//==============================================================================
// RTOS Tasks
//==============================================================================
void Task1_Print_LEDBlink(void *argument)
{
	for(;;)
	{
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin); // Defined by CubeMX, tho I can make it more explicit
		printf("Task1: LED toggled\r\n");
		osDelay(500);
	}
}

void Task2_Print_ButtonPressed(void *argument)
{
	for(;;)
	{
		if(osSemaphoreAcquire(buttonSemaphoreHandle, osWaitForever) == osOK)
		{
			printf("Task2: Button pressed!\r\n");
		}
	}
}

//==============================================================================
// GPIO Interrupt Callback
//==============================================================================
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	static uint32_t lastPress = 0;
	uint32_t currentTime = HAL_GetTick();
	
	if(GPIO_Pin == BUTTON_Pin)
	{
		// Simple debouncing: ignore presses within 200ms
		if((currentTime - lastPress) > 200)
		{
			osSemaphoreRelease(buttonSemaphoreHandle);
			lastPress = currentTime;
		}
	}
}

//==============================================================================
// Application
//==============================================================================
void app_init()
{

}

void app_rtos_init()
{
	// Semaphore instantiation
	buttonSemaphoreHandle = osSemaphoreNew(1 /*max_tokens*/, 0 /*initial_tokens*/, NULL /*pointer to optional attributes*/);

	// Create tasks attributes
	const osThreadAttr_t task1_attributes = {
		.name = "LEDBlink",
		.stack_size = 128 * 4,
		.priority = (osPriority_t) osPriorityNormal,
	};
	const osThreadAttr_t task2_attributes = {
		.name = "ButtonPressed",
		.stack_size = 128 * 4,
		.priority = (osPriority_t) osPriorityNormal,
	};

	// Create tasks
	task1Handle = osThreadNew(Task1_Print_LEDBlink, NULL, &task1_attributes);
	task2Handle = osThreadNew(Task2_Print_ButtonPressed, NULL, &task2_attributes);
}

void delay_me(int delay)
{
	int ms = delay * 10000;

	for(int i = 0; i < ms; i++)
	{

	}
}
