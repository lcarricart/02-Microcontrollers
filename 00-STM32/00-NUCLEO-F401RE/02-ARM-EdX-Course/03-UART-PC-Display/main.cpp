/*******************************************************************************************************************
 * Objective of the program: UART demonstration. Use the PC terminal to display information sent by the microcontroller.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student at HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "mbed.h"

BufferedSerial pc(USBTX, USBRX, 9600); // USB Transmitter, USB Receiver, using a UART port. 9600 is the baud rate.
int cycle = 0; // Just a counter.

int main() 
{
    while (true)
    {
        printf("Hello world. This is loop %i \n\r", cycle);

        thread_sleep_for(500);

        cycle++;
    }
}
