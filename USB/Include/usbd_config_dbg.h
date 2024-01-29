/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_config_dbg.h
 * Purpose: USB Device (USBD) - Debug Configuration
 *----------------------------------------------------------------------------*/

#ifndef __USBD_CONFIG_DBG_H__
#define __USBD_CONFIG_DBG_H__

#include "EventRecorder.h"

#define USBD_DEBUG_EVR_VAL0            (EventRecordNone)
#define USBD_DEBUG_EVR_VAL1            (EventRecordError)
#define USBD_DEBUG_EVR_VAL2            (EventRecordError + EventRecordAPI)
#define USBD_DEBUG_EVR_VAL3            (EventRecordAll)

#define USBD_DEBUG_EVR_TO_LVL(x)        USBD_DEBUG_EVR_TO_LVL_(x)
#define USBD_DEBUG_EVR_TO_LVL_(x)      (USBD_DEBUG_EVR_VAL##x)

#define USBD_DEBUG_EVR_Core_Level       USBD_DEBUG_EVR_TO_LVL(USBD_DEBUG_EVR_Core)
#define USBD_DEBUG_EVR_Driver_Level     USBD_DEBUG_EVR_TO_LVL(USBD_DEBUG_EVR_Driver)
#define USBD_DEBUG_EVR_CC_Level         USBD_DEBUG_EVR_TO_LVL(USBD_DEBUG_EVR_CC)
#define USBD_DEBUG_EVR_ADC_Level        USBD_DEBUG_EVR_TO_LVL(USBD_DEBUG_EVR_ADC)
#define USBD_DEBUG_EVR_CDC_Level        USBD_DEBUG_EVR_TO_LVL(USBD_DEBUG_EVR_CDC)
#define USBD_DEBUG_EVR_HID_Level        USBD_DEBUG_EVR_TO_LVL(USBD_DEBUG_EVR_HID)
#define USBD_DEBUG_EVR_MSC_Level        USBD_DEBUG_EVR_TO_LVL(USBD_DEBUG_EVR_MSC)

#endif  /* __USBD_CONFIG_DBG_H__ */
