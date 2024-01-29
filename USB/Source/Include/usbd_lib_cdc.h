/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_cdc.h
 * Purpose: USB Device - Communication Device Class (CDC) module header file
 *----------------------------------------------------------------------------*/

#ifndef __USBD_LIB_CDC_H__
#define __USBD_LIB_CDC_H__

#include <stdint.h>
#include <stdbool.h>

#include "usb_cdc.h"


/// \brief CDC class (ACM) send encapsulated command (calls USBD_CDC_ACM_SendEncapsulatedCommand function)
/// \param[in]     instance      instance of CDC class.
/// \param[in]     len           number of data bytes that request contains
/// \return        true          success
/// \return        false         fail
extern bool USBD_CDC_ACM_CoreSendEncapsulatedCommand (uint8_t instance, uint16_t len);

/// \brief CDC class (ACM) get encapsulated response (calls USBD_CDC_ACM_GetEncapsulatedResponse function)
/// \param[in]     instance      instance of CDC class.
/// \param[in]     max_len       maximum number of data bytes that USB Host expects to receive
/// \param[out]    buf           pointer to buffer containing get encapsulated response to be returned to USB Host
/// \param[out]    len           pointer to number of data bytes to be returned to USB Host
/// \return        true          success
/// \return        false         fail
extern bool USBD_CDC_ACM_CoreGetEncapsulatedResponse (uint8_t instance, uint16_t max_len, uint8_t **buf, uint16_t *len);

/// \brief CDC class (ACM) set line coding (calls USBD_CDC_ACM_SetLineCoding function)
/// \param[in]     instance      instance of CDC class.
/// \return        true          success
/// \return        false         fail
extern bool USBD_CDC_ACM_CoreSetLineCoding (uint8_t instance);

/// \brief CDC class (ACM) get line coding (calls USBD_CDC_ACM_GetLineCoding function)
/// \param[in]     instance      instance of CDC class.
/// \return        true          success
/// \return        false         fail
extern bool USBD_CDC_ACM_CoreGetLineCoding (uint8_t instance);

/// \brief CDC class (ACM) set control line state (calls USBD_CDC_ACM_SetControlLineState function)
/// \param[in]     instance      instance of CDC class.
/// \param[in]     state         control line settings bitmap
///                                - bit 0: DTR state
///                                - bit 1: RTS state
/// \return        true          success
/// \return        false         fail
extern bool USBD_CDC_ACM_CoreSetControlLineState (uint8_t instance, uint16_t state);

extern bool USBD_CDC_ACM_CoreSetCommFeature                          (uint8_t instance, uint16_t feat);
extern bool USBD_CDC_ACM_CoreGetCommFeature                          (uint8_t instance, uint16_t feat);
extern bool USBD_CDC_ACM_CoreClearCommFeature                        (uint8_t instance, uint16_t feat);
extern bool USBD_CDC_ACM_CoreSendBreak                               (uint8_t instance, uint16_t dur);

extern bool USBD_CDC_NCM_CoreSetEthernetMulticastFilters             (uint8_t instance, const uint8_t *addr_list, uint16_t num_of_filters);
extern bool USBD_CDC_NCM_CoreSetEthernetPowerManagementPatternFilter (uint8_t instance, uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size);
extern bool USBD_CDC_NCM_CoreGetEthernetPowerManagementPatternFilter (uint8_t instance, uint16_t filter_number, uint16_t *pattern_active);
extern bool USBD_CDC_NCM_CoreSetEthernetPacketFilter                 (uint8_t instance, uint16_t packet_filter_bitmap);
extern bool USBD_CDC_NCM_CoreGetEthernetStatistic                    (uint8_t instance, uint16_t feature_selector, uint32_t *data);
extern bool USBD_CDC_NCM_CoreGetNtbParameters                        (uint8_t instance, CDC_NCM_NTB_PARAM *ntb_params);
extern bool USBD_CDC_NCM_CoreGetNetAddress                           (uint8_t instance, uint8_t *net_addr);
extern bool USBD_CDC_NCM_CoreSetNetAddress                           (uint8_t instance, const uint8_t *net_addr);
extern bool USBD_CDC_NCM_CoreGetNtbFormat                            (uint8_t instance, uint16_t *ntb_format);
extern bool USBD_CDC_NCM_CoreSetNtbFormat                            (uint8_t instance, uint16_t ntb_format);
extern bool USBD_CDC_NCM_CoreGetNtbInputSize                         (uint8_t instance, uint32_t *ntb_input_size);
extern bool USBD_CDC_NCM_CoreSetNtbInputSize                         (uint8_t instance, uint32_t ntb_input_size);
extern bool USBD_CDC_NCM_CoreGetMaxDatagramSize                      (uint8_t instance, uint16_t *max_datagram_size);
extern bool USBD_CDC_NCM_CoreSetMaxDatagramSize                      (uint8_t instance, uint16_t max_datagram_size);
extern bool USBD_CDC_NCM_CoreGetCrcMode                              (uint8_t instance, uint16_t *crc_mode);
extern bool USBD_CDC_NCM_CoreSetCrcMode                              (uint8_t instance, uint16_t crc_mode);

#endif  // __USBD_LIB_CDC_H__
