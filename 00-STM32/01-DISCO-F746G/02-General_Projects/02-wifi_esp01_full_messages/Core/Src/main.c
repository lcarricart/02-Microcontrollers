/* USER CODE BEGIN Header */
/**
********************************************************************************************************************
 * @file        : main.c
 * @brief       : Main program body
 *******************************************************************************************************************
 * Objective    : Create a walkie-talkie application from scratch communicating two end-nodes, an STM32F4 and an
                  ESP32 via WiFi.
                  The application involves microphones and speakers using I2S, WiFi modules and OLED screens.

                  After completion, it is planned to add a central base that would act as a switch and processing
                  unit. It could incorporate digital signal processing to add effects to the audio and direct
                  audio messages to specific members of the network.
 *******************************************************************************************************************
 * Remarks      : - Developed using an STM32F401RE and CubeIDE
 *              : - WiFi module configured both via official examples and AT Commands manual
 *                https://docs.espressif.com/projects/esp-at/en/release-v2.1.0.0_esp8266/AT_Command_Examples/TCP-IP_AT_Examples.html
 *******************************************************************************************************************
 * Author       : Embedded Sunday Group
 * Members      : - Imran Rizwan
                  - Faizul Robin
                  - Jinju Ok
                  - Stephen Onyewuenyi
                  - Faiaz Nazmul
                  - Luciano Carricart
 *******************************************************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART6_UART_Init(void);
/* USER CODE BEGIN PFP */
/* Public API */
void wifi_server_config(void);
void wifi_client_config(void);
void server_connected_devices(void);
void server_receive_loop(void);

/* Private prototypes */
static void esp_flush_rx(void);
static int esp_receive_string(char *buf, int max_len, uint32_t first_timeout_ms, uint32_t interchar_timeout_ms);
static void esp_cmd(const char *cmd);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(500);   // Give time for WiFi moduel to settle down after reset

  printf("STM32 started\r\n");

  wifi_server_config();
  //wifi_client_config();

  server_connected_devices();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    server_receive_loop();
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 10;
  RCC_OscInitStruct.PLL.PLLN = 210;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  huart6.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart6.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/*
 * @brief override function that enables printf() support
 */
int _write(int file, char *ptr, int len)
{
  (void)file;
  int DataIdx;

  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    ITM_SendChar(*ptr++);
  }
  return len;
}

/*
 * @brief Set the first module as a TCP server (host the network and lets others join) via AT commands
 * TODO return a boolean upon succesful creation
 */
void wifi_server_config(void) {
  esp_cmd("AT\r\n");                                            // Checks the module is alive
  esp_cmd("ATE0\r\n");                                          // Disables echo so replies are cleaner
  esp_cmd("AT+GMR\r\n");                                        // Confirms firmware/version.
  esp_cmd("AT+CWMODE_CUR=2\r\n");                               // Makes the module a SoftAP only. Mode 2 is softAP mode.
  esp_cmd("AT+CWSAP_CUR=\"ESP_LINK\",\"12345678\",5,3\r\n");    // SSID = `ESP_LINK`, Password = `12345678`, Channel = `5`, Encryption = `3` = WPA2_PSK
  esp_cmd("AT+CWDHCP_CUR=0,1\r\n");                             // Enables DHCP on the softAP side (mode 0, enable 1).
  esp_cmd("AT+CIPMUX=1\r\n");                                   // Enables multi-connection mode. The manual says a TCP server can only be created when CIPMUX=1.
  esp_cmd("AT+CIPSERVER=1,1001\r\n");                           // Starts a TCP server on port 1001.
  esp_cmd("AT+CIFSR\r\n");                                      // Lets you inspect the local IP. On ESP8266 SoftAP, that is typically the AP-side IP you will connect to.
}

/*
 * @brief Set the second module as a TCP client (connects to the existing network) via AT commands
 * TODO return a boolean upon successful joining
 */
void wifi_client_config(void) {
  esp_cmd("AT\r\n");                                            // Checks the module is alive
  esp_cmd("ATE0\r\n");                                          // Disables echo so replies are cleaner
  esp_cmd("AT+GMR\r\n");                                        // Confirms firmware/version.
  esp_cmd("AT+CWMODE_CUR=1\r\n");                               // Puts it into station mode. Mode 1 is station mode.
  esp_cmd("AT+CWJAP_CUR=\"ESP_LINK\",\"12345678\"\r\n");        // Joins ESP A’s access point. The manual notes this command requires station mode.
  esp_cmd("AT+CIFSR\r\n");                                      // Lets you confirm the station got an IP address.
  esp_cmd("AT+CIPSTART=\"TCP\",\"192.168.4.1\",1001\r\n");      // Opens a TCP connection to the server. The manual documents AT+CIPSTART for TCP with remote IP and remote port.
}

/*
 * @brief Returns the IP and MAC of stations connected to the SoftAP (only if DHCP is enabled)
 */
void server_connected_devices(void) {
  esp_cmd("AT+CWLIF\r\n");
}

/*
 * @brief Runs a loop that prints data coming out of the UART bus (useful to read received messages).
 * TODO This may work under normal circumstances but needs to be stress tested.
 * CURRENTLY WORKING ON A CALLBACK FOR IRQ/ISR to solve the issue with messages being interrupted/halted.
 * TODO Turn a flag on once entering this function and have an interrupt turning it off once a button is pressed.
 */
void server_receive_loop(void) {
  char rx[512];
  int len;
  uint8_t ch;

  while (HAL_UART_Receive(&huart6, &ch, 1, 80)) /* dummy receive to trigger the reading */ {
    len = esp_receive_string(rx, sizeof(rx), 1000, 80);

    if (len > 0) {
        printf("Message received: %s\r\n", rx);
    }
    else if (len == 0) {
        printf("[ESP RX] <no data>\r\n");
    }
    else {
        printf("[ESP RX ERROR]\r\n");
    }

    esp_flush_rx();
  }
}

/*
 * @brief Flush pending bytes from ESP RX buffer
 */
static void esp_flush_rx(void)
{
    uint8_t ch;

    while (HAL_UART_Receive(&huart6, &ch, 1, 10) == HAL_OK)
    {
        /* discard pending data */
    }
}

/*
 * @brief Receive a full ESP response string in blocking mode.
 *
 * @param first_timeout_ms:     maximum wait for the first byte
 * @param interchar_timeout_ms: maximum gap allowed between two consecutive bytes
 *
 * @output:                     number of received characters
 *                               0  -> no data received
 *                              -1  -> invalid arguments
 */
static int esp_receive_string(char *buf, int max_len,
                              uint32_t first_timeout_ms,
                              uint32_t interchar_timeout_ms)
{
    uint8_t ch;
    int idx = 0;
    HAL_StatusTypeDef st;

    if (buf == NULL || max_len <= 1)
    {
        return -1;
    }

    memset(buf, 0, max_len);

    /* Wait for first byte */
    st = HAL_UART_Receive(&huart6, &ch, 1, first_timeout_ms);
    if (st != HAL_OK)
    {
        return 0;
    }

    buf[idx++] = (char)ch;
    buf[idx] = '\0';

    /* Read until the response finishes */
    while (idx < (max_len - 1))
    {
        st = HAL_UART_Receive(&huart6, &ch, 1, interchar_timeout_ms);

        if (st == HAL_OK)
        {
            buf[idx++] = (char)ch;
            buf[idx] = '\0';

            /* Stop early on common ESP-AT endings */
            if (strstr(buf, "OK\r\n") != NULL)    break;
            if (strstr(buf, "ERROR\r\n") != NULL) break;
            if (strstr(buf, "busy p") != NULL)        break;
            if (strstr(buf, "FAIL\r\n") != NULL)      break;
        }
        else
        {
            /* No new byte arrived within inter-character timeout */
            break;
        }
    }

    return idx;
}

/*
 * @brief Send one AT command and print the full reply
 */
static void esp_cmd(const char *cmd)
{
    char rx[512];
    int len;

    esp_flush_rx();

    printf("\r\n[ESP TX] %s", cmd);

    HAL_UART_Transmit(&huart6, (uint8_t *)cmd, strlen(cmd), 1000);

    len = esp_receive_string(rx, sizeof(rx), 1000, 80);

    if (len > 0)
    {
        printf("[ESP RX] %s\r\n", rx);
    }
    else if (len == 0)
    {
        printf("[ESP RX] <no data>\r\n");
    }
    else
    {
        printf("[ESP RX ERROR]\r\n");
    }
}

/*
 * @brief UART IRQ and ISR
 */
// HAL manage the peripheral details and then respond in the callback.


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
