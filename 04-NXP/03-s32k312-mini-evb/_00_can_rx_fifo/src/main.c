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
#include "FlexCAN_Ip.h"

/*------------------------------------------------------------------------------------------------------------------
 * Prototypes
 *------------------------------------------------------------------------------------------------------------------*/
void init(void);
void DelayMs(uint32_t delayMs);
void can_init(void);
void can_loopback(void);

/*------------------------------------------------------------------------------------------------------------------
 * Variables
 *------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------
 * CAN Variables
 *------------------------------------------------------------------------------------------------------------------*/
uint8 dummyData[8] = {0,2,3,4,5,6,7};
Flexcan_Ip_MsgBuffType rxFrame = {0};
uint8_t tx_errors, rx_errors = 0;

Flexcan_Ip_DataInfoType tx_info =
{
	.msg_id_type = FLEXCAN_MSG_ID_STD,  // 11-bit ID
	.data_length = 8u,                  // DLC=7
	.fd_enable   = FALSE,               // classic CAN
	.fd_padding  = FALSE,
	.enable_brs  = FALSE,
	.is_polling  = TRUE,                // works well with *SendBlocking*
	.is_remote   = FALSE                // data frame (not RTR)
};

Flexcan_Ip_DataInfoType rx_info =
{
	.msg_id_type = FLEXCAN_MSG_ID_STD,
	.data_length = 8u,
	.is_polling = TRUE,
	.is_remote = FALSE
};

Flexcan_Ip_StatusType rxStatus;
Flexcan_Ip_StatusType txStatus;

/*------------------------------------------------------------------------------------------------------------------
 * Functions
 *------------------------------------------------------------------------------------------------------------------*/
int main(void)
{
	init();

	Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, STD_LOW);
	DelayMs(500);
	Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, STD_HIGH);

	DelayMs(5000);
	Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, STD_LOW);
	DelayMs(500);
	Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, STD_HIGH);

	while(1)
	{
#ifdef CAN_LOOPBACK
		can_loopback();
#endif
	}
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

	/* FlexCAN driver init */
	can_init();
}

void can_init(void)
{
#define RX_MB   (0U)
#define TX_MB   (1U)
	Flexcan_Ip_StatusType can_status = 0x01U; /* E_NOT_OK */
	uint8_t init = 0, config = 0, start = 0;

	/* I know from experience that checking the value of these variables in runtims is unreliable most of the times. Best is checking if an if is entered or not */

	can_status = FlexCAN_Ip_Init(INST_FLEXCAN_0, &FlexCAN_State0, &FlexCAN_Config0);
	if(can_status != FLEXCAN_STATUS_SUCCESS) {
		init++;
	}
#ifdef CAN_LOOPBACK
	can_status = FlexCAN_Ip_ConfigRxMb(INST_FLEXCAN_0, RX_MB, &rx_info, 0x123U); /* Acceptance mask, CAN MSG_ID configured for that RX message buffer */
	if(can_status != FLEXCAN_STATUS_SUCCESS) {
		config++;
	}
	can_status = FlexCAN_Ip_SetStartMode(INST_FLEXCAN_0);
	if(can_status != FLEXCAN_STATUS_SUCCESS)
	{
		start++;
	}

	/* My best attempt to not get these important variables optimized out */
	dummyData[0] = init + config + start;

	/* Do not continue the program if any failed */
#endif
}

void can_loopback(void)
{
	/* Arm the reception to message buffer RX_MB */
	rxStatus = FlexCAN_Ip_Receive(INST_FLEXCAN_0, RX_MB, &rxFrame, TRUE /* polling */);
	if (rxStatus != FLEXCAN_STATUS_SUCCESS)
	{
		rx_errors++;

		DelayMs(500);
		Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, STD_LOW);
		DelayMs(500);
		Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, STD_HIGH);
	}

	/* Transmit from message buffer TX_MB */
	txStatus = FlexCAN_Ip_SendBlocking(INST_FLEXCAN_0, TX_MB, &tx_info, 0x123U, dummyData, 100u);
	if (txStatus != FLEXCAN_STATUS_SUCCESS) {
		tx_errors++;

		DelayMs(500);
		Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, STD_LOW);
		DelayMs(500);
		Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, STD_HIGH);
	}

	/* Poll RX until reception completes */
    do
    {
        FlexCAN_Ip_MainFunctionRead(INST_FLEXCAN_0, RX_MB);
        rxStatus = FlexCAN_Ip_GetTransferStatus(INST_FLEXCAN_0, RX_MB);
    } while (rxStatus == FLEXCAN_STATUS_BUSY);
    if (rxStatus != FLEXCAN_STATUS_SUCCESS) {
    	rx_errors++;

    	DelayMs(500);
		Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, STD_LOW);
		DelayMs(500);
		Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, STD_HIGH);
    }
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
