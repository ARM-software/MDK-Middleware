/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_config_hc_2.c
 * Purpose: USB Host (USBH) - Host Controller 2 Data Definitions
 * Note:    this file should only be included when USBH_Config_2.c is being 
 *          compiled as it depends on definitions from USBH_Config_2.c
 *----------------------------------------------------------------------------*/

#include "usb_os.h"

// Global variable declarations

extern   ARM_DRIVER_USBH     *usbh2_hcd_ptr;
extern   ARM_DRIVER_USBH_HCI *usbh2_hci_hcd_ptr;
extern   const uint32_t       usbh2_pipe_num;
extern   const uint32_t       usbh2_transfer_num;

extern   ARM_DRIVER_USBH      USBHn_DRIVER(USBH2_HC_NUM);

extern   USBH_PIPE            usbh2_pipe             [USBH2_HC_PIPE_NUM];
extern   uint32_t             usbh2_mem_pool        [(USBH2_HC_MEM_POOL_SIZE + 8 * USBH2_HC_PIPE_NUM + 32 + 4 + 3) / 4];

extern   const uint8_t        usbh2_power;
extern   const uint8_t        usbh2_mem_pool_reloc;
extern   const uint32_t       usbh2_mem_pool_size;

extern   USBH_HC_t            usbh2_hc;
extern   USBH_HC_t           *usbh2_hc_ptr;

// Global variable definitions

         ARM_DRIVER_USBH     *usbh2_hcd_ptr  =       &USBHn_DRIVER(USBH2_HC_NUM);
         USBH_PIPE            usbh2_pipe             [USBH2_HC_PIPE_NUM];
const    uint32_t             usbh2_pipe_num =        USBH2_HC_PIPE_NUM;
const    uint32_t             usbh2_transfer_num =    USBH2_HC_PIPE_NUM;
         uint32_t             usbh2_mem_pool        [(USBH2_HC_MEM_POOL_SIZE + 8 * USBH2_HC_PIPE_NUM + 32 + 4 + 3) / 4] __USBH2_AT_MEM(USBH2_HC_MEM_POOL_ADDR);
const    uint8_t              usbh2_power =          (uint8_t)(USBH2_HC_POWER / 2);
const    uint8_t              usbh2_mem_pool_reloc =  USBH2_HC_MEM_POOL_RELOC;
const    uint32_t             usbh2_mem_pool_size = ((USBH2_HC_MEM_POOL_SIZE + 8 * USBH2_HC_PIPE_NUM + 32 + 4 + 3) / 4) * 4;

         USBH_HC_t            usbh2_hc       =      { USBH2_HC_PIPE_NUM };
         USBH_HC_t           *usbh2_hc_ptr   =       &usbh2_hc;

// Core Thread definitions

#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t  usbh2_core_thread_cb_mem                                              __SECTION(.bss.os.thread.cb);
static uint64_t       usbh2_core_thread_stack_mem[(USBH2_CORE_THREAD_STACK_SIZE + 7U) / 8U] __SECTION(.bss.os.thread.stack);
#endif
extern 
const  osThreadAttr_t usbh2_core_thread_attr;
const  osThreadAttr_t usbh2_core_thread_attr = {
  "USBH2_Core_Thread",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
  &usbh2_core_thread_cb_mem,
  sizeof(usbh2_core_thread_cb_mem),
  &usbh2_core_thread_stack_mem,
#else
  NULL,
  0U,
  NULL,
#endif
  ((USBH2_CORE_THREAD_STACK_SIZE + 7U) / 8U) * 8U,
  USBH2_CORE_THREAD_PRIORITY,
  0U,
  0U
};
#endif
