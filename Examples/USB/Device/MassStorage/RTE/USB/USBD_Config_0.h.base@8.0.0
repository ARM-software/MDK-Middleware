/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_Config_0.h
 * Purpose: USB Device Configuration
 * Rev.:    V8.0.0
 *------------------------------------------------------------------------------
 * Use the following configuration settings in the Device Class configuration
 * files to assign a Device Class to this USB Device 0.
 *
 * Configuration Setting               Value
 * ---------------------               -----
 * Assign Device Class to USB Device # = 0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
//------ With VS Code: Open Preview for Configuration Wizard -------------------

// <h>USB Device 0
//   <y>Connect to hardware via Driver_USBD#
//   <i>Select driver control block for hardware interface.
#define USBD0_PORT                      0

//   <o.0>High-speed
//   <i>Enable High-speed functionality (if device supports it).
#define USBD0_HS                        0

//   <h>Device Settings
//   <i>These settings are used to create the Device Descriptor
//     <o>Max Endpoint 0 Packet Size
//     <i>Maximum packet size for Endpoint 0 (bMaxPacketSize0).
//       <8=>8 Bytes <16=>16 Bytes <32=>32 Bytes <64=>64 Bytes
#define USBD0_MAX_PACKET0               8

//     <o.0..15>Vendor ID <0x0000-0xFFFF>
//     <i>Vendor ID assigned by USB-IF (idVendor).
#define USBD0_DEV_DESC_IDVENDOR         0xC251

//     <o.0..15>Product ID <0x0000-0xFFFF>
//     <i>Product ID assigned by manufacturer (idProduct).
#define USBD0_DEV_DESC_IDPRODUCT        0x0000

//     <o.0..15>Device Release Number <0x0000-0xFFFF>
//     <i>Device Release Number in binary-coded decimal (bcdDevice).
#define USBD0_DEV_DESC_BCDDEVICE        0x0100

//   </h>

//   <h>Configuration Settings
//   <i>These settings are used to create the Configuration Descriptor.
//     <o.6>Power
//     <i>Default Power Setting (D6: of bmAttributes).
//       <0=>Bus-powered
//       <1=>Self-powered
//     <o.5>Remote Wakeup
//     <i>Configuration support for Remote Wakeup (D5: of bmAttributes).
#define USBD0_CFG_DESC_BMATTRIBUTES     0x80

//     <o.0..7>Maximum Power Consumption (in mA) <0-510><#/2>
//     <i>Maximum Power Consumption of USB Device from bus in this
//     <i>specific configuration when device is fully operational (bMaxPower).
#define USBD0_CFG_DESC_BMAXPOWER        250

//   </h>

//   <h>String Settings
//   <i>These settings are used to create the String Descriptor.
//     <o.0..15>Language ID <0x0000-0xFCFF>
//     <i>English (United States) = 0x0409.
#define USBD0_STR_DESC_LANGID           0x0409

//     <s.126>Manufacturer String
//     <i>String Descriptor describing Manufacturer.
#define USBD0_STR_DESC_MAN_RAW          "Keil Software"

//     <s.126>Product String
//     <i>String Descriptor describing Product.
#define USBD0_STR_DESC_PROD_RAW         "Keil USB Device 0"

//     <e.0>Serial Number String
//     <i>Enable Serial Number String.
//     <i>If disabled Serial Number String will not be assigned to USB Device.
#define USBD0_STR_DESC_SER_EN           1

//       <s.126>Default value
//       <i>Default device's Serial Number String.
#define USBD0_STR_DESC_SER_RAW          "0001A0000000"

//       <o.0..7>Maximum Length (in characters) <0-126>
//       <i>Specifies the maximum number of Serial Number String characters that can be set at run-time.
//       <i>Maximum value is 126. Use value 0 to disable RAM allocation for string.
#define USBD0_STR_DESC_SER_MAX_LEN      16

//     </e>
//   </h>

//   <h>Microsoft OS Descriptors Settings
//   <i>These settings are used to create the Microsoft OS Descriptors.
//     <e.0>OS String
//     <i>Enable creation of Microsoft OS String and Extended Compat ID OS Feature Descriptors.
#define USBD0_OS_DESC_EN                1

//       <o.0..7>Vendor Code <0x01-0xFF>
//       <i>Specifies Vendor Code used to retrieve OS Feature Descriptors.
#define USBD0_OS_DESC_VENDOR_CODE       0x01

//     </e>
//   </h>

//   <o>Control Transfer Buffer Size <64-65536:64>
//   <i>Specifies size of buffer used for Control Transfers.
//   <i>It should be at least as big as maximum packet size for Endpoint 0.
#define USBD0_EP0_BUF_SIZE              128

//   <h>Data Buffers
//     <e>Locate
//       <i>Specifies if the USB data buffers are located in a specific memory
//       <i>(via the linker script).
#define USBD0_BUF_MEM_LOCATE            0

//       <s.128>Section Name
//         <i>Specifies the section name for the USB data buffers
//         <i>(for positioning via the linker script).
#define USBD0_BUF_MEM_SECTION_NAME      ".driver.usbd0"

//     </e>
//   </h>

//   <h>OS Resources Settings
//   <i>These settings are used to optimize usage of OS resources.
//     <o>Core Thread Stack Size <64-65536>
#define USBD0_CORE_THREAD_STACK_SIZE    1024

//        Core Thread Priority
#define USBD0_CORE_THREAD_PRIORITY      osPriorityAboveNormal

//   </h>
// </h>
