/*******************************************************************************************************************
 * Objective:  Send a UDP packet to a PC on each button press, under a shared LAN. MCU --> Ethernet. PC --> WiFi
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
#include "EthernetInterface.h"
#include "UDPSocket.h"

  // MAC Address
  // byte mac[] = {0x02, 0x00, 0xF7, 0x46, 0x00, 0x01};

  // Network interface
EthernetInterface net;

int main() {
    // nsapi_error_t is an integer error-code type used by Mbed OS’s Network Socket API
    // functions like net.connect() and sock.open(&net) return an nsapi_error_t value.
    nsapi_error_t rc = net.connect(); // DHCP

    // If the return is not ERROR_OK, then it failed
    if (rc != NSAPI_ERROR_OK) {
        printf("net.connect failed: %d\n", rc);
        return 0;
    }

    SocketAddress ip;                             // empty address container
    net.get_ip_address(&ip);                      // "net" stores the address in a variable
    printf("MCU IP: %s\n", ip.get_ip_address());  // getter() for the content of "ip"

    UDPSocket sock;
    rc = sock.open(&net);
    if (rc != NSAPI_ERROR_OK) {
        printf("sock.open failed: %d\n", rc);
        net.disconnect();
        return 0;
    }
    sock.set_timeout(2000);

    SocketAddress pc("192.168.178.38", 5005);        // <-- replace with your PC IP and chosen port
    const char msg[] = "hello from MCU";
    rc = sock.sendto(pc, msg, sizeof(msg) - 1);
    printf("sendto rc=%d, bytes=%u\n", rc, (unsigned)(sizeof(msg) - 1));

    char buf[64];
    SocketAddress from;
    rc = sock.recvfrom(&from, buf, sizeof(buf));       // wait for echo
    if (rc > 0) {
        printf("reply %dB from %s:%d: %.*s\n",
            rc, from.get_ip_address(), from.get_port(), rc, buf);
    }
    else {
        printf("no reply, rc=%d (timeout)\n", rc);  // NSAPI_ERROR_WOULD_BLOCK on timeout
    }

    sock.close();
    net.disconnect();

    while (true) { ThisThread::sleep_for(1s); }
}


