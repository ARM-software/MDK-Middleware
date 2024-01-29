/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device:CDC
 * Copyright (c) 2018-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_CDC_ACM_RNDIS_ETH_%Instance%.c
 * Purpose: USB Device Communication Device Class (CDC)
 *          Abstract Control Model (ACM)
 *          Remote Network Driver Interface Specification (RNDIS)
 *          User Module for an USB CDC ACM RNDIS Ethernet Bridge
 * Rev.:    V1.0.4
 *----------------------------------------------------------------------------*/
/**
 * \addtogroup usbd_cdcFunctions
 *
 * USBD_User_CDC_ACM_RNDIS_ETH_%Instance%.c implements the application specific
 * functionality of the CDC ACM class using RNDIS protocol and is used
 * to implement Network Interface Card (NIC) to Ethernet Bridge
 * to the USB Host.
 *
 * The implementation depends on the configuration file USBD_Config_CDC_%Instance%.h.
 *
 */
 
 
//! [code_USBD_User_CDC_ACM_RNDIS_ETH]
 
#include "RTE_Components.h"
 
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
 
//   <o>Ethernet MAC Driver Index <0-255>
//     <i>Select Ethernet MAC driver index
#define ETH_MAC_NUM       0
 
//   <o>Ethernet PHY Driver Index <0-255>
//     <i>Select Ethernet PHY driver index
#define ETH_PHY_NUM       0
 
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
 
 
extern ARM_DRIVER_ETH_MAC ARM_Driver_ETH_MAC_(ETH_MAC_NUM);
extern ARM_DRIVER_ETH_PHY ARM_Driver_ETH_PHY_(ETH_PHY_NUM);
 
static ARM_DRIVER_ETH_MAC *EthMac = &ARM_Driver_ETH_MAC_(ETH_MAC_NUM);
static ARM_DRIVER_ETH_PHY *EthPhy = &ARM_Driver_ETH_PHY_(ETH_PHY_NUM);
 
// Local functions
static void               MAC_str_to_addr (const char *mac_str, uint8_t *mac_addr);
static void               InitVars        (void);
static void               ResetVars       (void);
static void               EthMac_Notify   (uint32_t event);
 
// Local variables
static uint32_t           rndis_state;
static ARM_ETH_LINK_STATE link_state;
static ARM_ETH_LINK_INFO  link_info;
static uint32_t           link_speed;
 
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
 
// Threads
#ifdef USB_CMSIS_RTOS2
static void ThreadConnection (void *arg);
static void ThreadDataIN     (void *arg);
static void ThreadDataOUT    (void *arg);
 
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t      thread_cb_mem_connection           __SECTION(.bss.os.thread.cb);
static uint64_t           thread_stack_mem_connection[512/8] __SECTION(.bss.os.thread.stack);
static osRtxThread_t      thread_cb_mem_data_in              __SECTION(.bss.os.thread.cb);
static uint64_t           thread_stack_mem_data_in   [512/8] __SECTION(.bss.os.thread.stack);
static osRtxThread_t      thread_cb_mem_data_out             __SECTION(.bss.os.thread.cb);
static uint64_t           thread_stack_mem_data_out  [512/8] __SECTION(.bss.os.thread.stack);
#endif
 
static const osThreadAttr_t thread_attr_connection = {
  "ThreadConnection",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
 &thread_cb_mem_connection,
  sizeof(osRtxThread_t),
 &thread_stack_mem_connection[0],
#else
  NULL,
  0U,
  NULL,
#endif
  512U,
  osPriorityNormal,
  0U,
  0U
};
 
static const osThreadAttr_t thread_attr_data_in = {
  "ThreadDataIN",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
 &thread_cb_mem_data_in,
  sizeof(osRtxThread_t),
 &thread_stack_mem_data_in[0],
#else
  NULL,
  0U,
  NULL,
#endif
  512U,
  osPriorityNormal,
  0U,
  0U
};
 
static const osThreadAttr_t thread_attr_data_out = {
  "ThreadDataOUT",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
 &thread_cb_mem_data_out,
  sizeof(osRtxThread_t),
 &thread_stack_mem_data_out[0],
#else
  NULL,
  0U,
  NULL,
#endif
  512U,
  osPriorityNormal,
  0U,
  0U
};
#else
static void ThreadConnection (void const *arg);
static void ThreadDataIN     (void const *arg);
static void ThreadDataOUT    (void const *arg);
 
extern const osThreadDef_t os_thread_def_ThreadConnection;
osThreadDef(ThreadConnection, osPriorityNormal, 1, NULL);
extern const osThreadDef_t os_thread_def_ThreadDataIN;
osThreadDef(ThreadDataIN,  osPriorityNormal, 1, NULL);
extern const osThreadDef_t os_thread_def_ThreadDataOUT;
osThreadDef(ThreadDataOUT, osPriorityNormal, 1, NULL);
#endif
 
static void *thread_id_Connection;
static void *thread_id_DataIN;
static void *thread_id_DataOUT;
 
 
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
  link_info.speed  = 0U;
  link_info.duplex = 0U;
  link_speed       = 0U;
 
  packet_filter    = 0U;
 
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
 
// Initialize Ethernet (MAC and PHY) Interface
// \return      true    initialization succeeded.
// \return      false   initialization failed.
static bool ENET_Initialize (void) {
  ARM_ETH_MAC_CAPABILITIES eth_capabilities;
 
  packet_filter   = ARM_ETH_MAC_ADDRESS_BROADCAST;
 
  // Initialize Media Access Controller
  eth_capabilities = EthMac->GetCapabilities();
 
  if (EthMac->Initialize(EthMac_Notify)    != ARM_DRIVER_OK) { return false; }
  if (EthMac->PowerControl(ARM_POWER_FULL) != ARM_DRIVER_OK) { return false; }
 
  if (eth_capabilities.mac_address != 0U) {
    // Hardware provided MAC address
    if (EthMac->GetMacAddress(&mac_address) != ARM_DRIVER_OK) { return false; }
  } else {
    // MAC address defined as ETH_MAC_ADDR string
    MAC_str_to_addr(RNDIS_MAC_ADDR, (uint8_t *)&mac_address);
    if (EthMac->SetMacAddress(&mac_address) != ARM_DRIVER_OK) { return false; }
  }
 
  // Initialize Physical Media Interface
  if (EthPhy->Initialize(EthMac->PHY_Read, EthMac->PHY_Write) != ARM_DRIVER_OK) { return false; }
  if (EthPhy->PowerControl(ARM_POWER_FULL)                    != ARM_DRIVER_OK) { return false; }
  if (EthPhy->SetInterface(eth_capabilities.media_interface)  != ARM_DRIVER_OK) { return false; }
  if (EthPhy->SetMode(ARM_ETH_PHY_AUTO_NEGOTIATE)             != ARM_DRIVER_OK) { return false; }
 
  // Create Threads
#ifdef USB_CMSIS_RTOS2
  thread_id_Connection = osThreadNew(ThreadConnection, NULL, &thread_attr_connection);
  thread_id_DataIN     = osThreadNew(ThreadDataIN,     NULL, &thread_attr_data_in);
  thread_id_DataOUT    = osThreadNew(ThreadDataOUT,    NULL, &thread_attr_data_out);
#else
  thread_id_Connection = osThreadCreate(osThread(ThreadConnection), NULL);
  thread_id_DataIN     = osThreadCreate(osThread(ThreadDataIN),     NULL);
  thread_id_DataOUT    = osThreadCreate(osThread(ThreadDataOUT),    NULL);
#endif
 
  // Set initial signal to Connection thread to check for current connection
#ifdef USB_CMSIS_RTOS2
  (void)osThreadFlagsSet(thread_id_Connection, 1U);
#else
  (void)osSignalSet(thread_id_Connection, 1U);
#endif
 
  return true;
}
 
 
// Uninitialize Ethernet (MAC and PHY) Interface
static void ENET_Uninitialize (void) {
 
  packet_filter = ARM_ETH_MAC_ADDRESS_BROADCAST;
 
  (void)EthMac->SetMacAddress(&mac_address);
  (void)EthMac->SetAddressFilter(NULL, 0);
  (void)EthMac->Control(ARM_ETH_MAC_CONFIGURE,
                       (uint32_t)(link_info.speed                           ) |
                       (uint32_t)(link_info.duplex << ARM_ETH_MAC_DUPLEX_Pos) |
                        packet_filter);
 
  // Terminate threads
  if (thread_id_Connection != NULL) { (void)osThreadTerminate(thread_id_Connection); thread_id_Connection = NULL; }
  if (thread_id_DataIN     != NULL) { (void)osThreadTerminate(thread_id_DataIN);     thread_id_DataIN     = NULL; }
  if (thread_id_DataOUT    != NULL) { (void)osThreadTerminate(thread_id_DataOUT);    thread_id_DataOUT    = NULL; }
 
  (void)EthPhy->PowerControl(ARM_POWER_OFF);
  (void)EthPhy->Uninitialize();
 
  (void)EthMac->PowerControl(ARM_POWER_OFF);
  (void)EthMac->Uninitialize();
}
 
 
// Called during USBD_Initialize to initialize the USB CDC class instance (ACM).
void USBD_CDC%Instance%_ACM_Initialize (void) {
  InitVars();
}
 
 
// Called during USBD_Uninitialize to de-initialize the USB CDC class instance (ACM).
void USBD_CDC%Instance%_ACM_Uninitialize (void) {
  InitVars();
  ENET_Uninitialize();
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
 
      (void)ENET_Initialize ();
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
 
      (void)ENET_Uninitialize ();
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
          ptr_query_cmplt->OIDInputBuffer[0]       = link_speed / 100U;
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
            num = by / sizeof(ARM_ETH_MAC_ADDR);
            if (EthMac->SetAddressFilter(mcast_address, num) != ARM_DRIVER_OK) {
              status = RNDIS_STATUS_FAILURE;
            }
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
              (void)EthMac->Control(ARM_ETH_MAC_CONFIGURE,
                                   (uint32_t)(link_info.speed                           ) |
                                   (uint32_t)(link_info.duplex << ARM_ETH_MAC_DUPLEX_Pos) |
                                    packet_filter);
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
 
      // Set flags to message processing threads
#ifdef USB_CMSIS_RTOS2
      (void)osThreadFlagsSet(thread_id_DataIN,  1U);
      (void)osThreadFlagsSet(thread_id_DataOUT, 1U);
#else
      (void)osSignalSet(thread_id_DataIN,  1U);
      (void)osSignalSet(thread_id_DataOUT, 1U);
#endif
 
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
 
#ifdef USB_CMSIS_RTOS2
  (void)osThreadFlagsSet(thread_id_DataOUT, 1U);
#else
  (void)osSignalSet(thread_id_DataOUT, 1U);
#endif
}
 
 
//! [code_USBD_User_CDC_ACM_RNDIS_ETH]
 
 
// Called upon Ethernet MAC Event
static void EthMac_Notify (uint32_t event) {
 
  switch (event) {
    case ARM_ETH_MAC_EVENT_RX_FRAME:
#ifdef USB_CMSIS_RTOS2
      (void)osThreadFlagsSet(thread_id_DataIN, 1U);
#else
      (void)osSignalSet(thread_id_DataIN, 1U);
#endif
      break;
    default:
      break;
  }
}
 
 
// Thread handling ETH Connection
#ifdef USB_CMSIS_RTOS2
__NO_RETURN static void ThreadConnection (void *arg) {
#else
__NO_RETURN static void ThreadConnection (void const *arg) {
#endif
  ARM_ETH_LINK_STATE link_state_local;
  uint32_t           speed_local;
 
  (void)(arg);
 
  for (;;) {
#ifdef USB_CMSIS_RTOS2
    (void)osThreadFlagsWait(1U, osFlagsWaitAll, 500U);
#else
    (void)osSignalWait(1U, 500U);
#endif
    link_state_local = EthPhy->GetLinkState();
    if (link_state_local == ARM_ETH_LINK_UP) {
      link_info = EthPhy->GetLinkInfo();
      switch (link_info.speed) {
        case 0:  speed_local =   10000000U; break;
        case 1:  speed_local =  100000000U; break;
        case 2:  speed_local = 1000000000U; break;
        default: speed_local = 0U;
      }
    } else {
      speed_local = 0U;
    }
    if (link_speed != speed_local) {
      link_speed = speed_local;
    }
    if (link_state != link_state_local) {
      link_state = link_state_local;
      if (link_state == ARM_ETH_LINK_UP) {
        (void)EthMac->Control(ARM_ETH_MAC_CONFIGURE,
                             (uint32_t)(link_info.speed                           ) |
                             (uint32_t)(link_info.duplex << ARM_ETH_MAC_DUPLEX_Pos) |
                              packet_filter);
        (void)EthMac->Control(ARM_ETH_MAC_CONTROL_TX, 1U);
        (void)EthMac->Control(ARM_ETH_MAC_CONTROL_RX, 1U);
        link_state_down = false;
        link_state_up   = true;
      } else {
        (void)EthMac->Control(ARM_ETH_MAC_FLUSH,
                              ARM_ETH_MAC_FLUSH_TX |
                              ARM_ETH_MAC_FLUSH_RX);
        (void)EthMac->Control(ARM_ETH_MAC_CONTROL_TX, 0U);
        (void)EthMac->Control(ARM_ETH_MAC_CONTROL_RX, 0U);
        link_state_up   = false;
        link_state_down = true;
      }
    }
  }
}
 
 
// Thread handling Data IN (ETH -> USB)
#ifdef USB_CMSIS_RTOS2
__NO_RETURN static void ThreadDataIN (void *arg) {
#else
__NO_RETURN static void ThreadDataIN (void const *arg) {
#endif
  REMOTE_NDIS_PACKET_MSG_t *ptr_packet_msg;
  uint32_t                  eth_rx_size;
   int32_t                  usb_cdc_acm_status;
 
  (void)(arg);
 
  ptr_packet_msg = (REMOTE_NDIS_PACKET_MSG_t *)((void *)packet_in);
  for (;;) {
#ifdef USB_CMSIS_RTOS2
    (void)osThreadFlagsWait(1U, osFlagsWaitAll, osWaitForever);
#else
    (void)osSignalWait(1U, osWaitForever);
#endif
    for (;;) {
      eth_rx_size = EthMac->GetRxFrameSize();
      if (eth_rx_size == 0) {           // No new data available on the Ethernet
        break;                          // exit the loop, and wait for new message
      }
      if ((link_state  == ARM_ETH_LINK_UP)        &&
          (rndis_state == RNDIS_DATA_INITIALIZED) &&
          (eth_rx_size >= ETH_MIN_SIZE)           &&
          (eth_rx_size <= ETH_MAX_SIZE))           {
        (void)EthMac->ReadFrame((uint8_t *)&ptr_packet_msg->PayLoad[0], eth_rx_size);
        ptr_packet_msg->MessageType              = REMOTE_NDIS_PACKET_MSG;
        ptr_packet_msg->MessageLength            = eth_rx_size + 44U;
        ptr_packet_msg->DataOffset               = 36U;
        ptr_packet_msg->DataLength               = eth_rx_size;
        ptr_packet_msg->OutOfBandDataOffset      = 0U;
        ptr_packet_msg->OutOfBandDataLength      = 0U;
        ptr_packet_msg->NumOutOfBandDataElements = 0U;
        ptr_packet_msg->PerPacketInfoOffset      = 0U;
        ptr_packet_msg->PerPacketInfoLength      = 0U;
        ptr_packet_msg->Reserved[0]              = 0U;
        ptr_packet_msg->Reserved[1]              = 0U;
        do {
          usb_cdc_acm_status = USBD_CDC_ACM_WriteData (%Instance%, (const uint8_t *)ptr_packet_msg, (int32_t)ptr_packet_msg->MessageLength);
        } while (usb_cdc_acm_status == 0);
        if (usb_cdc_acm_status == (int32_t)ptr_packet_msg->MessageLength) {
          rcv_ok++;
        } else {
          rcv_error++;
        }
      } else {
        (void)EthMac->ReadFrame(NULL, 0);       // Dump packet
      }
    }
  }
}
 
 
// Thread handling Data OUT (USB -> ETH)
#ifdef USB_CMSIS_RTOS2
__NO_RETURN static void ThreadDataOUT (void *arg) {
#else
__NO_RETURN static void ThreadDataOUT (void const *arg) {
#endif
  REMOTE_NDIS_PACKET_MSG_t *ptr_packet_msg;
  uint32_t                  eth_tx_size;
   int32_t                  usb_cdc_acm_status, eth_mac_status;
 
  (void)(arg);
 
  eth_tx_size = 0U;
 
  ptr_packet_msg = (REMOTE_NDIS_PACKET_MSG_t *)((void *)packet_out);
  for (;;) {
#ifdef USB_CMSIS_RTOS2
    (void)osThreadFlagsWait(1U, osFlagsWaitAll, osWaitForever);
#else
    (void)osSignalWait(1U, osWaitForever);
#endif
    for (;;) {
      usb_cdc_acm_status = USBD_CDC_ACM_ReadData (%Instance%, (uint8_t *)ptr_packet_msg, USBD_CDC0_ACM_RECEIVE_BUF_SIZE);
      if (usb_cdc_acm_status == 0) {    // No new data available on the USB
        break;                          // exit the loop, and wait for new message
      } else if (usb_cdc_acm_status > (int32_t)ptr_packet_msg->DataLength) {
        eth_tx_size = ptr_packet_msg->DataLength;
      }
      if ((link_state  == ARM_ETH_LINK_UP)        &&
          (rndis_state == RNDIS_DATA_INITIALIZED) &&
          (eth_tx_size >= ETH_MIN_SIZE)           &&
          (eth_tx_size <= ETH_MAX_SIZE))           {
        do {
          eth_mac_status = EthMac->SendFrame((uint8_t *)&ptr_packet_msg->PayLoad[0], (uint32_t)(eth_tx_size), 0U);
        } while (eth_mac_status == ARM_DRIVER_ERROR_BUSY);
        if (eth_mac_status == ARM_DRIVER_OK) {
          xmit_ok++;
        } else {
          xmit_error++;
        }
      }
    }
  }
}
