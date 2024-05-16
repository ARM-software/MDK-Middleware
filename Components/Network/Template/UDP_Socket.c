/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    UDP_Socket.c
 * Purpose: UDP Socket Code Template
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/
//! [code_UDP_Socket]
#include "rl_net.h"
 
int32_t udp_sock;                       // UDP socket handle
 
// Notify the user application about UDP socket events.
uint32_t udp_cb_func (int32_t socket, const  NET_ADDR *addr, const uint8_t *buf, uint32_t len) {
 
  // Data received
  /* Example
  if ((buf[0] == 0x01) && (len == 2)) {
    // Switch LEDs on and off
    // LED_out (buf[1]);
  }
  */
  return (0);
}
 
// Send UDP data to destination client.
void send_udp_data (void) {
 
  if (udp_sock > 0) {
    /* Example
    // IPv4 address: 192.168.0.1
    NET_ADDR addr = { NET_ADDR_IP4, 2000, 192, 168, 0, 1 };
    // IPv6 address: [fe80::1c30:6cff:fea2:455e]
//  NET_ADDR addr = { NET_ADDR_IP6, 2000,
//                    0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//                    0x1c, 0x30, 0x6c, 0xff, 0xfe, 0xa2, 0x45, 0x5e };
    uint8_t *sendbuf;
 
    sendbuf = netUDP_GetBuffer (2);
    sendbuf[0] = 0x01;
    sendbuf[1] = 0xAA;
 
    netUDP_Send (udp_sock, &addr, sendbuf, 2);
    */
  }
}
 
// Allocate and initialize the socket.
/* Example
int main (void) {
 
  netInitialize ();
 
  // Initialize UDP socket and open port 2000
  udp_sock = netUDP_GetSocket (udp_cb_func);
  if (udp_sock > 0) {
    netUDP_Open (udp_sock, 2000);
  }
}
*/
//! [code_UDP_Socket]
