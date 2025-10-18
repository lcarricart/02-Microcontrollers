// Purpose of the program: to send a PING from my F7 board to the PC through Ethernet communication every time a button is pressed

#include "mbed.h"

InterruptIn button(BUTTON1);         // Pre-defined user button
DigitalOut led1(LED1);               // Pre-defined LED

// Globals
bool buttonPressed = false;

// Functions prototypes
void onButtonPressed_isr();

int main() {
    printf("MCU --> Ethernet --> PC\n");
    button.fall(callback(onButtonPressed_isr));

    while(1) {
        sleep();
    }
}

// Interrup Service Routine
void onButtonPressed_isr() {
    led1 = !led1;
}