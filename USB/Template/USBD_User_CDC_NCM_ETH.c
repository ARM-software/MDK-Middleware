/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device:CDC
 * Copyright (c) 2004-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_CDC_NCM_ETH_%Instance%.c
 * Purpose: USB Device Communication Device Class (CDC)
 *          Network Control Model (NCM) User module for an USB Ethernet Bridge
 * Rev.:    V1.0.8
 *----------------------------------------------------------------------------*/
/**
 * \addtogroup usbd_cdcFunctions
 *
 * USBD_User_CDC_NCM_ETH_%Instance%.c implements the application specific functionality
 * of the CDC NCM class and is used to respond to control endpoint requests
 * as well as receive and send data to the USB Host.
 * It implements an USB Ethernet Bridge.
 *
 * The implementation depends on the configuration file USBD_Config_CDC_%Instance%.h.
 *
 */
 
 
//! [code_USBD_User_CDC_NCM_ETH]
 
#include <stdio.h>
#include <string.h>
 
#include "rl_usb.h"
 
#include "Driver_ETH_MAC.h"             // ::CMSIS Driver:Ethernet MAC
#include "Driver_ETH_PHY.h"             // ::CMSIS Driver:Ethernet PHY
#include "USBD_Config_CDC_%Instance%.h"
 
#define ETH_MAC_NUM             0       /* ETH MAC Driver number */
#define ETH_PHY_NUM             0       /* ETH PHY Driver number */
#define USB_CDC_NUM             %Instance%       /* USB CDC Instance number */
 
#define MAX_IN_DATAGRAMS        10
 
extern ARM_DRIVER_ETH_MAC ARM_Driver_ETH_MAC_(ETH_MAC_NUM);
extern ARM_DRIVER_ETH_PHY ARM_Driver_ETH_PHY_(ETH_PHY_NUM);
 
static ARM_DRIVER_ETH_MAC *EthMac = &ARM_Driver_ETH_MAC_(ETH_MAC_NUM);
static ARM_DRIVER_ETH_PHY *EthPhy = &ARM_Driver_ETH_PHY_(ETH_PHY_NUM);
 
static const wchar_t   *wsMacAddress = USBD_CDC%Instance%_NCM_MAC_ADDRESS;
static ARM_ETH_MAC_ADDR   MacAddress;
 
static ARM_ETH_LINK_STATE LinkState;
static ARM_ETH_LINK_INFO  LinkInfo;
static uint32_t           LinkSpeed;
 
static uint32_t           PacketFilter;
 
static struct {
  uint32_t ntb_input_size;
  uint16_t ntb_format;
  uint16_t max_datagram_size;
  uint16_t crc_mode;
#if ((USBD_CDC%Instance%_NCM_BM_NETWORK_CAPABILITIES & 0x02) != 0)
  uint8_t  net_address[6];
#endif
#if (USBD_CDC%Instance%_NCM_W_NUMBER_MC_FILTERS != 0)
  uint8_t  mc_filters[6*(USBD_CDC%Instance%_NCM_W_NUMBER_MC_FILTERS & 0x7FFF)];
#endif
#if (USBD_CDC%Instance%_NCM_B_NUMBER_POWER_FILTERS != 0)
  uint8_t  power_filter_active[(USBD_CDC%Instance%_NCM_B_NUMBER_POWER_FILTERS + 7)/8];
#endif
  uint16_t packet_filter_bitmap;
#if (USBD_CDC%Instance%_NCM_BM_ETHERNET_STATISTICS != 0)
  uint32_t eth_statistics[29];
#endif
} NCM_State;
 
static uint32_t FrameIN_Size;
static uint8_t  FrameIN [USBD_CDC%Instance%_NCM_W_MAX_SEGMENT_SIZE];
static uint8_t  FrameOUT[USBD_CDC%Instance%_NCM_W_MAX_SEGMENT_SIZE];
 
static void EthMac_Notify (uint32_t event);
 
#ifdef USB_CMSIS_RTOS2
static void Connection_Thread (void *arg);
static void DataIN_Thread     (void *arg);
static void DataOUT_Thread    (void *arg);
 
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t        connection_thread_cb_mem           __SECTION(.bss.os.thread.cb);
static uint64_t             connection_thread_stack_mem[512/8] __SECTION(.bss.os.thread.stack);
static osRtxThread_t        data_in_thread_cb_mem              __SECTION(.bss.os.thread.cb);
static uint64_t             data_in_thread_stack_mem   [512/8] __SECTION(.bss.os.thread.stack);
static osRtxThread_t        data_out_thread_cb_mem             __SECTION(.bss.os.thread.cb);
static uint64_t             data_out_thread_stack_mem  [512/8] __SECTION(.bss.os.thread.stack);
#endif
 
static const osThreadAttr_t connection_thread_attr = {
  "Connection_Thread",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
 &connection_thread_cb_mem,
  sizeof(osRtxThread_t),
 &connection_thread_stack_mem[0],
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
 
static const osThreadAttr_t data_in_thread_attr = {
  "DataIN_Thread",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
 &data_in_thread_cb_mem,
  sizeof(osRtxThread_t),
 &data_in_thread_stack_mem[0],
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
 
static const osThreadAttr_t data_out_thread_attr = {
  "DataOUT_Thread",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
 &data_out_thread_cb_mem,
  sizeof(osRtxThread_t),
 &data_out_thread_stack_mem[0],
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
static void Connection_Thread (void const *arg);
static void DataIN_Thread     (void const *arg);
static void DataOUT_Thread    (void const *arg);
 
extern const osThreadDef_t os_thread_def_Connection_Thread;
osThreadDef(Connection_Thread, osPriorityNormal, 1, NULL);
extern const osThreadDef_t os_thread_def_DataIN_Thread;
osThreadDef(DataIN_Thread,     osPriorityNormal, 1, NULL);
extern const osThreadDef_t os_thread_def_DataOUT_Thread;
osThreadDef(DataOUT_Thread,    osPriorityNormal, 1, NULL);
#endif
 
static void *Connection_ThreadId;
static void *DataIN_ThreadId;
static void *DataOUT_ThreadId;
 
 
// MAC Address conversion from wide string
// \param[in]   wstr    Pointer to wide string.
// \param[out]  addr    Pointer to address.
static void MAC_wstr_to_addr (const wchar_t *wstr, uint8_t *addr) {
  uint8_t  c;
  uint8_t  n;
  uint32_t i;

  for (i = 0U; i < 12U; i++) {
    c = (uint8_t)wstr[i];
    if        ((c >= '0') && (c <= '9')) {
      n = c - '0';
    } else if ((c >= 'A') && (c <= 'F')) {
      n = c - ('A' + 10U);
    } else if ((c >= 'a') && (c <= 'f')) {
      n = c - ('a' + 10U);
    } else {
      n = 0U;
    }
    if ((i & 1U) != 0U) {
      addr[i>>1] |= n;
    } else {
      addr[i>>1]  = (uint8_t)((uint32_t)n << 4);
    }
  }
}
 
 
// Called during USBD_Initialize to initialize the USB CDC class instance (NCM).
void USBD_CDC%Instance%_NCM_Initialize (void) {
  // Add code for initialization
  ARM_ETH_MAC_CAPABILITIES capabilities;
  int32_t                  status;
 
  MAC_wstr_to_addr(wsMacAddress, (uint8_t *)&MacAddress);
 
  memset(&NCM_State, 0, sizeof(NCM_State));
  NCM_State.ntb_input_size = USBD_CDC%Instance%_NCM_DW_NTB_IN_MAX_SIZE;
  NCM_State.max_datagram_size = USBD_CDC%Instance%_NCM_W_MAX_SEGMENT_SIZE;
#if ((USBD_CDC%Instance%_NCM_BM_NETWORK_CAPABILITIES & 0x02) != 0)
  memcpy(NCM_State.net_address, &MacAddress, 6);
#endif
  NCM_State.packet_filter_bitmap = 0x0CU;
 
  LinkState = ARM_ETH_LINK_DOWN;
  LinkInfo.speed  = 0U;
  LinkInfo.duplex = 0U;
  LinkSpeed = 0U;
 
  PacketFilter = ARM_ETH_MAC_ADDRESS_BROADCAST;
 
  FrameIN_Size = 0U;
  memset(FrameIN, 0, sizeof(FrameIN));
 
  // Initialize Media Access Controller
  capabilities = EthMac->GetCapabilities();
 
  status = EthMac->Initialize(EthMac_Notify);
  if (status != ARM_DRIVER_OK) { return; }
  status = EthMac->PowerControl(ARM_POWER_FULL);
  if (status != ARM_DRIVER_OK) { return; }
 
  status = EthMac->SetMacAddress(&MacAddress);
  if (status != ARM_DRIVER_OK) { return; }
 
  // Initialize Physical Media Interface
  status = EthPhy->Initialize(EthMac->PHY_Read, EthMac->PHY_Write);
  if (status != ARM_DRIVER_OK) { return; }
  status = EthPhy->PowerControl(ARM_POWER_FULL);
  if (status != ARM_DRIVER_OK) { return; }
  status = EthPhy->SetInterface(capabilities.media_interface);
  if (status != ARM_DRIVER_OK) { return; }
  status = EthPhy->SetMode(ARM_ETH_PHY_AUTO_NEGOTIATE);
  if (status != ARM_DRIVER_OK) { return; }
 
  // Create Threads
#ifdef USB_CMSIS_RTOS2
  Connection_ThreadId = osThreadNew(Connection_Thread, NULL, &connection_thread_attr);
  DataIN_ThreadId     = osThreadNew(DataIN_Thread,     NULL, &data_in_thread_attr);
  DataOUT_ThreadId    = osThreadNew(DataOUT_Thread,    NULL, &data_out_thread_attr);
#else
  Connection_ThreadId = osThreadCreate(osThread(Connection_Thread), NULL);
  DataIN_ThreadId     = osThreadCreate(osThread(DataIN_Thread),     NULL);
  DataOUT_ThreadId    = osThreadCreate(osThread(DataOUT_Thread),    NULL);
#endif
}
 
 
// Called during USBD_Uninitialize to de-initialize the USB CDC class instance (NCM).
void USBD_CDC%Instance%_NCM_Uninitialize (void) {
  // Add code for de-initialization
  (void)osThreadTerminate(Connection_ThreadId);
  (void)osThreadTerminate(DataIN_ThreadId);
  (void)osThreadTerminate(DataOUT_ThreadId);
 
  (void)EthPhy->PowerControl(ARM_POWER_OFF);
  (void)EthPhy->Uninitialize();
 
  (void)EthMac->PowerControl(ARM_POWER_OFF);
  (void)EthMac->Uninitialize();
}
 
 
// Called upon USB Bus Reset Event.
void USBD_CDC%Instance%_NCM_Reset (void) {
  // Add code for reset
}
 
 
// Called when USB Host changes data interface from alternate 0 to alternate 1 (activates data interface).
void USBD_CDC%Instance%_NCM_Start (void) {
  // Add code for data interface activation
#ifdef USB_CMSIS_RTOS2
  (void)osThreadFlagsSet(Connection_ThreadId, 1U);
  (void)osThreadFlagsSet(DataIN_ThreadId,     1U);
#else
  (void)osSignalSet(Connection_ThreadId, 1U);
  (void)osSignalSet(DataIN_ThreadId,     1U);
#endif
}
 
 
// Called when USB Host changes data interface from alternate 1 to alternate 0 (de-activates data interface).
void USBD_CDC%Instance%_NCM_Stop (void) {
  // Add code for data interface de-activation
  // Explained in ncm10.pdf document from www.usb.org in paragraph 7.2
 
  FrameIN_Size = 0U;
#ifdef USB_CMSIS_RTOS
  osSignalClear(DataIN_ThreadId, 1U);
#endif
 
  memset(&NCM_State, 0, sizeof(NCM_State));
  NCM_State.ntb_input_size = USBD_CDC%Instance%_NCM_DW_NTB_IN_MAX_SIZE;
  NCM_State.max_datagram_size = USBD_CDC%Instance%_NCM_W_MAX_SEGMENT_SIZE;
#if ((USBD_CDC%Instance%_NCM_BM_NETWORK_CAPABILITIES & 0x02) != 0)
  memcpy(NCM_State.net_address, &MacAddress, 6);
#endif
  NCM_State.packet_filter_bitmap = 0x0CU;
 
  PacketFilter = ARM_ETH_MAC_ADDRESS_BROADCAST;
 
  (void)EthMac->SetMacAddress(&MacAddress);
  (void)EthMac->SetAddressFilter(NULL, 0);
  (void)EthMac->Control(ARM_ETH_MAC_CONFIGURE,
                       (uint32_t)(LinkInfo.speed                           ) |
                       (uint32_t)(LinkInfo.duplex << ARM_ETH_MAC_DUPLEX_Pos) |
                        PacketFilter);
}
 
 
// Called upon USB Host request to set the Ethernet device multicast filters.
// \param[in]   addr_list            Pointer to list of 48-bit Multicast addresses.
// \param[in]   num_of_filters       Number of filters.
// \return      true                 if this request was handled.
// \return      false                if this request was not handled.
bool USBD_CDC%Instance%_NCM_SetEthernetMulticastFilters (const uint8_t *addr_list, uint16_t num_of_filters) {
#if (USBD_CDC%Instance%_NCM_W_NUMBER_MC_FILTERS != 0)
  int32_t status;
 
  if (num_of_filters > (USBD_CDC%Instance%_NCM_W_NUMBER_MC_FILTERS & 0x7FFF)) { return false; }
 
  // Add code for handling request
  status = EthMac->SetAddressFilter((ARM_ETH_MAC_ADDR *)((uint32_t)addr_list), num_of_filters);
  if (status != ARM_DRIVER_OK) { return false; }
 
  memcpy(NCM_State.mc_filters, addr_list, num_of_filters * 6);
 
  return true;
#else
  (void)addr_list;
  (void)num_of_filters;
  return false;
#endif
}
 
 
// Called upon USB Host request to set up the specified Ethernet power management pattern filter.
// \param[in]   filter_number        Filter number.
// \param[in]   pattern_filter       Power management pattern filter structure.
// \param[in]   pattern_filter_size  Size of pattern filter structure.
// \return      true                 if this request was handled.
// \return      false                if this request was not handled.
bool USBD_CDC%Instance%_NCM_SetEthernetPowerManagementPatternFilter (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size) {
#if (USBD_CDC%Instance%_NCM_B_NUMBER_POWER_FILTERS != 0)
  if (filter_number >= USBD_CDC%Instance%_NCM_B_NUMBER_POWER_FILTERS) { return false; }
 
  // Add code for handling request
 
  NCM_State.power_filter_active[filter_number / 8] |= 1U << (filter_number % 8);
 
  return true;
#else
  (void)filter_number;
  (void)pattern_filter;
  (void)pattern_filter_size;
  return false;
#endif
}
 
 
// Called upon USB Host request to retrieve the status of the specified Ethernet power management pattern filter.
// \param[in]   filter_number        Filter number.
// \param[out]  pattern_active       Pointer to pattern active boolean.
// \return      true                 if this request was handled.
// \return      false                if this request was not handled.
bool USBD_CDC%Instance%_NCM_GetEthernetPowerManagementPatternFilter (uint16_t filter_number, uint16_t *pattern_active) {
#if (USBD_CDC%Instance%_NCM_B_NUMBER_POWER_FILTERS != 0)
  *pattern_active = (NCM_State.power_filter_active[filter_number / 8] & (1U << (filter_number % 8))) ? 1 : 0;
  return true;
#else
  (void)filter_number;
  (void)pattern_active;
  return false;
#endif
}
 
 
// Called upon USB Host request to configure device Ethernet packet filter settings.
// \param[in]   packet_filter_bitmap Packet Filter Bitmap.
// \return      true                 if this request was handled.
// \return      false                if this request was not handled.
bool USBD_CDC%Instance%_NCM_SetEthernetPacketFilter (uint16_t packet_filter_bitmap) {
#if ((USBD_CDC%Instance%_NCM_BM_NETWORK_CAPABILITIES & 0x01) != 0)
  int32_t status;
 
  // Add code for handling request
  PacketFilter = ((packet_filter_bitmap & 0x01U) ? ARM_ETH_MAC_ADDRESS_ALL       : 0) |
                 ((packet_filter_bitmap & 0x02U) ? ARM_ETH_MAC_ADDRESS_MULTICAST : 0) |
                 ((packet_filter_bitmap & 0x08U) ? ARM_ETH_MAC_ADDRESS_BROADCAST : 0);
 
  status = EthMac->Control(ARM_ETH_MAC_CONFIGURE,
                          (uint32_t)(LinkInfo.speed                           ) |
                          (uint32_t)(LinkInfo.duplex << ARM_ETH_MAC_DUPLEX_Pos) |
                           PacketFilter);
  if (status != ARM_DRIVER_OK) { return false; }
 
  NCM_State.packet_filter_bitmap = packet_filter_bitmap;
 
  return true;
#else
  (void)packet_filter_bitmap;
  return false;
#endif
}
 
 
// Called upon USB Host request to retrieve a statistic based on the feature selector.
// \param[in]   feature_selector     Feature Selector.
// \param[out]  data                 Pointer to Statistics Value.
// \return      true                 if this request was handled.
// \return      false                if this request was not handled.
bool USBD_CDC%Instance%_NCM_GetEthernetStatistic (uint16_t feature_selector, uint32_t *data) {
#if (USBD_CDC%Instance%_NCM_BM_ETHERNET_STATISTICS != 0)
  if (feature_selector == 0x00U) { return true;  }
  if (feature_selector >  0x1DU) { return false; }
  *data = NCM_State.eth_statistics[feature_selector - 1];
  return true;
#else
  (void)feature_selector;
  (void)data;
  return false;
#endif
}
 
 
// Called upon USB Host request to retrieve the parameters that describe NTBs for each direction.
// \param[out]  ntb_params           Pointer to NTB Parameter Structure.
// \return      true                 if this request was handled.
// \return      false                if this request was not handled.
bool USBD_CDC%Instance%_NCM_GetNtbParameters (CDC_NCM_NTB_PARAM *ntb_params) {
 
  ntb_params->wLength                 = sizeof(CDC_NCM_NTB_PARAM);
  ntb_params->bmNtbFormatsSupported   = USBD_CDC%Instance%_NCM_BM_NTB_FORMATS_SUPPORTED;
  ntb_params->dwNtbInMaxSize          = USBD_CDC%Instance%_NCM_DW_NTB_IN_MAX_SIZE;
  ntb_params->wNdpInDivisor           = USBD_CDC%Instance%_NCM_W_NDP_IN_DIVISOR;
  ntb_params->wNdpInPayloadRemainder  = USBD_CDC%Instance%_NCM_W_NDP_IN_PAYLOAD_REMINDER;
  ntb_params->wNdpInAlignment         = USBD_CDC%Instance%_NCM_W_NDP_IN_ALIGNMENT;
  ntb_params->Reserved0               = 0U;
  ntb_params->dwNtbOutMaxSize         = USBD_CDC%Instance%_NCM_DW_NTB_OUT_MAX_SIZE;
  ntb_params->wNdpOutDivisor          = USBD_CDC%Instance%_NCM_W_NDP_OUT_DIVISOR;
  ntb_params->wNdpOutPayloadRemainder = USBD_CDC%Instance%_NCM_W_NDP_OUT_PAYLOAD_REMINDER;
  ntb_params->wNdpOutAlignment        = USBD_CDC%Instance%_NCM_W_NDP_OUT_ALIGNMENT;
  ntb_params->Reserved1               = 0U;
 
  return true;
}
 
 
// Called upon USB Host request to return the USB Device’s current EUI-48 station address.
// \param[out]  net_addr             Pointer to EUI-48 current address, in network byte order.
// \return      true                 if this request was handled.
// \return      false                if this request was not handled.
bool USBD_CDC%Instance%_NCM_GetNetAddress (uint8_t *net_addr) {
#if ((USBD_CDC%Instance%_NCM_BM_NETWORK_CAPABILITIES & 0x02) != 0)
  memcpy(net_addr, NCM_State.net_address, 6);
  return true;
#else
  (void)net_addr;
  return false;
#endif
}
 
 
// Called upon USB Host request to set the USB Device’s current EUI-48 station address.
// \param[in]   net_addr             Pointer to EUI-48 address, in network byte order.
// \return      true                 if this request was handled.
// \return      false                if this request was not handled.
bool USBD_CDC%Instance%_NCM_SetNetAddress (const uint8_t *net_addr) {
#if ((USBD_CDC%Instance%_NCM_BM_NETWORK_CAPABILITIES & 0x02) != 0)
  int32_t status;
 
  // Add code for handling request
  status = EthMac->SetMacAddress((ARM_ETH_MAC_ADDR *)((uint32_t)net_addr));
  if (status != ARM_DRIVER_OK) { return false; }
 
  memcpy(NCM_State.net_address, net_addr, 6);
 
  return true;
#else
  (void)net_addr;
  return false;
#endif
}
 
 
// Called upon USB Host request to return the NTB data format currently being used.
// \param[out]  ntb_format           Pointer to NTB format code.
// \return      true                 if this request was handled.
// \return      false                if this request was not handled.
bool USBD_CDC%Instance%_NCM_GetNtbFormat (uint16_t *ntb_format) {
  *ntb_format = NCM_State.ntb_format;
  return true;
}
 
 
// Called upon USB Host request to select the format of NTB to be used for NTBs transmitted to the USB Host.
// \param[in]   ntb_format           NTB format selection:
//                - value 0 :        NTB-16
//                - value 1 :        NTB-32
// \return      true                 if this request was handled.
// \return      false                if this request was not handled.
bool USBD_CDC%Instance%_NCM_SetNtbFormat (uint16_t ntb_format) {
#if (USBD_CDC%Instance%_NCM_BM_NTB_FORMATS_SUPPORTED > 1)
  if (ntb_format > 1) { return false; }
#else
  if (ntb_format > 0) { return false; }
#endif
  NCM_State.ntb_format = ntb_format;
  return true;
}
 
 
// Called upon USB Host request to return NTB input size currently being used.
// \param[out]  ntb_input_size       Pointer to NTB input size.
// \return      true                 if this request was handled.
// \return      false                if this request was not handled.
bool USBD_CDC%Instance%_NCM_GetNtbInputSize (uint32_t *ntb_input_size) {
  *ntb_input_size = NCM_State.ntb_input_size;
  return true;
}
 
 
// Called upon USB Host request to select the maximum size of NTB that is permitted to be sent to the USB Host.
// \param[in]   ntb_input_size       Maximum NTB size.
// \return      true                 if this request was handled.
// \return      false                if this request was not handled.
bool USBD_CDC%Instance%_NCM_SetNtbInputSize (uint32_t ntb_input_size) {
  if (ntb_input_size > USBD_CDC%Instance%_NCM_DW_NTB_IN_MAX_SIZE) { return false; }
  NCM_State.ntb_input_size = ntb_input_size;
  return true;
}
 
 
// Called upon USB Host request to return the currently effective maximum datagram size.
// \param[out]  max_datagram_size    Pointer to current maximum datagram size.
// \return      true                 if this request was handled.
// \return      false                if this request was not handled.
bool USBD_CDC%Instance%_NCM_GetMaxDatagramSize (uint16_t *max_datagram_size) {
#if ((USBD_CDC%Instance%_NCM_BM_NETWORK_CAPABILITIES & 0x08) != 0)
  *max_datagram_size = NCM_State.max_datagram_size;
  return true;
#else
  (void)max_datagram_size;
  return false;
#endif
}
 
 
// Called upon USB Host request to select the maximum datagram size that can be sent in an NTB.
// \param[in]   max_datagram_size    Maximum datagram size.
// \return      true                 if this request was handled.
// \return      false                if this request was not handled.
bool USBD_CDC%Instance%_NCM_SetMaxDatagramSize (uint16_t max_datagram_size) {
#if ((USBD_CDC%Instance%_NCM_BM_NETWORK_CAPABILITIES & 0x08) != 0)
  if (max_datagram_size > USBD_CDC%Instance%_NCM_W_MAX_SEGMENT_SIZE) { return false; }
  NCM_State.max_datagram_size = max_datagram_size;
  return true;
#else
  (void)max_datagram_size;
  return false;
#endif
}
 
 
// Called upon USB Host request to return the currently selected CRC mode.
// \param[out]  crc_mode             Pointer to current CRC mode.
// \return      true                 if this request was handled.
// \return      false                if this request was not handled.
bool USBD_CDC%Instance%_NCM_GetCrcMode (uint16_t *crc_mode) {
#if ((USBD_CDC%Instance%_NCM_BM_NETWORK_CAPABILITIES & 0x10) != 0)
  *crc_mode = NCM_State.crc_mode;
  return true;
#else
  (void)crc_mode;
  return false;
#endif
}
 
 
// Called upon USB Host request to control CRC mode.
// \param[in]   crc_mode             CRC mode:
//                - value 0 :        CRCs shall not be appended
//                - value 1 :        CRCs shall be appended
// \return      true                 if this request was handled.
// \return      false                if this request was not handled.
bool USBD_CDC%Instance%_NCM_SetCrcMode (uint16_t crc_mode) {
#if ((USBD_CDC%Instance%_NCM_BM_NETWORK_CAPABILITIES & 0x10) != 0)
  if (crc_mode > 1) { return false; }
  NCM_State.crc_mode = crc_mode;
  return true;
#else
  (void)crc_mode;
  return false;
#endif
}
 
 
// Called when NTB was successfully sent.
void USBD_CDC%Instance%_NCM_NTB_IN_Sent (void) {
  // Add code for handling request
#ifdef USB_CMSIS_RTOS2
  (void)osThreadFlagsSet(DataIN_ThreadId, 1U);
#else
  (void)osSignalSet(DataIN_ThreadId, 1U);
#endif
}
 
// Called when NTB was successfully received.
void USBD_CDC%Instance%_NCM_NTB_OUT_Received (void) {
  // Add code for handling request
#ifdef USB_CMSIS_RTOS2
  (void)osThreadFlagsSet(DataOUT_ThreadId, 1U);
#else
  (void)osSignalSet(DataOUT_ThreadId, 1U);
#endif
}
 
//! [code_USBD_User_CDC_NCM_ETH]


// Called upon Ethernet MAC Event
static void EthMac_Notify (uint32_t event) {

  switch (event) {
    case ARM_ETH_MAC_EVENT_RX_FRAME:
#ifdef USB_CMSIS_RTOS2
      (void)osThreadFlagsSet(DataIN_ThreadId, 2U);
#else
      (void)osSignalSet(DataIN_ThreadId, 2U);
#endif
      break;
    default:
      break;
  }
}


// Thread handling ETH Connection
#ifdef USB_CMSIS_RTOS2
__NO_RETURN static void Connection_Thread (void *arg) {
#else
__NO_RETURN static void Connection_Thread (void const *arg) {
#endif
  ARM_ETH_LINK_STATE link_state;
#ifdef USB_CMSIS_RTOS
  osEvent            os_event;
#endif
  uint32_t           event;
  uint32_t           speed;
  uint16_t           state;
  int32_t            status;

  (void)(arg);

  for (;;) {
#ifdef USB_CMSIS_RTOS2
    event = osThreadFlagsWait(1U, osFlagsWaitAll, 500U);
#else
    os_event = osSignalWait(1U, 500U);
    if (os_event.status == osEventSignal) {
      event = (uint32_t)os_event.value.signals;
    } else {
      event = 0x80000000U;
    }
#endif
    link_state = EthPhy->GetLinkState();
    if (link_state == ARM_ETH_LINK_UP) {
      LinkInfo = EthPhy->GetLinkInfo();
      switch (LinkInfo.speed) {
        case 0:  speed =   10000000U; break;
        case 1:  speed =  100000000U; break;
        case 2:  speed = 1000000000U; break;
        default: speed = 0U;
      }
    } else {
      speed = 0U;
    }
    if (speed != LinkSpeed) {
      LinkSpeed = speed;
      if (link_state == ARM_ETH_LINK_UP) {
        do {
          status = USBD_CDC_NCM_Notify_ConnectionSpeedChange(USB_CDC_NUM, speed, speed);
        } while (status == -(int32_t)usbDriverBusy);
      }
    }
    if (link_state != LinkState) {
      LinkState = link_state;
      if (link_state == ARM_ETH_LINK_UP) {
        (void)EthMac->Control(ARM_ETH_MAC_CONFIGURE,
                             (uint32_t)(LinkInfo.speed                           ) |
                             (uint32_t)(LinkInfo.duplex << ARM_ETH_MAC_DUPLEX_Pos) |
                              PacketFilter);
        (void)EthMac->Control(ARM_ETH_MAC_CONTROL_TX, 1U);
        (void)EthMac->Control(ARM_ETH_MAC_CONTROL_RX, 1U);
      } else {
        (void)EthMac->Control(ARM_ETH_MAC_FLUSH,
                              ARM_ETH_MAC_FLUSH_TX |
                              ARM_ETH_MAC_FLUSH_RX);
        (void)EthMac->Control(ARM_ETH_MAC_CONTROL_TX, 0U);
        (void)EthMac->Control(ARM_ETH_MAC_CONTROL_RX, 0U);
      }
      state = (link_state == ARM_ETH_LINK_UP) ? 1U : 0U;
      do {
        status = USBD_CDC_NCM_Notify_NetworkConnection(USB_CDC_NUM, state);
      } while (status == -(int32_t)usbDriverBusy);
    }
    if ((event & 0x80000000U) == 0U) {
      if (LinkState == ARM_ETH_LINK_UP) {
        do {
          status = USBD_CDC_NCM_Notify_ConnectionSpeedChange(USB_CDC_NUM, LinkSpeed, LinkSpeed);
        } while (status == -(int32_t)usbDriverBusy);
      }
      state = (LinkState == ARM_ETH_LINK_UP) ? 1U : 0U;
      do {
        status = USBD_CDC_NCM_Notify_NetworkConnection(USB_CDC_NUM, state);
      } while (status == -(int32_t)usbDriverBusy);
    }
  }
}

// Thread handling Data IN (ETH -> USB)
#ifdef USB_CMSIS_RTOS2
__NO_RETURN static void DataIN_Thread (void *arg) {
#else
__NO_RETURN static void DataIN_Thread (void const *arg) {
#endif
  uint32_t size;
   int32_t status;

  (void)arg;

  for (;;) {
#ifdef USB_CMSIS_RTOS2
    (void)osThreadFlagsWait(3U, osFlagsWaitAll, osWaitForever);
#else
    (void)osSignalWait(3U, osWaitForever);
#endif
    do {
      status = USBD_CDC_NCM_NTB_IN_Initialize(USB_CDC_NUM);
      if (status != (int32_t)usbOK) { continue; }
      status = USBD_CDC_NCM_NTB_IN_CreateNDP(USB_CDC_NUM, MAX_IN_DATAGRAMS);
      if (status != (int32_t)usbOK) { continue; }
      for (;;) {
        if (FrameIN_Size != 0) {
          (void)USBD_CDC_NCM_NTB_IN_WriteDatagram(USB_CDC_NUM, FrameIN, FrameIN_Size);
          FrameIN_Size = 0;
        }
        size = EthMac->GetRxFrameSize();
        if (size == 0U) {
          break;
        }
        if ((size < 14U) || (size > NCM_State.max_datagram_size)) {
          (void)EthMac->ReadFrame(NULL, 0U);
          continue;
        }
        (void)EthMac->ReadFrame(FrameIN, size);
        if (size > 0) {
          #if ((USBD_CDC%Instance%_NCM_BM_ETHERNET_STATISTICS & 0x00000002) != 0)
            NCM_State.eth_statistics[1]++;
          #endif
          status = USBD_CDC_NCM_NTB_IN_WriteDatagram(USB_CDC_NUM, FrameIN, size);
          if (status != (int32_t)usbOK) {
            FrameIN_Size = size;
            break;
          }
        }
      }
      (void)USBD_CDC_NCM_NTB_IN_Send(USB_CDC_NUM);
    } while (FrameIN_Size);
  }
}

// Thread handling Data OUT (USB -> ETH)
#ifdef USB_CMSIS_RTOS2
__NO_RETURN static void DataOUT_Thread (void *arg) {
#else
__NO_RETURN static void DataOUT_Thread (void const *arg) {
#endif
  uint32_t size;
  int32_t  len, status;

  (void)(arg);

  for (;;) {
#ifdef USB_CMSIS_RTOS2
    (void)osThreadFlagsWait(1U, osFlagsWaitAll, osWaitForever);
#else
    (void)osSignalWait(1U, osWaitForever);
#endif
    for (;;) {
      status = USBD_CDC_NCM_NTB_OUT_ProcessNDP(USB_CDC_NUM);
      if (status != (int32_t)usbOK) { break; }
      for (;;) {
        size = USBD_CDC_NCM_NTB_OUT_GetDatagramSize(USB_CDC_NUM);
        if (size == 0U) { break; }
        if ((size < 14U) || (size > NCM_State.max_datagram_size)) {
          (void)USBD_CDC_NCM_NTB_OUT_ReadDatagram(USB_CDC_NUM, NULL, 0U);
          continue;
        }
        len = USBD_CDC_NCM_NTB_OUT_ReadDatagram(USB_CDC_NUM, FrameOUT, size);
        if (len > 0) {
          do {
            status = EthMac->SendFrame(FrameOUT, (uint32_t)(len), 0U);
          } while (status == ARM_DRIVER_ERROR_BUSY);
          #if ((USBD_CDC%Instance%_NCM_BM_ETHERNET_STATISTICS & 0x00000001) != 0)
            NCM_State.eth_statistics[0]++;
          #endif
        }
      }
    }
    (void)USBD_CDC_NCM_NTB_OUT_Release(USB_CDC_NUM);
  }
}
