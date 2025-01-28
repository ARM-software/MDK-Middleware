/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_Config_MSC_0.h
 * Purpose: USB Device Mass Storage Class (MSC) Configuration
 * Rev.:    V5.2.1
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
//------ With VS Code: Open Preview for Configuration Wizard -------------------

// <h>USB Device: Mass Storage Class (MSC) 0
//   <o>Assign Device Class to USB Device # <0-3>
//   <i>Select USB Device that is used for this Device Class instance
#define USBD_MSC0_DEV                   0

//   <h>Bulk Endpoint Settings
//   <i>By default, the settings match the first USB Class instance in a USB Device.
//   <i>Endpoint conflicts are flagged by compile-time error messages.
//
//     <o.0..3>Bulk IN Endpoint Number
//               <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//       <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
#define USBD_MSC0_EP_BULK_IN            1

//     <o.0..3>Bulk OUT Endpoint Number
//               <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//       <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
#define USBD_MSC0_EP_BULK_OUT           1


//     <h>Endpoint Settings
//       <i>Parameters are used to create Endpoint Descriptors
//       <i>and for memory allocation in the USB component.
//
//       <h>Full/Low-speed (High-speed disabled)
//       <i>Parameters apply when High-speed is disabled in USBD_Config_n.h
//         <o.0..6>Maximum Endpoint Packet Size (in bytes) <8=>8 <16=>16 <32=>32 <64=>64
//         <i>Specifies the physical packet size used for information exchange.
//         <i>Maximum value is 64.
#define USBD_MSC0_WMAXPACKETSIZE        64

//       </h>

//       <h>High-speed
//       <i>Parameters apply when High-speed is enabled in USBD_Config_n.h
//
//         <o.0..9>Maximum Endpoint Packet Size (in bytes) <512=>512
//         <i>Specifies the physical packet size used for information exchange.
//         <i>Only available value is 512.
#define USBD_MSC0_HS_WMAXPACKETSIZE     512

//         <o.0..7>Maximum NAK Rate <0-255>
//         <i>Specifies the interval in which Bulk Endpoint can NAK.
//         <i>Value of 0 indicates that Bulk Endpoint never NAKs.
#define USBD_MSC0_HS_BINTERVAL          0

//       </h>
//     </h>
//   </h>

//   <h>Mass Storage Class Settings
//   <i>Parameters are used to create USB Descriptors and for memory allocation
//   <i>in the USB component.
//
//     <s.126>MSC Interface String
#define USBD_MSC0_STR_DESC_RAW          "USB_MSC0"

//     <o0.0..1>Maximum Number of Logical Units (LUN)
//     <i>For single LUN use 1, otherwise 2-4.
//       <0=>1 <1=>2 <2=>3 <3=>4
#define USBD_MSC0_MAX_LUN               0

//     <h>Logical Unit 0 Inquiry Data
//     <i>Data returned on Get Inquiry for Logical Unit 0 request from the USB Host.
//
//       <s0.8>Vendor Identification
//       <s1.16>Product Identification
//       <s2.4>Product Revision Level
#define USBD_MSC0_INQUIRY_DATA          "Keil    "         \
                                        "Disk 0          " \
                                        "1.0 "

//     </h>

//     <h>Logical Unit 1 Inquiry Data
//     <i>Data returned on Get Inquiry for Logical Unit 1 request from the USB Host.
//
//       <s0.8>Vendor Identification
//       <s1.16>Product Identification
//       <s2.4>Product Revision Level
#define USBD_MSC0_LUN1_INQUIRY_DATA     "Keil    "         \
                                        "Disk LUN 1      " \
                                        "1.0 "

//     </h>

//     <h>Logical Unit 2 Inquiry Data
//     <i>Data returned on Get Inquiry for Logical Unit 2 request from the USB Host.
//
//       <s0.8>Vendor Identification
//       <s1.16>Product Identification
//       <s2.4>Product Revision Level
#define USBD_MSC0_LUN2_INQUIRY_DATA     "Keil    "         \
                                        "Disk LUN 2      " \
                                        "1.0 "

//     </h>

//     <h>Logical Unit 3 Inquiry Data
//     <i>Data returned on Get Inquiry for Logical Unit 3 request from the USB Host.
//
//       <s0.8>Vendor Identification
//       <s1.16>Product Identification
//       <s2.4>Product Revision Level
#define USBD_MSC0_LUN3_INQUIRY_DATA     "Keil    "         \
                                        "Disk LUN 3      " \
                                        "1.0 "

//     </h>

//     <o>Maximum Mass Storage Device Bulk Buffer Size <0x00000000-0xFFFFFFFF>
//     <i>Specifies size of buffer used for bulk transfers.
//     <i>It should be at least as big as bulk maximum packet size.
#define USBD_MSC0_BULK_BUF_SIZE         512

//   </h>

//   <h>OS Resources Settings
//   <i>These settings are used to optimize usage of OS resources.
//     <o>Mass Storage Device Class Thread Stack Size <64-65536>
#define USBD_MSC0_THREAD_STACK_SIZE     512

//        Mass Storage Device Class Thread Priority
#define USBD_MSC0_THREAD_PRIORITY       osPriorityAboveNormal

//   </h>
// </h>
