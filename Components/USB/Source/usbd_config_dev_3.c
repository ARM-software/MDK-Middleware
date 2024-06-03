/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_config_dev_3.c
 * Purpose: USB Device 3 Data and Settings
 *----------------------------------------------------------------------------*/

#include "RTE_Components.h"

extern  ARM_DRIVER_USBD  USBDn_DRIVER(USBD3_PORT);

extern 
uint8_t     usbd3_alt_setting  [USBD3_IF_CNT];
uint8_t     usbd3_alt_setting  [USBD3_IF_CNT];
extern 
uint8_t     usbd3_ep0_buf      [((USBD3_EP0_BUF_SIZE+31)/32)*32];
uint8_t     usbd3_ep0_buf      [((USBD3_EP0_BUF_SIZE+31)/32)*32] __ALIGNED(32) USBD3_DEV_BUF_MEM_SECTION;
extern 
usbd_data_t usbd3_data;
usbd_data_t usbd3_data;
extern 
usbd_dev_t  usbd3_dev;
usbd_dev_t  usbd3_dev      = {
                                       &USBDn_DRIVER(USBD3_PORT),
                                        usbd3_alt_setting,
                                        usbd3_ep0_buf,
                                       &usbd3_data,
                                        USBD3_CFG_DESC_BMATTRIBUTES,
                                        USBD3_HS,
                                        USBD3_IF_CNT,
                                        USBD3_EP_CNT,
                                        USBD3_MAX_PACKET0,
                                        USBD3_STR_DESC_SER_MAX_LEN,
                                        USBD3_OS_DESC_VENDOR_CODE,
                                        0U,
                                        USBD3_EP0_BUF_SIZE
                                      };

// Core Thread definitions

#ifdef RTE_CMSIS_RTOS2
#ifdef RTE_CMSIS_RTOS2_RTX5
static osRtxThread_t  usbd3_core_thread_cb_mem                                              __attribute__((section(".bss.os.thread.cb")));
static uint64_t       usbd3_core_thread_stack_mem[(USBD3_CORE_THREAD_STACK_SIZE + 7U) / 8U] __attribute__((section(".bss.os.thread.stack")));
#endif
extern 
const  osThreadAttr_t usbd3_core_thread_attr;
const  osThreadAttr_t usbd3_core_thread_attr = {
  "USBD3_Core_Thread",
  0U,
#ifdef RTE_CMSIS_RTOS2_RTX5
  &usbd3_core_thread_cb_mem,
  sizeof(usbd3_core_thread_cb_mem),
  &usbd3_core_thread_stack_mem,
#else
  NULL,
  0U,
  NULL,
#endif
  ((USBD3_CORE_THREAD_STACK_SIZE + 7U) / 8U) * 8U,
  USBD3_CORE_THREAD_PRIORITY,
  0U,
  0U
};
#endif
