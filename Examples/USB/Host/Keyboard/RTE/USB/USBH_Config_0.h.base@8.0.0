/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBH_Config_0.h
 * Purpose: USB Host Configuration
 * Rev.:    V8.0.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
//------ With VS Code: Open Preview for Configuration Wizard -------------------

// <h>USB Host 0
//   <y>Connect to hardware via Driver_USBH#
//   <i>Specifies instance of the Host Controller hardware interface.
#define USBH0_HC_NUM                    0

//   <o>Maximum Port Power Delivery (in mA) <2-500:2>
//     <i>Specifies the maximum power delivery per port.
#define USBH0_HC_POWER                  500

//   <o>Maximum Pipes in System
//     <i>Specifies the maximum number of Pipes that will be used by
//     <i>the USB Host controller in the system simultaneously.
#define USBH0_HC_PIPE_NUM               3

//   <h>Memory Pool
//     <o>Size <640-1048576:4>
//       <i>Specifies the size of the USB memory pool (in bytes) that
//       <i>the USB Host controller will use for the USB communication data.
//       <i>This size should be at least equal to the size
//       <i>of the maximum expected packet + 128 bytes.
#define USBH0_HC_MEM_POOL_SIZE          640

//     <e>Locate
//       <i>Specifies if the USB memory pool is located in a specific memory
//       <i>(via the linker script).
#define USBH0_HC_MEM_POOL_LOCATE        0

//       <s.128>Section Name
//         <i>Specifies the section name for the USB memory pool
//         <i>(for positioning via the linker script).
#define USBH0_HC_MEM_POOL_SECTION_NAME  ".driver.usbh0"

//     </e>
//   </h>

//   <h>OS Resources Settings
//   <i>These settings are used to optimize usage of OS resources.
//     <o>Core Thread Stack Size <64-65536>
#define USBH0_CORE_THREAD_STACK_SIZE    2048

//        Core Thread Priority
#define USBH0_CORE_THREAD_PRIORITY      osPriorityAboveNormal

//   </h>
// </h>
