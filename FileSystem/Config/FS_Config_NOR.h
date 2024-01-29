/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System:Drive
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    FS_Config_NOR_%Instance%.h
 * Purpose: File System Configuration for NOR Flash Drive
 * Rev.:    V6.2.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>NOR Flash Drive %Instance%
// <i>Configuration for NOR Flash device assigned to drive letter "F%Instance%:"
#define NOR%Instance%_ENABLE             1

//   <o>Connect to hardware via Driver_Flash# <0-255>
//   <i>Select driver control block for hardware interface
#define NOR%Instance%_DRIVER             0

// </h>
