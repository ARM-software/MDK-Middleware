/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_config_hc_1.c
 * Purpose: USB Host (USBH) - Host Controller 1 Data Definitions
 * Note:    this file should only be included when USBH_Config_1.c is being 
 *          compiled as it depends on definitions from USBH_Config_1.c
 *----------------------------------------------------------------------------*/

#include "usb_os.h"

// Global variable declarations

extern   ARM_DRIVER_USBH     *usbh1_hcd_ptr;
extern   ARM_DRIVER_USBH_HCI *usbh1_hci_hcd_ptr;
extern   const uint32_t       usbh1_pipe_num;
extern   const uint32_t       usbh1_transfer_num;

extern   ARM_DRIVER_USBH      USBHn_DRIVER(USBH1_HC_NUM);

extern   USBH_PIPE            usbh1_pipe             [USBH1_HC_PIPE_NUM];
extern   uint32_t             usbh1_mem_pool        [(USBH1_HC_MEM_POOL_SIZE + 8 * USBH1_HC_PIPE_NUM + 32 + 4 + 3) / 4];

extern   const uint8_t        usbh1_power;
extern   const uint8_t        usbh1_mem_pool_reloc;
extern   const uint32_t       usbh1_mem_pool_size;

extern   USBH_HC_t            usbh1_hc;
extern   USBH_HC_t           *usbh1_hc_ptr;

// Global variable definitions

         ARM_DRIVER_USBH     *usbh1_hcd_ptr  =       &USBHn_DRIVER(USBH1_HC_NUM);
         USBH_PIPE            usbh1_pipe             [USBH1_HC_PIPE_NUM];
const    uint32_t             usbh1_pipe_num =        USBH1_HC_PIPE_NUM;
const    uint32_t             usbh1_transfer_num =    USBH1_HC_PIPE_NUM;
         uint32_t             usbh1_mem_pool        [(USBH1_HC_MEM_POOL_SIZE + 8 * USBH1_HC_PIPE_NUM + 32 + 4 + 3) / 4] __USBH1_AT_MEM(USBH1_HC_MEM_POOL_ADDR);
const    uint8_t              usbh1_power =          (uint8_t)(USBH1_HC_POWER / 2);
const    uint8_t              usbh1_mem_pool_reloc =  USBH1_HC_MEM_POOL_RELOC;
const    uint32_t             usbh1_mem_pool_size = ((USBH1_HC_MEM_POOL_SIZE + 8 * USBH1_HC_PIPE_NUM + 32 + 4 + 3) / 4) * 4;

         USBH_HC_t            usbh1_hc       =      { USBH1_HC_PIPE_NUM };
         USBH_HC_t           *usbh1_hc_ptr   =       &usbh1_hc;

// Core Thread definitions

#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t  usbh1_core_thread_cb_mem                                              __SECTION(.bss.os.thread.cb);
static uint64_t       usbh1_core_thread_stack_mem[(USBH1_CORE_THREAD_STACK_SIZE + 7U) / 8U] __SECTION(.bss.os.thread.stack);
#endif
extern 
const  osThreadAttr_t usbh1_core_thread_attr;
const  osThreadAttr_t usbh1_core_thread_attr = {
  "USBH1_Core_Thread",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
  &usbh1_core_thread_cb_mem,
  sizeof(usbh1_core_thread_cb_mem),
  &usbh1_core_thread_stack_mem,
#else
  NULL,
  0U,
  NULL,
#endif
  ((USBH1_CORE_THREAD_STACK_SIZE + 7U) / 8U) * 8U,
  USBH1_CORE_THREAD_PRIORITY,
  0U,
  0U
};
#endif
