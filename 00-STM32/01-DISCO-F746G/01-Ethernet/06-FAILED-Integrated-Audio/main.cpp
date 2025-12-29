/*******************************************************************************************************************
 * (Last alternative: don't try to use the integrated microphone, use a sensor with known drivers)
 
   This program doesn't work :(. I should switch to bare-metal / rewrite the drivers / find Mbed OS compatible mic drivers (I haven't found)
   STM32 HAL BSP expects full control of DMA, interrupts, and peripherals
   Mbed OS RTOS also manages these same resources
   The only reference code with this board is for the CubeIDE, impressively enough. I wonder if they thought this board shouldn't be usable with other OSs / IDEs.

   Objective:  Send two UDP payloads through different ports (but same Ethernet cable) containing information about
        - IMU data at a established frequency (10 Hz).
        - Audio frames at a hardware-determined frequency captured with the baord integrated mic
          (I need flags and queues w/ hardware-triggered synchronization)

   The connection is straight [MCU --> PC] via an Ethernet cable (no Internet required). LED1 ON/OFF indicates the
   state of data transmission.

   Buffering pattern: DMA Ping-pong / Ping-pong buffer
   1) Start: DMA is configured in circular mode with two targets: Ping then Pong.
   2) Fill Ping: As samples arrive, DMA fills Ping.
   3) When Ping is full, DMA triggers the Transfer-Complete (or Half-Transfer, depending on layout) interrupt/callback.
   4) Switch to Pong: Without stopping, DMA immediately starts filling Pong.
   5) Meanwhile, CPU works on Ping: Your code (in thread context) processes/sends the just-completed Ping frame while DMA is busy filling Pong.
   6) Fill Pong complete → callback: Now DMA flips back to Ping and the cycle repeats.
   7) This alternation continues forever: Ping → Pong → Ping → … — hence “ping-pong”.

 *******************************************************************************************************************
 * Context:     - This is part of a major programming project, where live telemetry is transmitted to a PC, to be later
                  manipulated and displayed with a Java GUI application.
 *              - Development environment: Arm Mbed Studio
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student, HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

 /* Including the official STM32CubeF7 repository creates dependency problems due to outdated incompatible drivers.
    Download the folder "Dependencies" included in this repository; it uses slightly modified drivers.
    Include STM32 BSP headers BEFORE mbed.h to avoid PI_1 redefinition conflict.     */
#include <Dependencies/stm32746g_discovery_audio.h>    // Enough for audio

#include "mbed.h"
#include "EthernetInterface.h"    // https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/EthernetInterface.h
    // "EthernetInterface.cpp"           https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/source/EthernetInterface.cpp
#include "UDPSocket.h"
#include "I2C.h"
#include "mbed-mpu6050/MPU6050.h" // https://github.com/ET-BE/MBED-MPU6050

// Button, LED, and IMU
InterruptIn button(BUTTON1);
DigitalOut led(LED1);
I2C i2c(I2C_SDA, I2C_SCL);
MPU6050 imu(&i2c);

// ------------------- Audio variables START -------------------
constexpr uint32_t RATE = 16000;                    // 16 kHz
constexpr uint32_t FRAME_MS = 20;                       // 20 ms/packet
constexpr uint32_t SAMPLES = RATE * FRAME_MS / 1000;   // 320
constexpr uint32_t FRAME_BYTES = SAMPLES * 2;              // 640 (s16 mono)

// DMA ping–pong buffer in DTCM RAM (non-cached, perfect for DMA)
// STM32F746 DTCM is at 0x20000000, not cached by D-Cache
__attribute__((section(".dtcm_data"))) __attribute__((aligned(32)))
static int16_t s_dma_buf[2][SAMPLES];

// Audio sending flags and variable
volatile bool    s_frame_ready = false;
volatile uint8_t s_ready_idx = 0;

// Synchronization control
static Thread audio_tx_thread(osPriorityAboveNormal, 2048, nullptr, "audio_tx");
volatile bool s_stream_on = false;
// ------------------- Audio variables END -------------------

// Network interface and socket kept alive for the whole app
EthernetInterface   net;
UDPSocket           sock_IMU, sock_audio;   // I could use only one but this keeps it cleaner
SocketAddress       pc_IMU, pc_audio;

// nsapi_error_t is an integer error-code type used by Mbed OS’s Network Socket API
// functions like net.connect() and sock.open(&net) return an nsapi_error_t value.
nsapi_error_t rc;
nsapi_size_or_error_t n;

// Global variables
volatile bool buttonPressed = false;        // volatile modifier because it is an ISR variable

// Function prototypes
void onButtonPressed_isr();
long long int getTimeStamp_ms();
void sendPayload(const char*);
void BSP_AUDIO_IN_TransferComplete_CallBack(void);
void BSP_AUDIO_IN_HalfTransfer_CallBack(void);
void BSP_AUDIO_IN_Error_CallBack(void);
void audio_tx_worker();
void audio_start();
void audio_stop();

int main() {
    printf("\nProgram start: MCU --> PC (IMU + Audio)\n");

    /* Sets a static IPv4.Gateway 0.0.0.0 means "no default route". In other words : the MCU will only talk to hosts that
       are on its own subnet; it will not try to reach anything “somewhere else” via a router. It is NOT fine when you want
       to reach anything outside your subnet. */
    net.set_network("192.168.7.2", "255.255.255.0", "0.0.0.0");

    do {
        rc = net.connect();

        if (rc != NSAPI_ERROR_OK) {
            printf("Attempt net.connect failed: %d\n", rc);
            printf("Retrying...\n");
            ThisThread::sleep_for(2s);
        }
    } while (rc != NSAPI_ERROR_OK);                     // If the return is not ERROR_OK, then it failed
    printf("net.connect=%d\n", rc);

    SocketAddress myip;
    net.get_ip_address(&myip);                          // Store IP
    printf("MCU IP: %s\n", myip.get_ip_address());      // Print IP

    do {
        rc = sock_IMU.open(&net);

        if (rc != NSAPI_ERROR_OK) {
            printf("sock.open failed: %d\n", rc);
            printf("Retrying...\n");
            ThisThread::sleep_for(2s);
        }
    } while (rc != NSAPI_ERROR_OK);

    /* PC’s static IPv4 and chosen port.For a direct cable, give the PC the static IP 192.168.7.1 on its Ethernet adapter
       and run your PowerShell listener bound to that address and port.*/
    pc_IMU = SocketAddress("192.168.7.1", 55055);
    pc_audio = SocketAddress("192.168.7.1", 55054);

    // ----- IMU Sensor: START -----
    i2c.frequency(400000); // 400 kHz
    uint8_t whoami = imu.readByte(WHO_AM_I_MPU6050);

    if (whoami != 0x68) {
        printf("Invalid device...\n\r");
        return 1; // Correct device was not found
    }

    imu.reset(); // Reset registers to default in preparation for device calibration
    imu.calibrate(); // Calibrate gyro and accelerometers, load biases in bias registers
    imu.init();

    float a[3], g[3];
    // ----- IMU Sensor: END -----

    button.fall(callback(onButtonPressed_isr));

    printf("Press the button to start recording data (press again to stop)\r\n");
    while (buttonPressed == false) { ThisThread::yield(); }     // Give processing to other tasks
    buttonPressed = false;                                      // Reset the variable value

    // Audio disabled - STM32 HAL BSP conflicts with Mbed OS RTOS
    // The HAL DMA/interrupt handling is incompatible with Mbed's threading
    printf("Audio disabled (HAL/Mbed OS conflict)\r\n");
    uint8_t audio_status = AUDIO_ERROR;

    printf("Sending payload...");
    char msg[256] = "";              // Payload
    snprintf(msg, sizeof(msg), "timestamp, accX, accY, accZ, gyroX, gyroY, gyroZ, temperature\r\n");
    sendPayload(msg);
    ThisThread::sleep_for(10ms);     // Necessary to make the first payload show up
    sendPayload("");                 // Necessary to make the first payload show up
    long long int offset = getTimeStamp_ms();
    led = 1;                         // Indicates start of transmission

    while (buttonPressed == false) {
        // IMU: Read at 10 Hz
        if (imu.readByte(INT_STATUS) & 0x01)    // check if data ready interrupt
        {
            imu.readAccelData(a);    // Read the x/y/z acceleration
            imu.readGyroData(g);     // Read the x/y/z gyroscope values    
            float temp = imu.readTempData();

            // In order to enable this floating point representation, one needs to modify the "mbed_app.json" file 
            snprintf(msg, sizeof(msg), "%lld, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.1f\r\n", getTimeStamp_ms() - offset, a[0], a[1], a[2], g[0], g[1], g[2], temp);
            sendPayload(msg);
        }

        ThisThread::sleep_for(100ms); // 10 Hz 
    }
    led = 0;                          // Indicates end of transmission
}

void onButtonPressed_isr() {
    buttonPressed = true;
}

// Obtain the current timestamps in miliseconds
long long int getTimeStamp_ms() {
    // Grabs a time_point
    Kernel::Clock::time_point tp = Kernel::Clock::now();

    // Converts to a duration since epoch (clock’s zero reference point)
    Kernel::Clock::duration t = tp.time_since_epoch();

    // Casts that duration to milliseconds
    std::chrono::milliseconds t_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t);

    // Extracts the integer millisecond count (typically long long)
    std::chrono::milliseconds::rep ms = t_ms.count();

    return ms;
}

void sendPayload(const char* payload) {
    sock_IMU.sendto(pc_IMU, payload, strlen(payload) - 1);
    // Using strlen() ensures I send exactly what was formatted in the snprintf()
}

// BSP Callbacks - Ping-pong: HalfTransfer = Ping ready, TransferComplete = Pong ready
void BSP_AUDIO_IN_HalfTransfer_CallBack(void) {
    // DMA just filled first half (Ping)
    // No cache ops needed - buffer in DTCM (non-cached)
    if (!s_frame_ready) {
        s_ready_idx = 0;
        s_frame_ready = true;
    }
}

void BSP_AUDIO_IN_TransferComplete_CallBack(void) {
    // DMA just filled second half (Pong)
    // No cache ops needed - buffer in DTCM (non-cached)
    if (!s_frame_ready) {
        s_ready_idx = 1;
        s_frame_ready = true;
    }
}

void BSP_AUDIO_IN_Error_CallBack(void) {
    // Handle audio DMA/peripheral errors
    printf("Audio error!\n");
    s_frame_ready = false;
}

// Audio send thread
void audio_tx_worker() {
    printf("[Audio Thread] Starting, opening socket...\r\n");
    nsapi_error_t err = sock_audio.open(&net);
    printf("[Audio Thread] Socket open result: %d\r\n", err);
    while (true) {
        if (s_stream_on && s_frame_ready) {
            uint8_t idx = s_ready_idx;
            s_frame_ready = false; // free the slot immediately
            // No cache ops needed - buffer in DTCM (non-cached)
            (void)sock_audio.sendto(pc_audio, s_dma_buf[idx], FRAME_BYTES); // one UDP pkt/frame
        }
        else {
            ThisThread::sleep_for(1ms);
        }
    }
}

// Start onboard mic → UDP :55054
void audio_start() {
    s_stream_on = true;
    s_frame_ready = false;
    BSP_AUDIO_IN_Init(RATE, DEFAULT_AUDIO_IN_BIT_RESOLUTION, DEFAULT_AUDIO_IN_CHANNEL_NBR);
    BSP_AUDIO_IN_Record((uint16_t*)s_dma_buf, 2 * SAMPLES);  // start DMA circular on both buffers
}

void audio_stop() {
    s_stream_on = false;
    BSP_AUDIO_IN_Stop(CODEC_PDWN_SW);
    BSP_AUDIO_IN_DeInit();
    s_frame_ready = false;
}