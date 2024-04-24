/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_debug.c
 * Purpose: USB Device (USBD) - Debug
 *----------------------------------------------------------------------------*/

#include <stdint.h>

#include "USB_Debug.h"

#include "RTE_Components.h"

#include "usbd_evr.h"

#ifdef    RTE_CMSIS_View_EventRecorder
#include "EventRecorder.h"
#endif

#if ((USBD_DEBUG_EVR == 1) && !defined(RTE_CMSIS_View_EventRecorder))
#error USB Device debug events require CMSIS-View:Event Recorder component located in the ARM::CMSIS-View pack!
#endif

#if (USBD_DEBUG_EVR == 1)

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

void USBD_Debug_Initialize (uint8_t device) {
#ifdef RTE_CMSIS_View_EventRecorder
  static uint8_t executed = 0U;

  (void)device;

  if (executed == 0U) {
    EventRecorderDisable(EventRecordAll,              EvtCompNo_USBD_Core,   EvtCompNo_USBD_MSC);
    EventRecorderEnable (USBD_DEBUG_EVR_Core_Level,   EvtCompNo_USBD_Core,   EvtCompNo_USBD_Core);
    EventRecorderEnable (USBD_DEBUG_EVR_Driver_Level, EvtCompNo_USBD_Driver, EvtCompNo_USBD_Driver);
    EventRecorderEnable (USBD_DEBUG_EVR_CC_Level,     EvtCompNo_USBD_CC,     EvtCompNo_USBD_CC);
    EventRecorderEnable (USBD_DEBUG_EVR_ADC_Level,    EvtCompNo_USBD_ADC,    EvtCompNo_USBD_ADC);
    EventRecorderEnable (USBD_DEBUG_EVR_CDC_Level,    EvtCompNo_USBD_CDC,    EvtCompNo_USBD_CDC);
    EventRecorderEnable (USBD_DEBUG_EVR_HID_Level,    EvtCompNo_USBD_HID,    EvtCompNo_USBD_HID);
    EventRecorderEnable (USBD_DEBUG_EVR_MSC_Level,    EvtCompNo_USBD_MSC,    EvtCompNo_USBD_MSC);
    executed = 1U;
  }
#endif
}
#endif
