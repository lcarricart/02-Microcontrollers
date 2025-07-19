/*******************************************************************************************************************
 * Objective of the program: A Shift Register integrated circuit is used to send serial data (through the SPI,
 Synchronous Peripheral Interface) and transform them into parallel signals. The LCD can receive both serial and
 parallel signals but the latest require simpler wiring. Parallel signals involve transmitting multiple bits 
 simultaneously on separate wires.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student at HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "mbed.h"

#define ENABLE         0x08 // Set the enable (E) bit, which tells the LCD to block the data.
#define COMMAND_MODE   0x00 // RS = 0 (COMMAND_MODE) means the byte sent is a command.
#define DATA_MODE      0x04 // RS = 1 (DATA_MODE) means the byte is data (a character to display).

DigitalOut CS(D10); // Chip Select pin to enable or disable communication.
SPI ser_port(D11, D12, D13); // Initialise SPI, using default settings. D11 typically used for MOSI (Master Out, Slave In). D12 for MISO, tho not used. D13 Clock signal (SCLK).

// Function Prototypes
void clr_lcd(void);
void init_lcd(void);
void print_lcd(const char *string);
void shift_out(int data);
void write_cmd(int cmd);
void write_data(char c);
void write_4bit(int data, int mode);

//----------- MAIN function ---------------//
int main() 
{
    CS = 1;
    
    init_lcd();   // Initialise the LCD
    clr_lcd();    // Clear the LCD

    print_lcd(" Hello world!");
    write_cmd(0xc0); // Set cursor to second line
    wait_us(40);
    print_lcd(" Testing");

    while (1) // Idle in permanent loop (so that the screen shows this and then enters an infinite loop)
    {
        thread_sleep_for(100);
    }
}

//----------- Other functions --------------//
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
