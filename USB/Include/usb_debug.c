/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB
 * Copyright (c) 2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usb_debug.c
 * Purpose: USB Host/Device Debug
 * Rev.:    V1.0.0
 *----------------------------------------------------------------------------*/

#include "USB_Debug.h"

#include "RTE_Components.h"

#if      (defined(RTE_USB_Device_0) && defined(USBD_DEBUG_EVR) && (USBD_DEBUG_EVR == 1))
#include "usbd_debug.h"
#endif

#if      (defined(RTE_USB_Host_0)   && defined(USBH_DEBUG_EVR) && (USBH_DEBUG_EVR == 1))
#include "usbh_debug.h"
#endif
