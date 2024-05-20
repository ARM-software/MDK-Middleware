/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2021-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_config_def.h
 * Purpose: USB Device System Configuration header file (for USB Library)
 *----------------------------------------------------------------------------*/

#ifndef __USBD_LIB_CONFIG_DEF_H__
#define __USBD_LIB_CONFIG_DEF_H__

#include <stdint.h>
#include <stdbool.h>

#include "rl_usb.h"
#include "usbd_lib.h"
#include "Driver_USBD.h"

/*------------------------------------------------------------------------------
 *           USB Device Configuration
 *----------------------------------------------------------------------------*/

extern const uint8_t                              usbd_dev_num;
extern const uint8_t                              usbd_custom_class_num;
extern const uint8_t                              usbd_adc_num;
extern const uint8_t                              usbd_cdc_num;
extern const uint8_t                              usbd_hid_num;
extern const uint8_t                              usbd_msc_num;
extern const uint8_t                              usbd_ep_max_num;
extern       ARM_USBD_SignalDeviceEvent_t   const usbd_cb_device_event_ptr                         [];
extern       ARM_USBD_SignalEndpointEvent_t const usbd_cb_endpoint_event_ptr                       [];
extern       ARM_DRIVER_VERSION                   usbd_drv_version                                 [];
extern       ARM_USBD_CAPABILITIES                usbd_capabilities                                [];
extern       usbd_desc_t                  *       usbd_desc_ptr                                    [];
extern       usbd_dev_t                   * const usbd_dev_ptr                                     [];
extern       usbd_cc_desc_t               * const usbd_cc_desc_ptr                                 [];
extern       usbd_custom_class_t          * const usbd_custom_class_ptr                            [];
extern       usbd_adc_t                   * const usbd_adc_ptr                                     [];
extern       usbd_cdc_t                   * const usbd_cdc_ptr                                     [];
extern       usbd_hid_t                   * const usbd_hid_ptr                                     [];
extern       usbd_hid_desc_t              * const usbd_hid_desc_ptr                                [];
extern       usbd_msc_t                   * const usbd_msc_ptr                                     [];
extern       void                         *       usbd_core_thread_id                              [];
extern       void                         *       usbd_driver_semaphore_id                         [];
extern       void                         *       usbd_driver_ep_semaphore_id                      [];
extern       void                        **       usbd_ep_thread_id_ptr                            [];
extern       void                         *       usbd_custom_class_ep_thread_id                   [][16];
extern       void                         *       usbd_adc_thread_id                               [];
extern       void                         *       usbd_cdc_int_thread_id                           [];
extern       void                         *       usbd_cdc_bulk_thread_id                          [];
extern       void                         *       usbd_hid_thread_id                               [];
extern       void                         *       usbd_hid_timer_id                                [];
extern       void                         *       usbd_hid_semaphore_id                            [];
extern       void                         *       usbd_msc_thread_id                               [];

extern       void                        (* const fpUSBD_Device_Initialize                         []) (void);
extern       void                        (* const fpUSBD_Device_Uninitialize                       []) (void);
extern       void                        (* const fpUSBD_Device_Reset                              []) (void);
extern       void                        (* const fpUSBD_Device_VbusChanged                        []) (bool level);
extern       void                        (* const fpUSBD_Device_HighSpeedActivated                 []) (void);
extern       void                        (* const fpUSBD_Device_Suspended                          []) (void);
extern       void                        (* const fpUSBD_Device_Resumed                            []) (void);
extern       void                        (* const fpUSBD_Device_ConfigurationChanged               []) (uint8_t val);
extern       void                        (* const fpUSBD_Device_EnableRemoteWakeup                 []) (void);
extern       void                        (* const fpUSBD_Device_DisableRemoteWakeup                []) (void);
extern       usbdRequestStatus           (* const fpUSBD_Device_Endpoint0_SetupPacketReceived      []) (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len);
extern       void                        (* const fpUSBD_Device_Endpoint0_SetupPacketProcessed     []) (const USB_SETUP_PACKET *setup_packet);
extern       usbdRequestStatus           (* const fpUSBD_Device_Endpoint0_OutDataReceived          []) (uint32_t len);
extern       usbdRequestStatus           (* const fpUSBD_Device_Endpoint0_InDataSent               []) (uint32_t len);
extern       void                        (* const fpUSBD_CustomClass_Initialize                    []) (void);
extern       void                        (* const fpUSBD_CustomClass_Uninitialize                  []) (void);
extern       void                        (* const fpUSBD_CustomClass_Reset                         []) (void);
extern       void                        (* const fpUSBD_CustomClass_EndpointStart                 []) (uint8_t ep_addr);
extern       void                        (* const fpUSBD_CustomClass_EndpointStop                  []) (uint8_t ep_addr);
extern       usbdRequestStatus           (* const fpUSBD_CustomClass_Endpoint0_SetupPacketReceived []) (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len);
extern       void                        (* const fpUSBD_CustomClass_Endpoint0_SetupPacketProcessed[]) (const USB_SETUP_PACKET *setup_packet);
extern       usbdRequestStatus           (* const fpUSBD_CustomClass_Endpoint0_OutDataReceived     []) (uint32_t len);
extern       usbdRequestStatus           (* const fpUSBD_CustomClass_Endpoint0_InDataSent          []) (uint32_t len);
extern       void                        (* const fpUSBD_Endpoint_Event                            []) (uint32_t event);
extern       void                        (* const fpUSBD_ADC_Initialize                            []) (void);
extern       void                        (* const fpUSBD_ADC_Uninitialize                          []) (void);
extern       void                        (* const fpUSBD_ADC_PlayStart                             []) (void);
extern       void                        (* const fpUSBD_ADC_PlayStop                              []) (void);
extern       void                        (* const fpUSBD_ADC_SetSpeakerMute                        []) (uint8_t ch, bool     on);
extern       void                        (* const fpUSBD_ADC_SetSpeakerVolume                      []) (uint8_t ch, uint16_t vol);
extern       void                        (* const fpUSBD_ADC_RecordStart                           []) (void);
extern       void                        (* const fpUSBD_ADC_RecordStop                            []) (void);
extern       void                        (* const fpUSBD_ADC_SetMicrophoneMute                     []) (uint8_t ch, bool     on);
extern       void                        (* const fpUSBD_ADC_SetMicrophoneVolume                   []) (uint8_t ch, uint16_t vol);
extern       void                        (* const fpUSBD_ADC_ReceivedSamples                       []) (void);
extern       void                        (* const fpUSBD_CDC_ACM_Initialize                        []) (void);
extern       void                        (* const fpUSBD_CDC_ACM_Uninitialize                      []) (void);
extern       void                        (* const fpUSBD_CDC_ACM_Reset                             []) (void);
extern       bool                        (* const fpUSBD_CDC_ACM_SendEncapsulatedCommand           []) (             const uint8_t *buf, uint16_t  len);
extern       bool                        (* const fpUSBD_CDC_ACM_GetEncapsulatedResponse           []) (uint16_t max_len, uint8_t **buf, uint16_t *len);
extern       bool                        (* const fpUSBD_CDC_ACM_SetLineCoding                     []) (const CDC_LINE_CODING *line_coding);
extern       bool                        (* const fpUSBD_CDC_ACM_GetLineCoding                     []) (      CDC_LINE_CODING *line_coding);
extern       bool                        (* const fpUSBD_CDC_ACM_SetControlLineState               []) (uint16_t ctrl_bmp);
extern       void                        (* const fpUSBD_CDC_ACM_DataReceived                      []) (uint32_t len);
extern       void                        (* const fpUSBD_CDC_ACM_DataSent                          []) (void);
extern       void                        (* const fpUSBD_CDC_NCM_Initialize                        []) (void);
extern       void                        (* const fpUSBD_CDC_NCM_Uninitialize                      []) (void);
extern       void                        (* const fpUSBD_CDC_NCM_Reset                             []) (void);
extern       void                        (* const fpUSBD_CDC_NCM_Start                             []) (void);
extern       void                        (* const fpUSBD_CDC_NCM_Stop                              []) (void);
extern       bool                        (* const fpUSBD_CDC_NCM_SetEthernetMulticastFilters            []) (const uint8_t *addr_list, uint16_t num_of_filters);
extern       bool                        (* const fpUSBD_CDC_NCM_SetEthernetPowerManagementPatternFilter[]) (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size);
extern       bool                        (* const fpUSBD_CDC_NCM_GetEthernetPowerManagementPatternFilter[]) (uint16_t filter_number, uint16_t *pattern_active);
extern       bool                        (* const fpUSBD_CDC_NCM_SetEthernetPacketFilter           []) (uint16_t packet_filter_bitmap);
extern       bool                        (* const fpUSBD_CDC_NCM_GetEthernetStatistic              []) (uint16_t feature_selector, uint32_t *data);
extern       bool                        (* const fpUSBD_CDC_NCM_GetNtbParameters                  []) (CDC_NCM_NTB_PARAM *ntb_params);
extern       bool                        (* const fpUSBD_CDC_NCM_GetNetAddress                     []) (      uint8_t *addr);
extern       bool                        (* const fpUSBD_CDC_NCM_SetNetAddress                     []) (const uint8_t *addr);
extern       bool                        (* const fpUSBD_CDC_NCM_GetNtbFormat                      []) (uint16_t *ntb_format);
extern       bool                        (* const fpUSBD_CDC_NCM_SetNtbFormat                      []) (uint16_t  ntb_format);
extern       bool                        (* const fpUSBD_CDC_NCM_GetNtbInputSize                   []) (uint32_t *ntb_input_size);
extern       bool                        (* const fpUSBD_CDC_NCM_SetNtbInputSize                   []) (uint32_t  ntb_input_size);
extern       bool                        (* const fpUSBD_CDC_NCM_GetMaxDatagramSize                []) (uint16_t *max_datagram_size);
extern       bool                        (* const fpUSBD_CDC_NCM_SetMaxDatagramSize                []) (uint16_t  max_datagram_size);
extern       bool                        (* const fpUSBD_CDC_NCM_GetCrcMode                        []) (uint16_t *mode);
extern       bool                        (* const fpUSBD_CDC_NCM_SetCrcMode                        []) (uint16_t  mode);
extern       void                        (* const fpUSBD_CDC_NCM_NTB_IN_Sent                       []) (void);
extern       void                        (* const fpUSBD_CDC_NCM_NTB_OUT_Received                  []) (void);
extern       void                        (* const fpUSBD_HID_Initialize                            []) (void);
extern       void                        (* const fpUSBD_HID_Uninitialize                          []) (void);
extern       int32_t                     (* const fpUSBD_HID_GetReport                             []) (uint8_t rtype, uint8_t req, uint8_t rid,       uint8_t *buf);
extern       bool                        (* const fpUSBD_HID_SetReport                             []) (uint8_t rtype, uint8_t req, uint8_t rid, const uint8_t *buf, int32_t len);
extern       int8_t                      (* const fpUSBD_HID_GetProtocol                           []) (void);
extern       bool                        (* const fpUSBD_HID_SetProtocol                           []) (uint8_t protocol);
extern       void                        (* const fpUSBD_MSC_Initialize                            []) (void);
extern       void                        (* const fpUSBD_MSC_Uninitialize                          []) (void);
extern       bool                        (* const fpUSBD_MSC_GetCacheInfo                          []) (uint32_t *buffer, uint32_t *size);
extern       uint8_t                     (* const fpUSBD_MSC_GetMaxLUN                             []) (void);
extern       bool                        (* const fpUSBD_MSC_LUN_GetMediaCapacity                  []) (uint8_t lun, uint32_t *block_count, uint32_t *block_size);
extern       bool                        (* const fpUSBD_MSC_LUN_Read                              []) (uint8_t lun, uint32_t lba, uint32_t cnt,       uint8_t *buf);
extern       bool                        (* const fpUSBD_MSC_LUN_Write                             []) (uint8_t lun, uint32_t lba, uint32_t cnt, const uint8_t *buf);
extern       bool                        (* const fpUSBD_MSC_LUN_StartStop                         []) (uint8_t lun, bool start);
extern       uint32_t                    (* const fpUSBD_MSC_LUN_CheckMedia                        []) (uint8_t lun);

#endif  // __USBD_LIB_CONFIG_DEF_H__
