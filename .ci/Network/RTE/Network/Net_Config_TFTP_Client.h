/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Net_Config_TFTP_Client.h
 * Purpose: Network Configuration for TFTP Client
 * Rev.:    V5.0.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>TFTP Client
#define TFTP_CLIENT_ENABLE      1

//   <o>Block Size <128=>128   <256=>256   <512=>512
//                 <1024=>1024 <1428=>1428
//   <i>Size of transfer block in bytes.
//   <i>Default: 512
#define TFTP_CLIENT_BLOCK_SIZE  512

//   <o>Number of Retries <1-10>
//   <i>How many times TFTP Client will try to
//   <i>retransmit the data before giving up.
//   <i>Default: 4
#define TFTP_CLIENT_MAX_RETRY   4

//   <o>Retry Timeout <2=>200 ms <5=>500 ms <10=>1 sec
//                    <20=>2 sec <50=>5 sec <100=>10 sec
//   <i>If data frame not acknowledged within this time frame,
//   <i>TFTP Client will try to resend the data again.
//   <i>Default: 500 ms
#define TFTP_CLIENT_RETRY_TOUT  5

// </h>
