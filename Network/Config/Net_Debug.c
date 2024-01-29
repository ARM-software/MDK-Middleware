/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Net_Debug.c
 * Purpose: Network Debug Configuration
 * Rev.:    V7.1.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <q>Print Time Stamps
// <i>Enable printing time stamps in debug messages
#define DBG_TIME                1

// <e>System Debug
// <i>Enable printing system debug messages
#define DBG_SYSTEM_EN           1

//   <o>Memory Management  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure Dynamic Memory Management debugging
#define DBG_MEM                 1

//   <o>ETH Interface  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure Ethernet Interface debugging
#define DBG_ETH                 1

//   <o>WiFi Interface  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure WiFi Interface debugging
#define DBG_WIFI                1

//   <o>PPP Interface  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure Serial PPP Interface debugging
#define DBG_PPP                 0

//   <o>SLIP Interface  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure Serial SLIP Interface debugging
#define DBG_SLIP                0

//   <o>Loopback Interface  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure Local Loopback Interface debugging
#define DBG_LOOP                1
// </e>

// <e>IPv4 Core Debug
// <i>Enable printing IPv4 debug messages
#define DBG_CORE4_EN            0

//   <o>IP4 Core  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure IPv4 Core/localhost debugging
#define DBG_IP4                 1

//   <o>ICMP Control  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure Internet Control Message debugging
#define DBG_ICMP                1

//   <o>IGMP Management  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure debugging of Multicast Group Management
#define DBG_IGMP                1

//   <o>NBNS Client  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure NetBIOS Name Service Client debugging
#define DBG_NBNS                1

//   <o>DHCP Client  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure Dynamic Host Configuration Client debugging
#define DBG_DHCP                1

//   <o>ARP Address Resolution  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure Ethernet Address Resolution debugging
#define DBG_ARP                 1
// </e>

// <e>IPv6 Core Debug
// <i>Enable printing IPv6 debug messages
#define DBG_CORE6_EN            0

//   <o>IP6 Core  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure IPv6 Core/localhost debugging
#define DBG_IP6                 1

//   <o>ICMP6 Control  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure Internet Control Message ver.6 debugging
#define DBG_ICMP6               1

//   <o>DHCP6 Client  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure Dynamic Host Configuration ver.6 Client debugging
#define DBG_DHCP6               1

//   <o>NDP Neighbor Discovery  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure Neighbor Discovery in IPv6 debugging
#define DBG_NDP6                1
// </e>

// <e>Socket Debug
// <i>Enable printing socket debug messages
#define DBG_SOCKET_EN           0

//   <o>UDP Socket  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure native UDP Socket debugging
#define DBG_UDP_SOCKET          1

//   <o>TCP Socket  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure native TCP Socket debugging
#define DBG_TCP_SOCKET          1

//   <o>BSD Socket  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure BSD Socket Interface debugging
#define DBG_BSD_SOCKET          1
// </e>

// <e>Service Debug
// <i>Enable printing service debug messages
#define DBG_SERVICE_EN          0

//   <o>HTTP Server  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure Web Server debugging
#define DBG_HTTP_SERVER         1

//   <o>FTP Server  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure FTP Server debugging
#define DBG_FTP_SERVER          1

//   <o>FTP Client  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure FTP Client debugging
#define DBG_FTP_CLIENT          1

//   <o>Telnet Server  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure Telnet Server debugging
#define DBG_TELNET_SERVER       1

//   <o>TFTP Server  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure TFTP Server debugging
#define DBG_TFTP_SERVER         1

//   <o>TFTP Client  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure TFTP Client debugging
#define DBG_TFTP_CLIENT         1

//   <o>SMTP Client  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure SMTP Client debugging
#define DBG_SMTP_CLIENT         1

//   <o>DNS Client  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure Domain Name Service Client debugging
#define DBG_DNS_CLIENT          1

//   <o>SNMP Agent  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure Simple Network Management debugging
#define DBG_SNMP_AGENT          1

//   <o>SNTP Client  <0=>Off <1=>Errors only <2=>Full debug
//   <i>Configure Simple Network Time debugging
#define DBG_SNTP_CLIENT         1
// </e>

//------------- <<< end of configuration section >>> ---------------------------


#include "net_debug.h"


/**
  \fn          void net_debug_init (void)
  \brief       Initialize Network Debug Interface.
*/
void net_debug_init (void) {
  /* Add your code to initialize the debug output device. */
  /* This is usually the serial interface or ITM channel. */

}
