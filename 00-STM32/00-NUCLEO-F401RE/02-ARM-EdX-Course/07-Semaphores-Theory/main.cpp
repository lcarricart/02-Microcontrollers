/*******************************************************************************************************************
 * Objective of the program: Theory on semaphores and threads within an RTOS environment. Prints to the PC.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student at HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "mbed.h"

Semaphore one_slot(1); // Create semaphore, named one_slot, one resource
Thread t2;
Thread t3;

void test_thread(void const* name)
{
    while (true)
    {
        one_slot.acquire(); // Acquire semaphore
        printf("%s\n\r", (const char*)name);
        ThisThread::sleep_for(1000ms);
        one_slot.release(); // Release semaphore
    }
}

int main(void)
{
    t2.start(callback(test_thread, (void*)"Th 2"));
    t3.start(callback(test_thread, (void*)"Th 3"));
    test_thread((void*)"Th 1");
}