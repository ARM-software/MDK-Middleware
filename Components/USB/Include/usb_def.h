/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usb_def.h
 * Purpose: USB Definitions
 *----------------------------------------------------------------------------*/

#ifndef __USB_DEF_H
#define __USB_DEF_H

#include <stdint.h>

#include "usb_compiler.h"


// USB Speed
#define USB_SPEED_LOW                    0U
#define USB_SPEED_FULL                   1U
#define USB_SPEED_HIGH                   2U

// USB PID Types
#define USB_PID_RESERVED                 0U
#define USB_PID_OUT                      1U
#define USB_PID_ACK                      2U
#define USB_PID_DATA0                    3U
#define USB_PID_PING                     4U
#define USB_PID_SOF                      5U
#define USB_PID_DATA2                    7U
#define USB_PID_NYET                     6U
#define USB_PID_SPLIT                    8U
#define USB_PID_IN                       9U
#define USB_PID_NAK                     10U
#define USB_PID_DATA1                   11U
#define USB_PID_PRE                     12U
#define USB_PID_ERR                     12U
#define USB_PID_SETUP                   13U
#define USB_PID_STALL                   14U
#define USB_PID_MDATA                   15U

// bmRequestType.Dir
#define USB_REQUEST_HOST_TO_DEVICE      0U
#define USB_REQUEST_DEVICE_TO_HOST      1U

// bmRequestType.Type
#define USB_REQUEST_STANDARD            0U
#define USB_REQUEST_CLASS               1U
#define USB_REQUEST_VENDOR              2U
#define USB_REQUEST_RESERVED            3U

// bmRequestType.Recipient
#define USB_REQUEST_TO_DEVICE           0U
#define USB_REQUEST_TO_INTERFACE        1U
#define USB_REQUEST_TO_ENDPOINT         2U
#define USB_REQUEST_TO_OTHER            3U

/// bmRequestType Definition
typedef struct {
  uint8_t Recipient : 5;                ///< D4..0: Recipient
  uint8_t Type      : 2;                ///< D6..5: Type
  uint8_t Dir       : 1;                ///< D7:    Data Transfer Direction
} USB_REQUEST_TYPE;

// USB Standard Request Codes
#define USB_REQUEST_GET_STATUS           0U
#define USB_REQUEST_CLEAR_FEATURE        1U
#define USB_REQUEST_SET_FEATURE          3U
#define USB_REQUEST_SET_ADDRESS          5U
#define USB_REQUEST_GET_DESCRIPTOR       6U
#define USB_REQUEST_SET_DESCRIPTOR       7U
#define USB_REQUEST_GET_CONFIGURATION    8U
#define USB_REQUEST_SET_CONFIGURATION    9U
#define USB_REQUEST_GET_INTERFACE       10U
#define USB_REQUEST_SET_INTERFACE       11U
#define USB_REQUEST_SYNC_FRAME          12U

// USB GET_STATUS Bit Values
#define USB_GETSTATUS_SELF_POWERED      0x01U
#define USB_GETSTATUS_REMOTE_WAKEUP     0x02U
#define USB_GETSTATUS_ENDPOINT_STALL    0x01U

// USB Standard Feature selectors
#define USB_FEATURE_ENDPOINT_STALL      0U
#define USB_FEATURE_REMOTE_WAKEUP       1U

/// USB Default Control Pipe Setup Packet
typedef struct {
  USB_REQUEST_TYPE bmRequestType;       ///< Characteristics of request
  uint8_t          bRequest;            ///< Specific request
  uint16_t         wValue;              ///< Value according to request
  uint16_t         wIndex;              ///< Index or Offset according to request
  uint16_t         wLength;             ///< Number of bytes to transfer if there is a Data stage
} USB_SETUP_PACKET;


// USB Descriptor Types
#define USB_DEVICE_DESCRIPTOR_TYPE                 1U
#define USB_CONFIGURATION_DESCRIPTOR_TYPE          2U
#define USB_STRING_DESCRIPTOR_TYPE                 3U
#define USB_INTERFACE_DESCRIPTOR_TYPE              4U
#define USB_ENDPOINT_DESCRIPTOR_TYPE               5U
#define USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE       6U
#define USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE     7U
#define USB_INTERFACE_POWER_DESCRIPTOR_TYPE        8U
#define USB_OTG_DESCRIPTOR_TYPE                    9U
#define USB_DEBUG_DESCRIPTOR_TYPE                 10U
#define USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE 11U

// USB Device Classes
#define USB_DEVICE_CLASS_RESERVED                 0x00U
#define USB_DEVICE_CLASS_AUDIO                    0x01U
#define USB_DEVICE_CLASS_COMMUNICATIONS           0x02U
#define USB_DEVICE_CLASS_HUMAN_INTERFACE          0x03U
#define USB_DEVICE_CLASS_MONITOR                  0x04U
#define USB_DEVICE_CLASS_PHYSICAL_INTERFACE       0x05U
#define USB_DEVICE_CLASS_POWER                    0x06U
#define USB_DEVICE_CLASS_PRINTER                  0x07U
#define USB_DEVICE_CLASS_STORAGE                  0x08U
#define USB_DEVICE_CLASS_HUB                      0x09U
#define USB_DEVICE_CLASS_MISCELLANEOUS            0xEFU
#define USB_DEVICE_CLASS_VENDOR_SPECIFIC          0xFFU

// bmAttributes in Configuration Descriptor
#define USB_CONFIG_POWERED_MASK                   0x40U
#define USB_CONFIG_BUS_POWERED                    0x80U
#define USB_CONFIG_SELF_POWERED                   0xC0U
#define USB_CONFIG_REMOTE_WAKEUP                  0x20U

// bMaxPower in Configuration Descriptor
#define USB_CONFIG_POWER_MA(mA)                   ((mA)/2)

// bEndpointAddress in Endpoint Descriptor
#define USB_ENDPOINT_DIRECTION_MASK               0x80U
#define USB_ENDPOINT_OUT(addr)                    ( addr         )
#define USB_ENDPOINT_IN(addr)                     ((addr) | 0x80U)

// bmAttributes in Endpoint Descriptor
#define USB_ENDPOINT_TYPE_MASK                    0x03U
#define USB_ENDPOINT_TYPE_CONTROL                 0x00U
#define USB_ENDPOINT_TYPE_ISOCHRONOUS             0x01U
#define USB_ENDPOINT_TYPE_BULK                    0x02U
#define USB_ENDPOINT_TYPE_INTERRUPT               0x03U
#define USB_ENDPOINT_SYNC_MASK                    0x0CU
#define USB_ENDPOINT_SYNC_NO_SYNCHRONIZATION      0x00U
#define USB_ENDPOINT_SYNC_ASYNCHRONOUS            0x04U
#define USB_ENDPOINT_SYNC_ADAPTIVE                0x08U
#define USB_ENDPOINT_SYNC_SYNCHRONOUS             0x0CU
#define USB_ENDPOINT_USAGE_MASK                   0x30U
#define USB_ENDPOINT_USAGE_DATA                   0x00U
#define USB_ENDPOINT_USAGE_FEEDBACK               0x10U
#define USB_ENDPOINT_USAGE_IMPLICIT_FEEDBACK      0x20U
#define USB_ENDPOINT_USAGE_RESERVED               0x30U

/// USB Standard Device Descriptor
typedef __PACKED_STRUCT {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint16_t bcdUSB;
  uint8_t  bDeviceClass;
  uint8_t  bDeviceSubClass;
  uint8_t  bDeviceProtocol;
  uint8_t  bMaxPacketSize0;
  uint16_t idVendor;
  uint16_t idProduct;
  uint16_t bcdDevice;
  uint8_t  iManufacturer;
  uint8_t  iProduct;
  uint8_t  iSerialNumber;
  uint8_t  bNumConfigurations;
} USB_DEVICE_DESCRIPTOR;

/// USB 2.0 Device Qualifier Descriptor
typedef __PACKED_STRUCT {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint16_t bcdUSB;
  uint8_t  bDeviceClass;
  uint8_t  bDeviceSubClass;
  uint8_t  bDeviceProtocol;
  uint8_t  bMaxPacketSize0;
  uint8_t  bNumConfigurations;
  uint8_t  bReserved;
} USB_DEVICE_QUALIFIER_DESCRIPTOR;

/// USB Standard Configuration Descriptor
typedef __PACKED_STRUCT {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint16_t wTotalLength;
  uint8_t  bNumInterfaces;
  uint8_t  bConfigurationValue;
  uint8_t  iConfiguration;
  uint8_t  bmAttributes;
  uint8_t  bMaxPower;
} USB_CONFIGURATION_DESCRIPTOR;

/// USB Standard Interface Descriptor
typedef __PACKED_STRUCT {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bInterfaceNumber;
  uint8_t  bAlternateSetting;
  uint8_t  bNumEndpoints;
  uint8_t  bInterfaceClass;
  uint8_t  bInterfaceSubClass;
  uint8_t  bInterfaceProtocol;
  uint8_t  iInterface;
} USB_INTERFACE_DESCRIPTOR;

/// USB Standard Endpoint Descriptor
typedef __PACKED_STRUCT {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bEndpointAddress;
  uint8_t  bmAttributes;
  uint16_t wMaxPacketSize;
  uint8_t  bInterval;
} USB_ENDPOINT_DESCRIPTOR;

/// USB String Descriptor
typedef __PACKED_STRUCT {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint16_t bString;
} USB_STRING_DESCRIPTOR;

/// USB Common Descriptor
typedef __PACKED_STRUCT {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
} USB_COMMON_DESCRIPTOR;

/// USB Interface Association Descriptor
typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bFirstInterface;
  uint8_t  bInterfaceCount;
  uint8_t  bFunctionClass;
  uint8_t  bFunctionSubclass;
  uint8_t  bFunctionProtocol;
  uint8_t  iFunction;
} USB_INTERFACE_ASSOCIATION_DESCRIPTOR;

#endif  // __USB_DEF_H
