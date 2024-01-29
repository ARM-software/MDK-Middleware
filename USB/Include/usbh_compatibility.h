/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_compatibility.h
 * Purpose: USB Host (USBH) - Compatibility definitions
 *----------------------------------------------------------------------------*/

#ifndef __USBH_COMPATIBILITY__
#define __USBH_COMPATIBILITY__

/*------------------------------------------------------------------------------
 *      USB Host 0..3 compatibility defines
 *----------------------------------------------------------------------------*/

#ifdef  RTE_USB_Host_0
#ifndef USBH0_HC_PIPE_NUM
#define USBH0_HC_PIPE_NUM                      (USBH0_HC_ENDPOINT_NUM)
#endif
#ifndef USBH0_HC_OHCI_PIPE_NUM
#define USBH0_HC_OHCI_PIPE_NUM                 (USBH0_HC_OHCI_ENDPOINT_NUM)
#endif
#ifndef USBH0_HC_EHCI_PIPE_NUM
#define USBH0_HC_EHCI_PIPE_NUM                 (USBH0_HC_EHCI_ENDPOINT_NUM)
#endif

#ifndef USBH0_HC_POWER
#define USBH0_HC_POWER                         (500U)
#endif
#ifndef USBH0_HC_OHCI_POWER
#define USBH0_HC_OHCI_POWER                    (500U)
#endif
#ifndef USBH0_HC_EHCI_POWER
#define USBH0_HC_EHCI_POWER                    (500U)
#endif

#ifndef USBH0_CORE_THREAD_STACK_SIZE
#define USBH0_CORE_THREAD_STACK_SIZE           (0U)
#endif
#ifndef USBH0_CORE_THREAD_PRIORITY
#define USBH0_CORE_THREAD_PRIORITY             (osPriorityAboveNormal)
#endif
#endif

#ifdef  RTE_USB_Host_1
#ifndef USBH1_HC_PIPE_NUM
#define USBH1_HC_PIPE_NUM                      (USBH1_HC_ENDPOINT_NUM)
#endif
#ifndef USBH1_HC_OHCI_PIPE_NUM
#define USBH1_HC_OHCI_PIPE_NUM                 (USBH1_HC_OHCI_ENDPOINT_NUM)
#endif
#ifndef USBH1_HC_EHCI_PIPE_NUM
#define USBH1_HC_EHCI_PIPE_NUM                 (USBH1_HC_EHCI_ENDPOINT_NUM)
#endif

#ifndef USBH1_HC_POWER
#define USBH1_HC_POWER                         (500U)
#endif
#ifndef USBH1_HC_OHCI_POWER
#define USBH1_HC_OHCI_POWER                    (500U)
#endif
#ifndef USBH1_HC_EHCI_POWER
#define USBH1_HC_EHCI_POWER                    (500U)
#endif

#ifndef USBH1_CORE_THREAD_STACK_SIZE
#define USBH1_CORE_THREAD_STACK_SIZE           (0U)
#endif
#ifndef USBH1_CORE_THREAD_PRIORITY
#define USBH1_CORE_THREAD_PRIORITY             (osPriorityAboveNormal)
#endif
#endif

#ifdef  RTE_USB_Host_2
#ifndef USBH2_HC_PIPE_NUM
#define USBH2_HC_PIPE_NUM                      (USBH2_HC_ENDPOINT_NUM)
#endif
#ifndef USBH2_HC_OHCI_PIPE_NUM
#define USBH2_HC_OHCI_PIPE_NUM                 (USBH2_HC_OHCI_ENDPOINT_NUM)
#endif
#ifndef USBH2_HC_EHCI_PIPE_NUM
#define USBH2_HC_EHCI_PIPE_NUM                 (USBH2_HC_EHCI_ENDPOINT_NUM)
#endif

#ifndef USBH2_HC_POWER
#define USBH2_HC_POWER                         (500U)
#endif
#ifndef USBH2_HC_OHCI_POWER
#define USBH2_HC_OHCI_POWER                    (500U)
#endif
#ifndef USBH2_HC_EHCI_POWER
#define USBH2_HC_EHCI_POWER                    (500U)
#endif

#ifndef USBH2_CORE_THREAD_STACK_SIZE
#define USBH2_CORE_THREAD_STACK_SIZE           (0U)
#endif
#ifndef USBH2_CORE_THREAD_PRIORITY
#define USBH2_CORE_THREAD_PRIORITY             (osPriorityAboveNormal)
#endif
#endif

#ifdef  RTE_USB_Host_3
#ifndef USBH3_HC_PIPE_NUM
#define USBH3_HC_PIPE_NUM                      (USBH3_HC_ENDPOINT_NUM)
#endif
#ifndef USBH3_HC_OHCI_PIPE_NUM
#define USBH3_HC_OHCI_PIPE_NUM                 (USBH3_HC_OHCI_ENDPOINT_NUM)
#endif
#ifndef USBH3_HC_EHCI_PIPE_NUM
#define USBH3_HC_EHCI_PIPE_NUM                 (USBH3_HC_EHCI_ENDPOINT_NUM)
#endif

#ifndef USBH3_HC_POWER
#define USBH3_HC_POWER                         (500U)
#endif
#ifndef USBH3_HC_OHCI_POWER
#define USBH3_HC_OHCI_POWER                    (500U)
#endif
#ifndef USBH3_HC_EHCI_POWER
#define USBH3_HC_EHCI_POWER                    (500U)
#endif

#ifndef USBH3_CORE_THREAD_STACK_SIZE
#define USBH3_CORE_THREAD_STACK_SIZE           (0U)
#endif
#ifndef USBH3_CORE_THREAD_PRIORITY
#define USBH3_CORE_THREAD_PRIORITY             (osPriorityAboveNormal)
#endif
#endif


/*------------------------------------------------------------------------------
 *      USB Host HID compatibility defines
 *----------------------------------------------------------------------------*/

#ifdef  RTE_USB_Host_HID
#ifdef  USBH0_HID_THREAD_STACK_SIZE
#define USBH_HID_INT_IN_THREAD_STACK_SIZE      (USBH0_HID_THREAD_STACK_SIZE)
#endif
#ifndef USBH_HID_INT_IN_THREAD_STACK_SIZE
#define USBH_HID_INT_IN_THREAD_STACK_SIZE      (512U)
#endif
#ifdef  USBH0_HID_THREAD_PRIORITY
#define USBH_HID_INT_IN_THREAD_PRIORITY        (USBH0_HID_THREAD_PRIORITY)
#endif
#ifndef USBH_HID_INT_IN_THREAD_PRIORITY
#define USBH_HID_INT_IN_THREAD_PRIORITY        (osPriorityAboveNormal)
#endif
#endif

#endif // __USBH_COMPATIBILITY__
