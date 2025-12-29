// Purpose of the program: Use the I2C protocol to communicate with the LCD and the 6-axis IMU
// Remarks: Even though both devices are fed with different voltages, the I2C lines can be wired to 3.3 V
// Dev Environment: Arm Mbed Studio

#include "mbed.h"
#include "LCDi2c.h"

I2C i2c_bus(PB_9, PB_8);    // SDA, SCL

// LCD I2C backpack address: 7-bit base 0x27
LCDi2c lcd(PB_9, PB_8, LCD16x2, 0x27);

int main() {
    lcd.display(BACKLIGHT_ON);

    // Initial message
    lcd.cls();
    lcd.locate(0, 0);
    lcd.printf("Hello, Mbed!");
    lcd.locate(0, 1);
    lcd.printf("I2C LCD OK");

    while (true) {
        ThisThread::sleep_for(1000ms);
    }
}
