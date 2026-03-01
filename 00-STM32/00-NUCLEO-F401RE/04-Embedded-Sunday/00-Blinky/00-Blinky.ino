// STM32F401RE Example Blinky with Serial monitor prints

#include <Arduino.h>
#include <stdio.h>

#define LED1 PA5
#define BUTTON1 PC13


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED1, OUTPUT);
  pinMode(BUTTON1, INPUT);

  Serial.begin(9600); // Has to match the baud rate of your serial monitor. Show what happens when you choose a different one.
  // To see the printing results, open Tools > Serial Monitor
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED1, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(500);                      // wait for a second
  digitalWrite(LED1, LOW);   // turn the LED off by making the voltage LOW
  delay(500);                      // wait for a second
}
