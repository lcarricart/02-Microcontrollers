/*******************************************************************************************************************
 * Objective:  Recreate the classic RTOS problem of the dining philosophers proposed by Dijsktra.
 *******************************************************************************************************************
 * Remarks: When looking at the output, since forks are shared between neighboring philosophers, those seating next
 *          to each other cannot eat simultaneously. The idea of this exercise is to propose a solution that does
 *          not lead to a deadlock situation, where everyone stays waiting forever:
 *              - P1 owns fork1, waits fork2
 *              - P2 owns fork2, waits fork3
 *              - P3 owns fork3, waits fork4
 *              - P4 owns fork4, waits fork5
 *              - P5 owns fork5, waits fork1
 *******************************************************************************************************************
 * Conclusion:
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student, HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "app.h"
#include "main.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "semphr.h"

// FreeRTOS objects declaration
SemaphoreHandle_t fork1_mutex;
SemaphoreHandle_t fork2_mutex;
SemaphoreHandle_t fork3_mutex;
SemaphoreHandle_t fork4_mutex;
SemaphoreHandle_t fork5_mutex;

// Tasks prototypes
/* This Philosopher task refers to all the philosophers. They will just change in parameter passed, and 5 instances of this task will be used.*/
void Philosopher(void *pvParamaters);

// Helper functions


//==============================================================================
// RTOS Tasks
//==============================================================================
void Philosopher(void *pvParamaters)
{
  SemaphoreHandle_t first_fork;
  SemaphoreHandle_t second_fork;

  int id = *(int*) pvParamaters;

  while(1)
  {
//    printf("I am philosopher %d \r\n");

    if(id == 1)
    {
      first_fork = fork1_mutex;
      second_fork = fork2_mutex;
    }
    else if(id == 2)
    {
      first_fork = fork2_mutex;
      second_fork = fork3_mutex;
    }
    else if(id == 3)
    {
      first_fork = fork3_mutex;
      second_fork = fork4_mutex;
    }
    else if(id == 4)
    {
      first_fork = fork4_mutex;
      second_fork = fork5_mutex;
    }
    else if(id == 5)
    {
      first_fork = fork5_mutex;
      second_fork = fork1_mutex;
    }

    /* Take the forks */
    xSemaphoreTake(first_fork, portMAX_DELAY);
    xSemaphoreTake(second_fork, portMAX_DELAY);

    /* Simulate eating */
    printf("Philosopher %d is eating \r\n", id);
    vTaskDelay(pdMS_TO_TICKS(500));

    /* Put down the forks */
    xSemaphoreGive(second_fork);
    xSemaphoreGive(first_fork);

    /* Simulate thinking */
    printf("Philosopher %d is thinking \r\n", id);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

//==============================================================================
// Application
//==============================================================================
void app_init()
{
  fork1_mutex = xSemaphoreCreateMutex();
  fork2_mutex = xSemaphoreCreateMutex();
  fork3_mutex = xSemaphoreCreateMutex();
  fork4_mutex = xSemaphoreCreateMutex();
  fork5_mutex = xSemaphoreCreateMutex();
  assert_param(fork1_mutex != NULL);
  assert_param(fork2_mutex != NULL);
  assert_param(fork3_mutex != NULL);
  assert_param(fork4_mutex != NULL);
  assert_param(fork5_mutex != NULL);
}

void app_rtos_init()
{
  /* This variables need to be static or else once this function finishes execution, all the pointers will just lead nowhere */
  static int id1 = 1;
  static int id2 = 2;
  static int id3 = 3;
  static int id4 = 4;
  static int id5 = 5;

  xTaskCreate(Philosopher, "P1", 128, &id1, 1, NULL);
  xTaskCreate(Philosopher, "P2", 128, &id2, 1, NULL);
  xTaskCreate(Philosopher, "P3", 128, &id3, 1, NULL);
  xTaskCreate(Philosopher, "P4", 128, &id4, 1, NULL);
  xTaskCreate(Philosopher, "P5", 128, &id5, 1, NULL);
}
