/*******************************************************************************************************************
 * Objective of the program: Practice I2C (Inter Integrated Circuit) communication connecting a Remote 8-bit I/O
 expander (only I2C compatible component at home) to control 4 LEDS.
 *******************************************************************************************************************
 * Theory: An I/O expander works by selecting pins as addresses instead of names like D1, D2, D3. The expander is
 8 bits so it controls 8 pins. 00000001 would mean the first pin.
 + Using the Arduino pins of the STM32, thee A4 and A5 are the standard I2C pins. Even though they might seem like
 analog pins, they´re also prepared for I2C purpose. If using the other pins, the PB_8 and PB_9 should be used.
 + Lower resistor values increase the overall power consumption on the bus, which is typically unnecessary for I2C
 communication. Therefore an ideal pul-up resistor is from 4k - 10k.
 + The I/O expander is configured to drain current, not to provide it. Careful with the connection of the LEDs. This
 is the standard I2C configuration.
 *******************************************************************************************************************
 Source of mistake: I´ve tried for hours to make this work with my only I/O expander available at home and the issue
 was assuming that the default I2C address was defined by the PCF8574T integrated circuit itself. On the Addressing
 section of the datasheet it was seen that the slave had an address 0100 A2 A1 A0 (0x20), but in reality the
 implementation in the board I was using was changing the default address to 0x27. (Datasheet --> Functional
 Description --> Addressing --> slave address).
 How does 0100 A2 A1 A0 = 0x20? A2 A1 and A0 are all tied to ground making them all 0. As a result we have 0100 000.
 Lets transform this into hexadecimal. Adjust it first, 0 0100 000, meaning 0010 0000. Then easily we get 0x20.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student at HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "mbed.h"

 // Instantiate the I2C interface using your board's dedicated I2C pins.
I2C i2c(PB_9, PB_8);

// PCF8574T 7-bit I2C address (assuming A0, A1, A2 are all low).
// Mbed's I2C write function expects an 8-bit address. We shift left by one to move between addresses.
const int PCF8574T_ADDR = 0x27 << 1; // 0x20 becomes 0x40 when shifted (next address --> next pin).

int main() {
    // Set the I2C frequency to 100 kHz. We define because the default might differ depending on the board or its configuration.
    i2c.frequency(100000);

    // This loop will light up one LED at a time on the first four pins (P0 to P3).
    // Wiring assumption: LED connected from VCC (with resistor) to the expander pin.
    // When the pin is driven LOW, the LED turns ON.
    while (true) {
        for (int i = 0; i < 2; i++) {
            // Start with all outputs high (LEDs off, high inactive is the standard)
            // The variable pattern is used as a bitmask (or flag variable) where each bit represents the state of one of the 8 I/O pins on the PCF8574T.
            char pattern = 0xFF;
            // "1" turns 00000001, both expressions are equal. We shift one bit at a time turning on each LED. Pattern is the flag.
            pattern &= ~(1 << i);
            // Write the pattern via I2C to the PCF8574T.
            i2c.write(PCF8574T_ADDR, &pattern, 1);
            // Wait 1 second before moving to the next LED.
            ThisThread::sleep_for(1s);
        }
    }
}
