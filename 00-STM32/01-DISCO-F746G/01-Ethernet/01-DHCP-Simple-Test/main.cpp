/*******************************************************************************************************************
 * Objective:  Send a UDP packet to a PC only one time
 *******************************************************************************************************************
 * Context:  Adapted from Arduino IDE examples; refined to run on STM32F746NG, Mbed OS 6.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student, HAW Hamburg, Germany.
 * Profile: https://www.linkedin.com/in/lucianocarricart/
 *******************************************************************************************************************/

#include "mbed.h"
#include "EthernetInterface.h"
#include "UDPSocket.h"

  // MAC Address
  // byte mac[] = {0x02, 0x00, 0xF7, 0x46, 0x00, 0x01};

  // Network interface
EthernetInterface net;

int main() {
    // nsapi_error_t is an integer error-code type used by Mbed OS s Network Socket API
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

    net.get_ip_address(&ip);
    SocketAddress nm, gw;
    net.get_netmask(&nm);
    net.get_gateway(&gw);
    printf("IP=%s  NM=%s  GW=%s\n", ip.get_ip_address(), nm.get_ip_address(), gw.get_ip_address());


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

