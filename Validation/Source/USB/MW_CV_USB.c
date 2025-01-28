/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Common
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USB.c
 * Purpose: MDK Middleware - Component Validation - USB Common - module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"
#if     ((MW_CV_USBD != 0U) || (MW_CV_USBH != 0U))

#include "MW_CV_USB.h"


const uint8_t MW_CV_USB_STATUS_VAL_TO_STR[256] = {
   0U,  1U,  2U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U,
   3U,  4U,  5U,  6U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U,
   7U,  8U,  9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 21U, 21U, 21U, 21U,
  19U, 20U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U,
  21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U,
  21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U,
  21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U,
  21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U,
  21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U,
  21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U,
  21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U,
  21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U,
  21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U,
  21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U,
  21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U,
  21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U, 21U };

const char *MW_CV_USB_STATUS_STR[22] = {
  "usbOK",
  "usbTimeout",
  "usbInvalidParameter",
  "usbThreadError",
  "usbTimerError",
  "usbSemaphoreError",
  "usbMutexError",
  "usbControllerError",
  "usbDeviceError",
  "usbDriverError",
  "usbDriverBusy",
  "usbMemoryError",
  "usbNotConfigured",
  "usbClassErrorADC",
  "usbClassErrorCDC",
  "usbClassErrorHID",
  "usbClassErrorMSC",
  "usbClassErrorCustom",
  "usbUnsupportedClass",
  "usbTransferStall",
  "usbTransferError",
  "usbUnknownError"
};

const char *MW_CV_USB_SPEED_STR[3] = {
  "USB_SPEED_LOW",
  "USB_SPEED_FULL",
  "USB_SPEED_HIGH"
};

const char *MW_CV_USB_BOOL_STR[2] = {
  "false",
  "true"
};

#endif
