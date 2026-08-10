/*******************************************************************************************************************
 * Objective: Model a producer–processor–consumer pipeline using queues for communication between an ISR and two
 *            tasks. To applies to applications needing asynchronous communication.
 *******************************************************************************************************************
 * Conclusions: It was an interesting discovery to see how interrupts in RTOSs need to be handled with care. There
 *              are special APIs to be used, and considerations need to be taken if those ISRs actually interact with
 *              RTOS objects.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student, HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "app.h"
#include "main.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "queue.h"

// FreeRTOS objects declaration
QueueHandle_t incomingQueue;
QueueHandle_t outgoingQueue;
QueueHandle_t logQueue;

// Tasks prototypes
void Processor_Task(void *pvParameters);
void Consumer_Task(void *pvParameters);
void Logger_Task(void *pvParameters);

// Callback prototypes (I defined it in main.c)
extern void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

// Functions
uint32_t pseudoRandomValue(uint32_t min, uint32_t max);

//==============================================================================
// RTOS Tasks
//==============================================================================
/* void Producer_Task does not exist, this is the ISR Callback, that gets called every time the TIM7 timeouts. */

/**
 * @brief: Takes data from incomingQueue, alters it, and sends it to outgoingQueue
 */
void Processor_Task(void *pvParameters)
{
    uint32_t receivedData = 0;
    uint32_t processedData = 0;

    while (1)
    {
        /* Here one would maybe use an if() statement, but since we have portMAX_DELAY, our task will not return until the Queue contains data */
        xQueueReceive(incomingQueue, &receivedData, portMAX_DELAY /* because nothing useful to do otherwise; wait until next element */); // an item was successfully received
        processedData = receivedData + 1000;
        xQueueSend(outgoingQueue, &processedData, portMAX_DELAY);
    }
}

/**
 * @brief: Takes data from outgoingQueue and prints it
 */
void Consumer_Task(void *pvParameters)
{
    uint32_t receivedData = 0;

    while (1)
    {
        xQueueReceive(outgoingQueue, &receivedData, portMAX_DELAY);
        printf("Outgoing data = %u \n", (unsigned int) receivedData);
    }
}

/**
 * @brief: the ISR never calls printf(). It only copies a small log message into a dedicated logQueue, and a Logger_Task performs the actual printing.
 */
void Logger_Task(void *pvParameters)
{
    uint32_t message = 0;

    while (1)
    {
        xQueueReceive(logQueue, &message, portMAX_DELAY);
        printf("[ISR] Generated value = %u \r\n", (unsigned int) message);
    }
}

//==============================================================================
// Functions
//==============================================================================
/**
 * @brief: The idea of this function is to be called inside an ISR context, reason why it cannot include certain functions such as rand()
 * @param: minimum and maximum possible value range
 */
uint32_t pseudoRandomValue(uint32_t min, uint32_t max)
{
    static uint32_t state = 0x12345678U;

    // xorshift32
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;

    return min + (state % (max - min + 1U));
}

//==============================================================================
// Application
//==============================================================================
void app_init(void)
{
    incomingQueue = xQueueCreate(
            4,                   // maximum 4 elements
            sizeof(uint32_t)// each element is uint32_t
            );
    assert_param(incomingQueue != NULL);

    outgoingQueue = xQueueCreate(4, sizeof(uint32_t));
    assert_param(outgoingQueue != NULL);

    logQueue = xQueueCreate(8, sizeof(uint32_t));
    assert_param(logQueue != NULL);
}

void app_rtos_init(void)
{
    BaseType_t result;

    result = xTaskCreate(Processor_Task, "processor", 128, NULL, 1, NULL);
    assert_param(result == pdPASS);

    result = xTaskCreate(Consumer_Task, "consumer", 128, NULL, 1, NULL);
    assert_param(result == pdPASS);

    result = xTaskCreate(Logger_Task, "logger", 256 /* printf can require more stack size */, NULL, 1, NULL);
    assert_param(result == pdPASS);
}
