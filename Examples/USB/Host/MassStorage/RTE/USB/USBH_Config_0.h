/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBH_Config_0.h
 * Purpose: USB Host Configuration
 * Rev.:    V8.0.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>USB Host 0
//   <o>Connect to hardware via Driver_USBH# <0-255>
//   <i>Select driver control block for hardware interface.
#define USBH0_HC_NUM                    2

//   <h>Controller Interface Settings

//     <o>Controller Interface
//     <i> Selects the USB Host Controller Interface
//       <0=> Custom <1=> OHCI <2=> EHCI
//       <i>Custom Controller Interface is used for any non-standard USB Host
//       <i>Controller.
#define USBH0_HC_IF                     2

//     <h>Custom Host Controller Interface
//       <o>Maximum Port Power Consumption<2-500:2>
//         <i>Specifies the maximum power consumption per port (in mA)
#define USBH0_HC_POWER                  500

//       <o>Maximum Pipes in system
//         <i>Maximum number of Pipes that will be used by Custom Controller
//         <i>in system simultaneously.
#define USBH0_HC_PIPE_NUM               3

//       <o>Memory Pool Size <512-1048576:4>
//       <i>Specify size of memory pool (in bytes) that the USB Host Controller
//       <i>will use for USB communication data.
#define USBH0_HC_MEM_POOL_SIZE          512

//       <e>Relocate Memory Pool
//         <i>Locate the Memory Pool at a specific address.
#define USBH0_HC_MEM_POOL_RELOC         0

//         <o>Memory Pool Address <0-0xFFFFFE00:0x200>
//           <i>Start address of the Memory Pool.
#define USBH0_HC_MEM_POOL_ADDR          0x00000000
//       </e>
//     </h>

//     <h>Open Host Controller Interface (OHCI)
//       <o>Memory-mapped OHCI Host Controller registers base address
//         <i>This setting applies for OHCI Controller Interface.
#define USBH0_HC_OHCI_BASE_ADDRESS      0x5000C000

//       <o>Maximum Port Power Consumption<2-500:2>
//         <i>Specifies the maximum power consumption per port (in mA)
#define USBH0_HC_OHCI_POWER             500

//       <o>Maximum Pipes in system
//         <i>Maximum number of Pipes that will be used by OHCI Controller
//         <i>in system simultaneously.
//         <i>This setting affects memory allocated by OHCI controller.
#define USBH0_HC_OHCI_PIPE_NUM          3

//       <o>Memory Pool Size <512-1048576:4>
//       <i>Specify size of memory pool (in bytes) that the USB Host Controller
//       <i>will use for USB communication data.
#define USBH0_HC_OHCI_MEM_POOL_SIZE     512

//       <e>Relocate Memory Pool
//         <i>Locate the Memory Pool at a specific address.
#define USBH0_HC_OHCI_MEM_POOL_RELOC    0

//         <o>Memory Pool Address <0-0xFFFFFE00:0x200>
//           <i>Start address of the Memory Pool.
#define USBH0_HC_OHCI_MEM_POOL_ADDR     0x00000000
//       </e>
//     </h>

//     <h>Enhanced Host Controller Interface (EHCI)
//       <o>Memory-mapped EHCI Host Controller registers base address
//         <i>These settings apply for EHCI Controller Interface.
#define USBH0_HC_EHCI_BASE_ADDRESS      0x402E0300

//       <o>Maximum Port Power Consumption<2-500:2>
//         <i>Specifies the maximum power consumption per port (in mA)
#define USBH0_HC_EHCI_POWER             500

//       <o>Maximum Pipes in system
//         <i>Maximum number of Pipes that will be used by EHCI Controller
//         <i>in system simultaneously.
//         <i>This setting affects memory allocated by EHCI controller.
#define USBH0_HC_EHCI_PIPE_NUM          3

//       <o>Memory Pool Size <512-1048576:4>
//       <i>Specify size of memory pool (in bytes) that the USB Host Controller
//       <i>will use for USB communication data.
#define USBH0_HC_EHCI_MEM_POOL_SIZE     4096

//       <e>Relocate Memory Pool
//         <i>Locate the Memory Pool at a specific address.
#define USBH0_HC_EHCI_MEM_POOL_RELOC    0

//         <o>Memory Pool Address <0-0xFFFFFE00:0x200>
//           <i>Start address of the Memory Pool.
#define USBH0_HC_EHCI_MEM_POOL_ADDR     0x00000000
//       </e>
//     </h>
//   </h>

//   <h>OS Resources Settings
//   <i>These settings are used to optimize usage of OS resources.
//     <o>Core Thread Stack Size <64-65536>
#define USBH0_CORE_THREAD_STACK_SIZE    2048

//        Core Thread Priority
#define USBH0_CORE_THREAD_PRIORITY      osPriorityAboveNormal

//   </h>
// </h>
