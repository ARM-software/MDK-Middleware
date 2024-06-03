/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_config_dev_$x.c
 * Purpose: USB Device $x Data and Settings
 *----------------------------------------------------------------------------*/

#include "RTE_Components.h"

extern  ARM_DRIVER_USBD  USBDn_DRIVER(USBD$x_PORT);

extern 
uint8_t     usbd$x_alt_setting  [USBD$x_IF_CNT];
uint8_t     usbd$x_alt_setting  [USBD$x_IF_CNT];
extern 
uint8_t     usbd$x_ep0_buf      [((USBD$x_EP0_BUF_SIZE+31)/32)*32];
uint8_t     usbd$x_ep0_buf      [((USBD$x_EP0_BUF_SIZE+31)/32)*32] __ALIGNED(32) USBD$x_DEV_BUF_MEM_SECTION;
extern 
usbd_data_t usbd$x_data;
usbd_data_t usbd$x_data;
extern 
usbd_dev_t  usbd$x_dev;
usbd_dev_t  usbd$x_dev      = {
                                       &USBDn_DRIVER(USBD$x_PORT),
                                        usbd$x_alt_setting,
                                        usbd$x_ep0_buf,
                                       &usbd$x_data,
                                        USBD$x_CFG_DESC_BMATTRIBUTES,
                                        USBD$x_HS,
                                        USBD$x_IF_CNT,
                                        USBD$x_EP_CNT,
                                        USBD$x_MAX_PACKET0,
                                        USBD$x_STR_DESC_SER_MAX_LEN,
                                        USBD$x_OS_DESC_VENDOR_CODE,
                                        0U,
                                        USBD$x_EP0_BUF_SIZE
                                      };

// Core Thread definitions

#ifdef RTE_CMSIS_RTOS2
#ifdef RTE_CMSIS_RTOS2_RTX5
static osRtxThread_t  usbd$x_core_thread_cb_mem                                              __attribute__((section(".bss.os.thread.cb")));
static uint64_t       usbd$x_core_thread_stack_mem[(USBD$x_CORE_THREAD_STACK_SIZE + 7U) / 8U] __attribute__((section(".bss.os.thread.stack")));
#endif
extern 
const  osThreadAttr_t usbd$x_core_thread_attr;
const  osThreadAttr_t usbd$x_core_thread_attr = {
  "USBD$x_Core_Thread",
  0U,
#ifdef RTE_CMSIS_RTOS2_RTX5
  &usbd$x_core_thread_cb_mem,
  sizeof(usbd$x_core_thread_cb_mem),
  &usbd$x_core_thread_stack_mem,
#else
  NULL,
  0U,
  NULL,
#endif
  ((USBD$x_CORE_THREAD_STACK_SIZE + 7U) / 8U) * 8U,
  USBD$x_CORE_THREAD_PRIORITY,
  0U,
  0U
};
#endif
