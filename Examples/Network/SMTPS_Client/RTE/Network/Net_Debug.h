/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Net_Debug.h
 * Purpose: Network Debug Configuration
 * Rev.:    V8.0.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
//------ With VS Code: Open Preview for Configuration Wizard -------------------

// <e>Network Debug
// <i>Enable generation of diagnostic messages
#define NET_DEBUG_ENABLE        0

//   <o>Output Channel <0=>Event Recorder
//                     <1=>STDIO
//                     <2=>STDIO + Timestamp
//   <i>Select output channel for delivery of diagnostic messages
#define NET_DEBUG_CHANNEL       0

//   <e>System
//   <i>Enable generation of system diagnostic messages
#define NET_DEBUG_SYSTEM        1

//     <o>Memory Management  <0=>Off <1=>Errors <2=>All
//     <i>Configure Dynamic Memory Management diagnostics
#define NET_DEBUG_MEM           1

//     <o>ETH Interface  <0=>Off <1=>Errors <2=>All
//     <i>Configure Ethernet Interface diagnostics
#define NET_DEBUG_ETH           1

//     <o>WiFi Interface  <0=>Off <1=>Errors <2=>All
//     <i>Configure WiFi Interface diagnostics
#define NET_DEBUG_WIFI          1

//     <o>PPP Interface  <0=>Off <1=>Errors <2=>All
//     <i>Configure Serial PPP Interface diagnostics
#define NET_DEBUG_PPP           0

//     <o>SLIP Interface  <0=>Off <1=>Errors <2=>All
//     <i>Configure Serial SLIP Interface diagnostics
#define NET_DEBUG_SLIP          0

//     <o>Loopback Interface  <0=>Off <1=>Errors <2=>All
//     <i>Configure Local Loopback Interface diagnostics
#define NET_DEBUG_LOOP          1
//   </e>

//   <e>IPv4
//   <i>Enable generation of IPv4 diagnostic messages
#define NET_DEBUG_IP4           1

//     <o>IPv4 Core  <0=>Off <1=>Errors <2=>All
//     <i>Configure IPv4 Core diagnostics
#define NET_DEBUG_CORE_IP4      1

//     <o>Control Message (ICMP) <0=>Off <1=>Errors <2=>All
//     <i>Configure Internet Control Message diagnostics
#define NET_DEBUG_ICMP          1

//     <o>Address Resolution (ARP) <0=>Off <1=>Errors <2=>All
//     <i>Configure Ethernet Address Resolution diagnostics
#define NET_DEBUG_ARP           1

//     <o>Group Management (IGMP) <0=>Off <1=>Errors <2=>All
//     <i>Configure Multicast Group Management diagnostics
#define NET_DEBUG_IGMP          1

//     <o>NBNS Client  <0=>Off <1=>Errors <2=>All
//     <i>Configure NetBIOS Name Service diagnostics
#define NET_DEBUG_NBNS          1

//     <o>DHCP Client  <0=>Off <1=>Errors <2=>All
//     <i>Configure Dynamic Host Configuration diagnostics
#define NET_DEBUG_DHCP          2
//   </e>

//   <e>IPv6
//   <i>Enable generation of IPv6 diagnostic messages
#define NET_DEBUG_IP6           1

//     <o>IPv6 Core  <0=>Off <1=>Errors <2=>All
//     <i>Configure IPv6 Core diagnostics
#define NET_DEBUG_CORE_IP6      1

//     <o>Control Message (ICMP6) <0=>Off <1=>Errors <2=>All
//     <i>Configure Internet Control Message ver.6 diagnostics
#define NET_DEBUG_ICMP6         1

//     <o>Neighbor Discovery (NDP) <0=>Off <1=>Errors <2=>All
//     <i>Configure Neighbor Discovery in IPv6 diagnostics
#define NET_DEBUG_NDP6          1

//     <o>Multicast Listener Discovery (MLD) <0=>Off <1=>Errors <2=>All
//     <i>Configure Multicast Listener Discovery diagnostics
#define NET_DEBUG_MLD6          1

//     <o>DHCP6 Client  <0=>Off <1=>Errors <2=>All
//     <i>Configure Dynamic Host Configuration ver.6 diagnostics
#define NET_DEBUG_DHCP6         2
//   </e>

//   <e>Socket
//   <i>Enable generation of socket diagnostic messages
#define NET_DEBUG_SOCKET        1

//     <o>UDP Socket  <0=>Off <1=>Errors <2=>All
//     <i>Configure UDP native socket diagnostics
#define NET_DEBUG_UDP           1

//     <o>TCP Socket  <0=>Off <1=>Errors <2=>All
//     <i>Configure TCP native socket diagnostics
#define NET_DEBUG_TCP           1

//     <o>BSD Socket  <0=>Off <1=>Errors <2=>All
//     <i>Configure BSD interface socket diagnostics
#define NET_DEBUG_BSD           1
//   </e>

//   <e>Service
//   <i>Enable generation of network service diagnostic messages
#define NET_DEBUG_SERVICE       1

//     <o>HTTP Server  <0=>Off <1=>Errors <2=>All
//     <i>Configure HTTP server diagnostics
#define NET_DEBUG_HTTP_SERVER   1

//     <o>FTP Server  <0=>Off <1=>Errors <2=>All
//     <i>Configure FTP Server diagnostics
#define NET_DEBUG_FTP_SERVER    1

//     <o>FTP Client  <0=>Off <1=>Errors <2=>All
//     <i>Configure FTP client diagnostics
#define NET_DEBUG_FTP_CLIENT    1

//     <o>Telnet Server  <0=>Off <1=>Errors <2=>All
//     <i>Configure Telnet server diagnostics
#define NET_DEBUG_TELNET_SERVER 1

//     <o>TFTP Server  <0=>Off <1=>Errors <2=>All
//     <i>Configure TFTP server diagnostics
#define NET_DEBUG_TFTP_SERVER   1

//     <o>TFTP Client  <0=>Off <1=>Errors <2=>All
//     <i>Configure TFTP client diagnostics
#define NET_DEBUG_TFTP_CLIENT   1

//     <o>SMTP Client  <0=>Off <1=>Errors <2=>All
//     <i>Configure SMTP client diagnostics
#define NET_DEBUG_SMTP_CLIENT   2

//     <o>DNS Client  <0=>Off <1=>Errors <2=>All
//     <i>Configure Domain Name Service diagnostics
#define NET_DEBUG_DNS_CLIENT    1

//     <o>SNMP Agent  <0=>Off <1=>Errors <2=>All
//     <i>Configure Simple Network Management diagnostics
#define NET_DEBUG_SNMP_AGENT    1

//     <o>SNTP Client  <0=>Off <1=>Errors <2=>All
//     <i>Configure Simple Network Time diagnostics
#define NET_DEBUG_SNTP_CLIENT   1
//   </e>
// </e>
