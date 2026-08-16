/*------------------------------------------------------------------------------------------------------------------
 * Objective: CAN Receiver application using the Legacy RxFIFO feature of the FlexCAN driver. The project is part of
 * a node to node system, and the CAN Sender project can be found in FRDM-MCXN947/ running Zephyr.
 *
 * Future stages of this project involve
 * 		- Testing a DelayMs function using a PIT instance
 * 		- Interrupts integration using the AUTOSAR Platform driver.
 -------------------------------------------------------------------------------------------------------------------
 * Remarks: I spent a long time configuring the toolchain for this project, and the SDK for this board expects a
 * project configured for NXP GCC 10.2, extension that isn't default and needs to be installed.
 -------------------------------------------------------------------------------------------------------------------
 * Conclusions:
 -------------------------------------------------------------------------------------------------------------------
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student, HAW Hamburg, Germany.
 ------------------------------------------------------------------------------------------------------------------*/

/**
*   @file main.c
*   @brief Application entry-point
*/

#include "OsIf.h"
#include "Mcu.h"
#include "Port.h"
#include "Dio.h"
#include "Gpt.h"

/* Prototypes */
void init(void);
void DelayMs(uint32_t delayMs);

int main(void)
{
	init();

	DelayMs(5000);

	Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, STD_LOW);
	DelayMs(500);
	Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, STD_HIGH);

	DelayMs(5000);
	Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, STD_LOW);
	DelayMs(500);
	Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, STD_HIGH);
}

/* Initialization of core microcontroller drivers (Mcu, Port, Dio) */
void init(void)
{
	/* Mcu driver init (may include PLL and other calls if applicable) */
	Mcu_Init(NULL_PTR);
	Mcu_InitClock(0u);

#if (MCU_NO_PLL == STD_OFF)
    while (Mcu_GetPllStatus() != MCU_PLL_LOCKED) {}
    Mcu_DistributePllClock();
#endif

	/* BaseNXP driver init */
	OsIf_Init(NULL_PTR);

	/* Port driver init */
	Port_Init(NULL_PTR);

	/* Dio driver init */
	// No call required

	/* Gpt driver init */
	/* Using PIT1_CH0 in one-shot mode is the simplest for the DelayMs UC */
	Gpt_Init(NULL_PTR);
}

/*------------------------------------------------------------------------------------------------------------------
 * Aux Functions
 *------------------------------------------------------------------------------------------------------------------*/
void DelayMs(uint32_t delayMs)
{
#define GPT_CHANNEL_TICK_FREQUENCY (30000000) /* Defined in GUI but not as a variable */
#define DELAY_GPT_CHANNEL (0u) /* PIT1 CH0 */

	Gpt_ValueType timeoutTicks = ((uint64_t)delayMs * GPT_CHANNEL_TICK_FREQUENCY) / 1000ULL;
	Gpt_StartTimer(DELAY_GPT_CHANNEL, timeoutTicks);

	/* Poll until it time-outs (in one-shot mode it won't restart) */
	while (Gpt_GetTimeRemaining(DELAY_GPT_CHANNEL) != 0U)
	{

	}
}
