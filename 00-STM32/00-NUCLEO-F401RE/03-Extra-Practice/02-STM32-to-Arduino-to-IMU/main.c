/*******************************************************************************************************************
 * Objective of the program: I have two problems 
    1) I need a magnetometer, and the only one I have is integrated in an Arduino Nano BLE 33.
    2) I do not have a data cable to load a program in my Arduino board.
   
   Solution: I have adapted the program "01-IMU-Acc-Gyro" to read the sensor through my STM board. My main STM board
   communicates with the Arduino through I2C to talk directly with magnetometer.
 *******************************************************************************************************************
 * Remarks: Even though both devices are fed with different voltages, the I2C lines can be wired to 3.3 V.
 *******************************************************************************************************************
 * Development Environment: Arm Mbed Studio
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student at HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "mbed.h"
#include "LCDi2c.h"

I2C i2c_bus(PB_9, PB_8);    // SDA, SCL

// MPU6050 I2C address (AD0=0): 8-bit base 0x68
const int MPU_ADDR = 0x68 << 1;

// LCD I2C backpack address: 7-bit base 0x27
LCDi2c lcd(PB_9, PB_8, LCD16x2, 0x27);

// Prototypes
bool mpu_write_reg(uint8_t, uint8_t);
bool mpu_read_regs(uint8_t, uint8_t*, int);
bool mpu_read_int16(uint8_t, int16_t&);
bool mpu_initialize();


int main() {
    lcd.display(BACKLIGHT_ON);

    // Initial message
    lcd.cls();
    lcd.locate(0, 0);
    lcd.printf("MPU6050 init...");
    // Initialize MPU6050
    if (!mpu_initialize()) {
        lcd.locate(0, 1);
        lcd.printf("MPU init FAIL");
        while (true) {
            ThisThread::sleep_for(500ms);
        }
    }
    // Initialization OK
    lcd.locate(0, 1);
    lcd.printf("MPU OK");
    thread_sleep_for(1000);
    lcd.cls();

    // Main loop: read and display
    while (true) {
        int16_t ax = 0, ay = 0, az = 0;
        int16_t gx = 0, gy = 0, gz = 0;
        bool ok = true;
        ok &= mpu_read_int16(0x3B, ax);
        ok &= mpu_read_int16(0x3D, ay);
        ok &= mpu_read_int16(0x3F, az);
        ok &= mpu_read_int16(0x43, gx);
        ok &= mpu_read_int16(0x45, gy);
        ok &= mpu_read_int16(0x47, gz);

        if (!ok) {
            // I2C read error
            lcd.cls();
            lcd.locate(0, 0);
            lcd.printf("MPU read ERR");
            ThisThread::sleep_for(500ms);
            continue;
        }

        // Displays raw values.
        // Line 1: Ax, Ay
        lcd.locate(0, 0);
        lcd.printf("Ax:%2d", ax);
        lcd.locate(9, 0);
        lcd.printf("Ay:%2d", ay);

        // Line 2: Gx, Gy
        lcd.locate(0, 1);
        lcd.printf("Gx:%2d", gx);
        lcd.locate(9, 1);
        lcd.printf("Gy:%2d", gy);

        ThisThread::sleep_for(500ms);
    }
}

// Helper: write one byte to MPU register
bool mpu_write_reg(uint8_t reg, uint8_t value) {
    char buf[2] = { (char)reg, (char)value };
    int ack = i2c_bus.write(MPU_ADDR, buf, 2);
    return (ack == 0);
}

// Helper: read N bytes from MPU starting at reg; returns true on success
bool mpu_read_regs(uint8_t reg, uint8_t* data, int length) {
    // Repeated start: write register address, then read
    int ack = i2c_bus.write(MPU_ADDR, (char*)&reg, 1, true);
    if (ack != 0) return false;
    ack = i2c_bus.read(MPU_ADDR, (char*)data, length);
    return (ack == 0);
}

// Read a signed 16-bit from two consecutive registers
bool mpu_read_int16(uint8_t reg_high, int16_t& out) {
    uint8_t buf[2];
    if (!mpu_read_regs(reg_high, buf, 2)) return false;
    out = (int16_t)((buf[0] << 8) | buf[1]);
    return true;
}

// Initialize MPU6050: wake-up and optionally verify WHO_AM_I
bool mpu_initialize() {
    // Wake up: write 0 to PWR_MGMT_1 (0x6B)
    bool ok = mpu_write_reg(0x6B, 0x00);
    if (!ok) return false;
    // Optionally check WHO_AM_I (0x75 should read 0x68)
    uint8_t who = 0;
    if (!mpu_read_regs(0x75, &who, 1)) return false;
    if (who != 0x68) {
        // Unexpected ID
        return false;
    }
    // Optionally set full-scale ranges; default is ±2g for accel, ±250°/s for gyro.
    // E.g., write 0x00 to ACCEL_CONFIG (0x1C) and GYRO_CONFIG (0x1B) for default ranges.
    // For now, keep defaults.
    return true;
}
