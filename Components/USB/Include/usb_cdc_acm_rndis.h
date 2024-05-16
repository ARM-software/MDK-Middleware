/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB
 * Copyright (c) 2004-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usb_cdc_acm_rndis.h
 * Purpose: USB Device Communication Device Class (CDC)
 *          Abstract Control Model (ACM)
 *          Remote Network Driver Interface Specification (RNDIS) Definitions
 *----------------------------------------------------------------------------*/

#ifndef __USB_CDC_ACM_RNDIS_H
#define __USB_CDC_ACM_RNDIS_H

#include <stdint.h>

#include "usb_compiler.h"
#include "usb_def.h"


/*------------------------------------------------------------------------------
 *  Definitions based on:
 *  [MS-RNDIS].pdf (https://msdn.microsoft.com/en-us/library/ee524902.aspx)
 *----------------------------------------------------------------------------*/

// RNDIS Major version
#define RNDIS_MAJOR_VERSION                     0x00000001U

// RNDIS Minor version
#define RNDIS_MINOR_VERSION                     0x00000000U

// RNDIS message types
#define REMOTE_NDIS_PACKET_MSG                  0x00000001U
#define REMOTE_NDIS_INITIALIZE_MSG              0x00000002U
#define REMOTE_NDIS_INITIALIZE_CMPLT            0x80000002U
#define REMOTE_NDIS_HALT_MSG                    0x00000003U
#define REMOTE_NDIS_QUERY_MSG                   0x00000004U
#define REMOTE_NDIS_QUERY_CMPLT                 0x80000004U
#define REMOTE_NDIS_SET_MSG                     0x00000005U
#define REMOTE_NDIS_SET_CMPLT                   0x80000005U
#define REMOTE_NDIS_RESET_MSG                   0x00000006U
#define REMOTE_NDIS_RESET_CMPLT                 0x80000006U
#define REMOTE_NDIS_INDICATE_STATUS_MSG         0x00000007U
#define REMOTE_NDIS_KEEPALIVE_MSG               0x00000008U
#define REMOTE_NDIS_KEEPALIVE_CMPLT             0x80000008U

// RNDIS messages status values
#define RNDIS_STATUS_SUCCESS                    0x00000000U     // Success
#define RNDIS_STATUS_FAILURE                    0xC0000001U     // Unspecified error
#define RNDIS_STATUS_INVALID_DATA               0xC0010015U     // Invalid data error
#define RNDIS_STATUS_NOT_SUPPORTED              0xC00000BBU     // Unsupported request error
#define RNDIS_STATUS_MEDIA_CONNECT              0x4001000BU     // Device is connected to a network medium
#define RNDIS_STATUS_MEDIA_DISCONNECT           0x4001000CU     // Device is disconnected from the medium

// RNDIS states
#define RNDIS_UNINITIALIZED                     0x00000000U
#define RNDIS_BUS_INITIALIZED                   0x00000001U
#define RNDIS_INITIALIZED                       0x00000002U
#define RNDIS_DATA_INITIALIZED                  0x00000003U

// RNDIS connection types
#define RNDIS_DF_CONNECTIONLESS                 0x00000001UL
#define RNDIS_DF_CONNECTION_ORIENTED            0x00000002UL


/*------------------------------------------------------------------------------
 *  Definitions based on:
 *  https://docs.microsoft.com/en-us/windows-hardware/drivers/network/remote-ndis-oids
 *----------------------------------------------------------------------------*/

// General OIDs - required
#define OID_GEN_SUPPORTED_LIST                  0x00010101
#define OID_GEN_HARDWARE_STATUS                 0x00010102
#define OID_GEN_MEDIA_SUPPORTED                 0x00010103
#define OID_GEN_MEDIA_IN_USE                    0x00010104
#define OID_GEN_MAXIMUM_FRAME_SIZE              0x00010106
#define OID_GEN_LINK_SPEED                      0x00010107
#define OID_GEN_TRANSMIT_BLOCK_SIZE             0x0001010A
#define OID_GEN_RECEIVE_BLOCK_SIZE              0x0001010B
#define OID_GEN_VENDOR_ID                       0x0001010C
#define OID_GEN_VENDOR_DESCRIPTION              0x0001010D
#define OID_GEN_CURRENT_PACKET_FILTER           0x0001010E
#define OID_GEN_MAXIMUM_TOTAL_SIZE              0x00010111
#define OID_GEN_MEDIA_CONNECT_STATUS            0x00010114

// General OIDs - optional
#define OID_GEN_MAC_OPTIONS                     0x00010113
#define OID_GEN_VENDOR_DRIVER_VERSION           0x00010116
#define OID_GEN_NETWORK_LAYER_ADDRESSES         0x00010118  // Set only
#define OID_GEN_TRANSPORT_HEADER_OFFSET         0x00010119  // Set only
#define OID_GEN_RNDIS_CONFIG_PARAMETER          0x0001021B  // Set only
#define OID_GEN_MEDIA_CAPABILITIES              0x00010201
#define OID_GEN_PHYSICAL_MEDIUM                 0x00010202

// General Statistic OIDs - required
#define OID_GEN_XMIT_OK                         0x00020101
#define OID_GEN_RCV_OK                          0x00020102
#define OID_GEN_XMIT_ERROR                      0x00020103
#define OID_GEN_RCV_ERROR                       0x00020104
#define OID_GEN_RCV_NO_BUFFER                   0x00020105

// General Statistic OIDs - optional
#define OID_GEN_DIRECTED_BYTES_XMIT             0x00020201
#define OID_GEN_DIRECTED_FRAMES_XMIT            0x00020202
#define OID_GEN_MULTICAST_BYTES_XMIT            0x00020203
#define OID_GEN_MULTICAST_FRAMES_XMIT           0x00020204
#define OID_GEN_BROADCAST_BYTES_XMIT            0x00020205
#define OID_GEN_BROADCAST_FRAMES_XMIT           0x00020206
#define OID_GEN_DIRECTED_BYTES_RCV              0x00020207
#define OID_GEN_DIRECTED_FRAMES_RCV             0x00020208
#define OID_GEN_MULTICAST_BYTES_RCV             0x00020209
#define OID_GEN_MULTICAST_FRAMES_RCV            0x0002020A
#define OID_GEN_BROADCAST_BYTES_RCV             0x0002020B
#define OID_GEN_BROADCAST_FRAMES_RCV            0x0002020C
#define OID_GEN_RCV_CRC_ERROR                   0x0002020D
#define OID_GEN_TRANSMIT_QUEUE_LENGTH           0x0002020E

// 802.3 OIDs - required
#define OID_802_3_PERMANENT_ADDRESS             0x01010101
#define OID_802_3_CURRENT_ADDRESS               0x01010102
#define OID_802_3_MULTICAST_LIST                0x01010103
#define OID_802_3_MAXIMUM_LIST_SIZE             0x01010104

#define OID_802_3_RCV_ERROR_ALIGNMENT           0x01020101
#define OID_802_3_XMIT_ONE_COLLISION            0x01020102
#define OID_802_3_XMIT_MORE_COLLISIONS          0x01020103

// 802.3 OIDs - optional
#define OID_802_3_MAC_OPTIONS                   0x01010105  // deprecated

// 802.3 Statistic OIDs - optional
#define OID_802_3_XMIT_DEFERRED                 0x01020201
#define OID_802_3_XMIT_MAX_COLLISIONS           0x01020202
#define OID_802_3_RCV_OVERRUN                   0x01020203
#define OID_802_3_XMIT_UNDERRUN                 0x01020204
#define OID_802_3_XMIT_HEARTBEAT_FAILURE        0x01020205
#define OID_802_3_XMIT_TIMES_CRS_LOST           0x01020206
#define OID_802_3_XMIT_LATE_COLLISIONS          0x01020207

// Power Management OIDs - optional
#define OID_PNP_CAPABILITIES                    0xFD010100
#define OID_PNP_SET_POWER                       0xFD010101
#define OID_PNP_QUERY_POWER                     0xFD010102

// Network Wake Up OIDs - optional
#define OID_PNP_ADD_WAKE_UP_PATTERN             0xFD010103
#define OID_PNP_REMOVE_WAKE_UP_PATTERN          0xFD010104
#define OID_PNP_ENABLE_WAKE_UP                  0xFD010106

// Hardware status codes (OID_GEN_HARDWARE_STATUS)
typedef enum {
  NdisHardwareStatusReady,
  NdisHardwareStatusInitializing,
  NdisHardwareStatusReset,
  NdisHardwareStatusClosing,
  NdisHardwareStatusNotReady
} NDIS_HARDWARE_STATUS_t;

//
// Medium the Ndis Driver is running on (OID_GEN_MEDIA_SUPPORTED/ OID_GEN_MEDIA_IN_USE).
//
typedef enum _NDIS_MEDIUM
{
  NdisMedium802_3,
  NdisMedium802_5,
  NdisMediumFddi,
  NdisMediumWan,
  NdisMediumLocalTalk,
  NdisMediumDix,                // defined for convenience, not a real medium
  NdisMediumArcnetRaw,
  NdisMediumArcnet878_2,
  NdisMediumAtm,
  NdisMediumWirelessWan,
  NdisMediumIrda,
  NdisMediumBpc,
  NdisMediumCoWan,
  NdisMedium1394,
  NdisMediumInfiniBand,
  NdisMediumTunnel,
  NdisMediumNative802_11,
  NdisMediumLoopback,
  NdisMediumWiMAX,
  NdisMediumIP,
  NdisMediumMax                 // Not a real medium, defined as an upper-bound
} NDIS_MEDIUM_t;

// Physical Medium Type definitions. Used with OID_GEN_PHYSICAL_MEDIUM.
typedef enum {
  NdisPhysicalMediumUnspecified,
  NdisPhysicalMediumWirelessLan,
  NdisPhysicalMediumCableModem,
  NdisPhysicalMediumPhoneLine,
  NdisPhysicalMediumPowerLine,
  NdisPhysicalMediumDSL,        // includes ADSL and UADSL (G.Lite)
  NdisPhysicalMediumFibreChannel,
  NdisPhysicalMedium1394,
  NdisPhysicalMediumWirelessWan,
  NdisPhysicalMediumNative802_11,
  NdisPhysicalMediumBluetooth,
  NdisPhysicalMediumInfiniband,
  NdisPhysicalMediumWiMax,
  NdisPhysicalMediumUWB,
  NdisPhysicalMedium802_3,
  NdisPhysicalMedium802_5,
  NdisPhysicalMediumIrda,
  NdisPhysicalMediumWiredWAN,
  NdisPhysicalMediumWiredCoWan,
  NdisPhysicalMediumOther,
  NdisPhysicalMediumMax         // Not a real physical type, defined as an upper-bound
} NDIS_PHYSICAL_MEDIUM_t;

// Ethernet sizes
#define ETH_MAX_SIZE                   (1522U)  // MAC header (14) + Data (1500) + CRC (4) + VLAN id (4)
#define ETH_MIN_SIZE                   (14U)    // Only MAC header
#define ETH_MTU_SIZE                   (1500U)  // Ethernet Maximum Transfer Unit

// Defines the state of the LAN media
typedef enum {
  NdisMediaStateConnected,
  NdisMediaStateDisconnected
} NDIS_MEDIA_STATE_t;

// Packet Filter Bits (OID_GEN_CURRENT_PACKET_FILTER)
#define RNDIS_FILTER_DIRECTED          (1U     )
#define RNDIS_FILTER_MULTICAST         (1U << 1)
#define RNDIS_FILTER_ALL_MULTICAST     (1U << 2)
#define RNDIS_FILTER_BROADCAST         (1U << 3)
#define RNDIS_FILTER_PROMISCUOUS       (1U << 5)

// MAC ADDRESS type definition
typedef uint8_t MAC_ADDRESS_t[6];


/*------------------------------------------------------------------------------
 *  Structures based on:
 *  [MS-RNDIS].pdf (https://msdn.microsoft.com/en-us/library/ee524902.aspx)
 *----------------------------------------------------------------------------*/

// REMOTE_NDIS_INITIALIZE_MSG structure
typedef __PACKED_STRUCT {
  uint32_t MessageType;
  uint32_t MessageLength;
  uint32_t RequestID;
  uint32_t MajorVersion;
  uint32_t MinorVersion;
  uint32_t MaxTransferSize;
} REMOTE_NDIS_INITIALIZE_MSG_t;

// REMOTE_NDIS_INITIALIZE_CMPLT structure
typedef __PACKED_STRUCT {
  uint32_t MessageType;
  uint32_t MessageLength;
  uint32_t RequestID;
  uint32_t Status;
  uint32_t MajorVersion;
  uint32_t MinorVersion;
  uint32_t DeviceFlags;
  uint32_t Medium;
  uint32_t MaxPacketsPerTransfer;
  uint32_t MaxTransferSize;
  uint32_t PacketAlignmentFactor;
  uint32_t Reserved[2];
} REMOTE_NDIS_INITIALIZE_CMPLT_t;

// REMOTE_NDIS_HALT_MSG structure
typedef __PACKED_STRUCT {
  uint32_t MessageType;
  uint32_t MessageLength;
  uint32_t RequestID;
} REMOTE_NDIS_HALT_MSG_t;

// REMOTE_NDIS_QUERY_MSG structure
typedef __PACKED_STRUCT {
  uint32_t MessageType;
  uint32_t MessageLength;
  uint32_t RequestID;
  uint32_t Oid;
  uint32_t InformationBufferLength;
  uint32_t InformationBufferOffset;
  uint32_t Reserved;
  uint32_t OIDInputBuffer[1];
} REMOTE_NDIS_QUERY_MSG_t;

// REMOTE_NDIS_QUERY_CMPLT structure
typedef __PACKED_STRUCT {
  uint32_t MessageType;
  uint32_t MessageLength;
  uint32_t RequestID;
  uint32_t Status;
  uint32_t InformationBufferLength;
  uint32_t InformationBufferOffset;
  uint32_t OIDInputBuffer[1];
} REMOTE_NDIS_QUERY_CMPLT_t;

// REMOTE_NDIS_SET_MSG structure
typedef __PACKED_STRUCT {
  uint32_t MessageType;
  uint32_t MessageLength;
  uint32_t RequestID;
  uint32_t Oid;
  uint32_t InformationBufferLength;
  uint32_t InformationBufferOffset;
  uint32_t Reserved;
  uint32_t OIDInputBuffer[1];
} REMOTE_NDIS_SET_MSG_t;

// REMOTE_NDIS_SET_CMPLT structure
typedef __PACKED_STRUCT {
  uint32_t MessageType;
  uint32_t MessageLength;
  uint32_t RequestID;
  uint32_t Status;
} REMOTE_NDIS_SET_CMPLT_t;

// REMOTE_NDIS_RESET_MSG structure
typedef __PACKED_STRUCT {
  uint32_t MessageType;
  uint32_t MessageLength;
  uint32_t Reserved;
} REMOTE_NDIS_RESET_MSG_t;

// REMOTE_NDIS_RESET_CMPLT structure
typedef __PACKED_STRUCT {
  uint32_t MessageType;
  uint32_t MessageLength;
  uint32_t Status;
  uint32_t AddressingReset;
} REMOTE_NDIS_RESET_CMPLT_t;

// REMOTE_NDIS_INDICATE_STATUS_MSG structure
typedef __PACKED_STRUCT {
  uint32_t MessageType;
  uint32_t MessageLength;
  uint32_t Status;
  uint32_t StatusBufferLength;
  uint32_t StatusBufferOffset;
  uint32_t DiagnosticInfoBuffer[2];
  uint32_t StatusBuffer[1];
} REMOTE_NDIS_INDICATE_STATUS_MSG_t;

// REMOTE_NDIS_KEEPALIVE_MSG structure
typedef __PACKED_STRUCT {
  uint32_t MessageType;
  uint32_t MessageLength;
  uint32_t RequestID;
} REMOTE_NDIS_KEEPALIVE_MSG_t;

// REMOTE_NDIS_KEEPALIVE_CMPLT structure
typedef __PACKED_STRUCT {
  uint32_t MessageType;
  uint32_t MessageLength;
  uint32_t RequestID;
  uint32_t Status;
} REMOTE_NDIS_KEEPALIVE_CMPLT_t;

// REMOTE_NDIS_PACKET_MSG structure
typedef __PACKED_STRUCT {
  uint32_t MessageType;
  uint32_t MessageLength;
  uint32_t DataOffset;
  uint32_t DataLength;
  uint32_t OutOfBandDataOffset;
  uint32_t OutOfBandDataLength;
  uint32_t NumOutOfBandDataElements;
  uint32_t PerPacketInfoOffset;
  uint32_t PerPacketInfoLength;
  uint32_t Reserved[2];
  uint32_t PayLoad[1];
} REMOTE_NDIS_PACKET_MSG_t;

// REMOTE_NDIS_OUT_OF_BAND_DATA structure
typedef __PACKED_STRUCT {
  uint32_t Size;
  uint32_t Type;
  uint32_t ClassInformationOffset;
  uint32_t OutOfBandData[1];
} REMOTE_NDIS_OUT_OF_BAND_DATA_t;

// REMOTE_NDIS_PER_PACKET_INFO_DATA structure
typedef __PACKED_STRUCT {
  uint32_t Size;
  uint32_t Type;
  uint32_t PerPacketInformationOffset;
  uint32_t PerPacketData[1];
} REMOTE_NDIS_PER_PACKET_INFO_DATA_t;

#endif
