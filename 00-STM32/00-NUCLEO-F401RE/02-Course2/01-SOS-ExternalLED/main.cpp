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
    // Initialise the digital pin LED1 as an output
    #ifdef LED1
        DigitalOut led(D10);
    #else
        bool led;
    #endif

    DigitalIn user_button(BUTTON1);     

    int i;

    while (true)
    {
        if (user_button == 0)       
        {    
            i = 0;
    
            while (i <= 2) 
            {
                led = 1;
                ThisThread::sleep_for(SHORT_BLINK_RATE);     
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
