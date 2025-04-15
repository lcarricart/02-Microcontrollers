/*******************************************************************************************************************
 * Objective of the program: As part of a major project for a delivery robot competition, this program controls the
 playing ground of the robot. A sound sensor is used to start the whole system, intended to be triggered by the
 robot, while relay1 controls a blinking LED stripe, and relay2 the trigger to start the MP3 player and speaker.
 *******************************************************************************************************************
 * Learning outcome: In the search of digital switches, relays prove to be a very useful alternative. However, there
 must be ways to control switches through the microcontroller itself.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student at HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

 // Preprocessor directives
#include "mbed.h"

// Object declarations
DigitalOut relay(D5);         // Relay to control the LED stripe
DigitalOut relay2(D4);        // Relay to control the speaker´s trigger
InterruptIn soundSensor(D3);  // Activation method for the system

// This global flag is declared as volatile because it is modified inside an interrupt service routine
volatile bool already_started = 0;   // Indicates if system is already running or not

// Prototypes
void relay_activation(void);
void relay_activation2(void);

//-------------- Threads ----------------//

// If sound sensor is triggered, relay goes active
Thread thread1;
void relay_activation()
{
    while (1)
    {
        if (already_started == 1)
        {
            relay = !relay;
            ThisThread::sleep_for(200ms);
        }
        else {    // Check the flag more often
            ThisThread::sleep_for(10ms);
        }
    }
}

// If sound sensor is triggered, relay goes active
Thread thread2;
void relay_activation2()
{
    int i;
    while (1)
    {
        if (already_started == 1)
        {
            // This mechanism is needed because one single switch is not recognized by the MP3 player as a trigger signal. By doing it 6 times I make sure it always activates.
            for (i = 0; i < 6; i++)
            {
                relay2 = !relay2;
                ThisThread::sleep_for(500ms);
            }
            ThisThread::sleep_for(1000000s);    // Once the trigger is sent, this thread can go to sleep forever. I can improve this with a one-shot thread or sleep forever
        }
        else {
            ThisThread::sleep_for(10ms);
        }
    }
}

/*-------------- Handlers ---------------*/

// Interrupt Service Routine (ISR) triggered when the sensor output goes HIGH.
void soundDetectedISR()
{
    if (already_started == 0)
        already_started = 1;  // Flag goes active
}

//--------------- Main ------------------//
int main() {
    // Set up the sound sensor interrupt.
    soundSensor.rise(callback(soundDetectedISR));

    // Start threads.
    thread1.start(callback(relay_activation));
    thread2.start(callback(relay_activation2));

    while (1) {
        __WFI(); // Wait For Interrupt
    }
}