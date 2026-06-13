/*******************************************************************************************
 * File:    main.c
 * Target:  TM4C1294ncpdt
 * Author:  Luciano Carricart
 *******************************************************************************************
 * @brief
 * Solve the previous exercise where we write the drivers to operate an ultrasonic sensor and
 * an LED pendulum, but implemented using interrupts instead of polling or busy-waiting.
 *******************************************************************************************
 * @remarks
 * Interrupt driven design: the while(1) polls for flags but in a special way. It checks the
 * important flags (lets say 5 of them), and if no flag is active it goes to sleep mode. The
 * controller only awakes when an interrupt occurs. The interrupt triggers an ISR, which
 * then sets a flag, and since the controller woke up, the while(1) runs again, but this time
 * entering the part of the code where I do the heavy lifting, clearing the flag. At the end,
 * if no other interrupt --> flag ocurred, the controller goes back to sleep mode.
 * 
 * The sleep function is the classical CMSIS __WFI(), Wait For Interrupt!
 * 
 * The interrupts configuration can be found inside configure_timers_and_interrupts()
 *******************************************************************************************
 * @pinout
 *  - PD1 echo
 *  - PD5 trigger
 *  - PM0-5 LEDs
 *******************************************************************************************
 */

#include "inc/tm4c1294ncpdt.h"
//#include "core_cm4.h" /* Include the build path ${ProjDir}/C:/ti/ccs2050/ccs/ccs_base/arm/include/CMSIS. This is where the __WFI definition lives */
#include <stdint.h>
#include <stdio.h>
#include <math.h>

/***************************** E N U M E R A T I O N S *************************************/
typedef enum {
    LED_OFF = 0,
    LED_ON
} led_state_t;

/******************************** P R O T O T Y P E S **************************************/
void configure_trigger_echo_pins();
void configure_timers_and_interrupts();
void configure_internal_led();
void configure_pendulum_leds();
void configure_pendulum_control();

void toggle_led(led_state_t state);
void blink();

void sleep_us(uint32_t us);
void sleep_ms(uint32_t ms);
void wait(int ticks);

void trigger_ultrasonic();
uint32_t echo_read_time();
uint32_t calculate_distance_cm(uint32_t time);

/******************************** G L O B A L S **************************************/
uint8_t isr_counter = 0;
uint32_t first_edge;
uint32_t second_edge;
uint32_t captured_time;

uint8_t time_available_flag;
uint8_t first_time_flag = 0;
/***************************** E X T R A   F U N C T I O N S *******************************/

/**
 * @brief non-deterministic wait function
 * @param ticks this is not ticks strictly speaking, since each for() iteration takes way
 *              longer than just one tick (depends on the amount of instructions).
 */
void wait(int ticks) {
    for (int i = 0; i < ticks; i++);   
}

/********************** I N I T I A L   C O N F I G U R A T I O N S ************************/

/**
 * @brief TIM1A (periodic) and TIM0B (capture) peripheral configuration
 *          - Activate timer clock          (SYSCTL_RCGCTIMER_R)
 *          - Wait for stabilization        (SYSCTL_PRTIMER_R)
 *          - Stop timer                    (TIMERx_CTL_R)
 *          - Configure amount of bits      (TIMERx_CFG_R)
 *          - Operation mode                (TIMERx_TnMR_R)
 *          - Prescaler                     (TIMERx_TnPR_R)
 *          - Interval load value           (TIMERx_TnILR_R)
 *          - Match value                   (TIMERx_TnMATCHR_R)
 *          - Start timer                   (TIMERx_CTL_R)
 *
 * 
 * Interrupts implementation:
 * Timer reigsters
 *          - Interrupts Mask register (which peripheral matters to you)
 *          - Raw Interrupt status (a flag when an interrupts happens, masked or not masked)
 *          - Masked interrupt status register (same but only for the enabled ones)
 *          - Interrupt clear register (where you write to clear it, W1C)
 *
 * NVIC (buit-in inside the Arm core, always ready to operate; it does not need initialization)
 *          - Enable the timer's interrupt
 *          - Set priority level
 *
 * ISR
 *          - Enable interrupts in your desired peripheral
 *          - Enable the desired IRQ in the NVIC (there's a register for it)
 *          - Change the vector table to include the address of the ISR
 */
void configure_timers_and_interrupts() {
    // TIM1A (periodic), used by the sleep function
    SYSCTL_RCGCTIMER_R  |= (1 << 1);    // Enable TIM1
    while (!(SYSCTL_PRTIMER_R & (1 << 1)));
    TIMER1_CFG_R    = 0b000;            // Select 32 bits (unifies TIM A and B)
    TIMER1_CTL_R    &= ~(1 << 0);       // Stop timer
    TIMER1_TAMR_R   |= (1 << 1);        // TIM1A periodic mode
    TIMER1_TAMR_R   &= ~(1 << 4);       // TIM1A count down
    TIMER1_TAMR_R   |= (1 << 5);        // TIM1A match enable interrupt
    TIMER1_ICR_R    = (1 << 4);         // Clear match flag (initialization, this will happen again later)
    /* The following value is kept constant for the purpose of the application (only change match value) */
    TIMER1_TAPR_R   = 0x00000000;       // Prescaler value = 1 - 1

    // TIM0B (capture), used for measuring echo time
    SYSCTL_RCGCTIMER_R  |= (1 << 0);    // Enable TIM0
    while (!(SYSCTL_PRTIMER_R & (1 << 0)));
    TIMER0_CFG_R    = 0b100;            // Select 16 bits to use only TIM B (to be done once only and actually before any changes to the CTL_R)
    TIMER0_CTL_R    &= ~(1 << 8);       // Stop timer
    TIMER0_CTL_R    |= (0b11 << 10);    // Capture rising and falling edges
    TIMER0_TBMR_R   |= (0b11);          // TIM0B capture mode
    TIMER0_TBMR_R   &= ~(1 << 4);       // Count down
    TIMER0_TBMR_R   |= (1 << 2);        // Edge-time mode (opposite to counting amount of edges)

    // Interrupts code
    TIMER0_IMR_R    |= (1 << 10);       // TIM0B Capture Mode Event Interrupt Mask
    TIMER0_ICR_R    = (1 << 10);        // TIM0B Capture Mode Event Interrupt Clear
    NVIC_EN0_R      |= (1 << 20);       // Allow NVIC to process IRQ 20 (TIM0B)
    
    TIMER0_TBILR_R = 0xFFFF;            // Load value for Timer B
    TIMER0_TBPR_R  = 0xFF;              // Optional prescaler extension (since I'm using 16 bits I want this to be as slow as possible)
    TIMER0_CTL_R  |= (1 << 8);          // Start TIM0B 

    // TIM3A (capture). Capture is the only option for capturing edge events
    SYSCTL_RCGCTIMER_R  |= (1 << 3);    // Enable TIM3
    while (!(SYSCTL_PRTIMER_R & (1 << 3)));
    TIMER3_CFG_R    = 0b000;            // Select 32 bits (unifies TIM A and B)
    TIMER3_CTL_R    &= ~(1 << 0);       // Stop timer
    TIMER3_CTL_R    |= (0b11 << 2);     // Capture rising and falling edges
    TIMER3_TAMR_R   |= (0b11);          // TIM3A capture mode
    TIMER3_TAMR_R   &= ~(1 << 4);       // TIM3A count down
    TIMER3_TAMR_R   |= (1 << 2);        // Edge-time mode (opposite to counting amount of edges)
    TIMER3_ICR_R    = (1 << 2);         // GPTM Timer A Capture Mode Event Interrupt Clear
    TIMER3_TAILR_R  = 0xFFFFFFFF;       // Load value = maximum = 2^(32) - 1. I need to do this once only, since I only wanna see events and I dont care about the count value.
    TIMER3_TAPR_R   = 0xFF;             // Optional prescaler extension (none)
    TIMER3_CTL_R    |= (1 << 0);        // Start timer (will stay on forever, I dont need it to stop, just capture edges)
}

/**
 * @brief Pin configuration
 *          - Clock a port                  (RCGCGPIO_R)
 *          - Wait for port to be stable    (SYSCTL_PRGPIO_R)
 *          - GPIO alternate function       (GPIO_PORTx_AFSEL_R)
 *          - GPIO control register         (GPIO_PORTx_PCTL_R)
 */
void configure_trigger_echo_pins() {
    /* Configure trigger pin PD5
     * the LAB explicitly request to set the pin as a GPIO. We're making the crude implementation of this functionality.
     */
    SYSCTL_RCGCGPIO_R       |= (1 << 3);    // Clock Port D
    while (!(SYSCTL_PRGPIO_R & (1 << 3)));  // Wait for stabilization
    GPIO_PORTD_AHB_DEN_R    |= (1 << 5);    // Enable pin 5
    GPIO_PORTD_AHB_DIR_R    |= (1 << 5);    // Set output direction (=1)
    GPIO_PORTD_AHB_DATA_R    = 0x00;        // Set PortD Output to zero

    /* Configure echo pin PD1
     * This is a special one, since it needs to measure the time that an external signal goes high.
     * I need the pin to be smarter for this one, and therefore:
     *      - Trigger every signal edge 
     *      - Be configured as a stop watch (capture mode)
     *      - Can only use the alternate function to T0CCP1 timer
     */
    GPIO_PORTD_AHB_AFSEL_R  |= (1 << 1);    // Activate AFSEL for pin 1
    GPIO_PORTD_AHB_PCTL_R   |= 0x00000030;  // Use function 3 for pin 1
    GPIO_PORTD_AHB_DEN_R    |= (1 << 1);    // Enable pin 1
    GPIO_PORTD_AHB_DIR_R    &= ~(1 << 1);   // Set input direction (=0)
}

/**
 * @brief led configuration from LAB1
 */
void configure_internal_led() {
    SYSCTL_RCGCGPIO_R       |= (0x1 << 12);  // Switch on clock for Port N
    while (!(SYSCTL_PRGPIO_R & 0x1000));     // Wait for clock to stabilize
    GPIO_PORTN_DEN_R        |= (0x1 << 1);   // Digital I/O enable pin PN1
    GPIO_PORTN_DIR_R        |= 0x02;         // Set PortN 1 Output
    GPIO_PORTN_DATA_R        = 0x00;         // Set PortN Output to zero
}

void toggle_led(led_state_t state) {
    if (state) { // check if led is ON
        GPIO_PORTN_DATA_R |= 0x02;  // turn LED on
    } else {
        GPIO_PORTN_DATA_R &= ~0x02; // turn LED off
    }
}

/**
 * @brief Configure GPIO outputs to drive the pendulum LEDs
 */
void configure_pendulum_leds() {
    SYSCTL_RCGCGPIO_R       |= (1 << 11);    // Clock Port M
    while (!(SYSCTL_PRGPIO_R & (1 << 11)));  // Wait for stabilization
    GPIO_PORTM_DEN_R    |= 0x3F;        // Enable pins 0,1,2,3,4,5
    GPIO_PORTM_DIR_R    |= 0x3F;        // Set output direction (=1)
    GPIO_PORTM_DATA_R    = 0x00;        // Set PortM Output to zero
}

/**
 * @brief Configure input pin to read the triggers of the pendulum. This needs to be clocked to TIM3.
 */
void configure_pendulum_control() {
    /* Port D is already clocked from before */
    GPIO_PORTD_AHB_AFSEL_R  |= (1 << 4);    // Activate AFSEL for pin 4
    GPIO_PORTD_AHB_PCTL_R   |= 0x00030000;  // Use function 3 for pin 4
    GPIO_PORTD_AHB_DEN_R    |= (1 << 4);    // Enable pin 4
    GPIO_PORTD_AHB_DIR_R    &= ~(1 << 4);    // Set input direction (=0)
}

/**
 * @brief Toggle an LED every 100 ms
 */
void blink() {
    toggle_led(LED_ON);
    sleep_ms(100);
    toggle_led(LED_OFF);
    sleep_ms(100);
}

/*******************************************************************************************/

/**
 * @brief Start a GPT for a given time of miliseconds and wait until the timer is done.
 * @param ms amount of miliseconds
 */
void sleep_ms(uint32_t ms) {
    /* I need calculations but I need to avoid floating points, specially very small values, to avoid errors => work with frequencies */
    uint32_t steps;
    uint32_t match_value;

    steps       = ms * 16000;
    match_value = 0xFFFFFFFF - steps;

    TIMER1_CTL_R        &= ~(1 << 0);   // Stop timer
    TIMER1_TAILR_R      = 0xFFFFFFFF;   // Load value = maximum = 2^(32) - 1
    TIMER1_TAMATCHR_R   = match_value;  // Match value

    TIMER1_CTL_R |= (1 << 0);           // Start timer
    while(!(TIMER1_RIS_R & (1 << 4)));  // Do nothing until match value flag is active

    TIMER1_CTL_R &= ~(1 << 0);          // Stop timer
    TIMER1_ICR_R = (1 << 4);            // Clear match flag (it should not be a |= because I don't wanna preserve the previous value if 1. Furthermore, with = I'm only deleting those registers where I write a 1, since the register is a W1C!!!)
}

/**
 * @brief Start a GPT for a given time of microseconds and wait until the timer is done.
 * @param us amount of microseconds
 */
void sleep_us(uint32_t us) {
    /* I need calculations but I need to avoid floating points, specially very small values, to avoid errors => work with frequencies */
    uint32_t steps;
    uint32_t match_value;

    steps       = us * 16;
    match_value = 0xFFFFFFFF - steps;

    TIMER1_CTL_R        &= ~(1 << 0);   // Stop timer
    TIMER1_TAILR_R      = 0xFFFFFFFF;   // Load value = maximum = 2^(32) - 1
    TIMER1_TAMATCHR_R   = match_value;  // Match value

    TIMER1_CTL_R |= (1 << 0);           // Start timer
    while(!(TIMER1_RIS_R & (1 << 4)));  // Do nothing until match value flag is active

    TIMER1_CTL_R &= ~(1 << 0);          // Stop timer
    TIMER1_ICR_R = (1 << 4);            // Clear match flag (it should not be a |= because I don't wanna preserve the previous value if 1, I wanna write)
}

/**
 * @brief Send an pulse of >10us to trigger the ultrasonic sensor
 */
void trigger_ultrasonic() {
    GPIO_PORTD_AHB_DATA_R |= (1 << 5);
    sleep_us(15);
    GPIO_PORTD_AHB_DATA_R &= ~(1 << 5);
}

/**
 * @brief Calculate the distance between object and ultrasonic sensor using the time it takes for the ultrasonic wave to return to bounce back.
 *  Steps:
 *   - Take raw timer count value and convert to time (using clock reference, prescaler, bit width and so on)
 *   - The time recorded is what it took for the signal to be sent and bounce back (twice the time of interest)
 *   - From physics, distance [m] = speed [m/s] * time [s]
 *   - For the variable speed, I'll use the speed of sound = 343 m/s
 */
uint32_t calculate_distance_cm(uint32_t timer_count) {
    uint32_t time_us;
    uint32_t half_time_us;
    uint16_t sound_speed_cm_s = 34300;
    uint32_t distance_cm;

    time_us = timer_count / 16;
    half_time_us = time_us / 2;
    distance_cm = (uint32_t)(((uint64_t)sound_speed_cm_s * half_time_us) / 1000000);

    return distance_cm;
}

/**
 * @brief Read the control output of the pendulum and detect an edge
 *  Steps:
 *   - Turn off clock
 *   - Clear flag
 *   - 
 */
void read_pendulum_control() {
    TIMER3_ICR_R    = (1 << 2);             // GPTM Timer A Capture Mode Event Interrupt Clear
    TIMER3_CTL_R    |= (1 << 0);            // Start timer (unnecessary but for debug)
    GPIO_PORTM_DATA_R = 0x3F;               // Turn on LEDs
    while(!(TIMER3_RIS_R & (1 << 2)));      // Wait for an edge
    GPIO_PORTM_DATA_R = 0x00;               // Turn off LEDs
    TIMER3_ICR_R    = (1 << 2);             // Clear flag
    TIMER3_CTL_R    &= ~(1 << 0);           // Stop timer (unnecessary but for debug)
}

/************************************ I S R *************************************************/
/**
* @brief Handler to operate the read functions. It's a sort of state machine that reacts differently
* depending on how many timmes the interrupt occurred.
* First occurrence: rise. 
* Second occurrence: fall
*/
void echo_handler(void) {
    TIMER0_ICR_R = (1 << 10);               // Clear the capture event flag

    if (first_time_flag == 0) {
        TIMER0_CTL_R  &= ~(1 << 8);         // Stop TIM0B (FIXME, timer needs to be turned on one time because otherwise the interrupt never fires)
        first_time_flag = 1;
    }

    if (isr_counter == 0) {
        TIMER0_CTL_R  |= (1 << 8);          // Start TIM0B
        first_edge = TIMER0_TBR_R;          // Read TIM0B value (result is a raw timer count value)

        isr_counter++;
    } else if (isr_counter == 1) {
        TIMER0_CTL_R  &= ~(1 << 8);         // Stop TIM0B
        second_edge = TIMER0_TBR_R;         // Read TIM0B value (result is a raw timer count value)
        
        captured_time = first_edge - second_edge;
        time_available_flag = 1;            // Flag main to calculate and log the value

        isr_counter = 0;
    }
}

/*******************************************************************************************/

int main(void) {
    configure_timers_and_interrupts();
    configure_internal_led();
    configure_trigger_echo_pins();

    configure_pendulum_leds();
    configure_pendulum_control();

    printf("Application start! \r\n");
    sleep_ms(100);

    uint32_t distance_cm = 0;
    
    while(1) {
        // blink();

        trigger_ultrasonic();

        if (time_available_flag) {
            // Use the ultrasonic sensor to read distances
            distance_cm = calculate_distance_cm(captured_time);
            printf("Distance = %lu cm \r\n", distance_cm);

            time_available_flag = 0;
        }

        sleep_ms(50);

        /* // Non-functional yet; needs a small tweak
        read_pendulum_control();
        */

        /*
        * An improvement should be done over the current implementation to only read the first_edge and second_edge values when there're actually new values.
        * Else I need to find a smarter place to restart the timers. Right now they are not restarting.
        */

        /*
        * I can work on a variation that uses the __wfi() function, letting the controller sleep until an interrupt occurs.
        * This implementation would make more sense if a button could be pressed to send the ultrasonic_trigger.
        */
    }
}
