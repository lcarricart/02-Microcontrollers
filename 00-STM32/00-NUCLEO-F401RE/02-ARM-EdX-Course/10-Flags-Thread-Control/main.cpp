/*******************************************************************************************************************
 * Objective of the program: Using "Music-Player" project as the base, I modified the code to select songs through
 an Arrows & OK mechanism, giving the user the posibility to browse through songs instead of having to press complex
 button combinations. The program works underl flags-based thread synchronization.
 *******************************************************************************************************************
 Learning outcome: I realize that RTOS programming isn't just like any other program flow. There are different
 strategies to control the flow safely, and the one imnplemented here is flag-based (or state-variable) thread
 synchronization (use of flag variables at the end of each thread).
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student at HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "mbed.h"
#include "tunes.h"
#include "4bit_LCD.h"

PwmOut speaker(D3);         // for piezo sounder
AnalogIn volume(A0);          // for potentiometer
InterruptIn go(D7);           // button: move to the next menu
InterruptIn arrow_down(D6);   // button: arrow down
InterruptIn ok(D5);           // button: OK, select song
InterruptIn arrow_up(D4);     // button: arrow up

Mutex lcd_mutex;

// These flags indicate state of player
bool welcome = 1;   // Indicates welcome state
bool waiting = 0;   // waiting for tune selection
bool triggered = 0;    // tune is selected but hasn't started yet
bool playing = 0;   // tune is playing

// Song selection mechanism
int cursor = 1;
int ok_button = 0;
int next_menu = 0;

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
            print_lcd("Your MUSIC Player!");
            write_cmd(0xc0);
            print_lcd("Press GO to continue");
            write_cmd(0xD4);
            print_lcd("Status: Ready");
            lcd_mutex.unlock();
            thread_sleep_for(500);
            welcome = 0;
        } // end of if
    } // end of while(1)
}

Thread thread_songs_menu;
void Tune_menu()
{
    while (1)
    {
        // Shows the song selection menu if next_menu is high (triggered by interrupts)
        if (next_menu == 1)
        {
            lcd_mutex.lock();
            clr_lcd();
            print_lcd("Select a song:");

            switch (cursor)
            {
            case 1: song_ptr = &Oranges; break;
            case 2: song_ptr = &Cielito; break;
            case 3: song_ptr = &Malaika; break;
            case 4: song_ptr = &Guten_Abend; break;
            case 5: song_ptr = &Yankee; break;
            case 6: song_ptr = &Rasa; break;
            case 7: song_ptr = &Matilda; break;
            case 8: song_ptr = &Alouetta; break;
            case 9: song_ptr = &Twinkle; break;
            }

            write_cmd(0xc0);
            print_lcd(song_ptr->name); // display song name
            write_cmd(0xD4);
            print_lcd("Status: Choosing...");
            lcd_mutex.unlock();
            thread_sleep_for(500);

            next_menu = 0;  // Turn off song selection menu variable to avoid infinite displays of the menu.
        }
    }
}

Thread thread2;   // Reads the select buttons and readies the song
void Tune_select() {
    while (1) {
        if (ok_button) { // "triggered" is set by Interrupt
            switch (cursor) { // read song selection and load song pointer
            case 1: song_ptr = &Oranges; break;
            case 2: song_ptr = &Cielito; break;
            case 3: song_ptr = &Malaika; break;
            case 4: song_ptr = &Guten_Abend; break;
            case 5: song_ptr = &Yankee; break;
            case 6: song_ptr = &Rasa; break;
            case 7: song_ptr = &Matilda; break;
            case 8: song_ptr = &Alouetta; break;
            case 9: song_ptr = &Twinkle; break;
            } // end of switch

            lcd_mutex.lock();
            clr_lcd();             // Clear the LCD
            print_lcd("Now playing:");
            write_cmd(0xc0);
            print_lcd(song_ptr->name); // display song name
            write_cmd(0xD4);
            print_lcd("Status: Playing!!");
            lcd_mutex.unlock();

            ok_button = 0;
            playing = 1;
        } // end of if
    } // end of while(1)
}

Thread thread3;   // plays the chosen tune
void Play_tune() {
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

/*-------------- Handlers ---------------*/
void go_handler() { // responds to press of Play button, and sets "triggered"
    if (playing == 0) // only set "triggered" if song not playing
        next_menu = 1;
}

void down_handler()
{
    if (playing == 0)
    {
        cursor--;
        next_menu = 1;      // To show the song selection menu again
        if (cursor < 1) cursor = 9;
    }
}

void up_handler()
{
    if (playing == 0)
    {
        cursor++;
        next_menu = 1;      // To show the song selection menu again
        if (cursor > 9) cursor = 1;
    }
}

void ok_handler()
{
    if (playing == 0) ok_button = 1;
}

//--------------- Main ------------------//
int main() {

    // Interrupts
    go.fall(&go_handler);        // Int. when Play is pressed, ie on falling edge
    arrow_down.fall(&down_handler);
    ok.fall(&ok_handler);
    arrow_up.fall(&up_handler);

    // Launch the threads
    thread1.start(callback(LCD_cont));
    thread_songs_menu.start(callback(Tune_menu));
    thread2.start(callback(Tune_select));
    thread3.start(callback(Play_tune));

    while (1) {
        __WFI(); // wait for interrupt
    }
}
