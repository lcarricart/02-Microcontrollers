/*------------------------------------------------------------------------------------------------------------------
 * Objective: CAN Receiver application using the Legacy RxFIFO feature of the FlexCAN driver. The project is part of
 * a node to node system, and the CAN Sender project can be found in FRDM-MCXN947/ running Zephyr.
 *
 * Future stages of this project involve
 * 		- Interrupts integration using the AUTOSAR Platform driver.
 -------------------------------------------------------------------------------------------------------------------
 * Remarks: both boards include termination resistors in their CAN interfaces. Awesome!
 *
 * Possible configurations:
 * (In order to change between them, both ConfigTools and macros need to be adjusted)
 * 		- CAN_LOOPBACK_SIMPLE: internal loopback mode with regular buffer
 * 		- CAN_LOOPBACK_FIFO: internal loopback mode with legacy rxFIFO
 * 		- CAN_SIMPLE_RECEIVER: normal mode using with regular buffer (only receive)
 * 		- CAN_FIFO_RECEIVER: normal mode with legacy rxFIFO (only receive)
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

#include "assert.h"

/*------------------------------------------------------------------------------------------------------------------
 * Prototypes
 *------------------------------------------------------------------------------------------------------------------*/
void initialization(void);
void DelayMs(uint32_t delayMs);
void can_init(void);

#ifdef CAN_LOOPBACK_SIMPLE
void can_init_loopback_simple(void);
void can_loopback_simple(void);
#endif

#ifdef CAN_LOOPBACK_FIFO
void can_init_loopback_fifo(void);
void can_loopback_fifo(void);
#endif

#ifdef CAN_SIMPLE_RECEIVER
void can_init_simple_receiver(void);
void can_simple_receiver(void);
#endif

#ifdef CAN_FIFO_RECEIVER
void can_init_fifo_receiver(void);
void can_fifo_receiver(void);
#endif

/*------------------------------------------------------------------------------------------------------------------
 * Variables
 *------------------------------------------------------------------------------------------------------------------*/
uint8_t led_green_state = STD_HIGH;
uint8_t led_red_state = STD_HIGH;

/*------------------------------------------------------------------------------------------------------------------
 * CAN Variables
 *------------------------------------------------------------------------------------------------------------------*/
uint8 dummyData[8] = {0,1,2,3,4,5,6,7};
Flexcan_Ip_MsgBuffType rxFrame = {0};

Flexcan_Ip_StatusType can_status = 0x01U; /* E_NOT_OK */
uint8_t init = 0, config = 0, start = 0;
uint8_t tx_errors = 0, rx_errors = 0;
uint16_t received_frames = 0;

Flexcan_Ip_DataInfoType tx_info =
{
	.msg_id_type = FLEXCAN_MSG_ID_STD,  // 11-bit ID
	.data_length = 8u,
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

#if defined(CAN_LOOPBACK_FIFO) || defined(CAN_FIFO_RECEIVER)
/* (not true) The manual requires exactly one table element per configured FIFO filter */
const Flexcan_Ip_IdTableType filterTable[8] =
{
    { .isExtendedFrame = FALSE, .isRemoteFrame = FALSE, .id = 0x123U },
    { .isExtendedFrame = FALSE, .isRemoteFrame = FALSE, .id = 0x7FFU },
    { .isExtendedFrame = FALSE, .isRemoteFrame = FALSE, .id = 0x7FFU }
};


#endif

/*------------------------------------------------------------------------------------------------------------------
 * Functions
 *------------------------------------------------------------------------------------------------------------------*/
int main(void)
{
	initialization();

	Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, STD_LOW);
	DelayMs(500);
	Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, STD_HIGH);

	DelayMs(5000);
	Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, STD_LOW);
	DelayMs(500);
	Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, STD_HIGH);

	while(1)
	{
#ifdef CAN_LOOPBACK_SIMPLE
		can_loopback_simple();
#endif
#ifdef CAN_LOOPBACK_FIFO
		can_loopback_fifo();
#endif
#ifdef CAN_SIMPLE_RECEIVER
		can_simple_receiver();
#endif
#ifdef CAN_FIFO_RECEIVER
		can_fifo_receiver();
#endif
	}
}

/* Initialization of core microcontroller drivers (Mcu, Port, Dio) */
void initialization(void)
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
	/* I know from experience that checking the value of these variables in runtims is unreliable most of the times. Best is checking if an if is entered or not */
	can_status = FlexCAN_Ip_Init(INST_FLEXCAN_0, &FlexCAN_State0, &FlexCAN_Config0);
	if(can_status != FLEXCAN_STATUS_SUCCESS) {
		init++;
	}
#ifdef CAN_LOOPBACK_SIMPLE
#define RX_MB   (0U)
#define TX_MB   (1U)
	can_init_loopback_simple();
#endif

#ifdef CAN_LOOPBACK_FIFO
#define RX_MB   (0U)

/* Because of rxFIFO filter numbers; if 8 configure, 0-7 are used, and minimum accessible is 8
 * It is important to have the max number of MB also correct. If set to 8 and rxFIFO has also 8 filter IDs, then not sufficient */
#define TX_MB   (8U)
	can_init_loopback_fifo();
#endif

#ifdef CAN_SIMPLE_RECEIVER
#define RX_MB   (0U)
	can_init_simple_receiver();
#endif

#ifdef CAN_FIFO_RECEIVER
#define RX_MB   (0U)
	can_init_fifo_receiver();
#endif
}

#ifdef CAN_LOOPBACK_SIMPLE
void can_init_loopback_simple(void)
{
	can_status = FlexCAN_Ip_ConfigRxMb(INST_FLEXCAN_0, RX_MB, &rx_info, 0x123U); /* Acceptance mask, CAN MSG_ID configured for that RX message buffer */
	if(can_status != FLEXCAN_STATUS_SUCCESS) {
		config++;
	}
	can_status = FlexCAN_Ip_SetStartMode(INST_FLEXCAN_0);
	if(can_status != FLEXCAN_STATUS_SUCCESS)
	{
		start++;
	}

	DevAssert((init + config + start) == 0);
}

void can_loopback_simple(void)
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
}
#endif

#ifdef CAN_LOOPBACK_FIFO
void can_init_loopback_fifo(void)
{
	can_status = FlexCAN_Ip_ConfigRxFifo(INST_FLEXCAN_0, FLEXCAN_RX_FIFO_ID_FORMAT_A /* standard */, filterTable);
	if(can_status != FLEXCAN_STATUS_SUCCESS)
	{
		config++;
	}

	can_status = FlexCAN_Ip_SetStartMode(INST_FLEXCAN_0);
	if(can_status != FLEXCAN_STATUS_SUCCESS)
	{
		start++;
	}

	DevAssert((init + config + start) == 0);
}

void can_loopback_fifo(void)
{
    rxStatus = FlexCAN_Ip_RxFifo(INST_FLEXCAN_0, &rxFrame);
    if (rxStatus != FLEXCAN_STATUS_SUCCESS)
    {
        rx_errors++;
    }

    txStatus = FlexCAN_Ip_SendBlocking(INST_FLEXCAN_0, TX_MB, &tx_info, 0x123U, dummyData, 100U);
    if (txStatus != FLEXCAN_STATUS_SUCCESS)
    {
        tx_errors++;
    }

    do
    {
        FlexCAN_Ip_MainFunctionRead(INST_FLEXCAN_0, 0U);
        rxStatus = FlexCAN_Ip_GetTransferStatus(INST_FLEXCAN_0, 0U);
    } while (rxStatus == FLEXCAN_STATUS_BUSY);
}
#endif

#ifdef CAN_SIMPLE_RECEIVER
void can_init_simple_receiver(void)
{
	/* Configuration of the MessageBuffer receive ID for that RX MB */
	can_status = FlexCAN_Ip_ConfigRxMb(INST_FLEXCAN_0, RX_MB, &rx_info, 0x123U);
	if(can_status != FLEXCAN_STATUS_SUCCESS) {
		config++;
	}
	can_status = FlexCAN_Ip_SetStartMode(INST_FLEXCAN_0);
	if(can_status != FLEXCAN_STATUS_SUCCESS)
	{
		start++;
	}

	DevAssert((init + config + start) == 0);
}

void can_simple_receiver(void)
{
	/* Arm the reception to message buffer RX_MB */
	rxStatus = FlexCAN_Ip_Receive(INST_FLEXCAN_0, RX_MB, &rxFrame, TRUE);
	if (rxStatus != FLEXCAN_STATUS_SUCCESS)
	{
		rx_errors++;

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

		if (rxStatus == FLEXCAN_STATUS_SUCCESS)
		{
			received_frames++;
			led_green_state ^= 0x01; /* toggle LED */
			Dio_WriteChannel(DioConf_DioChannel_dio_rgb_green, led_green_state);
		}
	} while (rxStatus == FLEXCAN_STATUS_BUSY);
}
#endif

#ifdef CAN_FIFO_RECEIVER
void can_init_fifo_receiver(void)
{
	can_status = FlexCAN_Ip_ConfigRxFifo(INST_FLEXCAN_0, FLEXCAN_RX_FIFO_ID_FORMAT_A /* standard */, filterTable);
	if(can_status != FLEXCAN_STATUS_SUCCESS)
	{
		config++;
	}

	can_status = FlexCAN_Ip_SetStartMode(INST_FLEXCAN_0);
	if(can_status != FLEXCAN_STATUS_SUCCESS)
	{
		start++;
	}

	DevAssert((init + config + start) == 0);
}

void can_fifo_receiver(void)
{
	rxStatus = FlexCAN_Ip_RxFifo(INST_FLEXCAN_0, &rxFrame);
	if ((rxStatus != FLEXCAN_STATUS_SUCCESS) && (rxStatus != FLEXCAN_STATUS_BUSY))
	{
		rx_errors++;
		led_red_state ^= 0x01; /* toggle LED */
		Dio_WriteChannel(DioConf_DioChannel_dio_rgb_red, led_red_state);
	}

	FlexCAN_Ip_MainFunctionRead(INST_FLEXCAN_0, 0U);
	rxStatus = FlexCAN_Ip_GetTransferStatus(INST_FLEXCAN_0, 0U);

	if (rxStatus == FLEXCAN_STATUS_SUCCESS)
	{
		received_frames++;
		led_green_state ^= 0x01; /* toggle LED */
		Dio_WriteChannel(DioConf_DioChannel_dio_rgb_green, led_green_state);
	}
}
#endif
/*------------------------------------------------------------------------------------------------------------------
 * Aux Functions
 *------------------------------------------------------------------------------------------------------------------*/
void DelayMs(uint32_t delayMs)
{
#define GPT_CHANNEL_TICK_FREQUENCY (30000000) /* Defined in GUI but not as a variable (tho I think I can instruct ConfigTools to create variables of these (via "Preferences") */
#define DELAY_GPT_CHANNEL (0u) /* PIT1 CH0 */

	Gpt_ValueType timeoutTicks = ((uint64_t)delayMs * GPT_CHANNEL_TICK_FREQUENCY) / 1000ULL;
	Gpt_StartTimer(DELAY_GPT_CHANNEL, timeoutTicks);

	/* Poll until it time-outs (in one-shot mode it won't restart) */
	while (Gpt_GetTimeRemaining(DELAY_GPT_CHANNEL) != 0U)
	{

	}
}
