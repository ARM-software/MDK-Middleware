/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device:CDC
 * Copyright (c) 2018-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_CDC_ACM_RNDIS_VETH_%Instance%.c
 * Purpose: USB Device Communication Device Class (CDC)
 *          Abstract Control Model (ACM)
 *          Remote Network Driver Interface Specification (RNDIS)
 *          User Module for a Virtual Ethernet
 * Rev.:    V1.0.4
 *----------------------------------------------------------------------------*/
/**
 * \addtogroup usbd_cdcFunctions
 *
 * USBD_User_CDC_ACM_RNDIS_VETH_%Instance%.c implements the application specific
 * functionality of the CDC ACM class using RNDIS protocol and is used
 * to implement Network Interface Card (NIC) to the USB Host.
 * This user module works together with EMAC_CDC_ACM_RNDIS.c driver
 * to provide USB Host network access to Embedded Device over USB.
 *
 * The implementation depends on the configuration file USBD_Config_CDC_%Instance%.h.
 *
 */
 
 
//! [code_USBD_User_CDC_ACM_RNDIS_VETH]
 
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
 
#include "rl_usb.h"
 
#include "Driver_ETH.h"
#include "Driver_ETH_MAC.h"
#include "Driver_ETH_PHY.h"
#include "USBD_Config_CDC_%Instance%.h"
 
 
//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
 
// Configuration defines
 
//   <s.17>MAC Address
//     <i>Ethernet MAC Address in text representation
//     <i>Value FF-FF-FF-FF-FF-FF is not allowed,
//     <i>LSB of first byte must be 0 (an ethernet Multicast bit).
//     <i>Default: "1E-30-6C-A2-45-5E"
#define RNDIS_MAC_ADDR    "1E-30-6C-A2-45-5E"           // RNDIS MAC Address
 
//   <o.0..5>Maximum number of multicast addresses <1-32>
#define RNDIS_MCAST_NUM   16                            // RNDIS Number of Multicast Addresses supported
 
//   <s.32>RNDIS Vendor Description
#define RNDIS_VENDOR_DESC "Keil NIC (USB <-> ETH)"      // RNDIS Vendor Description
 
//   <o.0..23>RNDIS Vendor Id Code <0x000000-0xFFFFFF>
#define RNDIS_VENDOR_ID   0xFFFFFF                      // RNDIS three-byte IEEE-registered Vendor Code
 
//------------- <<< end of configuration section >>> ---------------------------
 
 
// Global functions exported by this module
       ARM_ETH_LINK_STATE RNDIS%Instance%_GetLinkState  (void);
       int32_t            RNDIS%Instance%_SendFrame     (const uint8_t *frame, uint32_t len);
       int32_t            RNDIS%Instance%_ReadFrame     (      uint8_t *frame, uint32_t len);
       uint32_t           RNDIS%Instance%_GetRxFrameSize(void);
 
// Local functions
static void               MAC_str_to_addr (const char *mac_str, uint8_t *mac_addr);
static void               InitVars        (void);
static void               ResetVars       (void);
 
// Local variables
static uint32_t           rndis_state;
static ARM_ETH_LINK_STATE link_state;
 
static bool               link_state_up;
static bool               link_state_down;
 
static uint32_t           packet_filter;
static ARM_ETH_MAC_ADDR   mac_address;
static ARM_ETH_MAC_ADDR   mcast_address[RNDIS_MCAST_NUM];
 
static uint32_t           get_encapsulated_response_len;
static uint32_t           get_encapsulated_response_buf[128/4];
 
static uint32_t           xmit_ok;
static uint32_t           rcv_ok;
static uint32_t           xmit_error;
static uint32_t           rcv_error;
static uint32_t           rcv_no_buffer;
 
static uint32_t           packet_in [(USBD_CDC%Instance%_ACM_SEND_BUF_SIZE   +3)/4];
static uint32_t           packet_out[(USBD_CDC%Instance%_ACM_RECEIVE_BUF_SIZE+3)/4];
 
 
// Local functions
 
// MAC Address conversion from string
// \param[in]   mac_str   Pointer to wide string.
// \param[out]  mac_addr  Pointer to address.
static void MAC_str_to_addr (const char *mac_str, uint8_t *mac_addr) {
  uint8_t  c;
  uint8_t  n;
  uint32_t i, j;
  uint32_t str_len;
 
  str_len = strlen(mac_str);
  j = 0U;
  for (i = 0U; i < str_len; i++) {
    c = (uint8_t)mac_str[i];
    if         (c == '-') {
      continue;
    } else if ((c >= '0') && (c <= '9')) {
      n = c - '0';
    } else if ((c >= 'A') && (c <= 'F')) {
      n = c - ('A' + 10U);
    } else if ((c >= 'a') && (c <= 'f')) {
      n = c - ('a' + 10U);
    } else {
      n = 0U;
    }
    if ((j & 1U) != 0U) {
      mac_addr[j>>1] |= n;
    } else {
      mac_addr[j>>1]  = (uint8_t)((uint32_t)n << 4);
    }
    j++;
  }
}
 
// Initialize variables
static void InitVars (void) {
 
  rndis_state      = RNDIS_UNINITIALIZED;
  link_state       = ARM_ETH_LINK_DOWN;
 
  packet_filter    = 0U;
 
  MAC_str_to_addr(RNDIS_MAC_ADDR, (uint8_t *)&mac_address);
  memset((void *)mcast_address, 0, sizeof(mcast_address));
 
  ResetVars();
}
 
// Reset variables
static void ResetVars (void) {
  link_state_up    = false;
  link_state_down  = false;
 
  get_encapsulated_response_len = 0U;
 
  xmit_ok          = 0U;
  rcv_ok           = 0U;
  xmit_error       = 0U;
  rcv_error        = 0U;
  rcv_no_buffer    = 0U;
}
 
 
// USB CDC ACM callback global functions
 
// Called during USBD_Initialize to initialize the USB CDC class instance (ACM).
void USBD_CDC%Instance%_ACM_Initialize (void) {
  InitVars();
}
 
 
// Called during USBD_Uninitialize to de-initialize the USB CDC class instance (ACM).
void USBD_CDC%Instance%_ACM_Uninitialize (void) {
  InitVars();
}
 
 
// Called upon USB Bus Reset Event.
void USBD_CDC%Instance%_ACM_Reset (void) {
  InitVars();
}
 
 
// Callback function called upon reception of request send encapsulated command sent by the USB Host.
// \param[in]   buf           buffer that contains send encapsulated command request.
// \param[in]   len           length of send encapsulated command request.
// \return      true          send encapsulated command request processed.
// \return      false         send encapsulated command request not supported or not processed.
bool USBD_CDC%Instance%_ACM_SendEncapsulatedCommand (const uint8_t *buf, uint16_t len) {
  const REMOTE_NDIS_INITIALIZE_MSG_t   *ptr_init_msg;
        REMOTE_NDIS_INITIALIZE_CMPLT_t *ptr_init_cmplt;
  const REMOTE_NDIS_HALT_MSG_t         *ptr_halt_msg;
  const REMOTE_NDIS_QUERY_MSG_t        *ptr_query_msg;
        REMOTE_NDIS_QUERY_CMPLT_t      *ptr_query_cmplt;
  const REMOTE_NDIS_SET_MSG_t          *ptr_set_msg;
        REMOTE_NDIS_SET_CMPLT_t        *ptr_set_cmplt;
  const REMOTE_NDIS_RESET_MSG_t        *ptr_reset_msg;
        REMOTE_NDIS_RESET_CMPLT_t      *ptr_reset_cmplt;
  const REMOTE_NDIS_KEEPALIVE_MSG_t    *ptr_keepalive_msg;
        REMOTE_NDIS_KEEPALIVE_CMPLT_t  *ptr_keepalive_cmplt;
        uint32_t                        status, val;
        uint32_t                        i;
        uint32_t                        num, by;
        uint16_t                        msg_type;
 
  (void)len;
 
  msg_type = __UNALIGNED_UINT16_READ(buf);  // Extract message type of received message
 
  // In uninitialized state only allowed messages are INITALIZE and HALT
  if ((rndis_state == RNDIS_UNINITIALIZED)     &&
      (msg_type != REMOTE_NDIS_INITIALIZE_MSG) &&
      (msg_type != REMOTE_NDIS_HALT_MSG))       {
    return false;
  }
 
  if (((uint32_t)buf & 3) != 0) {           // buf has to be 32 bit aligned
    return false;
  }
 
  status = RNDIS_STATUS_SUCCESS;            // Default message processing status
  get_encapsulated_response_len = 0U;       // Prepare default no response size
 
  switch (msg_type) {                       // MessageType
    case REMOTE_NDIS_INITIALIZE_MSG:
      // Check message is valid
      ptr_init_msg = (const REMOTE_NDIS_INITIALIZE_MSG_t *)((const void *)buf);
      if (ptr_init_msg->MessageLength       != sizeof(REMOTE_NDIS_INITIALIZE_MSG_t)) { return false; }
      if (ptr_init_msg->MajorVersion        != RNDIS_MAJOR_VERSION)                  { return false; }
      if (ptr_init_msg->MinorVersion        != RNDIS_MINOR_VERSION)                  { return false; }
      if (ptr_init_msg->MaxTransferSize     != 16384U)                               { return false; }
 
      rndis_state = RNDIS_INITIALIZED;
 
      // Prepare response
      ptr_init_cmplt = (REMOTE_NDIS_INITIALIZE_CMPLT_t *)((void *)get_encapsulated_response_buf);
      ptr_init_cmplt->MessageType            = REMOTE_NDIS_INITIALIZE_CMPLT;
      ptr_init_cmplt->MessageLength          = sizeof(REMOTE_NDIS_INITIALIZE_CMPLT_t);
      ptr_init_cmplt->RequestID              = ptr_init_msg->RequestID;
      ptr_init_cmplt->Status                 = status;
      ptr_init_cmplt->MajorVersion           = RNDIS_MAJOR_VERSION;
      ptr_init_cmplt->MinorVersion           = RNDIS_MINOR_VERSION;
      ptr_init_cmplt->DeviceFlags            = RNDIS_DF_CONNECTIONLESS;
      ptr_init_cmplt->Medium                 = (uint32_t)NdisMedium802_3;
      ptr_init_cmplt->MaxPacketsPerTransfer  = 1U;
      ptr_init_cmplt->MaxTransferSize        = USBD_CDC%Instance%_ACM_RECEIVE_BUF_SIZE;
      ptr_init_cmplt->PacketAlignmentFactor  = 2U;
      ptr_init_cmplt->Reserved[0]            = 0U;
      ptr_init_cmplt->Reserved[1]            = 0U;
      get_encapsulated_response_len          = ptr_init_cmplt->MessageLength;
      break;
 
    case REMOTE_NDIS_HALT_MSG:
      // Check message is valid
      ptr_halt_msg = (const REMOTE_NDIS_HALT_MSG_t *)((const void *)buf);
      if (ptr_halt_msg->MessageLength != sizeof(REMOTE_NDIS_HALT_MSG_t)) { return false; }
 
      rndis_state = RNDIS_UNINITIALIZED;
 
      // This message does not have a response
      return true;
 
    case REMOTE_NDIS_QUERY_MSG:
      // Check message is valid
      ptr_query_msg = (const REMOTE_NDIS_QUERY_MSG_t *)((const void *)buf);
      if (ptr_query_msg->MessageLength < 28U) { return false; }
 
      // Prepare response
      ptr_query_cmplt = (REMOTE_NDIS_QUERY_CMPLT_t *)((void *)get_encapsulated_response_buf);
      ptr_query_cmplt->MessageType             = REMOTE_NDIS_QUERY_CMPLT;
      ptr_query_cmplt->RequestID               = ptr_query_msg->RequestID;
      ptr_query_cmplt->InformationBufferOffset = 16U;
      switch (ptr_query_msg->Oid) {             // Handle OID
        case OID_GEN_SUPPORTED_LIST:
          ptr_query_cmplt->InformationBufferLength = 23U * 4U;
          ptr_query_cmplt->OIDInputBuffer[0]       = OID_GEN_SUPPORTED_LIST;
          ptr_query_cmplt->OIDInputBuffer[1]       = OID_GEN_HARDWARE_STATUS;
          ptr_query_cmplt->OIDInputBuffer[2]       = OID_GEN_MEDIA_SUPPORTED;
          ptr_query_cmplt->OIDInputBuffer[3]       = OID_GEN_MEDIA_IN_USE;
          ptr_query_cmplt->OIDInputBuffer[4]       = OID_GEN_MAXIMUM_FRAME_SIZE;
          ptr_query_cmplt->OIDInputBuffer[5]       = OID_GEN_LINK_SPEED;
          ptr_query_cmplt->OIDInputBuffer[6]       = OID_GEN_TRANSMIT_BLOCK_SIZE;
          ptr_query_cmplt->OIDInputBuffer[7]       = OID_GEN_RECEIVE_BLOCK_SIZE;
          ptr_query_cmplt->OIDInputBuffer[8]       = OID_GEN_VENDOR_ID;
          ptr_query_cmplt->OIDInputBuffer[9]       = OID_GEN_VENDOR_DESCRIPTION;
          ptr_query_cmplt->OIDInputBuffer[10]      = OID_GEN_CURRENT_PACKET_FILTER;
          ptr_query_cmplt->OIDInputBuffer[11]      = OID_GEN_MAXIMUM_TOTAL_SIZE;
          ptr_query_cmplt->OIDInputBuffer[12]      = OID_GEN_MEDIA_CONNECT_STATUS;
          ptr_query_cmplt->OIDInputBuffer[13]      = OID_GEN_PHYSICAL_MEDIUM;
          ptr_query_cmplt->OIDInputBuffer[14]      = OID_GEN_XMIT_OK;
          ptr_query_cmplt->OIDInputBuffer[15]      = OID_GEN_RCV_OK;
          ptr_query_cmplt->OIDInputBuffer[16]      = OID_GEN_XMIT_ERROR;
          ptr_query_cmplt->OIDInputBuffer[17]      = OID_GEN_RCV_ERROR;
          ptr_query_cmplt->OIDInputBuffer[18]      = OID_GEN_RCV_NO_BUFFER;
          ptr_query_cmplt->OIDInputBuffer[19]      = OID_802_3_PERMANENT_ADDRESS;
          ptr_query_cmplt->OIDInputBuffer[20]      = OID_802_3_CURRENT_ADDRESS;
          ptr_query_cmplt->OIDInputBuffer[21]      = OID_802_3_MULTICAST_LIST;
          ptr_query_cmplt->OIDInputBuffer[22]      = OID_802_3_MAXIMUM_LIST_SIZE;
          break;
        case OID_GEN_HARDWARE_STATUS:
          ptr_query_cmplt->InformationBufferLength = 4U;
          if (link_state == ARM_ETH_LINK_UP) {
            ptr_query_cmplt->OIDInputBuffer[0]     = (uint32_t)NdisHardwareStatusReady;
          } else {
            ptr_query_cmplt->OIDInputBuffer[0]     = (uint32_t)NdisHardwareStatusNotReady;
          }
          break;
        case OID_GEN_MEDIA_SUPPORTED:
        case OID_GEN_MEDIA_IN_USE:
          ptr_query_cmplt->InformationBufferLength = 4U;
          ptr_query_cmplt->OIDInputBuffer[0]       = (uint32_t)NdisMedium802_3;
          break;
        case OID_GEN_MAXIMUM_FRAME_SIZE:
          ptr_query_cmplt->InformationBufferLength = 4U;
          ptr_query_cmplt->OIDInputBuffer[0]       = ETH_MTU_SIZE;
          break;
        case OID_GEN_LINK_SPEED:
          ptr_query_cmplt->InformationBufferLength = 4U;
          ptr_query_cmplt->OIDInputBuffer[0]       = 100000000U / 100U; // 100 MBit/s
          break;
        case OID_GEN_TRANSMIT_BLOCK_SIZE:
          ptr_query_cmplt->InformationBufferLength = 4U;
          ptr_query_cmplt->OIDInputBuffer[0]       = USBD_CDC%Instance%_ACM_SEND_BUF_SIZE;
          break;
        case OID_GEN_RECEIVE_BLOCK_SIZE:
          ptr_query_cmplt->InformationBufferLength = 4U;
          ptr_query_cmplt->OIDInputBuffer[0]       = USBD_CDC%Instance%_ACM_RECEIVE_BUF_SIZE;
          break;
        case OID_GEN_VENDOR_ID:
          ptr_query_cmplt->InformationBufferLength = 4U;
          ptr_query_cmplt->OIDInputBuffer[0]       = RNDIS_VENDOR_ID;
          break;
        case OID_GEN_VENDOR_DESCRIPTION:
          ptr_query_cmplt->InformationBufferLength = strlen(RNDIS_VENDOR_DESC) + 1;
          memset((void *)&ptr_query_cmplt->OIDInputBuffer[0], 0, ptr_query_cmplt->InformationBufferLength + 1U);
          memcpy((void *)&ptr_query_cmplt->OIDInputBuffer[0], RNDIS_VENDOR_DESC, strlen(RNDIS_VENDOR_DESC));
          break;
        case OID_GEN_CURRENT_PACKET_FILTER:
          ptr_query_cmplt->InformationBufferLength = 4U;
          val = 0U;
          if ((packet_filter & ARM_ETH_MAC_ADDRESS_MULTICAST) != 0U) { val |= RNDIS_FILTER_ALL_MULTICAST; }
          if ((packet_filter & ARM_ETH_MAC_ADDRESS_BROADCAST) != 0U) { val |= RNDIS_FILTER_BROADCAST;     }
          if ((packet_filter & ARM_ETH_MAC_ADDRESS_ALL)       != 0U) { val |= RNDIS_FILTER_PROMISCUOUS;   }
          ptr_query_cmplt->OIDInputBuffer[0]       = val;
          break;
        case OID_GEN_MAXIMUM_TOTAL_SIZE:
          ptr_query_cmplt->InformationBufferLength = 4U;
          ptr_query_cmplt->OIDInputBuffer[0]       = 44U + ETH_MAX_SIZE;
          break;
        case OID_GEN_MEDIA_CONNECT_STATUS:
          ptr_query_cmplt->InformationBufferLength = 4U;
          if (link_state == ARM_ETH_LINK_UP) {
            ptr_query_cmplt->OIDInputBuffer[0]     = (uint32_t)NdisMediaStateConnected;
          } else {
            ptr_query_cmplt->OIDInputBuffer[0]     = (uint32_t)NdisMediaStateDisconnected;
          }
          break;
 
        case OID_GEN_PHYSICAL_MEDIUM:
          ptr_query_cmplt->InformationBufferLength = 4U;
          ptr_query_cmplt->OIDInputBuffer[0]       = (uint32_t)NdisPhysicalMediumUnspecified;
          break;
 
        case OID_GEN_XMIT_OK:
          ptr_query_cmplt->InformationBufferLength = 4U;
          ptr_query_cmplt->OIDInputBuffer[0]       = xmit_ok;
          break;
        case OID_GEN_RCV_OK:
          ptr_query_cmplt->InformationBufferLength = 4U;
          ptr_query_cmplt->OIDInputBuffer[0]       = rcv_ok;
          break;
        case OID_GEN_XMIT_ERROR:
          ptr_query_cmplt->InformationBufferLength = 4U;
          ptr_query_cmplt->OIDInputBuffer[0]       = xmit_error;
          break;
        case OID_GEN_RCV_ERROR:
          ptr_query_cmplt->InformationBufferLength = 4U;
          ptr_query_cmplt->OIDInputBuffer[0]       = rcv_error;
          break;
        case OID_GEN_RCV_NO_BUFFER:
          ptr_query_cmplt->InformationBufferLength = 4U;
          ptr_query_cmplt->OIDInputBuffer[0]       = rcv_no_buffer;
          break;
 
        case OID_802_3_PERMANENT_ADDRESS:
        case OID_802_3_CURRENT_ADDRESS:
          ptr_query_cmplt->InformationBufferLength = 6U;
          memcpy((void *)&ptr_query_cmplt->OIDInputBuffer[0], &mac_address, sizeof(ARM_ETH_MAC_ADDR));
          break;
        case OID_802_3_MULTICAST_LIST:
          for (i = 0U; i < RNDIS_MCAST_NUM; i++) {
            if ((__UNALIGNED_UINT32_READ(&mcast_address[0]) == 0U) && 
                (__UNALIGNED_UINT16_READ(&mcast_address[4]) == 0U)) { 
              break;
            }
          }
          if (i == 0U) {
            num = 0U;
            ptr_query_cmplt->InformationBufferOffset = 0U;
          } else {
            num = i;
            if (i < RNDIS_MCAST_NUM) {
              num = i + 1U;
            } else {
              num = i;
            }
            memcpy((void *)&ptr_query_cmplt->OIDInputBuffer[0], mcast_address, num * sizeof(ARM_ETH_MAC_ADDR));
          }
          ptr_query_cmplt->InformationBufferLength = num * sizeof(ARM_ETH_MAC_ADDR);
          break;
        case OID_802_3_MAXIMUM_LIST_SIZE:
          ptr_query_cmplt->InformationBufferLength = 4U;
          ptr_query_cmplt->OIDInputBuffer[0]       = RNDIS_MCAST_NUM;
          break;
 
        default:
          ptr_query_cmplt->InformationBufferOffset = 0U;
          ptr_query_cmplt->InformationBufferLength = 0U;
          status = RNDIS_STATUS_NOT_SUPPORTED;
          break;
      }
      ptr_query_cmplt->Status        = status;
      ptr_query_cmplt->MessageLength = ptr_query_cmplt->InformationBufferLength + 24U;
      get_encapsulated_response_len  = ptr_query_cmplt->MessageLength;
      break;
 
    case REMOTE_NDIS_SET_MSG:
      // Check message is valid
      ptr_set_msg = (const REMOTE_NDIS_SET_MSG_t *)((const void *)buf);
      if (ptr_set_msg->MessageLength < 28U) { return false; }
 
      // Prepare response
      ptr_set_cmplt = (REMOTE_NDIS_SET_CMPLT_t *)((void *)get_encapsulated_response_buf);
      ptr_set_cmplt->MessageType               = REMOTE_NDIS_SET_CMPLT;
      ptr_set_cmplt->MessageLength             = sizeof(REMOTE_NDIS_SET_CMPLT_t);
      ptr_set_cmplt->RequestID                 = ptr_set_msg->RequestID;
 
      switch (ptr_set_msg->Oid) {               // Handle OID
        case OID_802_3_MULTICAST_LIST:
          by = ptr_set_msg->InformationBufferLength;
          if (by > (sizeof(ARM_ETH_MAC_ADDR) * RNDIS_MCAST_NUM)) {
            by = sizeof(ARM_ETH_MAC_ADDR) * RNDIS_MCAST_NUM;
          }
          if (by > 0U) {
            memcpy(mcast_address, (const void *)&ptr_set_msg->OIDInputBuffer[0], by);
          }
          break;
        case OID_GEN_CURRENT_PACKET_FILTER:
          if ((ptr_set_msg->InformationBufferLength == 4U) &&
              (ptr_set_msg->InformationBufferOffset != 0U)) {
            val = __UNALIGNED_UINT32_READ(((const uint8_t *)&ptr_set_msg->RequestID) + ptr_set_msg->InformationBufferOffset);
            if (val != 0U) {
              if ((val & RNDIS_FILTER_ALL_MULTICAST) != 0U) { packet_filter |= ARM_ETH_MAC_ADDRESS_MULTICAST; }
              if ((val & RNDIS_FILTER_BROADCAST)     != 0U) { packet_filter |= ARM_ETH_MAC_ADDRESS_BROADCAST; }
              if ((val & RNDIS_FILTER_PROMISCUOUS)   != 0U) { packet_filter |= ARM_ETH_MAC_ADDRESS_ALL;       }
              if (link_state == ARM_ETH_LINK_DOWN) {
                link_state    = ARM_ETH_LINK_UP;
                link_state_up = true;
              }
              rndis_state = RNDIS_DATA_INITIALIZED;
            } else {
              if (rndis_state == RNDIS_DATA_INITIALIZED) {
                rndis_state = RNDIS_INITIALIZED;
              }
            }
          } else {
            status = RNDIS_STATUS_FAILURE;
          }
          break;
        default:
          status = RNDIS_STATUS_NOT_SUPPORTED;
          break;
      }
 
      ptr_set_cmplt->Status         = status;
      get_encapsulated_response_len = ptr_set_cmplt->MessageLength;
      break;
 
    case REMOTE_NDIS_RESET_MSG:
      // Check message is valid
      ptr_reset_msg = (const REMOTE_NDIS_RESET_MSG_t *)((const void *)buf);
      if (ptr_reset_msg->MessageLength != sizeof(REMOTE_NDIS_RESET_MSG_t)) { return false; }
 
      ResetVars();
 
      // Prepare response
      ptr_reset_cmplt = (REMOTE_NDIS_RESET_CMPLT_t *)((void *)get_encapsulated_response_buf);
      ptr_reset_cmplt->MessageType             = REMOTE_NDIS_RESET_CMPLT;
      ptr_reset_cmplt->MessageLength           = sizeof(REMOTE_NDIS_RESET_CMPLT_t);
      ptr_reset_cmplt->Status                  = status;
      ptr_reset_cmplt->AddressingReset         = 0U;
      get_encapsulated_response_len            = ptr_reset_cmplt->MessageLength;
      break;
 
    case REMOTE_NDIS_KEEPALIVE_MSG:
      // Check message is valid
      ptr_keepalive_msg = (const REMOTE_NDIS_KEEPALIVE_MSG_t *)((const void *)buf);
      if (ptr_keepalive_msg->MessageLength != sizeof(REMOTE_NDIS_KEEPALIVE_MSG_t)) { return false; }
 
      // Prepare response
      ptr_keepalive_cmplt = (REMOTE_NDIS_KEEPALIVE_CMPLT_t *)((void *)get_encapsulated_response_buf);
      ptr_keepalive_cmplt->MessageType         = REMOTE_NDIS_KEEPALIVE_CMPLT;
      ptr_keepalive_cmplt->MessageLength       = sizeof(REMOTE_NDIS_KEEPALIVE_CMPLT_t);
      ptr_keepalive_cmplt->RequestID           = ptr_keepalive_msg->RequestID;
      ptr_keepalive_cmplt->Status              = status;
      get_encapsulated_response_len            = ptr_keepalive_cmplt->MessageLength;
      break;
 
    default:
      return false;
  }
 
  if (get_encapsulated_response_len != 0U) {
    // If response is prepared send notification over Interrupt Endpoint
    (void)USBD_CDC_ACM_Notify_ResponseAvailable (%Instance%);
  }
 
  return true;
}
 
 
// Callback function called upon reception of request to get encapsulated response sent by the USB Host.
// \param[in]   max_len       maximum number of data bytes that USB Host expects to receive
// \param[out]  buf           pointer to buffer containing get encapsulated response to be returned to USB Host.
// \param[out]  len           pointer to number of data bytes to be returned to USB Host.
// \return      true          get encapsulated response request processed.
// \return      false         get encapsulated response request not supported or not processed.
bool USBD_CDC%Instance%_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len) {
  REMOTE_NDIS_INDICATE_STATUS_MSG_t *ptr_indicate_status_msg;
  uint32_t                           status;
 
  (void)max_len;
 
  if (link_state_up || link_state_down) {   // Generate unsolicited INDICATE STATUS message if link status has changed
    if (link_state_up) {
      status = RNDIS_STATUS_MEDIA_CONNECT;
    } else {
      status = RNDIS_STATUS_MEDIA_DISCONNECT;
    }
 
    // Prepare INDICATE STATUS message
    ptr_indicate_status_msg = (REMOTE_NDIS_INDICATE_STATUS_MSG_t *)((void *)get_encapsulated_response_buf);
    ptr_indicate_status_msg->MessageType        = REMOTE_NDIS_INDICATE_STATUS_MSG;
    ptr_indicate_status_msg->MessageLength      = 20U;
    ptr_indicate_status_msg->Status             = status;
    ptr_indicate_status_msg->StatusBufferLength = 0U;
    ptr_indicate_status_msg->StatusBufferOffset = 0U;
    get_encapsulated_response_len               = 20U;
 
    link_state_up   = false;
    link_state_down = false;
  }
 
  if (get_encapsulated_response_len != 0U) {    // If response is available return it
    *buf = (uint8_t *)get_encapsulated_response_buf;
    *len = (uint16_t) get_encapsulated_response_len;
    get_encapsulated_response_len = 0U;
  }
 
  return true;
}
 
 
// Callback function called when all data was sent
// \return                    none.
void USBD_CDC%Instance%_ACM_DataSent (void) {
}
 
 
// Callback function called when new data was received
// \param[in]   len           number of bytes available to read.
// \return                    none.
void USBD_CDC%Instance%_ACM_DataReceived (uint32_t len) {
  (void)len;
}
 
 
//! [code_USBD_User_CDC_ACM_RNDIS_VETH]
 
 
// Global functions exported for Virtual Ethernet driver
 
/**
  \fn          ARM_ETH_LINK_STATE RNDIS%Instance%_GetLinkState (void)
  \brief       Get RNDIS Device Link state (data initialized means link is up).
  \return      current link status \ref ARM_ETH_LINK_STATE
*/
ARM_ETH_LINK_STATE RNDIS%Instance%_GetLinkState (void) {
  if (rndis_state == RNDIS_DATA_INITIALIZED) {
    return ARM_ETH_LINK_UP;
  }
 
  return ARM_ETH_LINK_DOWN;
}
 
/**
  \fn          int32_t RNDIS%Instance%_SendFrame (const uint8_t *frame, uint32_t len, uint32_t flags)
  \brief       Send Ethernet frame over USB CDC ACM RNDIS.
  \param[in]   frame  Pointer to frame buffer with data to send
  \param[in]   len    Frame buffer length in bytes
  \return      \ref execution_status
*/
int32_t RNDIS%Instance%_SendFrame (const uint8_t *frame, uint32_t len) {
  REMOTE_NDIS_PACKET_MSG_t *ptr_packet_msg;
   int32_t                  usb_cdc_acm_status;
 
  ptr_packet_msg = (REMOTE_NDIS_PACKET_MSG_t *)((void *)packet_in);
 
  if ((rndis_state == RNDIS_DATA_INITIALIZED) &&
      (len >= ETH_MIN_SIZE)                   &&
      (len <= ETH_MAX_SIZE))                   {
    memcpy((void *)&ptr_packet_msg->PayLoad[0], frame, len);
    ptr_packet_msg->MessageType              = REMOTE_NDIS_PACKET_MSG;
    ptr_packet_msg->MessageLength            = len + 44U;
    ptr_packet_msg->DataOffset               = 36U;
    ptr_packet_msg->DataLength               = len;
    ptr_packet_msg->OutOfBandDataOffset      = 0U;
    ptr_packet_msg->OutOfBandDataLength      = 0U;
    ptr_packet_msg->NumOutOfBandDataElements = 0U;
    ptr_packet_msg->PerPacketInfoOffset      = 0U;
    ptr_packet_msg->PerPacketInfoLength      = 0U;
    ptr_packet_msg->Reserved[0]              = 0U;
    ptr_packet_msg->Reserved[1]              = 0U;
    usb_cdc_acm_status = USBD_CDC_ACM_WriteData (%Instance%, (const uint8_t *)ptr_packet_msg, (int32_t)ptr_packet_msg->MessageLength);
    if (usb_cdc_acm_status == (int32_t)ptr_packet_msg->MessageLength) {
      rcv_ok++;
      return ARM_DRIVER_OK;
    }
    if (usb_cdc_acm_status < 0) {
      rcv_error++;
      return ARM_DRIVER_ERROR;
    }
    if (usb_cdc_acm_status == 0) {
      return ARM_DRIVER_ERROR_BUSY;
    }
  }
 
  return ARM_DRIVER_ERROR;
}
 
/**
  \fn          int32_t RNDIS%Instance%_ReadFrame (uint8_t *frame, uint32_t len)
  \brief       Read data of Ethernet frame received over USB CDC ACM RNDIS.
  \param[in]   frame  Pointer to frame buffer for data to read into
  \param[in]   len    Frame buffer length in bytes
  \return      number of data bytes read or execution status
                 - value >= 0: number of data bytes read
                 - value < 0: error occurred, value is execution status as defined with \ref execution_status
*/
int32_t RNDIS%Instance%_ReadFrame (uint8_t *frame, uint32_t len) {
  REMOTE_NDIS_PACKET_MSG_t *ptr_packet_msg;
  uint32_t                  data_len;
   int32_t                  usb_cdc_acm_status;
 
  ptr_packet_msg = (REMOTE_NDIS_PACKET_MSG_t *)((void *)packet_out);
 
  if ((rndis_state == RNDIS_DATA_INITIALIZED) &&
      (len >= ETH_MIN_SIZE)                   &&
      (len <= ETH_MAX_SIZE))                   {
    usb_cdc_acm_status = USBD_CDC_ACM_ReadData (%Instance%, (uint8_t *)ptr_packet_msg, USBD_CDC%Instance%_ACM_RECEIVE_BUF_SIZE);
    if ((usb_cdc_acm_status != 0) && (usb_cdc_acm_status == (int32_t)ptr_packet_msg->MessageLength)) {
      data_len = len;
      if (data_len > ptr_packet_msg->DataLength) {
        data_len = ptr_packet_msg->DataLength;
      }
      memcpy(frame, (const void *)&ptr_packet_msg->PayLoad[0], data_len);
      xmit_ok++;
      return (int32_t)data_len;
    }
    if (usb_cdc_acm_status < 0) {
      xmit_error++;
      return ARM_DRIVER_ERROR;
    }
    if (usb_cdc_acm_status == 0) {
      return ARM_DRIVER_ERROR_BUSY;
    }
  }
 
  return 0;
}
 
/**
  \fn          uint32_t RNDIS%Instance%_GetRxFrameSize (void)
  \brief       Get size of Ethernet frame received over USB CDC ACM RNDIS.
  \return      number of bytes in received frame
*/
uint32_t RNDIS%Instance%_GetRxFrameSize (void) {
  uint32_t avail_data_len;
 
  avail_data_len = (uint32_t)USBD_CDC_ACM_DataAvailable (%Instance%);
 
  if (avail_data_len > 44U) {
    avail_data_len -= 44U;
  }
 
  return avail_data_len;
}
