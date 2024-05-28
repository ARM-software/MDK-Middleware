/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBH_Config_3.h
 * Purpose: USB Host Configuration
 * Rev.:    V8.0.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>USB Host 3
//   <y>Connect to hardware via Driver_USBH#
//   <i>Specifies instance of the Host Controller hardware interface.
#define USBH3_HC_NUM                    3

//   <o>Maximum Port Power Consumption<2-500:2>
//     <i>Specifies the maximum power consumption per port (in mA)
#define USBH3_HC_POWER                  500

//   <o>Maximum Pipes in system
//     <i>Specifies the maximum number of Pipes that will be used by
//     <i>the USB Host controller in the system simultaneously.
#define USBH3_HC_PIPE_NUM               3

//   <o>Memory Pool Size <640-1048576:4>
//     <i>Specifies the size of the memory pool (in bytes) that
//     <i>the USB Host controller will use for the USB communication data.
//     <i>This size should be at least equal to
//     <i>size of maximum expected packet + 128 bytes.
#define USBH3_HC_MEM_POOL_SIZE          640

//   <e>Relocate Memory Pool
//     <i>Specifies if the memory pool is located at a specific address
//     <i>(via linker script).
#define USBH3_HC_MEM_POOL_RELOC         0

//     <s.128>Memory Pool section name
//       <i>Specifies the section name of the Memory Pool
#define USBH3_HC_MEM_POOL_SECTION_NAME  ".driver.usbh3"
//   </e>

//   <h>OS Resources Settings
//   <i>These settings are used to optimize usage of OS resources.
//     <o>Core Thread Stack Size <64-65536>
#define USBH3_CORE_THREAD_STACK_SIZE    2048

//        Core Thread Priority
#define USBH3_CORE_THREAD_PRIORITY      osPriorityAboveNormal
//   </h>
// </h>
