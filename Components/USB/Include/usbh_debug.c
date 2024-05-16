/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_debug.c
 * Purpose: USB Host (USBH) - Debug
 *----------------------------------------------------------------------------*/

#include <stdint.h>

#include "USB_Debug.h"

#include "RTE_Components.h"

#include "usbh_evr.h"

#ifdef    RTE_CMSIS_View_EventRecorder
#include "EventRecorder.h"
#endif

#if ((USBH_DEBUG_EVR == 1) && !defined(RTE_CMSIS_View_EventRecorder))
#error USB Host debug events require CMSIS-View:Event Recorder component located in the ARM::CMSIS-View pack!
#endif

#if (USBH_DEBUG_EVR == 1)

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

void USBH_Debug_Initialize (uint8_t ctrl) {
#ifdef RTE_CMSIS_View_EventRecorder
  static uint8_t executed = 0U;

  (void)ctrl;

  if (executed == 0U) {
    EventRecorderDisable(EventRecordAll,              EvtCompNo_USBH_Core,   EvtCompNo_USBH_MSC);
    EventRecorderEnable (USBH_DEBUG_EVR_Core_Level,   EvtCompNo_USBH_Core,   EvtCompNo_USBH_Core);
    EventRecorderEnable (USBH_DEBUG_EVR_Driver_Level, EvtCompNo_USBH_Driver, EvtCompNo_USBH_Driver);
    EventRecorderEnable (USBH_DEBUG_EVR_CC_Level,     EvtCompNo_USBH_CC,     EvtCompNo_USBH_CC);
    EventRecorderEnable (USBH_DEBUG_EVR_CDC_Level,    EvtCompNo_USBH_CDC,    EvtCompNo_USBH_CDC);
    EventRecorderEnable (USBH_DEBUG_EVR_HID_Level,    EvtCompNo_USBH_HID,    EvtCompNo_USBH_HID);
    EventRecorderEnable (USBH_DEBUG_EVR_MSC_Level,    EvtCompNo_USBH_MSC,    EvtCompNo_USBH_MSC);
    executed = 1U;
  }
#endif
}
#endif
