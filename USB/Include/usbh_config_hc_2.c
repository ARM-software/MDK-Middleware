/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2021 Arm Limited (or its affiliates). All rights reserved.
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

#if     (USBH2_HC_IF == 0)              // If Host Controller 2 Interface = Custom

extern   ARM_DRIVER_USBH      USBHn_DRIVER(USBH2_HC_NUM);

extern   USBH_PIPE            usbh2_pipe             [USBH2_HC_PIPE_NUM];
extern   uint32_t             usbh2_mem_pool        [(USBH2_HC_MEM_POOL_SIZE + 8 * USBH2_HC_PIPE_NUM + 32 + 4 + 3) / 4];

#elif   (USBH2_HC_IF == 1)              // If Host Controller 2 Interface = OHCI

extern   ARM_DRIVER_USBH      USBHn_DRIVER_OHCI_LIB(2);
extern   ARM_DRIVER_USBH_HCI  USBHn_DRIVER_HCI(USBH2_HC_NUM);

extern   USBH_PIPE            usbh2_pipe             [USBH2_HC_OHCI_PIPE_NUM];
extern   USBH_TRANSFER_INFO_t usbh2_transfer_info    [USBH2_HC_OHCI_PIPE_NUM];
extern   uint32_t             usbh2_ohci_hcca       [(USBH2_OHCI_MEM_HCCA_SIZE + 3) / 4];
extern   uint32_t             usbh2_ohci_ed         [(USBH2_OHCI_MEM_ED_SIZE   + 3) / 4];
extern   uint32_t             usbh2_ohci_td         [(USBH2_OHCI_MEM_TD_SIZE   + 3) / 4];
extern   uint32_t             usbh2_mem_pool        [(USBH2_OHCI_MEM_POOL_SIZE + 3) / 4];

extern   USBH_OHCI_t          usbh2_ohci;

#elif   (USBH2_HC_IF == 2)              // If Host Controller 2 Interface = EHCI
                                        // with extensions for full/low-speed support

extern   ARM_DRIVER_USBH      USBHn_DRIVER_EHCI_LIB(2);
extern   ARM_DRIVER_USBH_HCI  USBHn_DRIVER_HCI(USBH2_HC_NUM);

extern   USBH_PIPE            usbh2_pipe             [USBH2_HC_EHCI_PIPE_NUM];
extern   USBH_TRANSFER_INFO_t usbh2_transfer_info    [USBH2_HC_EHCI_PIPE_NUM];
extern   uint32_t             usbh2_ehci_pfl        [(USBH2_EHCI_MEM_PFL_SIZE  + 3) / 4];
extern   uint32_t             usbh2_ehci_qh         [(USBH2_EHCI_MEM_QH_SIZE   + 3) / 4];
extern   uint32_t             usbh2_ehci_qtd        [(USBH2_EHCI_MEM_QTD_SIZE  + 3) / 4];
extern   uint32_t             usbh2_mem_pool        [(USBH2_EHCI_MEM_POOL_SIZE + 3) / 4];

extern   USBH_EHCI_t          usbh2_ehci;

#endif

extern   const uint8_t        usbh2_power;
extern   const uint8_t        usbh2_mem_pool_reloc;
extern   const uint32_t       usbh2_mem_pool_size;

extern   USBH_HC_t            usbh2_hc;
extern   USBH_HC_t           *usbh2_hc_ptr;
extern   USBH_OHCI_t         *usbh2_ohci_ptr;
extern   USBH_EHCI_t         *usbh2_ehci_ptr;


// Global variable definitions

#if     (USBH2_HC_IF == 0)              // If Host Controller 2 Interface = Custom

         ARM_DRIVER_USBH     *usbh2_hcd_ptr  =       &USBHn_DRIVER(USBH2_HC_NUM);
         ARM_DRIVER_USBH_HCI *usbh2_hci_hcd_ptr =     NULL;
         USBH_PIPE            usbh2_pipe             [USBH2_HC_PIPE_NUM];
const    uint32_t             usbh2_pipe_num =        USBH2_HC_PIPE_NUM;
const    uint32_t             usbh2_transfer_num =    USBH2_HC_PIPE_NUM;
         uint32_t             usbh2_mem_pool        [(USBH2_HC_MEM_POOL_SIZE + 8 * USBH2_HC_PIPE_NUM + 32 + 4 + 3) / 4] __USBH2_AT_MEM(USBH2_HC_MEM_POOL_ADDR);
const    uint8_t              usbh2_power =          (uint8_t)(USBH2_HC_POWER / 2);
const    uint8_t              usbh2_mem_pool_reloc =  USBH2_HC_MEM_POOL_RELOC;
const    uint32_t             usbh2_mem_pool_size = ((USBH2_HC_MEM_POOL_SIZE + 8 * USBH2_HC_PIPE_NUM + 32 + 4 + 3) / 4) * 4;

         USBH_HC_t            usbh2_hc       =      { USBH2_HC_PIPE_NUM };
         USBH_HC_t           *usbh2_hc_ptr   =       &usbh2_hc;
         USBH_OHCI_t         *usbh2_ohci_ptr =        NULL;
         USBH_EHCI_t         *usbh2_ehci_ptr =        NULL;

#elif   (USBH2_HC_IF == 1)              // If Host Controller 2 Interface = OHCI

         ARM_DRIVER_USBH     *usbh2_hcd_ptr =        &USBHn_DRIVER_OHCI_LIB(2);
         ARM_DRIVER_USBH_HCI *usbh2_hci_hcd_ptr =    &USBHn_DRIVER_HCI(USBH2_HC_NUM);
         USBH_PIPE            usbh2_pipe             [USBH2_HC_OHCI_PIPE_NUM];
const    uint32_t             usbh2_pipe_num =        USBH2_HC_OHCI_PIPE_NUM;
         USBH_TRANSFER_INFO_t usbh2_transfer_info    [USBH2_HC_OHCI_PIPE_NUM];
const    uint32_t             usbh2_transfer_num =    USBH2_HC_OHCI_PIPE_NUM;
#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
         uint32_t             usbh2_ohci_hcca       [(USBH2_OHCI_MEM_HCCA_SIZE + 3) / 4] __USBH2_AT_MEM(USBH2_HC_OHCI_MEM_POOL_ADDR) __USBH2_SECTION_MEM(USBH2_OHCI_HCCA) __USBH2_ALIGNED_MEM(256);
         uint32_t             usbh2_ohci_ed         [(USBH2_OHCI_MEM_ED_SIZE   + 3) / 4] __USBH2_AT_MEM(USBH2_HC_OHCI_MEM_POOL_ADDR) __USBH2_ALIGNED_MEM(16);
         uint32_t             usbh2_ohci_td         [(USBH2_OHCI_MEM_TD_SIZE   + 3) / 4] __USBH2_AT_MEM(USBH2_HC_OHCI_MEM_POOL_ADDR) __USBH2_ALIGNED_MEM(16);
         uint32_t             usbh2_mem_pool        [(USBH2_OHCI_MEM_POOL_SIZE + 3) / 4] __USBH2_AT_MEM(USBH2_HC_OHCI_MEM_POOL_ADDR);
#else
         uint32_t             usbh2_ohci_hcca       [(USBH2_OHCI_MEM_HCCA_SIZE + 3) / 4] __USBH2_AT_MEM(USBH2_OHCI_MEM_HCCA)         __USBH2_SECTION_MEM(USBH2_OHCI_HCCA) __USBH2_ALIGNED_MEM(256);
         uint32_t             usbh2_ohci_ed         [(USBH2_OHCI_MEM_ED_SIZE   + 3) / 4] __USBH2_AT_MEM(USBH2_OHCI_MEM_ED)           __USBH2_ALIGNED_MEM(16);
         uint32_t             usbh2_ohci_td         [(USBH2_OHCI_MEM_TD_SIZE   + 3) / 4] __USBH2_AT_MEM(USBH2_OHCI_MEM_TD)           __USBH2_ALIGNED_MEM(16);
         uint32_t             usbh2_mem_pool        [(USBH2_OHCI_MEM_POOL_SIZE + 3) / 4] __USBH2_AT_MEM(USBH2_OHCI_MEM_POOL_ADDR);
#endif
const    uint8_t              usbh2_power =          (uint8_t)(USBH2_HC_OHCI_POWER / 2);
const    uint8_t              usbh2_mem_pool_reloc =  USBH2_HC_OHCI_MEM_POOL_RELOC;
const    uint32_t             usbh2_mem_pool_size = ((USBH2_OHCI_MEM_POOL_SIZE + 3) / 4) * 4;

         USBH_HC_t           *usbh2_hc_ptr   =        NULL;
         USBH_OHCI_t          usbh2_ohci = {         (uint32_t *)USBH2_HC_OHCI_BASE_ADDRESS,
                                                      USBH2_HC_OHCI_PIPE_NUM,
                                                      USBH2_HC_OHCI_PIPE_NUM,
                                                      0U,
                                                      0U, // padding byte
                                                     &usbh2_ohci_hcca[0],
                                                     &usbh2_ohci_ed[0],
                                                     &usbh2_ohci_td[0],
                                                      NULL,
                                                     &usbh2_transfer_info[0] };
         USBH_OHCI_t         *usbh2_ohci_ptr =       &usbh2_ohci;
         USBH_EHCI_t         *usbh2_ehci_ptr =        NULL;

#elif   (USBH2_HC_IF == 2)              // If Host Controller 2 Interface = EHCI
                                        // with extensions for full/low-speed support

         ARM_DRIVER_USBH     *usbh2_hcd_ptr =        &USBHn_DRIVER_EHCI_LIB(2);
         ARM_DRIVER_USBH_HCI *usbh2_hci_hcd_ptr =    &USBHn_DRIVER_HCI(USBH2_HC_NUM);
         USBH_PIPE            usbh2_pipe             [USBH2_HC_EHCI_PIPE_NUM];
const    uint32_t             usbh2_pipe_num =        USBH2_HC_EHCI_PIPE_NUM;
         USBH_TRANSFER_INFO_t usbh2_transfer_info    [USBH2_HC_EHCI_PIPE_NUM];
         USBH_PipeEventInfo_t usbh2_pipe_evt_info                                        __ALIGNED(128);
const    uint32_t             usbh2_transfer_num =    USBH2_HC_EHCI_PIPE_NUM;
#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
         uint32_t             usbh2_ehci_pfl        [(USBH2_EHCI_MEM_PFL_SIZE  + 3) / 4] __USBH2_AT_MEM(USBH2_HC_EHCI_MEM_POOL_ADDR) __USBH2_SECTION_MEM(USBH2_EHCI_PFL) __USBH2_ALIGNED_MEM(4096);
         uint32_t             usbh2_ehci_qh         [(USBH2_EHCI_MEM_QH_SIZE   + 3) / 4] __USBH2_AT_MEM(USBH2_HC_EHCI_MEM_POOL_ADDR) __USBH2_ALIGNED_MEM(64);
         uint32_t             usbh2_ehci_qtd        [(USBH2_EHCI_MEM_QTD_SIZE  + 3) / 4] __USBH2_AT_MEM(USBH2_HC_EHCI_MEM_POOL_ADDR) __USBH2_ALIGNED_MEM(32);
         uint32_t             usbh2_ehci_sitd       [(USBH2_EHCI_MEM_SITD_SIZE + 3) / 4] __USBH2_AT_MEM(USBH2_HC_EHCI_MEM_POOL_ADDR) __USBH2_ALIGNED_MEM(32);
         uint32_t             usbh2_mem_pool        [(USBH2_EHCI_MEM_POOL_SIZE + 3) / 4] __USBH2_AT_MEM(USBH2_HC_EHCI_MEM_POOL_ADDR);
#else
         uint32_t             usbh2_ehci_pfl        [(USBH2_EHCI_MEM_PFL_SIZE  + 3) / 4] __USBH2_AT_MEM(USBH2_EHCI_MEM_PFL)          __USBH2_SECTION_MEM(USBH2_EHCI_PFL) __USBH2_ALIGNED_MEM(4096);
         uint32_t             usbh2_ehci_qh         [(USBH2_EHCI_MEM_QH_SIZE   + 3) / 4] __USBH2_AT_MEM(USBH2_EHCI_MEM_QH)           __USBH2_ALIGNED_MEM(64);
         uint32_t             usbh2_ehci_qtd        [(USBH2_EHCI_MEM_QTD_SIZE  + 3) / 4] __USBH2_AT_MEM(USBH2_EHCI_MEM_QTD)          __USBH2_ALIGNED_MEM(32);
         uint32_t             usbh2_ehci_sitd       [(USBH2_EHCI_MEM_SITD_SIZE + 3) / 4] __USBH2_AT_MEM(USBH2_EHCI_MEM_SITD)         __USBH2_ALIGNED_MEM(32);
         uint32_t             usbh2_mem_pool        [(USBH2_EHCI_MEM_POOL_SIZE + 3) / 4] __USBH2_AT_MEM(USBH2_EHCI_MEM_POOL_ADDR);
#endif
const    uint8_t              usbh2_power =          (uint8_t)(USBH2_HC_EHCI_POWER / 2);
const    uint8_t              usbh2_mem_pool_reloc =  USBH2_HC_EHCI_MEM_POOL_RELOC;
const    uint32_t             usbh2_mem_pool_size = ((USBH2_EHCI_MEM_POOL_SIZE + 3) / 4) * 4;

         USBH_HC_t           *usbh2_hc_ptr   =        NULL;
         USBH_OHCI_t         *usbh2_ohci_ptr =        NULL;
         USBH_EHCI_t          usbh2_ehci = {         (uint32_t *)USBH2_HC_EHCI_BASE_ADDRESS,
                                                      USBH2_HC_EHCI_PIPE_NUM,
                                                      USBH2_HC_EHCI_PIPE_NUM,
                                                      0U,
                                                      2*(USBH2_HC_EHCI_PIPE_NUM-1),
                                                      1024U,
                                                      0U, // padding byte
                                                     &usbh2_ehci_pfl[0],
                                                     &usbh2_ehci_qh[0],
                                                     &usbh2_ehci_qtd[0],
                                                      NULL,
                                                     &usbh2_ehci_sitd[0],
                                                      NULL,
                                                     &usbh2_transfer_info[0],
                                                     &usbh2_pipe_evt_info };
         USBH_EHCI_t         *usbh2_ehci_ptr =       &usbh2_ehci;
#endif


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
#else
extern const osThreadDef_t os_thread_def_USBH2_Core_Thread;
osThreadDef(USBH2_Core_Thread, USBH2_CORE_THREAD_PRIORITY, 1U, USBH2_CORE_THREAD_STACK_SIZE);
#endif
