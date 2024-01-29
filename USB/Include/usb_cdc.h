/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usb_cdc.h
 * Purpose: USB Communication Device Class Definitions
 *----------------------------------------------------------------------------*/

#ifndef __USB_CDC_H
#define __USB_CDC_H

#include <stdint.h>

#include "usb_compiler.h"
#include "usb_def.h"


/*------------------------------------------------------------------------------
 *      Definitions  based on: - usbcdc11.pdf  (www.usb.org)
 *                             - usbecm120.pdf (www.usb.org)
 *                             - usbncm10.pdf  (www.usb.org)
 *----------------------------------------------------------------------------*/
// Communication device class specification version 1.1
#define CDC_V1_1_0                              0x0110U

// Communication device class specification version 1.2
#define CDC_V1_2_0                              0x0120U

// Communication interface class code
// (usbcdc11.pdf, 4.2, Table 15)
#define CDC_COMMUNICATION_INTERFACE_CLASS       0x02U

// Communication interface class subclass codes
// (usbcdc11.pdf, 4.3, Table 16)
#define CDC_DIRECT_LINE_CONTROL_MODEL           0x01U
#define CDC_ABSTRACT_CONTROL_MODEL              0x02U
#define CDC_TELEPHONE_CONTROL_MODEL             0x03U
#define CDC_MULTI_CHANNEL_CONTROL_MODEL         0x04U
#define CDC_CAPI_CONTROL_MODEL                  0x05U
#define CDC_ETHERNET_NETWORKING_CONTROL_MODEL   0x06U
#define CDC_ATM_NETWORKING_CONTROL_MODEL        0x07U
#define CDC_WIRELESS_HANDSET_CONTROL_MODEL      0x08U
#define CDC_DEVICE_MANAGEMENT                   0x09U
#define CDC_MOBILE_DIRECT_LINE_MODEL            0x0AU
#define CDC_OBEX                                0x0BU
#define CDC_ETHERNET_EMULATION_MODEL            0x0CU
#define CDC_NETWORK_CONTROL_MODEL               0x0DU

// Communication interface class control protocol codes
// (usbcdc11.pdf, 4.4, Table 17)
#define CDC_PROTOCOL_COMMON_AT_COMMANDS         0x01U

// NCM Communication Interface Protocol Codes
// (usbncm10.pdf, 4.2, Table 4-2)
#define CDC_NCM_PROTOCOL_NONE                   0x00U
#define CDC_NCM_PROTOCOL_OEM                    0xFEU

// Data interface class code
// (usbcdc11.pdf, 4.5, Table 18)
#define CDC_DATA_INTERFACE_CLASS                0x0AU

// NCM Data Class Interface Protocol Codes
// (usbncm10.pdf, 4.3, Table 4-3)
#define CDC_NCM_DATA_NETWORK_TRANSFER_BLOCK     0x01U

// NCM Functional Descriptor Code
// (usbncm10.pdf, 4.4, Table 4-4)
#define CDC_NCM_FUNC_DESC_CODE                  0x1AU

// Data interface class protocol codes
// (usbcdc11.pdf, 4.7, Table 19)
#define CDC_PROTOCOL_ISDN_BRI                   0x30U
#define CDC_PROTOCOL_HDLC                       0x31U
#define CDC_PROTOCOL_TRANSPARENT                0x32U
#define CDC_PROTOCOL_Q921_MANAGEMENT            0x50U
#define CDC_PROTOCOL_Q921_DATA_LINK             0x51U
#define CDC_PROTOCOL_Q921_MULTIPLEXOR           0x52U
#define CDC_PROTOCOL_V42                        0x90U
#define CDC_PROTOCOL_EURO_ISDN                  0x91U
#define CDC_PROTOCOL_V24_RATE_ADAPTATION        0x92U
#define CDC_PROTOCOL_CAPI                       0x93U
#define CDC_PROTOCOL_HOST_BASED_DRIVER          0xFDU
#define CDC_PROTOCOL_DESCRIBED_IN_PUFD          0xFEU

// Type values for bDescriptorType field of functional descriptors
// (usbcdc11.pdf, 5.2.3, Table 24)
#define CDC_CS_INTERFACE                        0x24U
#define CDC_CS_ENDPOINT                         0x25U

// Type values for bDescriptorSubtype field of functional descriptors
// (usbcdc11.pdf, 5.2.3, Table 25)
#define CDC_HEADER                              0x00U
#define CDC_CALL_MANAGEMENT                     0x01U
#define CDC_ABSTRACT_CONTROL_MANAGEMENT         0x02U
#define CDC_DIRECT_LINE_MANAGEMENT              0x03U
#define CDC_TELEPHONE_RINGER                    0x04U
#define CDC_REPORTING_CAPABILITIES              0x05U
#define CDC_UNION                               0x06U
#define CDC_COUNTRY_SELECTION                   0x07U
#define CDC_TELEPHONE_OPERATIONAL_MODES         0x08U
#define CDC_USB_TERMINAL                        0x09U
#define CDC_NETWORK_CHANNEL                     0x0AU
#define CDC_PROTOCOL_UNIT                       0x0BU
#define CDC_EXTENSION_UNIT                      0x0CU
#define CDC_MULTI_CHANNEL_MANAGEMENT            0x0DU
#define CDC_CAPI_CONTROL_MANAGEMENT             0x0EU
#define CDC_ETHERNET_NETWORKING                 0x0FU
#define CDC_ATM_NETWORKING                      0x10U

// CDC class-specific request codes
// (usbcdc11.pdf, 6.2, Table 46)
// see Table 45 for info about the specific requests.
#define CDC_SEND_ENCAPSULATED_COMMAND           0x00U
#define CDC_GET_ENCAPSULATED_RESPONSE           0x01U
#define CDC_SET_COMM_FEATURE                    0x02U
#define CDC_GET_COMM_FEATURE                    0x03U
#define CDC_CLEAR_COMM_FEATURE                  0x04U
#define CDC_SET_AUX_LINE_STATE                  0x10U
#define CDC_SET_HOOK_STATE                      0x11U
#define CDC_PULSE_SETUP                         0x12U
#define CDC_SEND_PULSE                          0x13U
#define CDC_SET_PULSE_TIME                      0x14U
#define CDC_RING_AUX_JACK                       0x15U
#define CDC_SET_LINE_CODING                     0x20U
#define CDC_GET_LINE_CODING                     0x21U
#define CDC_SET_CONTROL_LINE_STATE              0x22U
#define CDC_SEND_BREAK                          0x23U
#define CDC_SET_RINGER_PARMS                    0x30U
#define CDC_GET_RINGER_PARMS                    0x31U
#define CDC_SET_OPERATION_PARMS                 0x32U
#define CDC_GET_OPERATION_PARMS                 0x33U
#define CDC_SET_LINE_PARMS                      0x34U
#define CDC_GET_LINE_PARMS                      0x35U
#define CDC_DIAL_DIGITS                         0x36U
#define CDC_SET_UNIT_PARAMETER                  0x37U
#define CDC_GET_UNIT_PARAMETER                  0x38U
#define CDC_CLEAR_UNIT_PARAMETER                0x39U
#define CDC_GET_PROFILE                         0x3AU
#define CDC_SET_ETHERNET_MULTICAST_FILTERS      0x40U
#define CDC_SET_ETHERNET_PMP_FILTER             0x41U
#define CDC_GET_ETHERNET_PMP_FILTER             0x42U
#define CDC_SET_ETHERNET_PACKET_FILTER          0x43U
#define CDC_GET_ETHERNET_STATISTIC              0x44U
#define CDC_SET_ATM_DATA_FORMAT                 0x50U
#define CDC_GET_ATM_DEVICE_STATISTICS           0x51U
#define CDC_SET_ATM_DEFAULT_VC                  0x52U
#define CDC_GET_ATM_VC_STATISTICS               0x53U
#define CDC_GET_NTB_PARAMETERS                  0x80U
#define CDC_GET_NET_ADDRESS                     0x81U
#define CDC_SET_NET_ADDRESS                     0x82U
#define CDC_GET_NTB_FORMAT                      0x83U
#define CDC_SET_NTB_FORMAT                      0x84U
#define CDC_GET_NTB_INPUT_SIZE                  0x85U
#define CDC_SET_NTB_INPUT_SIZE                  0x86U
#define CDC_GET_MAX_DATAGRAM_SIZE               0x87U
#define CDC_SET_MAX_DATAGRAM_SIZE               0x88U
#define CDC_GET_CRC_MODE                        0x89U
#define CDC_SET_CRC_MODE                        0x8AU

// Communication feature selector codes
// (usbcdc11.pdf, 6.2.2..6.2.4, Table 47)
#define CDC_ABSTRACT_STATE                      0x01U
#define CDC_COUNTRY_SETTING                     0x02U

// Feature Status returned for ABSTRACT_STATE Selector
// (usbcdc11.pdf, 6.2.3, Table 48)
#define CDC_IDLE_SETTING                        (1U)
#define CDC_DATA_MULTPLEXED_STATE               (1U << 1)


// Control signal bitmap values for the SetControlLineState request
// (usbcdc11.pdf, 6.2.14, Table 51)
#define CDC_DTE_PRESENT                         (1U)
#define CDC_ACTIVATE_CARRIER                    (1U << 1)

// CDC class-specific notification codes
// (usbcdc11.pdf, 6.3, Table 68)
// see Table 67 for Info about class-specific notifications
#define CDC_NOTIFICATION_NETWORK_CONNECTION     0x00U
#define CDC_RESPONSE_AVAILABLE                  0x01U
#define CDC_AUX_JACK_HOOK_STATE                 0x08U
#define CDC_RING_DETECT                         0x09U
#define CDC_NOTIFICATION_SERIAL_STATE           0x20U
#define CDC_CALL_STATE_CHANGE                   0x28U
#define CDC_LINE_STATE_CHANGE                   0x29U
#define CDC_CONNECTION_SPEED_CHANGE             0x2AU

// UART state bitmap values (Serial state notification).
// (usbcdc11.pdf, 6.3.5, Table 69)
#define CDC_SERIAL_STATE_OVERRUN                (1U << 6)  // receive data overrun error has occurred
#define CDC_SERIAL_STATE_OVERRUN_Pos            (      6)
#define CDC_SERIAL_STATE_OVERRUN_Msk            (1U << CDC_SERIAL_STATE_OVERRUN_Pos)
#define CDC_SERIAL_STATE_PARITY                 (1U << 5)  // parity error has occurred
#define CDC_SERIAL_STATE_PARITY_Pos             (      5)
#define CDC_SERIAL_STATE_PARITY_Msk             (1U << CDC_SERIAL_STATE_PARITY_Pos)
#define CDC_SERIAL_STATE_FRAMING                (1U << 4)  // framing error has occurred
#define CDC_SERIAL_STATE_FRAMING_Pos            (      4)
#define CDC_SERIAL_STATE_FRAMING_Msk            (1U << CDC_SERIAL_STATE_FRAMING_Pos)
#define CDC_SERIAL_STATE_RING                   (1U << 3)  // state of ring signal detection
#define CDC_SERIAL_STATE_RING_Pos               (      3)
#define CDC_SERIAL_STATE_RING_Msk               (1U << CDC_SERIAL_STATE_RING_Pos)
#define CDC_SERIAL_STATE_BREAK                  (1U << 2)  // state of break detection
#define CDC_SERIAL_STATE_BREAK_Pos              (      2)
#define CDC_SERIAL_STATE_BREAK_Msk              (1U << CDC_SERIAL_STATE_BREAK_Pos)
#define CDC_SERIAL_STATE_TX_CARRIER             (1U << 1)  // state of transmission carrier
#define CDC_SERIAL_STATE_TX_CARRIER_Pos         (      1)
#define CDC_SERIAL_STATE_TX_CARRIER_Msk         (1U << CDC_SERIAL_STATE_TX_CARRIER_Pos)
#define CDC_SERIAL_STATE_RX_CARRIER             (1U     )  // state of receiver carrier
#define CDC_SERIAL_STATE_RX_CARRIER_Pos         (      0)
#define CDC_SERIAL_STATE_RX_CARRIER_Msk         (1U << CDC_SERIAL_STATE_RX_CARRIER_Pos)


/*------------------------------------------------------------------------------
 *      Structures based on: - usbcdc11.pdf  (www.usb.org)
 *                           - usbecm120.pdf (www.usb.org)
 *                           - usbncm10.pdf  (www.usb.org)
 *----------------------------------------------------------------------------*/

/// Header functional descriptor
/// (usbcdc11.pdf, 5.2.3.1)
/// This header must precede any list of class-specific descriptors.
typedef __PACKED_STRUCT {
  uint8_t  bFunctionLength;             ///< size of this descriptor in bytes
  uint8_t  bDescriptorType;             ///< CS_INTERFACE descriptor type
  uint8_t  bDescriptorSubtype;          ///< Header functional descriptor subtype
  uint16_t bcdCDC;                      ///< USB CDC specification release version
} CDC_HEADER_DESCRIPTOR;

/// Call management functional descriptor
/// (usbcdc11.pdf, 5.2.3.2)
/// Describes the processing of calls for the communication class interface.
typedef __PACKED_STRUCT {
  uint8_t  bFunctionLength;             ///< size of this descriptor in bytes
  uint8_t  bDescriptorType;             ///< CS_INTERFACE descriptor type
  uint8_t  bDescriptorSubtype;          ///< call management functional descriptor subtype
  uint8_t  bmCapabilities;              ///< capabilities that this configuration supports
  uint8_t  bDataInterface;              ///< interface number of the data class interface used for call management (optional)
} CDC_CALL_MANAGEMENT_DESCRIPTOR;

/// Abstract control management functional descriptor
/// (usbcdc11.pdf, 5.2.3.3)
/// Describes the command supported by the communication interface class with the Abstract Control Model subclass code.
typedef struct {
  uint8_t  bFunctionLength;             ///< size of this descriptor in bytes
  uint8_t  bDescriptorType;             ///< CS_INTERFACE descriptor type
  uint8_t  bDescriptorSubtype;          ///< abstract control management functional descriptor subtype
  uint8_t  bmCapabilities;              ///< capabilities supported by this configuration
} CDC_ABSTRACT_CONTROL_MANAGEMENT_DESCRIPTOR;

/// Union functional descriptors
/// (usbcdc11.pdf, 5.2.3.8)
/// Describes the relationship between a group of interfaces that can be considered to form a functional unit.
typedef __PACKED_STRUCT {
  uint8_t  bFunctionLength;             ///< size of this descriptor in bytes
  uint8_t  bDescriptorType;             ///< CS_INTERFACE descriptor type
  uint8_t  bDescriptorSubtype;          ///< union functional descriptor subtype
  uint8_t  bMasterInterface;            ///< interface number designated as master
} CDC_UNION_DESCRIPTOR;

/// Union functional descriptors with one slave interface
/// (usbcdc11.pdf, 5.2.3.8)
typedef __PACKED_STRUCT {
  CDC_UNION_DESCRIPTOR sUnion;          ///< Union functional descriptor
  uint8_t  bSlaveInterfaces[1];         ///< Slave interface 0
} CDC_UNION_1SLAVE_DESCRIPTOR;

/// Line coding structure
/// Format of the data returned when a GetLineCoding request is received
/// (usbcdc11.pdf, 6.2.13)
typedef __PACKED_STRUCT {
  uint32_t dwDTERate;                   ///< Data terminal rate in bits per second
  uint8_t  bCharFormat;                 ///< Number of stop bits
  uint8_t  bParityType;                 ///< Parity bit type
  uint8_t  bDataBits;                   ///< Number of data bits
} CDC_LINE_CODING;

/// Notification header
/// Data sent on the notification endpoint must follow this header.
/// see  USB_SETUP_PACKET in file usb.h
typedef USB_SETUP_PACKET CDC_NOTIFICATION_HEADER;

/// NTB Parameters Structure
/// (usbncm10.pdf, 6.2.1)
typedef struct {
  uint16_t wLength;
  uint16_t bmNtbFormatsSupported;
  uint32_t dwNtbInMaxSize;
  uint16_t wNdpInDivisor;
  uint16_t wNdpInPayloadRemainder;
  uint16_t wNdpInAlignment;
  uint16_t Reserved0;
  uint32_t dwNtbOutMaxSize;
  uint16_t wNdpOutDivisor;
  uint16_t wNdpOutPayloadRemainder;
  uint16_t wNdpOutAlignment;
  uint16_t Reserved1;
} CDC_NCM_NTB_PARAM;

/// 16-bit NCM Transfer Header (NTH16) Structure
/// (usbncm10.pdf, 3.2.1)
typedef struct {
  uint32_t dwSignature;
  uint16_t wHeaderLength;
  uint16_t wSequence;
  uint16_t wBlockLength;
  uint16_t wNdpIndex;
} CDC_NCM_NTB_NTH16;

/// 32-bit NCM Transfer Header (NTH32) Structure
/// (usbncm10.pdf, 3.2.2)
typedef struct {
  uint32_t dwSignature;
  uint16_t wHeaderLength;
  uint16_t wSequence;
  uint32_t dwBlockLength;
  uint32_t dwNdpIndex;
} CDC_NCM_NTB_NTH32;

/// 16-bit Datagram Pointer Entry Structure
/// (usbncm10.pdf, 3.3.1)
typedef struct {
  uint16_t wDatagramIndex;
  uint16_t wDatagramLength;
} CDC_NCM_NTB_DATAGRAM16;

/// // 32-bit Datagram Pointer Entry Structure
/// (usbncm10.pdf, 3.3.2)
typedef struct {
  uint32_t dwDatagramIndex;
  uint32_t dwDatagramLength;
} CDC_NCM_NTB_DATAGRAM32;

/// 16-bit NCM Datagram Pointer Table (NDP16)
/// (usbncm10.pdf, 3.3.1)
typedef struct {
  uint32_t dwSignature;
  uint16_t wLength;
  uint16_t wNextNdpIndex;
  CDC_NCM_NTB_DATAGRAM16 datagram[1];
} CDC_NCM_NTB_NDP16;

/// 32-bit NCM Datagram Pointer Table (NDP32)
/// (usbncm10.pdf, 3.3.2)
typedef struct {
  uint32_t dwSignature;
  uint16_t wLength;
  uint16_t wReserved6;
  uint32_t dwNextNdpIndex;
  uint32_t dwReserved12;
  CDC_NCM_NTB_DATAGRAM32 datagram[1];
} CDC_NCM_NTB_NDP32;

#endif
