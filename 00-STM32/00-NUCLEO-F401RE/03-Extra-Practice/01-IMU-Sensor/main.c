// Purpose of the program: Display the data read by the inertial measurement unit (IMU) in a LCD.

#include "mbed.h"
#include "LCDi2c.h"

/*  D14 = PB_9 = SDA   |   D15 = PB_8 = SCL
 *  0x27 my IP scan showed 0x4E/0x4F (8-bit R/W), so the 7-bit base is 0x27
 */
LCDi2c lcd(PB_9, PB_8, LCD16x2, 0x27);   // (sda, scl, type, 7-bit addr)

int main() {
    lcd.display(BACKLIGHT_ON);

    lcd.locate(0, 0);          // column 0, row 0 (line 1)
    lcd.printf("Hello, Mbed!");

    /* second line */
    lcd.locate(0, 1);          // column 0, row 1 (line 2)
    lcd.printf("I2C LCD OK");

    while (true) {
        ThisThread::sleep_for(1s);
    }
}
