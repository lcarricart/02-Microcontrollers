﻿/*****************************************************************************************************************
 * Objective of the program: Theory on Queues and Memory Pool within an RTOS environment. Prints on the PC.
 *******************************************************************************************************************
 * Theory: A queue is like a global array, but with improved mechanisms to be used effectively in thread-communication.
 A queue is designed for safe inter-thread communication. It has mechanisms that allow threads to safely add (enqueue)
 and remove (dequeue) items without interfering with one another. With a global array, you’d have to implement your own
 synchronization to prevent data corruption.

 + Memory pools are some luck of improvement and error-proof mechanism to avoid complex memory allocation handling like
 malloc() or calloc() from C programming. They´re a technique commonly used in embedded systems and real-time applications
 to avoid the pitfalls. Instead of dynamically allocating memory from the heap at runtime—which can lead to fragmentation,
 unpredictable allocation times, and potential failures—a memory pool preallocates a fixed block of memory that's divided
 into smaller, fixed-size chunks.

 + To reduce the code size Mbed introduced Minimal printf and snprintf. As of Mbed OS 6.0 it is enabled by default. 
 Floating point parameters are only present when minimal-printf-enable-floating-point config is set to true (disabled by 
 default). I’m not sure but this could be the case when building with the online compiler. If your application requires 
 more advanced functionality (at the cost of using more flash memory) you can switch to the standard printf library 
 configuring it in mbed_app.json file by overriding the parameter target.printf_lib with the value std.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student at HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "mbed.h"

// Define a structure for the message with three variables.
typedef struct {
    int voltage;    /* AD result of measured voltage */
    int current;    /* AD result of measured current */
    uint32_t counter; /* A counter value */
} message_t;

// Create a memory pool and a queue to manage message_t objects.
MemoryPool<message_t, 16> mpool;
Queue<message_t, 16> queue;
Thread thread;

// Thread function that simulates sending data.
void send_thread(void) {
    uint32_t i = 0;
    while (true) {
        i++; // Simulated data update, e.g. reading from an ADC

        // Allocate a message from the memory pool.
        message_t* message = mpool.try_alloc();
        message->voltage = i * 3;
        message->current = i * 2;
        message->counter = i;

        // Place the message at the end of the queue.
        queue.try_put(message);

        // Sleep for 1000 ms before sending the next message.
        ThisThread::sleep_for(1000ms);
    }
}

int main(void) {
    // Start the send_thread.
    thread.start(callback(send_thread));

    // Main loop: receive messages from the queue.
    while (true) {
        osEvent evt = queue.get(osWaitForever);
        if (evt.status == osEventMessage) {
            // Retrieve the message from the queue.
            message_t* message = (message_t*)evt.value.p;

            // Print the message data.
            printf("\nVoltage: %d V\n\r", message->voltage);
            printf("Current: %d A\n\r", message->current);
            printf("Number of cycles: %u\n\r", message->counter);

            // Free the message back to the memory pool.
            mpool.free(message);
        }
    }
}
