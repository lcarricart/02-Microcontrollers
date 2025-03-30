/*******************************************************************************************************************
 * Objective of the program: Using the SPI-Communication project to display data in an LCD, we´ll proceed to familiarize
 with RTOS functionalities --such as the use of Threads.

 The project will feature a Counter that will increment every second and display in the LCD; simultaneously, a LED
 controlled through another thread will blink right after the counter is incremented. Another line of the LCD will be
 controlled by a third thread that will be triggered by the pressing of a button (it is important that the writings
 in the LCD are not overlapped in time, therefore a MUTEX must be incorporated). The button should be actually
 coded as an interrupt handler but for educational purposes I just keep it as another Thread so far (this adds
 complexity on how the button pressing needs to be read).
 *******************************************************************************************************************
 * Theory: In many RTOS-based systems, threads are written as infinite loops because they wait for events, messages,
 or timer expirations, which keeps them ready to act whenever needed without the overhead of constantly creating and
 destroying threads. If your task only needs to run occasionally, you can use event-triggered threads, or one-shot
 tasks.

 + Minimum sleep period for a thread would be around 1ms; why? based on how many RTOS schedulers, including Mbed OS,
 work. They typically operate on a tick-based system where the system tick (the smallest unit of time the scheduler
 can measure) is often set to 1 ms by default. When you call something like thread_sleep_for(1), you're requesting
 the thread to sleep for one tick period, which is why I mentioned 1 ms as the minimum sleep duration. In other words,
 even if you request a sleep shorter than one tick period, the scheduler will round it up to the duration of one tick,
 making 1 ms the effective lower bound on sleep time.
 *******************************************************************************************************************
 * Sources of error: depending on the Mbed OS and CMSIS inclusion files, the Wait For Interrupt function could be
 defined uppercase or lowercase [__WFI() or __wfi()].
 *******************************************************************************************************************
 * Points of interest: removing the MUTEX in both threads does not make the LCD collide as far as my experiment proves.
 However, the MUTEX ensures proper functioning under multiple circumstances, like fast pacing conditions, multiple
 processing units, or even different hardware can make the MUTEX to be strictly necessary.

 + Even though some specific pins of the board are PWM-enabled, through Mbed OS software we can define other pins as
 PwmOut or PwmIn too, despite of its hardware default configuration.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student at HAW Hamburg, Germany.
 * Profile: https://www.linchar_counteredin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "mbed.h"

 // Pre-definitions
#define ENABLE         0x08
#define COMMAND_MODE   0x00
#define DATA_MODE      0x04

// Instead of a plain DigitalIn, use an InterruptIn (on D3) so that you can detect when the button is pressed. This way, you won’t need to poll its state continuously.
InterruptIn userButton(PC_13, PullUp); // PullUp add-on avoid me including a resistor to my circuit.

DigitalOut led(LED1);
DigitalOut CS(D10);
SPI ser_port(D11, D12, D13);

/* New variables for the button counter and debouncing.
 + The Timer is used to measure the elapsed time between button presses for debouncing purposes. It doesn't "count
 seconds automatically" in the sense of a clock—it simply measures how much time has passed. The precision of a Timer
 depends on the underlying hardware; many microcontrollers offer microsecond resolution. While the RTOS system tick
 might be around 1 ms, the Timer itself can often provide finer granularity because it’s tied directly to a hardware
 counter running off the CPU clock, which is much faster than 1 ms per cycle.
*/
Timer debounce_timer;
char button_counter = 0; // global

// Prototypes
void clr_lcd(void);
void init_lcd(void);
void print_lcd(const char* string);
void shift_out(int data);
void write_cmd(int cmd);
void write_data(char c);
void write_4bit(int data, int mode);

// Extra task, LED brightness (three-level-control) through a potentiometer
PwmOut LED_var(D2); // The PWM defines different brigthness for values between 0.0 and 1.0.
AnalogIn pot1(A0); // The key point is that both the potentiometer reading and the PWM output use normalized values between 0.0 and 1.0.

/*-------------------------------------------Threads--------------------------------------------------------*/
Thread thread1; // Blinchar_counter an LED
void led1_thread(void const* args)
{
    led = 0;
    while (true)
    {
        //led = !led;
        thread_sleep_for(500);
    }
}

/* Mutex to protect LCD access from concurrent threads.
Semaphores are commonly used for allocating shared resources to applications. When a shared resource can only service one
client or application processor, we call it Mutual Exclusion (MUTEX).
*/
Mutex lcd_mutex;

Thread thread2; // Display a counter on the LCD
void count_thread(void const* args)
{
    char char_counter = 0; // Our counter is a character because that´s the best format for the LCD representation.
    while (true)
    {
        lcd_mutex.lock();
        write_cmd(0xc0);
        wait_us(40);
        print_lcd("Time count: ");
        write_data(char_counter | 0x30); // ASCII codes for the digits '0' to '9' start at 0x30 (48 in decimal)
        lcd_mutex.unlock();

        char_counter++;

        if (char_counter > 9) char_counter = 0;
        thread_sleep_for(1000);
    }
}

// Button polling thread function
Thread buttonThread;
void buttonThreadFunction() {
    // Read initial state (should be high due to PullUp)
    bool lastState = userButton.read();
    while (true) {
        bool currentState = userButton.read();
        // Detect a falling edge (transition from high to low)
        if (lastState && !currentState) {
            // Toggle LED to indicate button press
            led = !led;

            // Protect LCD access with the mutex
            lcd_mutex.lock();
            write_cmd(0x80);    // Set cursor to the beginning of the first line (0x80)
            wait_us(40);
            print_lcd("Button count: ");
            write_data(button_counter | 0x30); // ASCII codes for the digits '0' to '9' start at 0x30 (48 in decimal)
            lcd_mutex.unlock();

            button_counter++;

            if (button_counter > 9) button_counter = 0;
            // Simple debounce delay (adjust as necessary)
            thread_sleep_for(50);
        }
        lastState = currentState;
        thread_sleep_for(50);
    }
}

// Brights a LED in three different intensity levels depending on the position of the potentiometer.
Thread thread4;
void breadboardled_thread(void const* args)
{
    while (1)
    {
        if (pot1 > 0.66)
        {
            LED_var = 1;
        }
        else if (pot1 > 0.33)
        {
            LED_var = 0.33;
        }
        else
        {
            LED_var = 0.0;
        }
    }
}

/*----------------------------------------Main Function-----------------------------------------------------*/
int main()
{
    init_lcd();
    clr_lcd();

    debounce_timer.start();  // Start the timer for debounce measurement.

    // thread.start(callback(task, argument)) is the standard for calling threads
    thread1.start(callback(led1_thread, &led));
    thread2.start(callback(count_thread, &ser_port));
    buttonThread.start(callback(buttonThreadFunction));
    thread4.start(callback(breadboardled_thread, &pot1));

    /* The idle thread (running __WFI()) puts the CPU in low-power mode whenever no threads are ready to run. So, even
    if a task sleeps for a short time (like 1–100 ms), the CPU is in low-power mode during that interval (it halts its
    functioning until another interrupt occurs, saving energy).
    */
    while (true)
    {
        __WFI(); // Wait for (timer) interrupt
    }
}

/*----------------------------------------LCD Functions-----------------------------------------------------*/
void init_lcd(void) // Desginated procedure in the datasheet
{
    thread_sleep_for(40);
    shift_out(0x30);
    wait_us(37);
    write_cmd(0x20);
    wait_us(37);
    write_cmd(0x20);
    wait_us(37);
    write_cmd(0x0C);
    wait_us(37);
    write_cmd(0x01);
    wait_us(1520);
    write_cmd(0x06);
    wait_us(37);
    write_cmd(0x28);
    wait_us(37);
}

void write_4bit(int data, int mode) // Mode in RS line, cmd=0, data=1
{
    int hi_n, lo_n;

    hi_n = (data & 0xF0);
    lo_n = ((data << 4) & 0xF0);

    shift_out(hi_n | ENABLE | mode);
    wait_us(1);
    shift_out(hi_n & ~ENABLE);

    shift_out(lo_n | ENABLE | mode);
    wait_us(1);
    shift_out(lo_n & ~ENABLE);
}

void shift_out(int data) // Send word to SPI port
{
    CS = 0;
    ser_port.write(data);
    CS = 1;
}

void write_cmd(int cmd) // Configures LCD command word
{
    write_4bit(cmd, COMMAND_MODE);
}

void write_data(char c)  // Configures LCD data word
{
    write_4bit(c, DATA_MODE);
}

void clr_lcd(void) // Clears display
{
    write_cmd(0x01);
    wait_us(1520); // Required time
}

void print_lcd(const char* string) // Sends characted string to LCD
{
    while (*string)
    {
        write_data(*string++);
        wait_us(40);
    }
}
