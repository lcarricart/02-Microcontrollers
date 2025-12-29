/*******************************************************************************************************************
 * Objective:  Send a UDP packet to a PC only one time. The connection is straight [MCU --> PC] via an Ethernet cable.
 * This is the first step towards transferring live data to the PC for a major programming project, without internet.
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
#include "EthernetInterface.h"   // https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/EthernetInterface.h
// "EthernetInterface.cpp"          https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/source/EthernetInterface.cpp
#include "UDPSocket.h"

int main() {
    printf("\nMCU --> PC\n");

    // Creates a network interface
    EthernetInterface net;

    /* Sets a static IPv4.Gateway 0.0.0.0 means "no default route".In other words : the MCU will only talk to hosts that 
       are on its own subnet; it will not try to reach anything “somewhere else” via a router. It is NOT fine when you want 
       to reach anything outside your subnet. */
    net.set_network("192.168.7.2", "255.255.255.0", "0.0.0.0");
    nsapi_error_t rc = net.connect();
    printf("net.connect=%d\n", rc);

    SocketAddress myip;
    net.get_ip_address(&myip);                          // Store IP
    printf("MCU IP: %s\n", myip.get_ip_address());      // Print IP

    UDPSocket sock; 
    sock.open(&net);
    SocketAddress pc("192.168.7.1", 55055);             // PC’s static IPv4 and chosen port. For a direct cable, give the PC the static 
                                                        // IP 192.168.7.1 on its Ethernet adapter and run your PowerShell listener bound 
                                                        // to that address and port.
    const char msg[] = "hello from MCU";
    nsapi_size_or_error_t n = sock.sendto(pc, msg, sizeof(msg) - 1);
    printf("sendto -> %d\n", n);

    while (true) { ThisThread::sleep_for(1s); }
}
