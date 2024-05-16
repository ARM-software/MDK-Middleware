/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Interface
 * Copyright (c) 2004-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Net_Config_SLIP.h
 * Purpose: Network Configuration for SLIP Interface
 * Rev.:    V7.2.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>SLIP Network Interface
#define SLIP_ENABLE             1

//   <o>Connect to hardware via Driver_USART# <0-255>
//   <i>Select driver control block for hardware interface
#define SLIP_USART_DRIVER       0

//   <h>Modem Settings
//   <i>Serial Modem connection parameters
//     <o>Speed <300=>300
//              <600=>600
//              <1200=>1200
//              <2400=>2400
//              <4800=>4800
//              <9600=>9600
//              <14400=>14400
//              <19200=>19200
//              <28800=>28800
//              <38400=>38400
//              <57600=>57600
//              <115200=>115200
//              <128000=>128000
//              <230400=>230400
//              <256000=>256000
//              <460800=>460800
//              <921600=>921600
//     <i>Baud rate setting for USART driver
#define SLIP_MODEM_SPEED        115200

//     <o>Flow Control <0=>None
//                     <1=>RTS/CTS
//     <i>Flow control setting for USART driver
#define SLIP_MODEM_FLOW_CONTROL 0

//     <s.50>Initialization String
//     <i>Custom commands for modem initialization
//     <i>Default: none
#define SLIP_MODEM_INIT_STRING  ""
//   </h>

//   <s.15>IP Address
//   <i>Static IPv4 Address in text representation
//   <i>Default: "192.168.225.1"
#define SLIP_IP4_ADDR           "192.168.225.1"

//   <s.15>Primary DNS Server
//   <i>IP Address of Primary DNS Server in text representation
//   <i>Default: "8.8.8.8"
#define SLIP_IP4_PRIMARY_DNS    "8.8.8.8"

//   <s.15>Secondary DNS Server
//   <i>IP Address of Secondary DNS Server in text representation
//   <i>Default: "8.8.4.4"
#define SLIP_IP4_SECONDARY_DNS  "8.8.4.4"

//   <e>IP Fragmentation
//   <i>This option enables fragmentation of outgoing IP datagrams,
//   <i>and reassembling the fragments of incoming IP datagrams.
//   <i>Default: enabled
#define SLIP_IP4_FRAG_ENABLE    1

//     <o>MTU size <296-1500>
//     <i>Maximum Transmission Unit in bytes
//     <i>Default: 1500
#define SLIP_IP4_MTU            1500
//   </e>

//   <q>Use Default Gateway on remote Network
//   <i>This option only applies when both Ethernet and SLIP Dial-up
//   <i>are used. If checked, data that cannot be sent to local LAN
//   <i>is forwarded to Dial-up network instead.
#define SLIP_DEFAULT_GW         1

//   Disable ICMP Echo response
#define SLIP_ICMP_NO_ECHO       0

//   <h>OS Resource Settings
//   <i>These settings are used to optimize usage of OS resources.
//     <o>Interface Thread Stack Size <512-65535:4>
//     <i>Default: 512 bytes
#define SLIP_THREAD_STACK_SIZE  512

//        Interface Thread Priority
#define SLIP_THREAD_PRIORITY    osPriorityAboveNormal

//   </h>
// </h>

//------------- <<< end of configuration section >>> ---------------------------
