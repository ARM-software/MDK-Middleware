/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    TCP_Socket_Server.c
 * Purpose: TCP Socket Server Code Template
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/
//! [code_TCP_Socket_Server]
#include "rl_net.h"
 
// Notify the user application about TCP socket events.
uint32_t tcp_cb_server (int32_t socket, netTCP_Event event,
                        const NET_ADDR *addr, const uint8_t *buf, uint32_t len) {
 
  switch (event) {
    case netTCP_EventConnect:
      // Connect request received
      /* Example
      if (addr->addr_type == NET_ADDR_IP4) {
        // IPv4 client
        if (addr->addr[0] == 192  &&
            addr->addr[1] == 168  &&
            addr->addr[2] == 0    &&
            addr->addr[3] == 1) {
          // Accept connection from client at 192.168.0.1
          return (1);
        }
      }
      else {
        // IPv6 client
        const uint8_t ip6_addr[NET_ADDR_IP6_LEN] = { 
                         0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x1c, 0x30, 0x6c, 0xff, 0xfe, 0xa2, 0x45, 0x5e };
        if (memcmp (addr->addr, ip6_addr, NET_ADDR_IP6_LEN) == 0) {
          // Accept connection from client at [fe80::1c30:6cff:fea2:455e]
          return (1);
        }
      }
      // Deny connection.
      return (0);
      */
      return (1);
 
    case netTCP_EventEstablished:
      // Connection established
      break;
 
    case netTCP_EventClosed:
      // Connection was properly closed
      break;
 
    case netTCP_EventAborted:
      // Connection is for some reason aborted
      break;
 
    case netTCP_EventACK:
      // Previously sent data acknowledged
      break;
 
    case netTCP_EventData:
      // Data received
      /* Example
      if ((buf[0] == 0x01) && (len == 2)) {
        // Switch LEDs on and off
        // LED_out (buf[1]);
      }
      */
      break;
  }
  return (0);
}
 
// Allocate and initialize the socket.
/* Example
int main (void) {
  int32_t tcp_sock;
 
  netInitialize ();
 
  // Initialize TCP Socket and start listening on port 2000
  tcp_sock = netTCP_GetSocket (tcp_cb_server);
  if (tcp_sock > 0) {
    netTCP_Listen (tcp_sock, 2000);
  }
}
*/
//! [code_TCP_Socket_Server]
