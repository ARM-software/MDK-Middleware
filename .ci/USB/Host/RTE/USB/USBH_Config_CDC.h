/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBH_Config_CDC.h
 * Purpose: USB Host Human Interface Device class (HID) Configuration
 * Rev.:    V5.0.1
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
//------ With VS Code: Open Preview for Configuration Wizard -------------------

// <h>USB Host: Communication Device Class (CDC)
//   <o> Number of concurrent CDC Devices in system <0-3>
#define USBH_CDC_NUM                        1

//   <h>OS Resources Settings
//   <i>These settings are used to optimize usage of OS resources.
//     <o>CDC Interrupt In Pipe (modem and error status reception) handling Thread Stack Size <64-65536>
#define USBH_CDC_INT_IN_THREAD_STACK_SIZE   512

//        CDC Interrupt In Pipe handling Thread Priority
#define USBH_CDC_INT_IN_THREAD_PRIORITY     osPriorityAboveNormal

//   </h>
// </h>
