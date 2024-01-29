/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2021 Arm Limited (or its affiliates). All rights reserved.
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

#if     (USBH1_HC_IF == 0)              // If Host Controller 1 Interface = Custom

extern   ARM_DRIVER_USBH      USBHn_DRIVER(USBH1_HC_NUM);

extern   USBH_PIPE            usbh1_pipe             [USBH1_HC_PIPE_NUM];
extern   uint32_t             usbh1_mem_pool        [(USBH1_HC_MEM_POOL_SIZE + 8 * USBH1_HC_PIPE_NUM + 32 + 4 + 3) / 4];

#elif   (USBH1_HC_IF == 1)              // If Host Controller 1 Interface = OHCI

extern   ARM_DRIVER_USBH      USBHn_DRIVER_OHCI_LIB(1);
extern   ARM_DRIVER_USBH_HCI  USBHn_DRIVER_HCI(USBH1_HC_NUM);

extern   USBH_PIPE            usbh1_pipe             [USBH1_HC_OHCI_PIPE_NUM];
extern   USBH_TRANSFER_INFO_t usbh1_transfer_info    [USBH1_HC_OHCI_PIPE_NUM];
extern   uint32_t             usbh1_ohci_hcca       [(USBH1_OHCI_MEM_HCCA_SIZE + 3) / 4];
extern   uint32_t             usbh1_ohci_ed         [(USBH1_OHCI_MEM_ED_SIZE   + 3) / 4];
extern   uint32_t             usbh1_ohci_td         [(USBH1_OHCI_MEM_TD_SIZE   + 3) / 4];
extern   uint32_t             usbh1_mem_pool        [(USBH1_OHCI_MEM_POOL_SIZE + 3) / 4];

extern   USBH_OHCI_t          usbh1_ohci;

#elif   (USBH1_HC_IF == 2)              // If Host Controller 1 Interface = EHCI
                                        // with extensions for full/low-speed support

extern   ARM_DRIVER_USBH      USBHn_DRIVER_EHCI_LIB(1);
extern   ARM_DRIVER_USBH_HCI  USBHn_DRIVER_HCI(USBH1_HC_NUM);

extern   USBH_PIPE            usbh1_pipe             [USBH1_HC_EHCI_PIPE_NUM];
extern   USBH_TRANSFER_INFO_t usbh1_transfer_info    [USBH1_HC_EHCI_PIPE_NUM];
extern   uint32_t             usbh1_ehci_pfl        [(USBH1_EHCI_MEM_PFL_SIZE  + 3) / 4];
extern   uint32_t             usbh1_ehci_qh         [(USBH1_EHCI_MEM_QH_SIZE   + 3) / 4];
extern   uint32_t             usbh1_ehci_qtd        [(USBH1_EHCI_MEM_QTD_SIZE  + 3) / 4];
extern   uint32_t             usbh1_mem_pool        [(USBH1_EHCI_MEM_POOL_SIZE + 3) / 4];

extern   USBH_EHCI_t          usbh1_ehci;

#endif

extern   const uint8_t        usbh1_power;
extern   const uint8_t        usbh1_mem_pool_reloc;
extern   const uint32_t       usbh1_mem_pool_size;

extern   USBH_HC_t            usbh1_hc;
extern   USBH_HC_t           *usbh1_hc_ptr;
extern   USBH_OHCI_t         *usbh1_ohci_ptr;
extern   USBH_EHCI_t         *usbh1_ehci_ptr;


// Global variable definitions

#if     (USBH1_HC_IF == 0)              // If Host Controller 1 Interface = Custom

         ARM_DRIVER_USBH     *usbh1_hcd_ptr  =       &USBHn_DRIVER(USBH1_HC_NUM);
         ARM_DRIVER_USBH_HCI *usbh1_hci_hcd_ptr =     NULL;
         USBH_PIPE            usbh1_pipe             [USBH1_HC_PIPE_NUM];
const    uint32_t             usbh1_pipe_num =        USBH1_HC_PIPE_NUM;
const    uint32_t             usbh1_transfer_num =    USBH1_HC_PIPE_NUM;
         uint32_t             usbh1_mem_pool        [(USBH1_HC_MEM_POOL_SIZE + 8 * USBH1_HC_PIPE_NUM + 32 + 4 + 3) / 4] __USBH1_AT_MEM(USBH1_HC_MEM_POOL_ADDR);
const    uint8_t              usbh1_power =          (uint8_t)(USBH1_HC_POWER / 2);
const    uint8_t              usbh1_mem_pool_reloc =  USBH1_HC_MEM_POOL_RELOC;
const    uint32_t             usbh1_mem_pool_size = ((USBH1_HC_MEM_POOL_SIZE + 8 * USBH1_HC_PIPE_NUM + 32 + 4 + 3) / 4) * 4;

         USBH_HC_t            usbh1_hc       =      { USBH1_HC_PIPE_NUM };
         USBH_HC_t           *usbh1_hc_ptr   =       &usbh1_hc;
         USBH_OHCI_t         *usbh1_ohci_ptr =        NULL;
         USBH_EHCI_t         *usbh1_ehci_ptr =        NULL;

#elif   (USBH1_HC_IF == 1)              // If Host Controller 1 Interface = OHCI

         ARM_DRIVER_USBH     *usbh1_hcd_ptr =        &USBHn_DRIVER_OHCI_LIB(1);
         ARM_DRIVER_USBH_HCI *usbh1_hci_hcd_ptr =    &USBHn_DRIVER_HCI(USBH1_HC_NUM);
         USBH_PIPE            usbh1_pipe             [USBH1_HC_OHCI_PIPE_NUM];
const    uint32_t             usbh1_pipe_num =        USBH1_HC_OHCI_PIPE_NUM;
         USBH_TRANSFER_INFO_t usbh1_transfer_info    [USBH1_HC_OHCI_PIPE_NUM];
const    uint32_t             usbh1_transfer_num =    USBH1_HC_OHCI_PIPE_NUM;
#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
         uint32_t             usbh1_ohci_hcca       [(USBH1_OHCI_MEM_HCCA_SIZE + 3) / 4] __USBH1_AT_MEM(USBH1_HC_OHCI_MEM_POOL_ADDR) __USBH1_SECTION_MEM(USBH1_OHCI_HCCA) __USBH1_ALIGNED_MEM(256);
         uint32_t             usbh1_ohci_ed         [(USBH1_OHCI_MEM_ED_SIZE   + 3) / 4] __USBH1_AT_MEM(USBH1_HC_OHCI_MEM_POOL_ADDR) __USBH1_ALIGNED_MEM(16);
         uint32_t             usbh1_ohci_td         [(USBH1_OHCI_MEM_TD_SIZE   + 3) / 4] __USBH1_AT_MEM(USBH1_HC_OHCI_MEM_POOL_ADDR) __USBH1_ALIGNED_MEM(16);
         uint32_t             usbh1_mem_pool        [(USBH1_OHCI_MEM_POOL_SIZE + 3) / 4] __USBH1_AT_MEM(USBH1_HC_OHCI_MEM_POOL_ADDR);
#else
         uint32_t             usbh1_ohci_hcca       [(USBH1_OHCI_MEM_HCCA_SIZE + 3) / 4] __USBH1_AT_MEM(USBH1_OHCI_MEM_HCCA)         __USBH1_SECTION_MEM(USBH1_OHCI_HCCA) __USBH1_ALIGNED_MEM(256);
         uint32_t             usbh1_ohci_ed         [(USBH1_OHCI_MEM_ED_SIZE   + 3) / 4] __USBH1_AT_MEM(USBH1_OHCI_MEM_ED)           __USBH1_ALIGNED_MEM(16);
         uint32_t             usbh1_ohci_td         [(USBH1_OHCI_MEM_TD_SIZE   + 3) / 4] __USBH1_AT_MEM(USBH1_OHCI_MEM_TD)           __USBH1_ALIGNED_MEM(16);
         uint32_t             usbh1_mem_pool        [(USBH1_OHCI_MEM_POOL_SIZE + 3) / 4] __USBH1_AT_MEM(USBH1_OHCI_MEM_POOL_ADDR);
#endif
const    uint8_t              usbh1_power =          (uint8_t)(USBH1_HC_OHCI_POWER / 2);
const    uint8_t              usbh1_mem_pool_reloc =  USBH1_HC_OHCI_MEM_POOL_RELOC;
const    uint32_t             usbh1_mem_pool_size = ((USBH1_OHCI_MEM_POOL_SIZE + 3) / 4) * 4;

         USBH_HC_t           *usbh1_hc_ptr   =        NULL;
         USBH_OHCI_t          usbh1_ohci = {         (uint32_t *)USBH1_HC_OHCI_BASE_ADDRESS,
                                                      USBH1_HC_OHCI_PIPE_NUM,
                                                      USBH1_HC_OHCI_PIPE_NUM,
                                                      0U,
                                                      0U, // padding byte
                                                     &usbh1_ohci_hcca[0],
                                                     &usbh1_ohci_ed[0],
                                                     &usbh1_ohci_td[0],
                                                      NULL,
                                                     &usbh1_transfer_info[0] };
         USBH_OHCI_t         *usbh1_ohci_ptr =       &usbh1_ohci;
         USBH_EHCI_t         *usbh1_ehci_ptr =        NULL;

#elif   (USBH1_HC_IF == 2)              // If Host Controller 1 Interface = EHCI
                                        // with extensions for full/low-speed support

         ARM_DRIVER_USBH     *usbh1_hcd_ptr =        &USBHn_DRIVER_EHCI_LIB(1);
         ARM_DRIVER_USBH_HCI *usbh1_hci_hcd_ptr =    &USBHn_DRIVER_HCI(USBH1_HC_NUM);
         USBH_PIPE            usbh1_pipe             [USBH1_HC_EHCI_PIPE_NUM];
const    uint32_t             usbh1_pipe_num =        USBH1_HC_EHCI_PIPE_NUM;
         USBH_TRANSFER_INFO_t usbh1_transfer_info    [USBH1_HC_EHCI_PIPE_NUM];
         USBH_PipeEventInfo_t usbh1_pipe_evt_info                                        __ALIGNED(128);
const    uint32_t             usbh1_transfer_num =    USBH1_HC_EHCI_PIPE_NUM;
#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
         uint32_t             usbh1_ehci_pfl        [(USBH1_EHCI_MEM_PFL_SIZE  + 3) / 4] __USBH1_AT_MEM(USBH1_HC_EHCI_MEM_POOL_ADDR) __USBH1_SECTION_MEM(USBH1_EHCI_PFL) __USBH1_ALIGNED_MEM(4096);
         uint32_t             usbh1_ehci_qh         [(USBH1_EHCI_MEM_QH_SIZE   + 3) / 4] __USBH1_AT_MEM(USBH1_HC_EHCI_MEM_POOL_ADDR) __USBH1_ALIGNED_MEM(64);
         uint32_t             usbh1_ehci_qtd        [(USBH1_EHCI_MEM_QTD_SIZE  + 3) / 4] __USBH1_AT_MEM(USBH1_HC_EHCI_MEM_POOL_ADDR) __USBH1_ALIGNED_MEM(32);
         uint32_t             usbh1_ehci_sitd       [(USBH1_EHCI_MEM_SITD_SIZE + 3) / 4] __USBH1_AT_MEM(USBH1_HC_EHCI_MEM_POOL_ADDR) __USBH1_ALIGNED_MEM(32);
         uint32_t             usbh1_mem_pool        [(USBH1_EHCI_MEM_POOL_SIZE + 3) / 4] __USBH1_AT_MEM(USBH1_HC_EHCI_MEM_POOL_ADDR);
#else
         uint32_t             usbh1_ehci_pfl        [(USBH1_EHCI_MEM_PFL_SIZE  + 3) / 4] __USBH1_AT_MEM(USBH1_EHCI_MEM_PFL)          __USBH1_SECTION_MEM(USBH1_EHCI_PFL) __USBH1_ALIGNED_MEM(4096);
         uint32_t             usbh1_ehci_qh         [(USBH1_EHCI_MEM_QH_SIZE   + 3) / 4] __USBH1_AT_MEM(USBH1_EHCI_MEM_QH)           __USBH1_ALIGNED_MEM(64);
         uint32_t             usbh1_ehci_qtd        [(USBH1_EHCI_MEM_QTD_SIZE  + 3) / 4] __USBH1_AT_MEM(USBH1_EHCI_MEM_QTD)          __USBH1_ALIGNED_MEM(32);
         uint32_t             usbh1_ehci_sitd       [(USBH1_EHCI_MEM_SITD_SIZE + 3) / 4] __USBH1_AT_MEM(USBH1_EHCI_MEM_SITD)         __USBH1_ALIGNED_MEM(32);
         uint32_t             usbh1_mem_pool        [(USBH1_EHCI_MEM_POOL_SIZE + 3) / 4] __USBH1_AT_MEM(USBH1_EHCI_MEM_POOL_ADDR);
#endif
const    uint8_t              usbh1_power =          (uint8_t)(USBH1_HC_EHCI_POWER / 2);
const    uint8_t              usbh1_mem_pool_reloc =  USBH1_HC_EHCI_MEM_POOL_RELOC;
const    uint32_t             usbh1_mem_pool_size = ((USBH1_EHCI_MEM_POOL_SIZE + 3) / 4) * 4;

         USBH_HC_t           *usbh1_hc_ptr   =        NULL;
         USBH_OHCI_t         *usbh1_ohci_ptr =        NULL;
         USBH_EHCI_t          usbh1_ehci = {         (uint32_t *)USBH1_HC_EHCI_BASE_ADDRESS,
                                                      USBH1_HC_EHCI_PIPE_NUM,
                                                      USBH1_HC_EHCI_PIPE_NUM,
                                                      0U,
                                                      2*(USBH1_HC_EHCI_PIPE_NUM-1),
                                                      1024U,
                                                      0U, // padding byte
                                                     &usbh1_ehci_pfl[0],
                                                     &usbh1_ehci_qh[0],
                                                     &usbh1_ehci_qtd[0],
                                                      NULL,
                                                     &usbh1_ehci_sitd[0],
                                                      NULL,
                                                     &usbh1_transfer_info[0],
                                                     &usbh1_pipe_evt_info };
         USBH_EHCI_t         *usbh1_ehci_ptr =       &usbh1_ehci;
#endif


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
#else
extern const osThreadDef_t os_thread_def_USBH1_Core_Thread;
osThreadDef(USBH1_Core_Thread, USBH1_CORE_THREAD_PRIORITY, 1U, USBH1_CORE_THREAD_STACK_SIZE);
#endif
