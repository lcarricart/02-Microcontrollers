/*******************************************************************************************************************
 * Objective:  Send a UDP payload to a PC every time a button is pressed, and store the real-time information in a .txt file.
   The connection is straight [MCU --> PC] via an Ethernet cable and requires no Internet. 
   Main changes are done in the .ps1 PowerShell file.
 *******************************************************************************************************************
 * Context:     - Adapted from Arduino IDE examples; refined to run on STM32F746NG, Mbed OS 6.
 *              - Development environment: Arm Mbed Studio
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student, HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

 /*******************************************************************************************************************
  * Theory:
    Internet doesn’t matter here. This works with or without internet as long as the MCU and the PC can reach each
    other on the same local network. If you move to a different Wi - Fi / router or use a direct cable, it still works
    -—you just need matching local IPs (update those addresses to match the new LAN).
    

 *******************************************************************************************************************/

#include "mbed.h"
#include "EthernetInterface.h"    // https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/EthernetInterface.h
// "EthernetInterface.cpp"           https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/source/EthernetInterface.cpp
#include "UDPSocket.h"

// Button and LED
InterruptIn button(BUTTON1);
DigitalOut led(LED1);

// Allows me to delegate to a complex function from an ISR
events::EventQueue queue(8 * 1024);         // bytes for the queue
Thread worker(osPriorityNormal, 4 * 1024);

// Network interface and socket kept alive for the whole app
EthernetInterface   net;
UDPSocket           sock;
SocketAddress       pc;

// Payload
const char msg[] = "hello from MCU";

// nsapi_error_t is an integer error-code type used by Mbed OS’s Network Socket API
// functions like net.connect() and sock.open(&net) return an nsapi_error_t value.
nsapi_error_t rc;
nsapi_size_or_error_t n;

// Function prototypes
void onButtonPressed_isr();
void onButton();                // Delegated from ISR onButtonPressed_isr()

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
    n = sock.sendto(pc, msg, sizeof(msg) - 1);

    if (n >= 0) {
        printf("Message sent, sendto() successful (%d bytes)\n", n);
    }
    else {
        printf("Error %d: message not sent, unsuccessful sendto()\n", n);
    }

    button.fall(callback(onButtonPressed_isr));
    worker.start(callback(&queue, &events::EventQueue::dispatch_forever));

    while (true) {
        ThisThread::sleep_for(1s);
    }
}

void onButtonPressed_isr() {
    led = !led;
    queue.call(onButton);      // Posts the handler, delegation
}

void onButton() {
    n = sock.sendto(pc, msg, sizeof(msg) - 1);

    if (n >= 0) {
        printf("Message sent, sendto() successful (%d bytes)\n", n);
    }
    else {
        printf("Error %d: message not sent, unsuccessful sendto()\n", n);
    }
}
