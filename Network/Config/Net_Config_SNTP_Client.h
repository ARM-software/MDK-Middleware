/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Net_Config_SNTP_Client.h
 * Purpose: Network Configuration for SNTP Client
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>SNTP Client
#define SNTP_CLIENT_ENABLE       1

//   <q>Broadcast Mode
//   <i>Enable this option, if you have NTP/SNTP server
//   <i>on LAN, which is broadcasting NTP time messages.
//   <i>Disable this option to access public NTP server.
//   <i>Default: disabled
#define SNTP_CLIENT_BCAST_MODE   0

//   <s.15>NTP Server
//   <i>NTP Server IPv4 Address in text representation
//   <i>Default: "217.79.179.106"
#define SNTP_CLIENT_NTP_SERVER  "217.79.179.106"

// </h>

//------------- <<< end of configuration section >>> ---------------------------
