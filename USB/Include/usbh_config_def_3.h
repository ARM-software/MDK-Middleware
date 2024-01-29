/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2021 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_config_def_3.h
 * Purpose: USB Host (USBH) - Host Controller 3 Definitions
 * Note:    this file should only be included when USBH_Config_3.c is being 
 *          compiled as it depends on definitions from USBH_Config_3.c
 *----------------------------------------------------------------------------*/

#ifndef __USBH_CONFIG_DEF_3_H__
#define __USBH_CONFIG_DEF_3_H__


// Check settings for minimum requirements

#if   (((USBH3_HC_IF == 0) && (USBH3_HC_MEM_POOL_SIZE      < 512)) || \
       ((USBH3_HC_IF == 1) && (USBH3_HC_OHCI_MEM_POOL_SIZE < 512)) || \
       ((USBH3_HC_IF == 2) && (USBH3_HC_EHCI_MEM_POOL_SIZE < 512))  )
#warning Memory Pool Size for Host Controller 3 not recommended less than 512 bytes!
#endif


// Define macros to size and position memory used by Host Controller

#if   (((USBH3_HC_IF == 0) && (USBH3_HC_MEM_POOL_RELOC      == 1)) || \
       ((USBH3_HC_IF == 1) && (USBH3_HC_OHCI_MEM_POOL_RELOC == 1)) || \
       ((USBH3_HC_IF == 2) && (USBH3_HC_EHCI_MEM_POOL_RELOC == 1))  )
#if      defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#define  __USBH3_AT_MEM__(x)          __attribute__((section(".ARM.__AT_"#x)))
#define  __USBH3_AT_MEM(x)            __USBH3_AT_MEM__(x)
#define  __USBH3_SECTION_MEM(x)
#else
#define  __USBH3_AT_MEM(x)            __attribute__((at(x)))
#define  __USBH3_SECTION_MEM(x)
#endif
#else
#define  __USBH3_AT_MEM(x)
#define  __USBH3_SECTION_MEM(x)       __attribute__((section(#x)))
#endif
#define  __USBH3_ALIGNED_MEM(x)       __attribute__((aligned(x)))

#if     (USBH3_HC_IF == 1)
#define  USBH3_OHCI_MEM_HCCA           (USBH3_HC_OHCI_MEM_POOL_ADDR)
#define  USBH3_OHCI_MEM_HCCA_SIZE      (256U)
#define  USBH3_OHCI_MEM_ED             (USBH3_OHCI_MEM_HCCA + USBH3_OHCI_MEM_HCCA_SIZE)
#define  USBH3_OHCI_MEM_ED_SIZE        (USBH3_HC_OHCI_PIPE_NUM * 16U)
#define  USBH3_OHCI_MEM_TD             (USBH3_OHCI_MEM_ED   + USBH3_OHCI_MEM_ED_SIZE)
#define  USBH3_OHCI_MEM_TD_SIZE        (USBH3_HC_OHCI_PIPE_NUM * 16U)
#define  USBH3_OHCI_MEM_POOL_ADDR      (USBH3_OHCI_MEM_HCCA + USBH3_OHCI_MEM_HCCA_SIZE + USBH3_OHCI_MEM_ED_SIZE + USBH3_OHCI_MEM_TD_SIZE)
#define  USBH3_OHCI_MEM_POOL_SIZE      (USBH3_HC_OHCI_MEM_POOL_SIZE + 8U * USBH3_HC_PIPE_NUM + 32U + 4U)
#endif

#if     (USBH3_HC_IF == 2)
#define  USBH3_EHCI_MEM_PFL            (USBH3_HC_EHCI_MEM_POOL_ADDR + (((USBH3_HC_EHCI_MEM_POOL_SIZE + 3U) / 4U) * 4U))
#define  USBH3_EHCI_MEM_PFL_SIZE       (4096U)
#define  USBH3_EHCI_MEM_QH             (USBH3_EHCI_MEM_PFL + USBH3_EHCI_MEM_PFL_SIZE)
#define  USBH3_EHCI_MEM_QH_SIZE        (USBH3_HC_EHCI_PIPE_NUM * 64U)
#define  USBH3_EHCI_MEM_QTD            (USBH3_EHCI_MEM_QH  + USBH3_EHCI_MEM_QH_SIZE)
#define  USBH3_EHCI_MEM_QTD_SIZE       (USBH3_HC_EHCI_PIPE_NUM * 32U)
#define  USBH3_EHCI_MEM_SITD           (USBH3_EHCI_MEM_QTD + USBH3_EHCI_MEM_QTD_SIZE)
#define  USBH3_EHCI_MEM_SITD_SIZE      (2U * (USBH3_HC_EHCI_PIPE_NUM - 1) * 32U)
#define  USBH3_EHCI_MEM_POOL_ADDR      (USBH3_EHCI_MEM_PFL + USBH3_EHCI_MEM_PFL_SIZE + USBH3_EHCI_MEM_QH_SIZE + USBH3_EHCI_MEM_QTD_SIZE)
#define  USBH3_EHCI_MEM_POOL_SIZE      (USBH3_HC_EHCI_MEM_POOL_SIZE + 8U * USBH3_HC_PIPE_NUM + 32U + 4U)
#endif

#endif
