/*******************************************************************************************************************
 * Objective:  An ultrasonic sensor acts as a measurement device, reflecting the distance to an object in the LCD
 * screen.
 *******************************************************************************************************************
 * Context: Developed using Mbed Studio. Demonstration prepared for the Electrical Engineering I tutorship.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student, HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "mbed.h"
#include "TextLCD_I2C.h"      // from pilotak/mbed-text-display

// NUCLEO-F401RE I2C on Arduino header:
//   D14 = SDA, D15 = SCL
#define LCD_SDA D14
#define LCD_SCL D15

// Ultrasonic sensor pins (change to match your wiring)
#define ULTRASONIC_TRIG D7
#define ULTRASONIC_ECHO D8

// false = standard backpack pinout, true = alternative
TextLCD_I2C lcd(LCD_SDA, LCD_SCL, false);

DigitalOut led(LED1);
DigitalOut trig(ULTRASONIC_TRIG);
DigitalIn  echo(ULTRASONIC_ECHO);
Timer      echo_timer;

// Measure distance in cm, return negative value on timeout/error
float measure_distance_cm()
{
    // Ensure trigger is low
    trig = 0;
    ThisThread::sleep_for(1ms);

    // Send 10 us pulse on TRIG
    trig = 1;
    wait_us(10);
    trig = 0;

    // Wait for ECHO to go high (start of pulse), with timeout
    int timeout_us = 30000;   // 30 ms ~ ~5 m range
    while (!echo) {
        if (--timeout_us <= 0) {
            return -1.0f;    // no echo
        }
        wait_us(1);
    }

    // Measure how long ECHO stays high
    echo_timer.reset();
    echo_timer.start();

    timeout_us = 30000;
    while (echo) {
        if (--timeout_us <= 0) {
            echo_timer.stop();
            return -1.0f;    // pulse too long / out of range
        }
        wait_us(1);
    }
    echo_timer.stop();

    // echo_timer.elapsed_time() is chrono::microseconds
    auto duration = echo_timer.elapsed_time();
    float us = duration.count();  // pulse length in microseconds

    // Distance in cm:
    // speed of sound ≈ 343 m/s
    // distance = (time * speed_of_sound) / 2
    // ≈ us * 0.01715
    float distance_cm = us * 0.01715f;
    return distance_cm;
}

int main()
{
    // Give the LCD controller some time after power-up
    ThisThread::sleep_for(50ms);

    lcd.init();
    lcd.display(TextLCD_I2C::DISPLAY_ON);
    lcd.setBacklight(true);

    while (true) {
        float distance = measure_distance_cm();

        lcd.cls();
        lcd.locate(0, 0);
        lcd.printf("Ultrasonic App!");

        lcd.locate(0, 1);

        if (distance >= 0.0f) {
            lcd.printf("Dist: %5.1f cm", distance);
        }

        led = !led;
        ThisThread::sleep_for(500ms);
    }
}
