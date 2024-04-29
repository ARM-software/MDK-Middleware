/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_config_hc_3.c
 * Purpose: USB Host (USBH) - Host Controller 3 Data Definitions
 * Note:    this file should only be included when USBH_Config_3.c is being 
 *          compiled as it depends on definitions from USBH_Config_3.c
 *----------------------------------------------------------------------------*/

#include "usb_os.h"

// Global variable declarations

extern   ARM_DRIVER_USBH     *usbh3_hcd_ptr;
extern   ARM_DRIVER_USBH_HCI *usbh3_hci_hcd_ptr;
extern   const uint32_t       usbh3_pipe_num;
extern   const uint32_t       usbh3_transfer_num;

extern   ARM_DRIVER_USBH      USBHn_DRIVER(USBH3_HC_NUM);

extern   USBH_PIPE            usbh3_pipe             [USBH3_HC_PIPE_NUM];
extern   uint32_t             usbh3_mem_pool        [(USBH3_HC_MEM_POOL_SIZE + 8 * USBH3_HC_PIPE_NUM + 32 + 4 + 3) / 4];

extern   const uint8_t        usbh3_power;
extern   const uint8_t        usbh3_mem_pool_reloc;
extern   const uint32_t       usbh3_mem_pool_size;

extern   USBH_HC_t            usbh3_hc;
extern   USBH_HC_t           *usbh3_hc_ptr;

// Global variable definitions

         ARM_DRIVER_USBH     *usbh3_hcd_ptr  =       &USBHn_DRIVER(USBH3_HC_NUM);
         USBH_PIPE            usbh3_pipe             [USBH3_HC_PIPE_NUM];
const    uint32_t             usbh3_pipe_num =        USBH3_HC_PIPE_NUM;
const    uint32_t             usbh3_transfer_num =    USBH3_HC_PIPE_NUM;
         uint32_t             usbh3_mem_pool        [(USBH3_HC_MEM_POOL_SIZE + 8 * USBH3_HC_PIPE_NUM + 32 + 4 + 3) / 4] __USBH3_AT_MEM(USBH3_HC_MEM_POOL_ADDR);
const    uint8_t              usbh3_power =          (uint8_t)(USBH3_HC_POWER / 2);
const    uint8_t              usbh3_mem_pool_reloc =  USBH3_HC_MEM_POOL_RELOC;
const    uint32_t             usbh3_mem_pool_size = ((USBH3_HC_MEM_POOL_SIZE + 8 * USBH3_HC_PIPE_NUM + 32 + 4 + 3) / 4) * 4;

         USBH_HC_t            usbh3_hc       =      { USBH3_HC_PIPE_NUM };
         USBH_HC_t           *usbh3_hc_ptr   =       &usbh3_hc;

// Core Thread definitions

#ifdef USB_CMSIS_RTOS2
#ifdef USB_CMSIS_RTOS2_RTX5
static osRtxThread_t  usbh3_core_thread_cb_mem                                              __SECTION(.bss.os.thread.cb);
static uint64_t       usbh3_core_thread_stack_mem[(USBH3_CORE_THREAD_STACK_SIZE + 7U) / 8U] __SECTION(.bss.os.thread.stack);
#endif
extern 
const  osThreadAttr_t usbh3_core_thread_attr;
const  osThreadAttr_t usbh3_core_thread_attr = {
  "USBH3_Core_Thread",
  0U,
#ifdef USB_CMSIS_RTOS2_RTX5
  &usbh3_core_thread_cb_mem,
  sizeof(usbh3_core_thread_cb_mem),
  &usbh3_core_thread_stack_mem,
#else
  NULL,
  0U,
  NULL,
#endif
  ((USBH3_CORE_THREAD_STACK_SIZE + 7U) / 8U) * 8U,
  USBH3_CORE_THREAD_PRIORITY,
  0U,
  0U
};
#endif
