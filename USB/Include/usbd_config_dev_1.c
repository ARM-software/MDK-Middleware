/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_config_dev_1.c
 * Purpose: USB Device 1 Data and Settings
 *----------------------------------------------------------------------------*/

#ifndef __NO_USB_LIB_C

#include "usb_os.h"


extern  ARM_DRIVER_USBD  USBDn_DRIVER(USBD1_PORT);

extern 
uint8_t     usbd1_alt_setting  [USBD1_IF_CNT];
uint8_t     usbd1_alt_setting  [USBD1_IF_CNT];
extern 
uint8_t     usbd1_ep0_buf      [((USBD1_EP0_BUF_SIZE+31)/32)*32];
uint8_t     usbd1_ep0_buf      [((USBD1_EP0_BUF_SIZE+31)/32)*32] __ALIGNED(32);
extern 
usbd_data_t usbd1_data;
usbd_data_t usbd1_data;
extern 
usbd_dev_t  usbd1_dev;
usbd_dev_t  usbd1_dev      = {
                                       &USBDn_DRIVER(USBD1_PORT),
                                        usbd1_alt_setting,
                                        usbd1_ep0_buf,
                                       &usbd1_data,
                                        USBD1_CFG_DESC_BMATTRIBUTES,
                                        USBD1_HS,
                                        USBD1_IF_CNT,
                                        USBD1_EP_CNT,
                                        USBD1_MAX_PACKET0,
                                        USBD1_STR_DESC_SER_MAX_LEN,
                                        USBD1_OS_DESC_VENDOR_CODE,
                                        0U,
                                        USBD1_EP0_BUF_SIZE
                                      };

// Core Thread definitions

#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t  usbd1_core_thread_cb_mem                                              __SECTION(.bss.os.thread.cb);
static uint64_t       usbd1_core_thread_stack_mem[(USBD1_CORE_THREAD_STACK_SIZE + 7U) / 8U] __SECTION(.bss.os.thread.stack);
#endif
extern 
const  osThreadAttr_t usbd1_core_thread_attr;
const  osThreadAttr_t usbd1_core_thread_attr = {
  "USBD1_Core_Thread",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
  &usbd1_core_thread_cb_mem,
  sizeof(usbd1_core_thread_cb_mem),
  &usbd1_core_thread_stack_mem,
#else
  NULL,
  0U,
  NULL,
#endif
  ((USBD1_CORE_THREAD_STACK_SIZE + 7U) / 8U) * 8U,
  USBD1_CORE_THREAD_PRIORITY,
  0U,
  0U
};
#else
extern const osThreadDef_t os_thread_def_USBD1_Core_Thread;
osThreadDef(USBD1_Core_Thread, USBD1_CORE_THREAD_PRIORITY, 1U, USBD1_CORE_THREAD_STACK_SIZE);
#endif

#endif
