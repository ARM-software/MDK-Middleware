/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2022 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_debug.h
 * Purpose: USB Host (USBH) - Debug
 *----------------------------------------------------------------------------*/

#include <stdint.h>

#include "usbh_config_dbg.h"
#include "usbh_evr.h"

void USBH_Debug_Initialize (uint8_t ctrl) {
  static uint8_t executed = 0U;

  (void)ctrl;

  if (executed == 0U) {
    EventRecorderDisable(EventRecordAll,              EvtCompNo_USBH_Core,   EvtCompNo_USBH_MSC);
#if defined(USBH_DEBUG_EVR) && (USBH_DEBUG_EVR != 0)
    EventRecorderEnable (USBH_DEBUG_EVR_Core_Level,   EvtCompNo_USBH_Core,   EvtCompNo_USBH_Core);
    EventRecorderEnable (USBH_DEBUG_EVR_Driver_Level, EvtCompNo_USBH_Driver, EvtCompNo_USBH_Driver);
    EventRecorderEnable (USBH_DEBUG_EVR_CC_Level,     EvtCompNo_USBH_CC,     EvtCompNo_USBH_CC);
    EventRecorderEnable (USBH_DEBUG_EVR_CDC_Level,    EvtCompNo_USBH_CDC,    EvtCompNo_USBH_CDC);
    EventRecorderEnable (USBH_DEBUG_EVR_HID_Level,    EvtCompNo_USBH_HID,    EvtCompNo_USBH_HID);
    EventRecorderEnable (USBH_DEBUG_EVR_MSC_Level,    EvtCompNo_USBH_MSC,    EvtCompNo_USBH_MSC);
#endif
    executed = 1U;
  }
}
