/*******************************************************************************************************************
 * Objective of the program:  Make the internal LED of the board throw an SOS morse code signal every time the USER
 button is pressed.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student at HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "mbed.h"


// Blinking rate in milliseconds
#define LONG_BLINK_RATE     1000ms
#define SHORT_BLINK_RATE    200ms
#define INTER_LETTER_DELAY  1000ms
#define INTER_BLINK_DELAY   200ms

int main()
{
    DigitalOut led(D10);
    DigitalIn user_button(BUTTON1);     // Mbed OS already maps the USER button pin to BUTTON1. On your board, it’s connected to GPIO pin PC_13, but you don’t need to remember that — just use BUTTON1.

    int i;

    while (true)
    {
        if (user_button == 0)       // The button is active-low, meaning it reads 0 when pressed, and 1 when not pressed.
        {    
            i = 0;
    
            while (i <= 2) 
            {
                led = 1;
                ThisThread::sleep_for(SHORT_BLINK_RATE);      //This function pauses the current thread for a specified duration (BLINKING_RATE1 in your case, like 500ms). It doesn't put the whole microcontroller into sleep — just the thread you're currently running (which is typically main() if you're not using an RTOS).
                led = 0;
                ThisThread::sleep_for(INTER_BLINK_DELAY);
                i++;
            }
            ThisThread::sleep_for(INTER_LETTER_DELAY);

            while (i <= 5) 
            {
                led = 1;
                ThisThread::sleep_for(LONG_BLINK_RATE);
                led = 0;
                ThisThread::sleep_for(INTER_BLINK_DELAY);
                i++;
            }
            ThisThread::sleep_for(INTER_LETTER_DELAY);

            while (i <= 8)
            {
                led = 1;
                ThisThread::sleep_for(SHORT_BLINK_RATE);
                led = 0;
                ThisThread::sleep_for(INTER_BLINK_DELAY);
                i++;
            }
        }
    }
}
