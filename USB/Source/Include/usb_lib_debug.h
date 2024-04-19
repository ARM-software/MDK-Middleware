/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB
 * Copyright (c) 2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usb_lib_debug.h
 * Purpose: Library debug definition
 *----------------------------------------------------------------------------*/

#ifndef  __USB_LIB_DEBUG_H__
#define  __USB_LIB_DEBUG_H__

#include "USB_Debug.h"

#ifdef   _RTE_
#include "RTE_Components.h"

#ifndef  USBD_DEBUG
#if      (defined(USBD_DEBUG_EVR) && (USBD_DEBUG_EVR == 1))
#define  USBD_DEBUG      1
#else
#define  USBD_DEBUG      0
#endif
#endif

#ifndef  USBH_DEBUG
#if      (defined(USBH_DEBUG_EVR) && (USBH_DEBUG_EVR == 1))
#define  USBH_DEBUG      1
#else
#define  USBH_DEBUG      0
#endif
#endif

#endif   // _RTE_

#endif   // __USB_LIB_DEBUG_H__
