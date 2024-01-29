/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usb_ms.h
 * Purpose: USB Microsoft Descriptors Specific Definitions
 *----------------------------------------------------------------------------*/

#ifndef __USB_MS_H
#define __USB_MS_H

#include <stdint.h>

#include "usb_compiler.h"


// USB Microsoft String Descriptor Index
#define USB_MS_OS_STRING_DESCRIPTOR_IDX                 0xEEU

// USB Microsoft OS Feature Descriptor Index
#define USB_MS_OS_FEAT_EXT_COMPAT_ID_DESCRIPTOR_IDX     4U      // Extended Compat ID Descriptor Index
#define USB_MS_OS_FEAT_EXT_PROP_DESCRIPTOR_IDX          5U      // Extended Properties Descriptor Index


/// Microsoft OS String Descriptor
typedef __PACKED_STRUCT {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  qwSignature[14];
  uint8_t  bMS_VendorCode;
  uint8_t  bPad;
} USB_MS_OS_STRING_DESCRIPTOR;

/// Microsoft OS Feature Descriptor: Extended Compat ID Header
typedef __PACKED_STRUCT {
  uint32_t dwLength;
  uint16_t bcdVersion;
  uint16_t wIndex;
  uint8_t  bCount;
  uint8_t  reserved[7];
} USB_MS_OS_FEAT_EXT_COMPAT_ID_HEADER;

/// Microsoft OS Feature Descriptor: Extended Compat ID Function
typedef __PACKED_STRUCT {
  uint8_t  bFirstInterfaceNum;
  uint8_t  reserved1;
  uint8_t  compatibleID[8];
  uint8_t  subCompatibleID[8];
  uint8_t  reserved2[6];
} USB_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION;

/// Microsoft OS Feature Descriptor: Extended Properties Header
typedef __PACKED_STRUCT {
  uint32_t dwLength;
  uint16_t bcdVersion;
  uint16_t wIndex;
  uint16_t wCount;
} USB_MS_OS_FEAT_EXT_PROP_HEADER;

#endif  // __USB_MS_H
