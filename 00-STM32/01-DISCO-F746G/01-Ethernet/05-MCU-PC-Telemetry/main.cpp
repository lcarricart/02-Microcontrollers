/*******************************************************************************************************************
 * Objective:  Send a UDP payload with IMU data at a established frequency (10 Hz). The connection is straight [MCU --> PC]
   via an Ethernet cable (no Internet required). This is part of a major programming project, where live telemetry is
   transmitted to a PC, to be later manipulated and displayed with a Java GUI application.
   LED1 ON/OFF indicates the state of data transmission.
 *******************************************************************************************************************
 * Context:  Adapted from Arduino IDE examples; refined to run on STM32F746NG, Mbed OS 6.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student, HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

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

// Network interface and socket kept alive for the whole app
EthernetInterface   net;
UDPSocket           sock;
SocketAddress       pc;

// nsapi_error_t is an integer error-code type used by Mbed OS’s Network Socket API
// functions like net.connect() and sock.open(&net) return an nsapi_error_t value.
nsapi_error_t rc;
nsapi_size_or_error_t n;

// Gloabl variables
volatile bool buttonPressed = false; // volatile modifier because it is an ISR variable

// Function prototypes
void onButtonPressed_isr();
long long int getTimeStamp_ms();
void sendPayload(const char*);

int main() {
    printf("\nProgram start: MCU --> PC\n");

    /* Sets a static IPv4.Gateway 0.0.0.0 means "no default route".In other words : the MCU will only talk to hosts that
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
        rc = sock.open(&net);

        if (rc != NSAPI_ERROR_OK) {
            printf("sock.open failed: %d\n", rc);
            printf("Retrying...\n");
            ThisThread::sleep_for(2s);
        }
    } while (rc != NSAPI_ERROR_OK);

    /* PC’s static IPv4 and chosen port.For a direct cable, give the PC the static IP 192.168.7.1 on its Ethernet adapter
       and run your PowerShell listener bound to that address and port.*/
    pc = SocketAddress("192.168.7.1", 55055);

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
    while (buttonPressed == false) {
        ThisThread::yield();         // Give processing to other tasks
    }
    buttonPressed = false;           // Reset the variable value

    printf("Sending payload...");
    char msg[256] = "";              // Payload
    snprintf(msg, sizeof(msg), "timestamp, accX, accY, accZ, gyroX, gyroY, gyroZ, temperature\r\n");
    sendPayload(msg);
    long long int offset = getTimeStamp_ms();
    led = 1;                         // Indicates start of transmission

    while (buttonPressed == false) {
        // If data ready bit set, all data registers have new data
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
    sock.sendto(pc, payload, strlen(payload) - 1);
    // Using strlen() ensures I send exactly what was formatted in the snprintf()
}