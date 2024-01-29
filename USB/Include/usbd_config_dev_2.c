/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_config_dev_2.c
 * Purpose: USB Device 2 Data and Settings
 *----------------------------------------------------------------------------*/

#ifndef __NO_USB_LIB_C

#include "usb_os.h"


extern  ARM_DRIVER_USBD  USBDn_DRIVER(USBD2_PORT);

extern 
uint8_t     usbd2_alt_setting  [USBD2_IF_CNT];
uint8_t     usbd2_alt_setting  [USBD2_IF_CNT];
extern 
uint8_t     usbd2_ep0_buf      [((USBD2_EP0_BUF_SIZE+31)/32)*32];
uint8_t     usbd2_ep0_buf      [((USBD2_EP0_BUF_SIZE+31)/32)*32] __ALIGNED(32);
extern 
usbd_data_t usbd2_data;
usbd_data_t usbd2_data;
extern 
usbd_dev_t  usbd2_dev;
usbd_dev_t  usbd2_dev      = {
                                       &USBDn_DRIVER(USBD2_PORT),
                                        usbd2_alt_setting,
                                        usbd2_ep0_buf,
                                       &usbd2_data,
                                        USBD2_CFG_DESC_BMATTRIBUTES,
                                        USBD2_HS,
                                        USBD2_IF_CNT,
                                        USBD2_EP_CNT,
                                        USBD2_MAX_PACKET0,
                                        USBD2_STR_DESC_SER_MAX_LEN,
                                        USBD2_OS_DESC_VENDOR_CODE,
                                        0U,
                                        USBD2_EP0_BUF_SIZE
                                      };

// Core Thread definitions

#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t  usbd2_core_thread_cb_mem                                              __SECTION(.bss.os.thread.cb);
static uint64_t       usbd2_core_thread_stack_mem[(USBD2_CORE_THREAD_STACK_SIZE + 7U) / 8U] __SECTION(.bss.os.thread.stack);
#endif
extern 
const  osThreadAttr_t usbd2_core_thread_attr;
const  osThreadAttr_t usbd2_core_thread_attr = {
  "USBD2_Core_Thread",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
  &usbd2_core_thread_cb_mem,
  sizeof(usbd2_core_thread_cb_mem),
  &usbd2_core_thread_stack_mem,
#else
  NULL,
  0U,
  NULL,
#endif
  ((USBD2_CORE_THREAD_STACK_SIZE + 7U) / 8U) * 8U,
  USBD2_CORE_THREAD_PRIORITY,
  0U,
  0U
};
#else
extern const osThreadDef_t os_thread_def_USBD2_Core_Thread;
osThreadDef(USBD2_Core_Thread, USBD2_CORE_THREAD_PRIORITY, 1U, USBD2_CORE_THREAD_STACK_SIZE);
#endif

#endif
