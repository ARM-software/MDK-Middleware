/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USB_Debug.c
 * Purpose: USB Host/Device Debug Configuration
 * Rev.:    V1.0.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <e>USB Host Debug
// <i>Enable USB Host debugging with Event Recorder
#define USBH_DEBUG_EVR                  1

//   <o>Core <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure USB Host Core event recording
#define USBH_DEBUG_EVR_Core             3

//   <o>Driver <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure USB Host Driver event recording
#define USBH_DEBUG_EVR_Driver           1

//   <o>Custom Class <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure USB Host Custom Class event recording
#define USBH_DEBUG_EVR_CC               1

//   <o>Communication Device Class (CDC) <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure USB Host Communication Device Class event recording
#define USBH_DEBUG_EVR_CDC              1

//   <o>Human Interface Device class (HID) <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure USB Host Human Interface Device class event recording
#define USBH_DEBUG_EVR_HID              1

//   <o>Mass Storage Class (MSC) <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure USB Host Mass Storage Class event recording
#define USBH_DEBUG_EVR_MSC              3
// </e>

// <e>USB Device Debug
// <i>Enable USB Device debugging with Event Recorder
#define USBD_DEBUG_EVR                  0

//   <o>Core <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure USB Device Core event recording
#define USBD_DEBUG_EVR_Core             1

//   <o>Driver <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure USB Device Driver event recording
#define USBD_DEBUG_EVR_Driver           1

//   <o>Custom Class <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure USB Device Custom Class event recording
#define USBD_DEBUG_EVR_CC               1

//   <o>Audio Device Class (ADC) <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure USB Device Audio Device Class event recording
#define USBD_DEBUG_EVR_ADC              1

//   <o>Communication Device Class (CDC) <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure USB Device Communication Device Class event recording
#define USBD_DEBUG_EVR_CDC              1

//   <o>Human Interface Device class (HID) <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure USB Device Human Interface Device class event recording
#define USBD_DEBUG_EVR_HID              1

//   <o>Mass Storage Class (MSC) <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure USB Device Mass Storage Class event recording
#define USBD_DEBUG_EVR_MSC              1
// </e>

//------------- <<< end of configuration section >>> ---------------------------


#include "RTE_Components.h"

#ifdef    RTE_USB_Core_Debug

#ifdef    RTE_USB_Device_0
#include "usbd_debug.h"
#endif

#ifdef    RTE_USB_Host_0
#include "usbh_debug.h"
#endif

#endif // RTE_USB_Core_Debug
