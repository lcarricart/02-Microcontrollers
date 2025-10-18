/*******************************************************************************************************************
 * Objective:  Send a UDP packet to a PC on each button press, under a shared LAN. [DHCP] MCU (Ethernet) --> Router --> PC (WiFi or Ethernet) 
 *******************************************************************************************************************
 * Context:  Adapted from Arduino IDE examples; refined to run on STM32F746NG, Mbed OS 6.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student, HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

/********************************************************************************************************************
 *  Theory:

 When your board boots with DHCP enabled, it runs a DHCP client that talks to the DHCP server (usually your router):
 DISCOVER ? MCU broadcasts “who can give me an IP?”
 OFFER ? Router proposes an IP (plus gateway, DNS, lease time…).
 REQUEST ? MCU says “I want that IP, please.”
 ACK ? Router confirms. The MCU configures its interface.

 You must choose a MAC for your board and make sure (a) it’s a unicast, locally-administered address and (b) it’s
 unique on your LAN and stable per board. A MAC is 48 bits. The first byte’s two lowest bits matter:
 bit0 = 0 ? unicast (not multicast)
 bit1 = 1 ? locally administered (LAA) (you chose it; not from an IEEE OUI)

 In hex, a safe first byte is 0x02 (binary 0000 0010 ? unicast + LAA). The middle bytes F7:46 are only a mnemonic for “F746"
 02:00:F7:46:00:01

 A UDP socket is an endpoint for sending/receiving datagrams using UDP (User Datagram Protocol) on top of IP (Internet Protocol).
 • Key traits of UDP : connectionless, message-oriented, no delivery guarantees, no ordering, no built-in retransmission
   or congestion control. You send a packet to an IP address and port; it may arrive, arrive out of order, or be dropped.
 • Common uses: DNS queries, real-time telemetry, VoIP (Voice over IP), simple request/response where loss is acceptable
   or handled at the application layer.
 • A UDP socket is perfect for “just send a short message to my computer and see it arrive.” If you later want a persistent
   channel with guaranteed delivery, switch to TCPSocket and have your PC run a TCP listener (server) on a port.
  ********************************************************************************************************************/

#include "mbed.h"
#include "EthernetInterface.h"   // https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/EthernetInterface.h
// "EthernetInterface.cpp"          https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/source/EthernetInterface.cpp
#include "UDPSocket.h"

// Button and LED
InterruptIn button(BUTTON1);
DigitalOut led(LED1);

// Allows me to delegate to a complex function from an ISR
events::EventQueue queue(8 * 1024);         // bytes for the queue
Thread worker(osPriorityNormal, 4 * 1024);

// Network interface and socket kept alive for the whole app
EthernetInterface  net;
UDPSocket          sock;
SocketAddress      pc;

// Payload
static const char  msg[] = "hello from MCU";

// nsapi_error_t is an integer error-code type used by Mbed OS’s Network Socket API
// functions like net.connect() and sock.open(&net) return an nsapi_error_t value.
nsapi_error_t rc;

// Function prototypes
void onButtonPressed_isr();
void onButton();                // Delegated from ISR onButtonPressed_isr()

// MAC Address (02:00:F7:46:00:01)
// byte mac[] = {0x02, 0x00, 0xF7, 0x46, 0x00, 0x01};

int main() {
    printf("\nMCU (Ethernet) --> Router --> PC (WiFi or Ethernet)\n");

    do {
        rc = net.connect(); // DHCP

        if (rc != NSAPI_ERROR_OK) {
            printf("Attempt net.connect failed: %d\n", rc);
            printf("Retrying...\n");
            ThisThread::sleep_for(2s);
        }
    } while (rc != NSAPI_ERROR_OK);               // If the return is not ERROR_OK, then it failed

    SocketAddress ip;                             // empty address container
    net.get_ip_address(&ip);                      // "net" stores the address in a variable
    printf("MCU IP: %s\n", ip.get_ip_address());  // getter() for the content of "ip"

    do {
        rc = sock.open(&net);

        if (rc != NSAPI_ERROR_OK) {
            printf("sock.open failed: %d\n", rc);
            printf("Retrying...\n");
            ThisThread::sleep_for(2s);
        }
    } while (rc != NSAPI_ERROR_OK);

    pc = SocketAddress("192.168.178.38", 5005);      // !!! Replace with your PC IP (IPv4, LAN) and chosen port (programmed in PowerShell)
    rc = sock.sendto(pc, msg, sizeof(msg) - 1);
    printf("Function call: sendto rc=%d, bytes=%u\n", rc, (unsigned)(sizeof(msg) - 1));

    char buf[64];
    SocketAddress from;
    sock.set_timeout(2000);                            // If you wait for a reply (echo), set_timeout prevents hanging forever.
    rc = sock.recvfrom(&from, buf, sizeof(buf));       // Waits for an echo (although not programmed finally)
    if (rc > 0) {
        printf("Reply %dB from %s:%d: %.*s\n",         // Precision string
            rc, from.get_ip_address(), from.get_port(), rc, buf);
    }
    else {
        printf("No reply received, rc=%d (timeout)\n", rc);  // NSAPI_ERROR_WOULD_BLOCK on timeout
    }

    // When the program used to run one time only
    // sock.close();
    // net.disconnect();
    // Actually a cleanup happens implicitly when the process ends, so you don’t need explicit close/disconnect.

    button.fall(callback(onButtonPressed_isr));
    worker.start(callback(&queue, &events::EventQueue::dispatch_forever));

    while (true) {
        // Wait only for interrupts
        ThisThread::sleep_for(1s);
    }
}

void onButtonPressed_isr() {
    led = !led;
    queue.call(onButton);      // Posts the handler, delegation
}

void onButton() {
    rc = sock.sendto(pc, msg, sizeof(msg) - 1);
    printf("New PING being sent!\n");
    printf("Function call: sendto rc=%d, bytes=%u\n", rc, (unsigned)(sizeof(msg) - 1));
}
