/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_config_dbg.h
 * Purpose: USB Host (USBH) - Debug Configuration
 *----------------------------------------------------------------------------*/

#ifndef __USBH_CONFIG_DBG_H__
#define __USBH_CONFIG_DBG_H__

#include "EventRecorder.h"

#define USBH_DEBUG_EVR_VAL0            (EventRecordNone)
#define USBH_DEBUG_EVR_VAL1            (EventRecordError)
#define USBH_DEBUG_EVR_VAL2            (EventRecordError + EventRecordAPI)
#define USBH_DEBUG_EVR_VAL3            (EventRecordAll)

#define USBH_DEBUG_EVR_TO_LVL(x)        USBH_DEBUG_EVR_TO_LVL_(x)
#define USBH_DEBUG_EVR_TO_LVL_(x)      (USBH_DEBUG_EVR_VAL##x)

#define USBH_DEBUG_EVR_Core_Level       USBH_DEBUG_EVR_TO_LVL(USBH_DEBUG_EVR_Core)
#define USBH_DEBUG_EVR_Driver_Level     USBH_DEBUG_EVR_TO_LVL(USBH_DEBUG_EVR_Driver)
#define USBH_DEBUG_EVR_CC_Level         USBH_DEBUG_EVR_TO_LVL(USBH_DEBUG_EVR_CC)
#define USBH_DEBUG_EVR_CDC_Level        USBH_DEBUG_EVR_TO_LVL(USBH_DEBUG_EVR_CDC)
#define USBH_DEBUG_EVR_HID_Level        USBH_DEBUG_EVR_TO_LVL(USBH_DEBUG_EVR_HID)
#define USBH_DEBUG_EVR_MSC_Level        USBH_DEBUG_EVR_TO_LVL(USBH_DEBUG_EVR_MSC)

#endif  /* __USBH_CONFIG_DBG_H__ */
