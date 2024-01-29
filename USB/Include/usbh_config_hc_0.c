/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2021 Arm Limited (or its affiliates). All rights reserved.
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

#if     (USBH0_HC_IF == 0)              // If Host Controller 0 Interface = Custom

extern   ARM_DRIVER_USBH      USBHn_DRIVER(USBH0_HC_NUM);

extern   USBH_PIPE            usbh0_pipe             [USBH0_HC_PIPE_NUM];
extern   uint32_t             usbh0_mem_pool        [(USBH0_HC_MEM_POOL_SIZE + 8 * USBH0_HC_PIPE_NUM + 32 + 4 + 3) / 4];

#elif   (USBH0_HC_IF == 1)              // If Host Controller 0 Interface = OHCI

extern   ARM_DRIVER_USBH      USBHn_DRIVER_OHCI_LIB(0);
extern   ARM_DRIVER_USBH_HCI  USBHn_DRIVER_HCI(USBH0_HC_NUM);

extern   USBH_PIPE            usbh0_pipe             [USBH0_HC_OHCI_PIPE_NUM];
extern   USBH_TRANSFER_INFO_t usbh0_transfer_info    [USBH0_HC_OHCI_PIPE_NUM];
extern   uint32_t             usbh0_ohci_hcca       [(USBH0_OHCI_MEM_HCCA_SIZE + 3) / 4];
extern   uint32_t             usbh0_ohci_ed         [(USBH0_OHCI_MEM_ED_SIZE   + 3) / 4];
extern   uint32_t             usbh0_ohci_td         [(USBH0_OHCI_MEM_TD_SIZE   + 3) / 4];
extern   uint32_t             usbh0_mem_pool        [(USBH0_OHCI_MEM_POOL_SIZE + 3) / 4];

extern   USBH_OHCI_t          usbh0_ohci;

#elif   (USBH0_HC_IF == 2)              // If Host Controller 0 Interface = EHCI
                                        // with extensions for full/low-speed support

extern   ARM_DRIVER_USBH      USBHn_DRIVER_EHCI_LIB(0);
extern   ARM_DRIVER_USBH_HCI  USBHn_DRIVER_HCI(USBH0_HC_NUM);

extern   USBH_PIPE            usbh0_pipe             [USBH0_HC_EHCI_PIPE_NUM];
extern   USBH_TRANSFER_INFO_t usbh0_transfer_info    [USBH0_HC_EHCI_PIPE_NUM];
extern   uint32_t             usbh0_ehci_pfl        [(USBH0_EHCI_MEM_PFL_SIZE  + 3) / 4];
extern   uint32_t             usbh0_ehci_qh         [(USBH0_EHCI_MEM_QH_SIZE   + 3) / 4];
extern   uint32_t             usbh0_ehci_qtd        [(USBH0_EHCI_MEM_QTD_SIZE  + 3) / 4];
extern   uint32_t             usbh0_mem_pool        [(USBH0_EHCI_MEM_POOL_SIZE + 3) / 4];

extern   USBH_EHCI_t          usbh0_ehci;

#endif

extern   const uint8_t        usbh0_power;
extern   const uint8_t        usbh0_mem_pool_reloc;
extern   const uint32_t       usbh0_mem_pool_size;

extern   USBH_HC_t            usbh0_hc;
extern   USBH_HC_t           *usbh0_hc_ptr;
extern   USBH_OHCI_t         *usbh0_ohci_ptr;
extern   USBH_EHCI_t         *usbh0_ehci_ptr;


// Global variable definitions

#if     (USBH0_HC_IF == 0)              // If Host Controller 0 Interface = Custom

         ARM_DRIVER_USBH     *usbh0_hcd_ptr  =       &USBHn_DRIVER(USBH0_HC_NUM);
         ARM_DRIVER_USBH_HCI *usbh0_hci_hcd_ptr =     NULL;
         USBH_PIPE            usbh0_pipe             [USBH0_HC_PIPE_NUM];
const    uint32_t             usbh0_pipe_num =        USBH0_HC_PIPE_NUM;
const    uint32_t             usbh0_transfer_num =    USBH0_HC_PIPE_NUM;
         uint32_t             usbh0_mem_pool        [(USBH0_HC_MEM_POOL_SIZE + 8 * USBH0_HC_PIPE_NUM + 32 + 4 + 3) / 4] __USBH0_AT_MEM(USBH0_HC_MEM_POOL_ADDR);
const    uint8_t              usbh0_power =          (uint8_t)(USBH0_HC_POWER / 2);
const    uint8_t              usbh0_mem_pool_reloc =  USBH0_HC_MEM_POOL_RELOC;
const    uint32_t             usbh0_mem_pool_size = ((USBH0_HC_MEM_POOL_SIZE + 8 * USBH0_HC_PIPE_NUM + 32 + 4 + 3) / 4) * 4;

         USBH_HC_t            usbh0_hc       =      { USBH0_HC_PIPE_NUM };
         USBH_HC_t           *usbh0_hc_ptr   =       &usbh0_hc;
         USBH_OHCI_t         *usbh0_ohci_ptr =        NULL;
         USBH_EHCI_t         *usbh0_ehci_ptr =        NULL;

#elif   (USBH0_HC_IF == 1)              // If Host Controller 0 Interface = OHCI

         ARM_DRIVER_USBH     *usbh0_hcd_ptr =        &USBHn_DRIVER_OHCI_LIB(0);
         ARM_DRIVER_USBH_HCI *usbh0_hci_hcd_ptr =    &USBHn_DRIVER_HCI(USBH0_HC_NUM);
         USBH_PIPE            usbh0_pipe             [USBH0_HC_OHCI_PIPE_NUM];
const    uint32_t             usbh0_pipe_num =        USBH0_HC_OHCI_PIPE_NUM;
         USBH_TRANSFER_INFO_t usbh0_transfer_info    [USBH0_HC_OHCI_PIPE_NUM];
const    uint32_t             usbh0_transfer_num =    USBH0_HC_OHCI_PIPE_NUM;
#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
         uint32_t             usbh0_ohci_hcca       [(USBH0_OHCI_MEM_HCCA_SIZE + 3) / 4] __USBH0_AT_MEM(USBH0_HC_OHCI_MEM_POOL_ADDR) __USBH0_SECTION_MEM(USBH0_OHCI_HCCA) __USBH0_ALIGNED_MEM(256);
         uint32_t             usbh0_ohci_ed         [(USBH0_OHCI_MEM_ED_SIZE   + 3) / 4] __USBH0_AT_MEM(USBH0_HC_OHCI_MEM_POOL_ADDR) __USBH0_ALIGNED_MEM(16);
         uint32_t             usbh0_ohci_td         [(USBH0_OHCI_MEM_TD_SIZE   + 3) / 4] __USBH0_AT_MEM(USBH0_HC_OHCI_MEM_POOL_ADDR) __USBH0_ALIGNED_MEM(16);
         uint32_t             usbh0_mem_pool        [(USBH0_OHCI_MEM_POOL_SIZE + 3) / 4] __USBH0_AT_MEM(USBH0_HC_OHCI_MEM_POOL_ADDR);
#else
         uint32_t             usbh0_ohci_hcca       [(USBH0_OHCI_MEM_HCCA_SIZE + 3) / 4] __USBH0_AT_MEM(USBH0_OHCI_MEM_HCCA)         __USBH0_SECTION_MEM(USBH0_OHCI_HCCA) __USBH0_ALIGNED_MEM(256);
         uint32_t             usbh0_ohci_ed         [(USBH0_OHCI_MEM_ED_SIZE   + 3) / 4] __USBH0_AT_MEM(USBH0_OHCI_MEM_ED)           __USBH0_ALIGNED_MEM(16);
         uint32_t             usbh0_ohci_td         [(USBH0_OHCI_MEM_TD_SIZE   + 3) / 4] __USBH0_AT_MEM(USBH0_OHCI_MEM_TD)           __USBH0_ALIGNED_MEM(16);
         uint32_t             usbh0_mem_pool        [(USBH0_OHCI_MEM_POOL_SIZE + 3) / 4] __USBH0_AT_MEM(USBH0_OHCI_MEM_POOL_ADDR);
#endif
const    uint8_t              usbh0_power =          (uint8_t)(USBH0_HC_OHCI_POWER / 2);
const    uint8_t              usbh0_mem_pool_reloc =  USBH0_HC_OHCI_MEM_POOL_RELOC;
const    uint32_t             usbh0_mem_pool_size = ((USBH0_OHCI_MEM_POOL_SIZE + 3) / 4) * 4;

         USBH_HC_t           *usbh0_hc_ptr   =        NULL;
         USBH_OHCI_t          usbh0_ohci = {         (uint32_t *)USBH0_HC_OHCI_BASE_ADDRESS,
                                                      USBH0_HC_OHCI_PIPE_NUM,
                                                      USBH0_HC_OHCI_PIPE_NUM,
                                                      0U,
                                                      0U, // padding byte
                                                     &usbh0_ohci_hcca[0],
                                                     &usbh0_ohci_ed[0],
                                                     &usbh0_ohci_td[0],
                                                      NULL,
                                                     &usbh0_transfer_info[0] };
         USBH_OHCI_t         *usbh0_ohci_ptr =       &usbh0_ohci;
         USBH_EHCI_t         *usbh0_ehci_ptr =        NULL;

#elif   (USBH0_HC_IF == 2)              // If Host Controller 0 Interface = EHCI
                                        // with extensions for full/low-speed support

         ARM_DRIVER_USBH     *usbh0_hcd_ptr =        &USBHn_DRIVER_EHCI_LIB(0);
         ARM_DRIVER_USBH_HCI *usbh0_hci_hcd_ptr =    &USBHn_DRIVER_HCI(USBH0_HC_NUM);
         USBH_PIPE            usbh0_pipe             [USBH0_HC_EHCI_PIPE_NUM];
const    uint32_t             usbh0_pipe_num =        USBH0_HC_EHCI_PIPE_NUM;
         USBH_TRANSFER_INFO_t usbh0_transfer_info    [USBH0_HC_EHCI_PIPE_NUM];
         USBH_PipeEventInfo_t usbh0_pipe_evt_info                                        __ALIGNED(128);
const    uint32_t             usbh0_transfer_num =    USBH0_HC_EHCI_PIPE_NUM;
#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
         uint32_t             usbh0_ehci_pfl        [(USBH0_EHCI_MEM_PFL_SIZE  + 3) / 4] __USBH0_AT_MEM(USBH0_HC_EHCI_MEM_POOL_ADDR) __USBH0_SECTION_MEM(USBH0_EHCI_PFL) __USBH0_ALIGNED_MEM(4096);
         uint32_t             usbh0_ehci_qh         [(USBH0_EHCI_MEM_QH_SIZE   + 3) / 4] __USBH0_AT_MEM(USBH0_HC_EHCI_MEM_POOL_ADDR) __USBH0_ALIGNED_MEM(64);
         uint32_t             usbh0_ehci_qtd        [(USBH0_EHCI_MEM_QTD_SIZE  + 3) / 4] __USBH0_AT_MEM(USBH0_HC_EHCI_MEM_POOL_ADDR) __USBH0_ALIGNED_MEM(32);
         uint32_t             usbh0_ehci_sitd       [(USBH0_EHCI_MEM_SITD_SIZE + 3) / 4] __USBH0_AT_MEM(USBH0_HC_EHCI_MEM_POOL_ADDR) __USBH0_ALIGNED_MEM(32);
         uint32_t             usbh0_mem_pool        [(USBH0_EHCI_MEM_POOL_SIZE + 3) / 4] __USBH0_AT_MEM(USBH0_HC_EHCI_MEM_POOL_ADDR);
#else
         uint32_t             usbh0_ehci_pfl        [(USBH0_EHCI_MEM_PFL_SIZE  + 3) / 4] __USBH0_AT_MEM(USBH0_EHCI_MEM_PFL)          __USBH0_SECTION_MEM(USBH0_EHCI_PFL) __USBH0_ALIGNED_MEM(4096);
         uint32_t             usbh0_ehci_qh         [(USBH0_EHCI_MEM_QH_SIZE   + 3) / 4] __USBH0_AT_MEM(USBH0_EHCI_MEM_QH)           __USBH0_ALIGNED_MEM(64);
         uint32_t             usbh0_ehci_qtd        [(USBH0_EHCI_MEM_QTD_SIZE  + 3) / 4] __USBH0_AT_MEM(USBH0_EHCI_MEM_QTD)          __USBH0_ALIGNED_MEM(32);
         uint32_t             usbh0_ehci_sitd       [(USBH0_EHCI_MEM_SITD_SIZE + 3) / 4] __USBH0_AT_MEM(USBH0_EHCI_MEM_SITD)         __USBH0_ALIGNED_MEM(32);
         uint32_t             usbh0_mem_pool        [(USBH0_EHCI_MEM_POOL_SIZE + 3) / 4] __USBH0_AT_MEM(USBH0_EHCI_MEM_POOL_ADDR);
#endif
const    uint8_t              usbh0_power =          (uint8_t)(USBH0_HC_EHCI_POWER / 2);
const    uint8_t              usbh0_mem_pool_reloc =  USBH0_HC_EHCI_MEM_POOL_RELOC;
const    uint32_t             usbh0_mem_pool_size = ((USBH0_EHCI_MEM_POOL_SIZE + 3) / 4) * 4;

         USBH_HC_t           *usbh0_hc_ptr   =        NULL;
         USBH_OHCI_t         *usbh0_ohci_ptr =        NULL;
         USBH_EHCI_t          usbh0_ehci = {         (uint32_t *)USBH0_HC_EHCI_BASE_ADDRESS,
                                                      USBH0_HC_EHCI_PIPE_NUM,
                                                      USBH0_HC_EHCI_PIPE_NUM,
                                                      0U,
                                                      2*(USBH0_HC_EHCI_PIPE_NUM-1),
                                                      1024U,
                                                      0U, // padding byte
                                                     &usbh0_ehci_pfl[0],
                                                     &usbh0_ehci_qh[0],
                                                     &usbh0_ehci_qtd[0],
                                                      NULL,
                                                     &usbh0_ehci_sitd[0],
                                                      NULL,
                                                     &usbh0_transfer_info[0],
                                                     &usbh0_pipe_evt_info };
         USBH_EHCI_t         *usbh0_ehci_ptr =       &usbh0_ehci;
#endif


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
#else
extern const osThreadDef_t os_thread_def_USBH0_Core_Thread;
osThreadDef(USBH0_Core_Thread, USBH0_CORE_THREAD_PRIORITY, 1U, USBH0_CORE_THREAD_STACK_SIZE);
#endif
