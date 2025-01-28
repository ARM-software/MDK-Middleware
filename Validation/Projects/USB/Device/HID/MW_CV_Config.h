/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_Config.h
 * Purpose: MDK Middleware - Component Validation - configuration definitions
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_CONFIG_H_
#define MW_CV_CONFIG_H_

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
//------ With VS Code: Open Preview for Configuration Wizard -------------------

// <h> Network
//   <i> Network Component Validation Settings
//   <e0.0> BSD Socket
//     <i> Enable/disable BSD socket validation
//     <q1> Socket API
//       <i> Enable/disable BSD socket API interface validation
//     <q2> Socket operation
//       <i> Enable/disable BSD sockets operation validation
//   </e>
#define MW_CV_NET                      0
#define MW_CV_NET_BSD_API              0
#define MW_CV_NET_BSD_OPERATION        0

//   <s.15> SockServer address
//     <i> Static IPv4 Address of SockServer in text representation
#define MW_CV_NET_SOCKSERVER_IP        "192.168.0.100"
// </h>

// <h> USB
//   <i> USB Components Validation Settings
//   <e0.0> USB Device
//     <i> Enable/disable USB Device validation
//     <q1> Core
//       <i> Enable/disable USB Device Core validation
//     <q2> Communication Device Class - Abstract Control Model (CDC ACM)
//       <i> Enable/disable USB Device Communication Device Class - Abstract Control Model validation
//     <q3> Human Interface Device (HID)
//       <i> Enable/disable USB Device Human Interface Device validation
//     <q4> Mass Storage Class (MSC)
//       <i> Enable/disable USB Device Mass Storage Class validation
//   </e>
#define MW_CV_USBD                      1
#define MW_CV_USBD_CORE                 0
#define MW_CV_USBD_CDC_ACM              0
#define MW_CV_USBD_HID                  1
#define MW_CV_USBD_MSC                  0

//   <e0.0> USB Host
//     <i> Enable/disable USB Host validation
//     <q1> Core
//       <i> Enable/disable USB Host Core validation
//     <q2> Communication Device Class - Abstract Control Model (CDC ACM)
//       <i> Enable/disable USB Host Communication Device Class - Abstract Control Model validation
//     <q3> Human Interface Device (HID) class
//       <i> Enable/disable USB Host Human Interface Device validation
//     <q4> Mass Storage Class (MSC)
//       <i> Enable/disable USB Host Mass Storage Class validation
//     <q5> Mass Storage Class (MSC) Performance
//       <i> Enable/disable USB Host Mass Storage Class performance measurement
//   </e>
#define MW_CV_USBH                      0
#define MW_CV_USBH_CORE                 0
#define MW_CV_USBH_CDC_ACM              0
#define MW_CV_USBH_HID                  0
#define MW_CV_USBH_MSC                  0
#define MW_CV_USBH_MSC_PERFORMANCE      0

// Time available for user operation of connect/disconnect (in seconds)
#define MW_CV_USB_TIMEOUT_IN_SEC        30

// Size of test file used for validation and performance tests (in MB)
#define MW_CV_USB_TEST_FILE_SIZE_IN_MB  10

#endif // MW_CV_CONFIG_H_
