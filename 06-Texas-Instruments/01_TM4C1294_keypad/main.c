/** Default Project for TM4C1294ncpdt
 *
 * File:    main.c
 * Author:
 * Date:
 * Version:
 *
 * Blinks the onboard LED using a busy-wait loop as delay
 */

#include "inc/tm4c1294ncpdt.h"
#include <stdint.h>
#include <stdio.h>

const char symbol[4][4] = {
        {'1', '2', '3', 'F'},
        {'4', '5', '6', 'E'},
        {'7', '8', '9', 'D'},
        {'A', '0', 'B', 'C'}
    };

/*******************************************************************************************/

void wait(int ticks) {
    for (int i = 0; i < ticks; i++);   
}

/*******************************************************************************************/

void configure_led() {
    SYSCTL_RCGCGPIO_R       |= (1 << 9);  // switch on clock for Port K
    SYSCTL_RCGCGPIO_R       |= (1 << 10);  // switch on clock for Port L
    
    /* TODO: set a while loop until the clock ready register is set */
    wait(2000);

    GPIO_PORTK_DEN_R        |= (0x7 << 0);   // digital I/O enable pin PK1
    GPIO_PORTK_DIR_R        |= 0x07;         // set PortK 1 Output
    GPIO_PORTK_DATA_R        = 0x00;         // set PortK Output to zero

    GPIO_PORTL_DEN_R        |= (0x3 << 0);   // digital I/O enable pin PL1
    GPIO_PORTL_DIR_R        |= 0x00;         // set PortL 1 Input
    GPIO_PORTL_DATA_R        = 0x00;         // set PortL Output to zero
}



/*******************************************************************************************/

/*******************************************************************************************/

int main(void) {

    // Configure LED 1
    configure_led();
    wait(2000);

    printf("\n...\r\n");
    printf("Program started\r\n");
    wait(2000);

    GPIO_PORTL_DATA_R = 0x00;

    while(1) {
        for(int i = 0; i < 3; i++) {
            /* Traversing this lines is done correctly */
            GPIO_PORTK_DATA_R = (0x01 << i);
            wait(2000);

            for(int k = 0; k < 2; k++) {
                if((GPIO_PORTL_DATA_R & (1 << k)) == 0) {
                    //if(GPIO_PORTK_DATA_R & (1 << i) == 1) {
                    printf("Key pressed: %c \r\n", symbol[k][i]);
                    wait(2000);
                    //}
                }
            }
        }
    }
}


// switch (GPIO_PORTK_DATA_R) {
//     case (1 << 0):
//         wait(2000);
//         printf("\nKey pressed: %c", symbol[k][0]);
//     break;

//     case (1 << 1):
//         wait(2000);
//         printf("\nKey pressed: %c", symbol[k][1]);
//     break;

//     case (1 << 2):
//         wait(2000);
//         printf("\nKey pressed: %c", symbol[k][2]);
//     break;

//     case (1 << 3):
//         wait(2000);
//         printf("\nKey pressed: %c", symbol[k][3]);
//     break;

//     default:
//     break;
// }