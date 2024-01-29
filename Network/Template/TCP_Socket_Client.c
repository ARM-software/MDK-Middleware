/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    TCP_Socket_Client.c
 * Purpose: TCP Socket Client Code Template
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/
//! [code_TCP_Socket_Client]
#include "rl_net.h"
 
int32_t tcp_sock;                       // TCP socket handle
 
// Notify the user application about TCP socket events.
uint32_t tcp_cb_client (int32_t socket, netTCP_Event event,
                        const NET_ADDR *addr, const uint8_t *buf, uint32_t len) {
 
  switch (event) {
    case netTCP_EventConnect:
      // Connect request received
      break;
 
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
      break;
  }
  return (0);
}
 
// Connect to TCP server and send data.
void send_data (void) {
  // IPv4 address: 192.168.0.1
  NET_ADDR addr = { NET_ADDR_IP4, 2000,
                    192, 168, 0, 1 };
  // IPv6 address: [fe80::1c30:6cff:fea2:455e]
//NET_ADDR addr = { NET_ADDR_IP6, 2000,
//                  0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//                  0x1c, 0x30, 0x6c, 0xff, 0xfe, 0xa2, 0x45, 0x5e };
 
  if (tcp_sock > 0) {
    switch (netTCP_GetState (tcp_sock)) {
      case netTCP_StateUNUSED:
      case netTCP_StateCLOSED:
        // Connect to TCP socket server on port 2000
        netTCP_Connect (tcp_sock, &addr, 0);
        break;
 
      case netTCP_StateESTABLISHED:
        // Connected, send the data
        if (netTCP_SendReady (tcp_sock) == true) {
          /* Example
          uint8_t *sendbuf;
 
          sendbuf = netTCP_GetBuffer (2);
          sendbuf[0] = 0x01;
          sendbuf[1] = 0x55;
 
          netTCP_Send (tcp_sock, sendbuf, 2);
          */
        }
        break;
 
      default:
        break;
    }
  }
}
 
// Allocate and initialize the socket.
/* Example
int main (void) {
 
  netInitialize ();
 
  // Allocate a free TCP socket.
  tcp_sock = netTCP_GetSocket (tcp_cb_client);
}
*/
//! [code_TCP_Socket_Client]
