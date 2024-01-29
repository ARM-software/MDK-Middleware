/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_config_dev_0.c
 * Purpose: USB Device 0 Data and Settings
 *----------------------------------------------------------------------------*/

#ifndef __NO_USB_LIB_C

#include "usb_os.h"


extern  ARM_DRIVER_USBD  USBDn_DRIVER(USBD0_PORT);

extern 
uint8_t     usbd0_alt_setting  [USBD0_IF_CNT];
uint8_t     usbd0_alt_setting  [USBD0_IF_CNT];
extern 
uint8_t     usbd0_ep0_buf      [((USBD0_EP0_BUF_SIZE+31)/32)*32];
uint8_t     usbd0_ep0_buf      [((USBD0_EP0_BUF_SIZE+31)/32)*32] __ALIGNED(32);
extern 
usbd_data_t usbd0_data;
usbd_data_t usbd0_data;
extern 
usbd_dev_t  usbd0_dev;
usbd_dev_t  usbd0_dev      = {
                                       &USBDn_DRIVER(USBD0_PORT),
                                        usbd0_alt_setting,
                                        usbd0_ep0_buf,
                                       &usbd0_data,
                                        USBD0_CFG_DESC_BMATTRIBUTES,
                                        USBD0_HS,
                                        USBD0_IF_CNT,
                                        USBD0_EP_CNT,
                                        USBD0_MAX_PACKET0,
                                        USBD0_STR_DESC_SER_MAX_LEN,
                                        USBD0_OS_DESC_VENDOR_CODE,
                                        0U,
                                        USBD0_EP0_BUF_SIZE
                                      };

// Core Thread definitions

#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t  usbd0_core_thread_cb_mem                                              __SECTION(.bss.os.thread.cb);
static uint64_t       usbd0_core_thread_stack_mem[(USBD0_CORE_THREAD_STACK_SIZE + 7U) / 8U] __SECTION(.bss.os.thread.stack);
#endif
extern 
const  osThreadAttr_t usbd0_core_thread_attr;
const  osThreadAttr_t usbd0_core_thread_attr = {
  "USBD0_Core_Thread",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
  &usbd0_core_thread_cb_mem,
  sizeof(usbd0_core_thread_cb_mem),
  &usbd0_core_thread_stack_mem,
#else
  NULL,
  0U,
  NULL,
#endif
  ((USBD0_CORE_THREAD_STACK_SIZE + 7U) / 8U) * 8U,
  USBD0_CORE_THREAD_PRIORITY,
  0U,
  0U
};
#else
extern const osThreadDef_t os_thread_def_USBD0_Core_Thread;
osThreadDef(USBD0_Core_Thread, USBD0_CORE_THREAD_PRIORITY, 1U, USBD0_CORE_THREAD_STACK_SIZE);
#endif

#endif
