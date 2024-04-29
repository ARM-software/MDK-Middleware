/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_config_hc_0.c
 * Purpose: USB Host (USBH) - Host Controller 0 Data Definitions
 * Note:    this file should only be included when USBH_Config_0.c is being 
 *          compiled as it depends on definitions from USBH_Config_0.c
 *----------------------------------------------------------------------------*/

#include "usb_os.h"

// Global variable declarations

extern   ARM_DRIVER_USBH     *usbh0_hcd_ptr;
extern   ARM_DRIVER_USBH_HCI *usbh0_hci_hcd_ptr;
extern   const uint32_t       usbh0_pipe_num;
extern   const uint32_t       usbh0_transfer_num;

extern   ARM_DRIVER_USBH      USBHn_DRIVER(USBH0_HC_NUM);

extern   USBH_PIPE            usbh0_pipe             [USBH0_HC_PIPE_NUM];
extern   uint32_t             usbh0_mem_pool        [(USBH0_HC_MEM_POOL_SIZE + 8 * USBH0_HC_PIPE_NUM + 32 + 4 + 3) / 4];

extern   const uint8_t        usbh0_power;
extern   const uint8_t        usbh0_mem_pool_reloc;
extern   const uint32_t       usbh0_mem_pool_size;

extern   USBH_HC_t            usbh0_hc;
extern   USBH_HC_t           *usbh0_hc_ptr;

// Global variable definitions

         ARM_DRIVER_USBH     *usbh0_hcd_ptr  =       &USBHn_DRIVER(USBH0_HC_NUM);
         USBH_PIPE            usbh0_pipe             [USBH0_HC_PIPE_NUM];
const    uint32_t             usbh0_pipe_num =        USBH0_HC_PIPE_NUM;
const    uint32_t             usbh0_transfer_num =    USBH0_HC_PIPE_NUM;
         uint32_t             usbh0_mem_pool        [(USBH0_HC_MEM_POOL_SIZE + 8 * USBH0_HC_PIPE_NUM + 32 + 4 + 3) / 4] __USBH0_AT_MEM(USBH0_HC_MEM_POOL_ADDR);
const    uint8_t              usbh0_power =          (uint8_t)(USBH0_HC_POWER / 2);
const    uint8_t              usbh0_mem_pool_reloc =  USBH0_HC_MEM_POOL_RELOC;
const    uint32_t             usbh0_mem_pool_size = ((USBH0_HC_MEM_POOL_SIZE + 8 * USBH0_HC_PIPE_NUM + 32 + 4 + 3) / 4) * 4;

         USBH_HC_t            usbh0_hc       =      { USBH0_HC_PIPE_NUM };
         USBH_HC_t           *usbh0_hc_ptr   =       &usbh0_hc;

// Core Thread definitions

#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t  usbh0_core_thread_cb_mem                                              __SECTION(.bss.os.thread.cb);
static uint64_t       usbh0_core_thread_stack_mem[(USBH0_CORE_THREAD_STACK_SIZE + 7U) / 8U] __SECTION(.bss.os.thread.stack);
#endif
extern 
const  osThreadAttr_t usbh0_core_thread_attr;
const  osThreadAttr_t usbh0_core_thread_attr = {
  "USBH0_Core_Thread",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
  &usbh0_core_thread_cb_mem,
  sizeof(usbh0_core_thread_cb_mem),
  &usbh0_core_thread_stack_mem,
#else
  NULL,
  0U,
  NULL,
#endif
  ((USBH0_CORE_THREAD_STACK_SIZE + 7U) / 8U) * 8U,
  USBH0_CORE_THREAD_PRIORITY,
  0U,
  0U
};
#endif
