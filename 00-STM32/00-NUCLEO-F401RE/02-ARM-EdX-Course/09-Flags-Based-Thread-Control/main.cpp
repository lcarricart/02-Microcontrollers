/*****************************************************************************************************************
 * Objective of the program: Selects and plays one of 8 tunes, with name displayed on LCD. The pattern selection is
 simple, using three buttons to control the bits of the "choose" pattern. This mechanism is achieved through the use
 of the object "choose" declared as BusIn, representing multi-bit logic levels. Each button adds one bit to the
 pattern of possibilities, 000 being the default state, and 001 song number one.

 The "4bit_LCD.cpp" file is created reusing the SPI-Communication project, and example songs are taken to create the
 "tunes.h" file.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student at HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "mbed.h"
#include "tunes.h"
#include "4bit_LCD.h"

PwmOut speaker(D3);         // for piezo sounder
BusIn choose(D4, D5, D6);     // for select buttons
// I´m replacing the two following LEDs by messages in the LCD.
// DigitalOut playing_led(D);   // Led indicator
// DigitalOut ready_led(D);     // Led indicator
AnalogIn volume(A0);          // for potentiometer
InterruptIn play(D7);         // for Play button

Mutex lcd_mutex;

// These flags indicate state of player
bool welcome = 1;   // Indicates welcome state
bool waiting = 0;   // waiting for tune selection
bool triggered = 0;    // tune is selected but hasn't started yet
bool playing = 0;   // tune is playing

void LCD_cont(void);
void Tune_select(void);
void Play_tune(void);

//-------------- Threads ----------------//

Thread thread1;   // Displays welcome message on LCD
void LCD_cont() {
    init_lcd();  // initialise the LCD

    // Waiting for song
    while (1) {
        if (welcome) {
            lcd_mutex.lock();
            clr_lcd();             // Clear the LCD
            print_lcd("Select a song:");
            write_cmd(0xc0);
            print_lcd("Then press Play");
            write_cmd(0xD4);
            print_lcd("Status: Ready");
            lcd_mutex.unlock();
            thread_sleep_for(500);
            welcome = 0;
        } // end of if
    } // end of while(1)
}

Thread thread2;   // Reads the select buttons and readies the song
void Tune_select(void const* args) {
    while (1) {
        if (triggered) { // "triggered" is set by Interrupt
            switch (choose) { // read song selection and load song pointer
            case 0x00: song_ptr = &Oranges; break;
            case 0x01: song_ptr = &Cielito; break;
            case 0x02: song_ptr = &Malaika; break;
            case 0x03: song_ptr = &Guten_Abend; break;
            case 0x04: song_ptr = &Yankee; break;
            case 0x05: song_ptr = &Rasa; break;
            case 0x06: song_ptr = &Matilda; break;
            case 0x07: song_ptr = &Alouetta; break;
            } // end of switch

            lcd_mutex.lock();
            clr_lcd();             // Clear the LCD
            print_lcd("Now playing:");
            write_cmd(0xc0);
            print_lcd(song_ptr->name); // display song name
            write_cmd(0xD4);
            print_lcd("Status: Playing!!");
            lcd_mutex.unlock();

            triggered = 0;
            playing = 1;
        } // end of if
    } // end of while(1)
}

Thread thread3;   // plays the chosen tune
void Play_tune(void const* args) {
    while (1) {
        if (playing) {
            // resume PWM operation, stopped after last song
            speaker.resume();
            for (int i = 0; i <= (song_ptr->length); i++) {
                speaker.period(1 / (2 * (*song_ptr).freq[i])); // set PWM period
                speaker = volume; // set duty cycle, hence volume control
                thread_sleep_for(200 * (*song_ptr).beat[i]); // hold for beat period
            }
            speaker = 0; // To shut the sound, volume = 0;

            // indicate end of song
            lcd_mutex.lock();
            write_cmd(0xD4);
            print_lcd("Status: Waiting...");
            lcd_mutex.unlock();
            thread_sleep_for(1000);

            playing = false;
            welcome = 1;
            speaker.suspend();
        } // end of if
    } // end of while(1)
}

void ISR() { // responds to press of Play button, and sets "triggered"
    if (playing == 0) // only set "triggered" if song not playing
        triggered = 1;
}

//--------------- Main ------------------//
int main() {

    play.fall(&ISR); // Int. when Play is pressed, ie on falling edge

    // Launch the threads
    thread1.start(callback(LCD_cont));
    thread2.start(callback(Tune_select, &choose));
    thread3.start(callback(Play_tune, &play));

    while (1) {
        __WFI(); // wait for interrupt
    }
}
