/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2021 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_config_def_$x.h
 * Purpose: USB Host (USBH) - Host Controller $x Definitions
 * Note:    this file should only be included when USBH_Config_$x.c is being 
 *          compiled as it depends on definitions from USBH_Config_$x.c
 *----------------------------------------------------------------------------*/

#ifndef __USBH_CONFIG_DEF_$x_H__
#define __USBH_CONFIG_DEF_$x_H__


// Check settings for minimum requirements

#if   (((USBH$x_HC_IF == 0) && (USBH$x_HC_MEM_POOL_SIZE      < 512)) || \
       ((USBH$x_HC_IF == 1) && (USBH$x_HC_OHCI_MEM_POOL_SIZE < 512)) || \
       ((USBH$x_HC_IF == 2) && (USBH$x_HC_EHCI_MEM_POOL_SIZE < 512))  )
#warning Memory Pool Size for Host Controller $x not recommended less than 512 bytes!
#endif


// Define macros to size and position memory used by Host Controller

#if   (((USBH$x_HC_IF == 0) && (USBH$x_HC_MEM_POOL_RELOC      == 1)) || \
       ((USBH$x_HC_IF == 1) && (USBH$x_HC_OHCI_MEM_POOL_RELOC == 1)) || \
       ((USBH$x_HC_IF == 2) && (USBH$x_HC_EHCI_MEM_POOL_RELOC == 1))  )
#if      defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#define  __USBH$x_AT_MEM__(x)          __attribute__((section(".ARM.__AT_"#x)))
#define  __USBH$x_AT_MEM(x)            __USBH$x_AT_MEM__(x)
#define  __USBH$x_SECTION_MEM(x)
#else
#define  __USBH$x_AT_MEM(x)            __attribute__((at(x)))
#define  __USBH$x_SECTION_MEM(x)
#endif
#else
#define  __USBH$x_AT_MEM(x)
#define  __USBH$x_SECTION_MEM(x)       __attribute__((section(#x)))
#endif
#define  __USBH$x_ALIGNED_MEM(x)       __attribute__((aligned(x)))

#if     (USBH$x_HC_IF == 1)
#define  USBH$x_OHCI_MEM_HCCA           (USBH$x_HC_OHCI_MEM_POOL_ADDR)
#define  USBH$x_OHCI_MEM_HCCA_SIZE      (256U)
#define  USBH$x_OHCI_MEM_ED             (USBH$x_OHCI_MEM_HCCA + USBH$x_OHCI_MEM_HCCA_SIZE)
#define  USBH$x_OHCI_MEM_ED_SIZE        (USBH$x_HC_OHCI_PIPE_NUM * 16U)
#define  USBH$x_OHCI_MEM_TD             (USBH$x_OHCI_MEM_ED   + USBH$x_OHCI_MEM_ED_SIZE)
#define  USBH$x_OHCI_MEM_TD_SIZE        (USBH$x_HC_OHCI_PIPE_NUM * 16U)
#define  USBH$x_OHCI_MEM_POOL_ADDR      (USBH$x_OHCI_MEM_HCCA + USBH$x_OHCI_MEM_HCCA_SIZE + USBH$x_OHCI_MEM_ED_SIZE + USBH$x_OHCI_MEM_TD_SIZE)
#define  USBH$x_OHCI_MEM_POOL_SIZE      (USBH$x_HC_OHCI_MEM_POOL_SIZE + 8U * USBH$x_HC_PIPE_NUM + 32U + 4U)
#endif

#if     (USBH$x_HC_IF == 2)
#define  USBH$x_EHCI_MEM_PFL            (USBH$x_HC_EHCI_MEM_POOL_ADDR + (((USBH$x_HC_EHCI_MEM_POOL_SIZE + 3U) / 4U) * 4U))
#define  USBH$x_EHCI_MEM_PFL_SIZE       (4096U)
#define  USBH$x_EHCI_MEM_QH             (USBH$x_EHCI_MEM_PFL + USBH$x_EHCI_MEM_PFL_SIZE)
#define  USBH$x_EHCI_MEM_QH_SIZE        (USBH$x_HC_EHCI_PIPE_NUM * 64U)
#define  USBH$x_EHCI_MEM_QTD            (USBH$x_EHCI_MEM_QH  + USBH$x_EHCI_MEM_QH_SIZE)
#define  USBH$x_EHCI_MEM_QTD_SIZE       (USBH$x_HC_EHCI_PIPE_NUM * 32U)
#define  USBH$x_EHCI_MEM_SITD           (USBH$x_EHCI_MEM_QTD + USBH$x_EHCI_MEM_QTD_SIZE)
#define  USBH$x_EHCI_MEM_SITD_SIZE      (2U * (USBH$x_HC_EHCI_PIPE_NUM - 1) * 32U)
#define  USBH$x_EHCI_MEM_POOL_ADDR      (USBH$x_EHCI_MEM_PFL + USBH$x_EHCI_MEM_PFL_SIZE + USBH$x_EHCI_MEM_QH_SIZE + USBH$x_EHCI_MEM_QTD_SIZE)
#define  USBH$x_EHCI_MEM_POOL_SIZE      (USBH$x_HC_EHCI_MEM_POOL_SIZE + 8U * USBH$x_HC_PIPE_NUM + 32U + 4U)
#endif

#endif
