/**
 * Objective				: Blink an LED
 *******************************************************************************************************************
 * Remarks					: - MCUXpresso IDE
 *******************************************************************************************************************
 * Conclusions				: - In order to get Driver documentation for a specific board, refer to the following
 * 								link: https://mcuxpresso.nxp.com/mcuxsdk/latest/html/api/devices/RW612/index.html#gpio-general-purpose-i-o
 * 							  - The flashing process was extremely faulty, but at the end the process improved by
 * 							    pressing the RESET button after flashing. Not clear yet why the J-Link was failing.
 *******************************************************************************************************************
 * Author					: Luciano Carricart
 * Status					: Information Engineering student, HAW Hamburg, Germany.
 * Profile					: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************
*/

/*
 * Copyright 2016-2026 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    01_RW612_Blinky.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */
int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    /* Define the init structure for the output LED pin*/
	gpio_pin_config_t led_config = {
		kGPIO_DigitalOutput,
		0,
	};

    GPIO_PortInit(GPIO, 0);
    GPIO_PinInit(GPIO, 0, 1U, &led_config);
    /* Pin initialization done in pin_mux.c */

    while(1) {
    	/* Lighting up the red LED, N2 in the MCU grid, but not Port N, Pin 2.
    	 * Instead the right information can be derived from the Pin ConfigTools */
    	GPIO_PinWrite(GPIO /*suggested pointer from source code*/, 0 /*port*/, 1 /*pin*/, 0U /*active low*/);
    	SDK_DelayAtLeastUs(500000U, CLOCK_GetCoreSysClkFreq());
    	GPIO_PinWrite(GPIO, 0, 1, 1U);
    	SDK_DelayAtLeastUs(500000U, CLOCK_GetCoreSysClkFreq());

    	/* Blue LED */
//    	GPIO_PinWrite(GPIO, 0, 0, 1U);
    }
    return 0 ;
}
