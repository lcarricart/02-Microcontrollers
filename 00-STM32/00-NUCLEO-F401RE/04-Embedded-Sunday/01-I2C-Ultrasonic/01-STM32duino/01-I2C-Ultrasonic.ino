#include <Arduino.h>
#include "LCD_I2C.h"

/* Application doesn't work because STM32duino is incorrectly mapping my SDA and SCL
*   _wire.setSDA(14);
*   _wire.setSCL(15);
*  Probably library is also doing something I don't realize */

#define LCD_I2C_ADDRESS (0x27)
#define LED1 PA5

LCD_I2C lcd(LCD_I2C_ADDRESS, 16 /*columns*/, 2 /*rows*/);

void setup() {
  pinMode(LED1, OUTPUT);

  // For on-screen display w/ computer
  Serial.begin(9600);
  Serial.println("(PC) Application started!");

  lcd.begin();
  lcd.backlight();
}

void loop() {
  lcd.clear();
  lcd.print("(LCD) Application");
  lcd.setCursor(5, 1); // Or setting the cursor in the desired position.
  lcd.print("Started!");
  delay(500);
}
