/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2022 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_debug.h
 * Purpose: USB Device (USBD) - Debug
 *----------------------------------------------------------------------------*/

#include <stdint.h>

#include "usbd_config_dbg.h"
#include "usbd_evr.h"

void USBD_Debug_Initialize (uint8_t device) {
  static uint8_t executed = 0U;

  (void)device;

  if (executed == 0U) {
    EventRecorderDisable(EventRecordAll,              EvtCompNo_USBD_Core,   EvtCompNo_USBD_MSC);
#if defined(USBD_DEBUG_EVR) && (USBD_DEBUG_EVR != 0)
    EventRecorderEnable (USBD_DEBUG_EVR_Core_Level,   EvtCompNo_USBD_Core,   EvtCompNo_USBD_Core);
    EventRecorderEnable (USBD_DEBUG_EVR_Driver_Level, EvtCompNo_USBD_Driver, EvtCompNo_USBD_Driver);
    EventRecorderEnable (USBD_DEBUG_EVR_CC_Level,     EvtCompNo_USBD_CC,     EvtCompNo_USBD_CC);
    EventRecorderEnable (USBD_DEBUG_EVR_ADC_Level,    EvtCompNo_USBD_ADC,    EvtCompNo_USBD_ADC);
    EventRecorderEnable (USBD_DEBUG_EVR_CDC_Level,    EvtCompNo_USBD_CDC,    EvtCompNo_USBD_CDC);
    EventRecorderEnable (USBD_DEBUG_EVR_HID_Level,    EvtCompNo_USBD_HID,    EvtCompNo_USBD_HID);
    EventRecorderEnable (USBD_DEBUG_EVR_MSC_Level,    EvtCompNo_USBD_MSC,    EvtCompNo_USBD_MSC);
#endif
    executed = 1U;
  }
}
