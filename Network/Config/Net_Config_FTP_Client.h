/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Net_Config_FTP_Client.h
 * Purpose: Network Configuration for FTP Client
 * Rev.:    V5.0.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>FTP Client
#define FTP_CLIENT_ENABLE       1

//   <o>Response Timeout in seconds <1-120>
//   <i>This is a time for FTP Client to wait for a response from
//   <i>the Server. If timeout expires, Client aborts operation.
//   <i>Default: 10
#define FTP_CLIENT_TOUT         10

//   <q>Passive mode (PASV)
//   <i>The client initiates a data connection to the server.
//   <i>Default: Not passive (Active)
#define FTP_CLIENT_PASSIVE_MODE 0

// </h>

//------------- <<< end of configuration section >>> ---------------------------
