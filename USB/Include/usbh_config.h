/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_config.h
 * Purpose: USB Host Configuration header file
 *----------------------------------------------------------------------------*/

#ifndef USBH_CONFIG_H_
#define USBH_CONFIG_H_

#include <stdint.h>
#include <stdbool.h>

#include "rl_usbh_lib.h"
#include "Driver_USBH.h"

/*------------------------------------------------------------------------------
 *           USB Host Configuration
 *----------------------------------------------------------------------------*/

extern const uint8_t                              usbh_hc_num;
extern const uint8_t                              usbh_hc_msk;
extern const uint8_t                              usbh_dcd_num;
extern const uint8_t                              usbh_dev_num;
extern const uint8_t                              usbh_msc_num;
extern const uint8_t                              usbh_hid_num;
extern const uint8_t                              usbh_cdc_num;
extern const uint8_t                              usbh_cls_num;
extern       ARM_USBH_SignalPortEvent_t     const usbh_cb_port_event_ptr                           [];
extern       ARM_USBH_SignalEndpointEvent_t const usbh_cb_pipe_event_ptr                           [];
extern       ARM_DRIVER_VERSION                   usbh_drv_version                                 [];
extern       ARM_USBH_CAPABILITIES                usbh_capabilities                                [];
extern       ARM_DRIVER_USBH             ** const usbh_hcd_ptr                                     [];
extern       USBH_HC_t                   ** const usbh_hc_ptr                                      [];
extern const uint8_t                      * const usbh_power_ptr                                   [];
extern const uint8_t                      * const usbh_mem_pool_reloc_ptr                          [];
extern       uint32_t                     * const usbh_mem_pool_ptr                                [];
extern const uint32_t                     * const usbh_mem_pool_size_ptr                           [];
extern       USBH_PIPE                    * const usbh_pipe_ptr                                    [];
extern const uint32_t                     * const usbh_pipe_num_ptr                                [];
extern       USBH_HCI                             usbh_hci                                         [];
extern       USBH_DEV                             usbh_dev                                         [];
extern       USBH_MSC                             usbh_msc                                         [];
extern       USBH_HID                             usbh_hid                                         [];
extern       USBH_CDC                             usbh_cdc                                         [];
extern       void                         *       usbh_core_thread_id                              [];
extern       void                         *       usbh_driver_semaphore_id                         [];
extern       void                         *       usbh_def_pipe_mutex_id                           [];
extern const uint16_t                             usbh_debounce_in_ms                              [];
extern       void                         *       usbh_debounce_timer_id                           [];

#endif  // USBH_CONFIG_H_
