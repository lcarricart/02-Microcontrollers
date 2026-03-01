// Objective: use the STM32duino to connect a Bluetooth module HC-05 (ZS-040) and blink an LED in different patterns (also Serial monitor printing)

#include <Arduino.h>
#include <stdio.h>

#define LED1 PA5

String message = " ";

void setup() {
  pinMode(LED1, OUTPUT);

  // For on-screen display w/ computer
  Serial.begin(9600);
  Serial.println("Start of the application!");

  // For BLE module
  Serial1.begin(9600);
}

void loop() {
  if(Serial1.available()) {
    message = Serial1.readString();
    Serial.println(message);

    if (message == "ON") {
      Serial.println(message);
      digitalWrite(LED1, HIGH);
      delay(500);     
    } else if (message == "OFF") {
      Serial.println(message);
      digitalWrite(LED1, LOW);
      delay(500);             
    } else if (message == "BLINK") {
      Serial.println(message);
      digitalWrite(LED1, HIGH);
      delay(500);            
      digitalWrite(LED1, LOW);
      delay(500);
      digitalWrite(LED1, HIGH);
      delay(500);            
      digitalWrite(LED1, LOW);
      delay(500);          
    }        
  }
}
