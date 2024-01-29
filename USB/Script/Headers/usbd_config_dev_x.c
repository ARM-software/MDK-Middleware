/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_config_dev_$x.c
 * Purpose: USB Device $x Data and Settings
 *----------------------------------------------------------------------------*/

#ifndef __NO_USB_LIB_C

#include "usb_os.h"


extern  ARM_DRIVER_USBD  USBDn_DRIVER(USBD$x_PORT);

extern 
uint8_t     usbd$x_alt_setting  [USBD$x_IF_CNT];
uint8_t     usbd$x_alt_setting  [USBD$x_IF_CNT];
extern 
uint8_t     usbd$x_ep0_buf      [((USBD$x_EP0_BUF_SIZE+31)/32)*32];
uint8_t     usbd$x_ep0_buf      [((USBD$x_EP0_BUF_SIZE+31)/32)*32] __ALIGNED(32);
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

#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t  usbd$x_core_thread_cb_mem                                              __SECTION(.bss.os.thread.cb);
static uint64_t       usbd$x_core_thread_stack_mem[(USBD$x_CORE_THREAD_STACK_SIZE + 7U) / 8U] __SECTION(.bss.os.thread.stack);
#endif
extern 
const  osThreadAttr_t usbd$x_core_thread_attr;
const  osThreadAttr_t usbd$x_core_thread_attr = {
  "USBD$x_Core_Thread",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
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
#else
extern const osThreadDef_t os_thread_def_USBD$x_Core_Thread;
osThreadDef(USBD$x_Core_Thread, USBD$x_CORE_THREAD_PRIORITY, 1U, USBD$x_CORE_THREAD_STACK_SIZE);
#endif

#endif
