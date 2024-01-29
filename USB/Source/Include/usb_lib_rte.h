/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB
 * Copyright (c) 2021 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usb_lib_rte.h
 * Purpose: RTE definition translation for library
 *----------------------------------------------------------------------------*/

#ifndef  __USB_LIB_RTE_H__
#define  __USB_LIB_RTE_H__

#ifdef   _RTE_
#include "RTE_Components.h"

#ifndef  USBD_DEBUG
#ifdef   RTE_USB_Core_Debug
#define  USBD_DEBUG      1
#else
#define  USBD_DEBUG      0
#endif
#endif

#ifndef  USBH_DEBUG
#ifdef   RTE_USB_Core_Debug
#define  USBH_DEBUG      1
#else
#define  USBH_DEBUG      0
#endif
#endif

#endif   // _RTE_

#endif   // __USB_LIB_RTE_H__
