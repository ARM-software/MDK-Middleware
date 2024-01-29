/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    FS_Config.c
 * Purpose: File System Configuration
 * Rev.:    V6.4.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>FAT File System
// <i>Define FAT File System parameters

//   <o>Number of open files <1-16>
//   <i>Define number of files that can be opened at the same time.
//   <i>Default: 4
#define FAT_MAX_OPEN_FILES      4

// </h>

// <h>Embedded File System
// <i>Define Embedded File System parameters

//   <o>Number of open files <1-16>
//   <i>Define number of files that can be opened at the same time.
//   <i>Default: 4
#define EFS_MAX_OPEN_FILES      4

// </h>

// <o>Initial Current Drive <0=>F0: <1=>F1:
//                          <2=>M0: <3=>M1:
//                          <4=>N0: <5=>N1:
//                          <6=>R0: <9=>R1:
//                          <7=>U0: <8=>U1:
// <i>Set initial setting for current drive. Current drive is used for File System functions
// <i>that are invoked with the "" string and can be altered anytime during run-time.
#define FS_INITIAL_CDRIVE       2

#include "fs_config.h"
