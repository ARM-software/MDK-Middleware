/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_config.h
 * Purpose: USB Host (0..3) Configuration header
 *----------------------------------------------------------------------------*/

#ifndef USBH_CONFIG_H_
#define USBH_CONFIG_H_

#include <stdint.h>

#include "RTE_Components.h"

#ifdef    RTE_USB_Host_0
#include "USBH_Config_0.h"
#endif

#ifdef    RTE_USB_Host_1
#include "USBH_Config_1.h"
#endif

#ifdef    RTE_USB_Host_2
#include "USBH_Config_2.h"
#endif

#ifdef    RTE_USB_Host_3
#include "USBH_Config_3.h"
#endif

#if      (defined(RTE_USB_Host_MSC) && !defined(USBH_MSC_NUM))
#include "USBH_Config_MSC.h"
#endif

#if      (defined(RTE_USB_Host_HID) && !defined(USBH_HID_NUM))
#include "USBH_Config_HID.h"
#endif

#if      (defined(RTE_USB_Host_CDC) && !defined(USBH_CDC_NUM))
#include "USBH_Config_CDC.h"
#endif

#if      (defined(RTE_USB_Host_CustomClass) && !defined(USBH_CUSTOM_CLASS_NUM))
#include "USBH_Config_CustomClass.h"
#endif

#include "usbh_lib.h"
#include "Driver_USBH.h"

/*------------------------------------------------------------------------------
 *      USB Host Configuration helper macros
 *----------------------------------------------------------------------------*/

#ifdef  RTE_USB_Host_0
#define USBH0_HC                1
#else
#define USBH0_HC                0
#endif

#ifdef  RTE_USB_Host_1
#define USBH1_HC                1
#else
#define USBH1_HC                0
#endif

#ifdef  RTE_USB_Host_2
#define USBH2_HC                1
#else
#define USBH2_HC                0
#endif

#ifdef  RTE_USB_Host_3
#define USBH3_HC                1
#else
#define USBH3_HC                0
#endif

#ifndef USBH_MSC_NUM
#define USBH_MSC_NUM            0
#endif

#ifndef USBH_HID_NUM
#define USBH_HID_NUM            0
#endif

#ifndef USBH_CDC_NUM
#define USBH_CDC_NUM            0
#endif

#ifndef USBH_CUSTOM_CLASS_NUM
#define USBH_CUSTOM_CLASS_NUM   0
#endif

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

#ifdef USBH0_HC_NUM
extern USBH_PIPE                                  usbh0_pipe[];
#endif
#ifdef USBH1_HC_NUM
extern USBH_PIPE                                  usbh1_pipe[];
#endif
#ifdef USBH2_HC_NUM
extern USBH_PIPE                                  usbh2_pipe[];
#endif
#ifdef USBH3_HC_NUM
extern USBH_PIPE                                  usbh3_pipe[];
#endif

extern const uint8_t                      * const usbh_power_ptr          [];
extern const uint8_t                      * const usbh_mem_pool_reloc_ptr [];
extern       uint32_t                     * const usbh_mem_pool_ptr       [];
extern const uint32_t                     * const usbh_mem_pool_size_ptr  [];
extern       USBH_PIPE                    * const usbh_pipe_ptr           [];
extern const uint32_t                     * const usbh_pipe_num_ptr       [];
extern       ARM_DRIVER_USBH             ** const usbh_hcd_ptr            [];

extern const uint16_t                             usbh_debounce_in_ms     [];

extern       void                         *       usbh_core_thread_id     [];
extern       void                         *       usbh_driver_semaphore_id[];
extern       void                         *       usbh_def_pipe_mutex_id  [];
extern       void                         *       usbh_debounce_timer_id  [];

extern       ARM_USBH_SignalPortEvent_t     const usbh_cb_port_event_ptr  [];
extern       ARM_USBH_SignalEndpointEvent_t const usbh_cb_pipe_event_ptr  [];

extern       ARM_DRIVER_VERSION                   usbh_drv_version        [];
extern       ARM_USBH_CAPABILITIES                usbh_capabilities       [];

extern       USBH_HC                              usbh_hc                 [];
extern       USBH_DEV                             usbh_dev                [];
extern       USBH_MSC                             usbh_msc                [];
extern       USBH_HID                             usbh_hid                [];
extern       USBH_CDC                             usbh_cdc                [];

#endif // USBH_CONFIG_H_
