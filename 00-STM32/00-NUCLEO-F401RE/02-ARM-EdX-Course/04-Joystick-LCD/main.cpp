/*******************************************************************************************************************
 * Objective of the program: Implementing the program used for the 02-SPI-Communication project I have added a
 joystick to read its X and Y axis values in the LCD display. The new implementation related to the joystick is 
 marked as **New Code, and it involves three parts.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student at HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "mbed.h"

#define ENABLE         0x08 
#define COMMAND_MODE   0x00 
#define DATA_MODE      0x04 

DigitalOut CS(D10);
SPI ser_port(D11, D12, D13);

// Old proptotypes
void clr_lcd(void);
void init_lcd(void);
void print_lcd(const char *string);
void shift_out(int data);
void write_cmd(int cmd);
void write_data(char c);
void write_4bit(int data, int mode);

// **New code. Part 1/3
AnalogIn joystickX(A0);
AnalogIn joystickY(A1);

int main() {
    while (true) 
    {
        CS = 1;
    
        init_lcd(); 
        clr_lcd();  

        /*  **New Code. Part 2/3
        Read the analog values; these are normalized from (0.1 to 1) * 100. The variables need to be multiplied because in many Mbed configurations, the default 
        minimal printf implementation does not support floating point formatting (i.e., using %f). By multiplying the normalized float (ranging from 0.0 to 1.0) by
        100 and storing it as an integer, you avoid the floating point formatting issue and can print the values using an integer specifier (%u).

        If you want to print them as floats (without converting to an integer), you would need to enable full floating point support in printf by adjusting your Mbed 
        configuration (for example, by setting "platform.stdio-minimal": false in your mbed_app.json)
        */
        int8_t x = joystickX.read() * 100;
        int8_t y = joystickY.read() * 100;

        // **New Code. Part 3/3. Print the joystick coordinates to the LCD
        write_cmd(0xc0); // Set cursor to second line (0x00 + 0x08)
        print_lcd("  Position: X = ");

        /* Now we have a problem because we wanna pass an integer but our function is built for strings. I can overload the functions (but they´re a chain of them),
        or rather transform my integer into a string. I´m chosing the latter.*/
        char buffer[4]; // This array will hold the value converted into a string.
        sprintf(buffer, "%d", x); // This function is versatile and can convert various data types to their string representations. First argument is where it´ll be stored.
        print_lcd(buffer);

        write_cmd(0x94); // Set cursor to third line (0xc0 + 0x08)
        wait_us(40);
        print_lcd("  Position: Y = ");
        sprintf(buffer, "%d", y);
        print_lcd(buffer);

        ThisThread::sleep_for(500ms);
    }
}

//----------- Old functions --------------//
void init_lcd(void) // Follow designated procedure in data sheet
{
    thread_sleep_for(40);
    shift_out(0x30); // Function set 8-bit
    wait_us(37); // This delays are part of the procedure but are included to ensure that each instruction has enough time to be completed.
    write_cmd(0x20); // Function set
    wait_us(37);
    write_cmd(0x20); // Function set
    wait_us(37);
    write_cmd(0x0C); // Display ON/OFF
    wait_us(37);
    write_cmd(0x01); // Display clear
    wait_us(1520);
    write_cmd(0x06); // Entry-mode set
    wait_us(37);
    write_cmd(0x28); // Function set
    wait_us(37);
}

void write_4bit(int data, int mode) // Mode is RS line, 0 for cmd, 1 for data
{
    int hi_n, lo_n;
    
    hi_n = (data & 0xF0);             // Form the high 4-bit nibble (F0 = 11110000, meaning upper half)
    lo_n = ((data << 4) & 0xF0);      // Form the low 4-bit nibble (F0 "shifted four times, 00001111, lower half")
    
    /* Many LCDs can operate in 4-bit mode, meaning you send each 8-bit command/data in two parts: */
    // Send high nibble with strobe
    shift_out(hi_n | ENABLE | mode);
    wait_us(1);
    shift_out(hi_n & ~ENABLE);
    
    // Send low nibble with strobe
    shift_out(lo_n | ENABLE | mode);
    wait_us(1);
    shift_out(lo_n & ~ENABLE);
}

void shift_out(int data) // Uses SPI to shift out data
{
    CS = 0; // Pulls the chip select low to start communication.
    ser_port.write(data); // Sends the byte via SPI.
    CS = 1; // Sets the chip select high again to finish communication.
}

// Sends a command byte using command mode.
void write_cmd(int cmd) 
{
    write_4bit(cmd, COMMAND_MODE);
}

// Sends a character to be displayed using data mode.
void write_data(char c) 
{
    write_4bit(c, DATA_MODE);
}

void clr_lcd(void) 
{
    write_cmd(0x01); // Display clear
    wait_us(1520);
}

void print_lcd(const char *string) 
{
    while (*string) // The null terminator (\0) at the end of the string has an ASCII value of 0, which is a logical false
    {
        write_data(*string++);
        wait_us(40);
    }
}