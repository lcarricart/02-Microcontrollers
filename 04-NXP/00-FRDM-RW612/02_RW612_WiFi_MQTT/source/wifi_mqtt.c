/**
  ******************************************************************************
  * @file           : wifi_mqtt.c
  * @brief          : Main program body
  ******************************************************************************
  * @description
  * Example code provided by NXP's board SDK to connect to an MQTT broker via
  * WiFi, sending subscribing to a topic, sending messages to it, and receiving
  * them yourself too.
  ******************************************************************************
  * @remarks
  *     - This requires Internet connectivity (configurable fields to connect to
  *       a WiFi network).
  *     - The example is programmed to connect to the open test.mosquitto.org
  *       broker that is apparently always running online and to which we can
  *       send things.
  *     - This broker can be changed by modifying the values
  *         - EXAMPLE_MQTT_SERVER_HOST
  *         - EXAMPLE_MQTT_SERVER_PORT
  ******************************************************************************
  */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "lwip/tcpip.h"
#include "board.h"
#include "app.h"
#include "wpl.h"
#include "timers.h"

#include "fsl_debug_console.h"
#include "mqtt_freertos.h"

#include <stdio.h>

#include "FreeRTOS.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef AP_SSID
#define AP_SSID "Baum"
#endif

#ifndef AP_PASSWORD
#define AP_PASSWORD "56146064529930441283"
#endif

#define WIFI_NETWORK_LABEL "Lara_WiFi"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/* Link lost callback */
static void LinkStatusChangeCallback(bool linkState)
{
    if (linkState == false)
    {
        /* -------- LINK LOST -------- */
        /* DO SOMETHING */
        PRINTF("-------- LINK LOST --------\r\n");
    }
    else
    {
        /* -------- LINK REESTABLISHED -------- */
        /* DO SOMETHING */
        PRINTF("-------- LINK REESTABLISHED --------\r\n");
    }
}

/* Connect to the external AP */
static void ConnectTo()
{
    int32_t result;

    /* Add Wi-Fi network */
    result = WPL_AddNetwork(AP_SSID, AP_PASSWORD, WIFI_NETWORK_LABEL);
    if (result == WPLRET_SUCCESS)
    {
        PRINTF("Connecting as client to ssid: %s with password %s\r\n", AP_SSID, AP_PASSWORD);
        result = WPL_Join(WIFI_NETWORK_LABEL);
    }

    if (result != WPLRET_SUCCESS)
    {
        PRINTF("[!] Cannot connect to Wi-Fi\r\n[!]ssid: %s\r\n[!]passphrase: %s\r\n", AP_SSID, AP_PASSWORD);

        while (1)
        {
            __BKPT(0);
        }
    }
    else
    {
        PRINTF("[i] Connected to Wi-Fi\r\nssid: %s\r\n[!]passphrase: %s\r\n", AP_SSID, AP_PASSWORD);
        char ip[16];
        WPL_GetIP(ip, 1);
    }
}

/*!
 * @brief The main task function
 */
static void main_task(void *arg)
{
    uint32_t result = 0;

    PRINTF("\r\n************************************************\r\n");
    PRINTF(" MQTT client example\r\n");
    PRINTF("************************************************\r\n");

    /* Initialize Wi-Fi board */
    PRINTF("[i] Initializing Wi-Fi connection... \r\n");

    result = WPL_Init();
    if (result != WPLRET_SUCCESS)
    {
        PRINTF("[!] WPL Init failed: %d\r\n", (uint32_t)result);
        __BKPT(0);
    }

    result = WPL_Start(LinkStatusChangeCallback);
    if (result != WPLRET_SUCCESS)
    {
        PRINTF("[!] WPL Start failed %d\r\n", (uint32_t)result);
        __BKPT(0);
    }

    PRINTF("[i] Successfully initialized Wi-Fi module\r\n");

    ConnectTo();
    /// wait_dns

    mqtt_freertos_run_thread(netif_default);

    vTaskDelete(NULL);
}

/*!
 * @brief Main function.
 */
int main(void)
{
    /* Initialize the hardware */
    BOARD_InitHardware();

    /* Create the main Task */
    if (xTaskCreate(main_task, "main_task", 1024, NULL, configMAX_PRIORITIES - 4, NULL) != pdPASS)
    {
        PRINTF("[!] MAIN Task creation failed!\r\n");
        while (1)
            ;
    }

    /* Run RTOS */
    vTaskStartScheduler();

    /* Should not reach this statement */
    for (;;)
        ;
}
