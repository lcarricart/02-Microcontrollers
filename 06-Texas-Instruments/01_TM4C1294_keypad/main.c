/*******************************************************************************************
 * File:    main.c
 * Target:  TM4C1294ncpdt
 * Author:  Luciano Carricart
 *******************************************************************************************
 * @brief
 * Recognize the button pressed in a keypad via polling
 *******************************************************************************************
 * Learning Outcomes
 *  - The best debugging was using the multimeter. The LEDs of my setup were increasing the
      voltage seen in the line (kinda negative drop) and instead of 1.5V I was geting 2.5V.
      Once removing the LEDs, the line dropped to 1.5V, seen as a digital 0. I dont think
      I should have voltage at all when pressing but maybe my homemade hardware setup wasn't
      perfect. In the lab we were overengineering it when a multimeter could have been enough.
 *******************************************************************************************
 */

#include "inc/tm4c1294ncpdt.h"
#include <stdint.h>
#include <stdio.h>

const char symbol[2][3] = {
        {'A', 'B', 'C'},
        {'D', 'E', 'F'}
    };

/*******************************************************************************************/

void wait(int ticks) {
    for (int i = 0; i < ticks; i++);   
}

/*******************************************************************************************/

void configure_pins() {
    /* Clock the ports */
    SYSCTL_RCGCGPIO_R       |= (1 << 9);    // Port K
    SYSCTL_RCGCGPIO_R       |= (1 << 10);   // Port L
    
    /* TODO: set a while loop until the clock ready register is set. This is just a quick non-deterministic workaround */
    wait(2000);

    /* Enable, Direction, and Data registers for each port */
    /* Pins K0, K1 and K2 */
    GPIO_PORTK_DEN_R        |= 0x07;         // digital I/O enable pin PK1
    GPIO_PORTK_DIR_R        |= 0x07;         // set PortK Output
    GPIO_PORTK_DATA_R        = 0x00;         // Initialize to zero

    /* Pins L0 and L1 */
    GPIO_PORTL_DEN_R        |= 0x03;         // digital I/O enable pin PL1
    GPIO_PORTL_DIR_R        |= 0x00;         // set PortL Input
    GPIO_PORTL_DATA_R        = 0x00;         // Initialize the values
}

/*******************************************************************************************/

/*******************************************************************************************/

int main(void) {
    // Initialize ports and pins
    configure_pins();
    wait(2000);

    printf("\nProgram started!\r\n");
    wait(2000);

    while(1) {
        for(int i = 0; i < 3; i++) {
            /* High volzage = 1, therefore we need to set 0s one at a time to GND the line */
            GPIO_PORTK_DATA_R = ~(1 << i); /* This is a bit innacurate because it sets all the bits in the register, not only the used ones */
            wait(2000);

            for(int k = 0; k < 2; k++) {
                if((GPIO_PORTL_DATA_R & (1 << k)) == 0) {
                    printf("Key pressed: %c \r\n", symbol[k][i]);
                    wait(2000);
                }
            }
        }
    }
}