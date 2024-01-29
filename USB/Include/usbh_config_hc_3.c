/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2021 Arm Limited (or its affiliates). All rights reserved.
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

#if     (USBH3_HC_IF == 0)              // If Host Controller 3 Interface = Custom

extern   ARM_DRIVER_USBH      USBHn_DRIVER(USBH3_HC_NUM);

extern   USBH_PIPE            usbh3_pipe             [USBH3_HC_PIPE_NUM];
extern   uint32_t             usbh3_mem_pool        [(USBH3_HC_MEM_POOL_SIZE + 8 * USBH3_HC_PIPE_NUM + 32 + 4 + 3) / 4];

#elif   (USBH3_HC_IF == 1)              // If Host Controller 3 Interface = OHCI

extern   ARM_DRIVER_USBH      USBHn_DRIVER_OHCI_LIB(3);
extern   ARM_DRIVER_USBH_HCI  USBHn_DRIVER_HCI(USBH3_HC_NUM);

extern   USBH_PIPE            usbh3_pipe             [USBH3_HC_OHCI_PIPE_NUM];
extern   USBH_TRANSFER_INFO_t usbh3_transfer_info    [USBH3_HC_OHCI_PIPE_NUM];
extern   uint32_t             usbh3_ohci_hcca       [(USBH3_OHCI_MEM_HCCA_SIZE + 3) / 4];
extern   uint32_t             usbh3_ohci_ed         [(USBH3_OHCI_MEM_ED_SIZE   + 3) / 4];
extern   uint32_t             usbh3_ohci_td         [(USBH3_OHCI_MEM_TD_SIZE   + 3) / 4];
extern   uint32_t             usbh3_mem_pool        [(USBH3_OHCI_MEM_POOL_SIZE + 3) / 4];

extern   USBH_OHCI_t          usbh3_ohci;

#elif   (USBH3_HC_IF == 2)              // If Host Controller 3 Interface = EHCI
                                        // with extensions for full/low-speed support

extern   ARM_DRIVER_USBH      USBHn_DRIVER_EHCI_LIB(3);
extern   ARM_DRIVER_USBH_HCI  USBHn_DRIVER_HCI(USBH3_HC_NUM);

extern   USBH_PIPE            usbh3_pipe             [USBH3_HC_EHCI_PIPE_NUM];
extern   USBH_TRANSFER_INFO_t usbh3_transfer_info    [USBH3_HC_EHCI_PIPE_NUM];
extern   uint32_t             usbh3_ehci_pfl        [(USBH3_EHCI_MEM_PFL_SIZE  + 3) / 4];
extern   uint32_t             usbh3_ehci_qh         [(USBH3_EHCI_MEM_QH_SIZE   + 3) / 4];
extern   uint32_t             usbh3_ehci_qtd        [(USBH3_EHCI_MEM_QTD_SIZE  + 3) / 4];
extern   uint32_t             usbh3_mem_pool        [(USBH3_EHCI_MEM_POOL_SIZE + 3) / 4];

extern   USBH_EHCI_t          usbh3_ehci;

#endif

extern   const uint8_t        usbh3_power;
extern   const uint8_t        usbh3_mem_pool_reloc;
extern   const uint32_t       usbh3_mem_pool_size;

extern   USBH_HC_t            usbh3_hc;
extern   USBH_HC_t           *usbh3_hc_ptr;
extern   USBH_OHCI_t         *usbh3_ohci_ptr;
extern   USBH_EHCI_t         *usbh3_ehci_ptr;


// Global variable definitions

#if     (USBH3_HC_IF == 0)              // If Host Controller 3 Interface = Custom

         ARM_DRIVER_USBH     *usbh3_hcd_ptr  =       &USBHn_DRIVER(USBH3_HC_NUM);
         ARM_DRIVER_USBH_HCI *usbh3_hci_hcd_ptr =     NULL;
         USBH_PIPE            usbh3_pipe             [USBH3_HC_PIPE_NUM];
const    uint32_t             usbh3_pipe_num =        USBH3_HC_PIPE_NUM;
const    uint32_t             usbh3_transfer_num =    USBH3_HC_PIPE_NUM;
         uint32_t             usbh3_mem_pool        [(USBH3_HC_MEM_POOL_SIZE + 8 * USBH3_HC_PIPE_NUM + 32 + 4 + 3) / 4] __USBH3_AT_MEM(USBH3_HC_MEM_POOL_ADDR);
const    uint8_t              usbh3_power =          (uint8_t)(USBH3_HC_POWER / 2);
const    uint8_t              usbh3_mem_pool_reloc =  USBH3_HC_MEM_POOL_RELOC;
const    uint32_t             usbh3_mem_pool_size = ((USBH3_HC_MEM_POOL_SIZE + 8 * USBH3_HC_PIPE_NUM + 32 + 4 + 3) / 4) * 4;

         USBH_HC_t            usbh3_hc       =      { USBH3_HC_PIPE_NUM };
         USBH_HC_t           *usbh3_hc_ptr   =       &usbh3_hc;
         USBH_OHCI_t         *usbh3_ohci_ptr =        NULL;
         USBH_EHCI_t         *usbh3_ehci_ptr =        NULL;

#elif   (USBH3_HC_IF == 1)              // If Host Controller 3 Interface = OHCI

         ARM_DRIVER_USBH     *usbh3_hcd_ptr =        &USBHn_DRIVER_OHCI_LIB(3);
         ARM_DRIVER_USBH_HCI *usbh3_hci_hcd_ptr =    &USBHn_DRIVER_HCI(USBH3_HC_NUM);
         USBH_PIPE            usbh3_pipe             [USBH3_HC_OHCI_PIPE_NUM];
const    uint32_t             usbh3_pipe_num =        USBH3_HC_OHCI_PIPE_NUM;
         USBH_TRANSFER_INFO_t usbh3_transfer_info    [USBH3_HC_OHCI_PIPE_NUM];
const    uint32_t             usbh3_transfer_num =    USBH3_HC_OHCI_PIPE_NUM;
#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
         uint32_t             usbh3_ohci_hcca       [(USBH3_OHCI_MEM_HCCA_SIZE + 3) / 4] __USBH3_AT_MEM(USBH3_HC_OHCI_MEM_POOL_ADDR) __USBH3_SECTION_MEM(USBH3_OHCI_HCCA) __USBH3_ALIGNED_MEM(256);
         uint32_t             usbh3_ohci_ed         [(USBH3_OHCI_MEM_ED_SIZE   + 3) / 4] __USBH3_AT_MEM(USBH3_HC_OHCI_MEM_POOL_ADDR) __USBH3_ALIGNED_MEM(16);
         uint32_t             usbh3_ohci_td         [(USBH3_OHCI_MEM_TD_SIZE   + 3) / 4] __USBH3_AT_MEM(USBH3_HC_OHCI_MEM_POOL_ADDR) __USBH3_ALIGNED_MEM(16);
         uint32_t             usbh3_mem_pool        [(USBH3_OHCI_MEM_POOL_SIZE + 3) / 4] __USBH3_AT_MEM(USBH3_HC_OHCI_MEM_POOL_ADDR);
#else
         uint32_t             usbh3_ohci_hcca       [(USBH3_OHCI_MEM_HCCA_SIZE + 3) / 4] __USBH3_AT_MEM(USBH3_OHCI_MEM_HCCA)         __USBH3_SECTION_MEM(USBH3_OHCI_HCCA) __USBH3_ALIGNED_MEM(256);
         uint32_t             usbh3_ohci_ed         [(USBH3_OHCI_MEM_ED_SIZE   + 3) / 4] __USBH3_AT_MEM(USBH3_OHCI_MEM_ED)           __USBH3_ALIGNED_MEM(16);
         uint32_t             usbh3_ohci_td         [(USBH3_OHCI_MEM_TD_SIZE   + 3) / 4] __USBH3_AT_MEM(USBH3_OHCI_MEM_TD)           __USBH3_ALIGNED_MEM(16);
         uint32_t             usbh3_mem_pool        [(USBH3_OHCI_MEM_POOL_SIZE + 3) / 4] __USBH3_AT_MEM(USBH3_OHCI_MEM_POOL_ADDR);
#endif
const    uint8_t              usbh3_power =          (uint8_t)(USBH3_HC_OHCI_POWER / 2);
const    uint8_t              usbh3_mem_pool_reloc =  USBH3_HC_OHCI_MEM_POOL_RELOC;
const    uint32_t             usbh3_mem_pool_size = ((USBH3_OHCI_MEM_POOL_SIZE + 3) / 4) * 4;

         USBH_HC_t           *usbh3_hc_ptr   =        NULL;
         USBH_OHCI_t          usbh3_ohci = {         (uint32_t *)USBH3_HC_OHCI_BASE_ADDRESS,
                                                      USBH3_HC_OHCI_PIPE_NUM,
                                                      USBH3_HC_OHCI_PIPE_NUM,
                                                      0U,
                                                      0U, // padding byte
                                                     &usbh3_ohci_hcca[0],
                                                     &usbh3_ohci_ed[0],
                                                     &usbh3_ohci_td[0],
                                                      NULL,
                                                     &usbh3_transfer_info[0] };
         USBH_OHCI_t         *usbh3_ohci_ptr =       &usbh3_ohci;
         USBH_EHCI_t         *usbh3_ehci_ptr =        NULL;

#elif   (USBH3_HC_IF == 2)              // If Host Controller 3 Interface = EHCI
                                        // with extensions for full/low-speed support

         ARM_DRIVER_USBH     *usbh3_hcd_ptr =        &USBHn_DRIVER_EHCI_LIB(3);
         ARM_DRIVER_USBH_HCI *usbh3_hci_hcd_ptr =    &USBHn_DRIVER_HCI(USBH3_HC_NUM);
         USBH_PIPE            usbh3_pipe             [USBH3_HC_EHCI_PIPE_NUM];
const    uint32_t             usbh3_pipe_num =        USBH3_HC_EHCI_PIPE_NUM;
         USBH_TRANSFER_INFO_t usbh3_transfer_info    [USBH3_HC_EHCI_PIPE_NUM];
         USBH_PipeEventInfo_t usbh3_pipe_evt_info                                        __ALIGNED(128);
const    uint32_t             usbh3_transfer_num =    USBH3_HC_EHCI_PIPE_NUM;
#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
         uint32_t             usbh3_ehci_pfl        [(USBH3_EHCI_MEM_PFL_SIZE  + 3) / 4] __USBH3_AT_MEM(USBH3_HC_EHCI_MEM_POOL_ADDR) __USBH3_SECTION_MEM(USBH3_EHCI_PFL) __USBH3_ALIGNED_MEM(4096);
         uint32_t             usbh3_ehci_qh         [(USBH3_EHCI_MEM_QH_SIZE   + 3) / 4] __USBH3_AT_MEM(USBH3_HC_EHCI_MEM_POOL_ADDR) __USBH3_ALIGNED_MEM(64);
         uint32_t             usbh3_ehci_qtd        [(USBH3_EHCI_MEM_QTD_SIZE  + 3) / 4] __USBH3_AT_MEM(USBH3_HC_EHCI_MEM_POOL_ADDR) __USBH3_ALIGNED_MEM(32);
         uint32_t             usbh3_ehci_sitd       [(USBH3_EHCI_MEM_SITD_SIZE + 3) / 4] __USBH3_AT_MEM(USBH3_HC_EHCI_MEM_POOL_ADDR) __USBH3_ALIGNED_MEM(32);
         uint32_t             usbh3_mem_pool        [(USBH3_EHCI_MEM_POOL_SIZE + 3) / 4] __USBH3_AT_MEM(USBH3_HC_EHCI_MEM_POOL_ADDR);
#else
         uint32_t             usbh3_ehci_pfl        [(USBH3_EHCI_MEM_PFL_SIZE  + 3) / 4] __USBH3_AT_MEM(USBH3_EHCI_MEM_PFL)          __USBH3_SECTION_MEM(USBH3_EHCI_PFL) __USBH3_ALIGNED_MEM(4096);
         uint32_t             usbh3_ehci_qh         [(USBH3_EHCI_MEM_QH_SIZE   + 3) / 4] __USBH3_AT_MEM(USBH3_EHCI_MEM_QH)           __USBH3_ALIGNED_MEM(64);
         uint32_t             usbh3_ehci_qtd        [(USBH3_EHCI_MEM_QTD_SIZE  + 3) / 4] __USBH3_AT_MEM(USBH3_EHCI_MEM_QTD)          __USBH3_ALIGNED_MEM(32);
         uint32_t             usbh3_ehci_sitd       [(USBH3_EHCI_MEM_SITD_SIZE + 3) / 4] __USBH3_AT_MEM(USBH3_EHCI_MEM_SITD)         __USBH3_ALIGNED_MEM(32);
         uint32_t             usbh3_mem_pool        [(USBH3_EHCI_MEM_POOL_SIZE + 3) / 4] __USBH3_AT_MEM(USBH3_EHCI_MEM_POOL_ADDR);
#endif
const    uint8_t              usbh3_power =          (uint8_t)(USBH3_HC_EHCI_POWER / 2);
const    uint8_t              usbh3_mem_pool_reloc =  USBH3_HC_EHCI_MEM_POOL_RELOC;
const    uint32_t             usbh3_mem_pool_size = ((USBH3_EHCI_MEM_POOL_SIZE + 3) / 4) * 4;

         USBH_HC_t           *usbh3_hc_ptr   =        NULL;
         USBH_OHCI_t         *usbh3_ohci_ptr =        NULL;
         USBH_EHCI_t          usbh3_ehci = {         (uint32_t *)USBH3_HC_EHCI_BASE_ADDRESS,
                                                      USBH3_HC_EHCI_PIPE_NUM,
                                                      USBH3_HC_EHCI_PIPE_NUM,
                                                      0U,
                                                      2*(USBH3_HC_EHCI_PIPE_NUM-1),
                                                      1024U,
                                                      0U, // padding byte
                                                     &usbh3_ehci_pfl[0],
                                                     &usbh3_ehci_qh[0],
                                                     &usbh3_ehci_qtd[0],
                                                      NULL,
                                                     &usbh3_ehci_sitd[0],
                                                      NULL,
                                                     &usbh3_transfer_info[0],
                                                     &usbh3_pipe_evt_info };
         USBH_EHCI_t         *usbh3_ehci_ptr =       &usbh3_ehci;
#endif


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
#else
extern const osThreadDef_t os_thread_def_USBH3_Core_Thread;
osThreadDef(USBH3_Core_Thread, USBH3_CORE_THREAD_PRIORITY, 1U, USBH3_CORE_THREAD_STACK_SIZE);
#endif
