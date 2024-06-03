/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_section.h
 * Purpose: USB Device (USBD) - Section definitions
 *----------------------------------------------------------------------------*/

#ifndef  USBD_SECTION_
#define  USBD_SECTION_

/*------------------------------------------------------------------------------
 *      USB Device 0..3 section defines
 *----------------------------------------------------------------------------*/

#define USBDn_DEV_BUF_MEM_SECTION_(str) __attribute__((section(str)))

#ifdef  USBD0_PORT
#if    (USBD0_BUF_MEM_RELOC == 1)
#define USBD0_DEV_BUF_MEM_SECTION       USBDn_DEV_BUF_MEM_SECTION_(USBD0_BUF_MEM_SECTION_NAME)
#else 
#define USBD0_DEV_BUF_MEM_SECTION
#endif
#endif

#ifdef  USBD1_PORT
#if    (USBD1_BUF_MEM_RELOC == 1)
#define USBD1_DEV_BUF_MEM_SECTION       USBDn_DEV_BUF_MEM_SECTION_(USBD1_BUF_MEM_SECTION_NAME)
#else 
#define USBD1_DEV_BUF_MEM_SECTION
#endif
#endif

#ifdef  USBD2_PORT
#if    (USBD2_BUF_MEM_RELOC == 1)
#define USBD2_DEV_BUF_MEM_SECTION       USBDn_DEV_BUF_MEM_SECTION_(USBD2_BUF_MEM_SECTION_NAME)
#else 
#define USBD2_DEV_BUF_MEM_SECTION
#endif
#endif

#ifdef  USBD3_PORT
#if    (USBD3_BUF_MEM_RELOC == 1)
#define USBD3_DEV_BUF_MEM_SECTION       USBDn_DEV_BUF_MEM_SECTION_(USBD3_BUF_MEM_SECTION_NAME)
#else 
#define USBD3_DEV_BUF_MEM_SECTION
#endif
#endif

#define USBDn_BUF_MEM_SECTION__(dev)    USBD##dev##_DEV_BUF_MEM_SECTION
#define USBDn_BUF_MEM_SECTION_(dev)     USBDn_BUF_MEM_SECTION__(dev)

/*------------------------------------------------------------------------------
 *      USB Device ADC 0..3 section defines
 *----------------------------------------------------------------------------*/

#define USBD_ADCn_BUF_MEM_SECTION(n)    USBDn_BUF_MEM_SECTION_(USBD_ADC##n##_DEV)

#ifdef  RTE_USB_Device_ADC_0
#define USBD_ADC0_BUF_MEM_SECTION       USBD_ADCn_BUF_MEM_SECTION(0)
#endif
#ifdef  RTE_USB_Device_ADC_1
#define USBD_ADC1_BUF_MEM_SECTION       USBD_ADCn_BUF_MEM_SECTION(1)
#endif
#ifdef  RTE_USB_Device_ADC_2
#define USBD_ADC2_BUF_MEM_SECTION       USBD_ADCn_BUF_MEM_SECTION(2)
#endif
#ifdef  RTE_USB_Device_ADC_3
#define USBD_ADC3_BUF_MEM_SECTION       USBD_ADCn_BUF_MEM_SECTION(3)
#endif

/*------------------------------------------------------------------------------
 *      USB Device CDC 0..7 section defines
 *----------------------------------------------------------------------------*/

#define USBD_CDCn_BUF_MEM_SECTION(n)    USBDn_BUF_MEM_SECTION_(USBD_CDC##n##_DEV)

#ifdef  RTE_USB_Device_CDC_0
#define USBD_CDC0_BUF_MEM_SECTION       USBD_CDCn_BUF_MEM_SECTION(0)
#endif
#ifdef  RTE_USB_Device_CDC_1
#define USBD_CDC1_BUF_MEM_SECTION       USBD_CDCn_BUF_MEM_SECTION(1)
#endif
#ifdef  RTE_USB_Device_CDC_2
#define USBD_CDC2_BUF_MEM_SECTION       USBD_CDCn_BUF_MEM_SECTION(2)
#endif
#ifdef  RTE_USB_Device_CDC_3
#define USBD_CDC3_BUF_MEM_SECTION       USBD_CDCn_BUF_MEM_SECTION(3)
#endif
#ifdef  RTE_USB_Device_CDC_4
#define USBD_CDC4_BUF_MEM_SECTION       USBD_CDCn_BUF_MEM_SECTION(4)
#endif
#ifdef  RTE_USB_Device_CDC_5
#define USBD_CDC5_BUF_MEM_SECTION       USBD_CDCn_BUF_MEM_SECTION(5)
#endif
#ifdef  RTE_USB_Device_CDC_6
#define USBD_CDC6_BUF_MEM_SECTION       USBD_CDCn_BUF_MEM_SECTION(6)
#endif
#ifdef  RTE_USB_Device_CDC_7
#define USBD_CDC7_BUF_MEM_SECTION       USBD_CDCn_BUF_MEM_SECTION(7)
#endif

/*------------------------------------------------------------------------------
 *      USB Device HID 0..7 section defines
 *----------------------------------------------------------------------------*/

#define USBD_HIDn_BUF_MEM_SECTION(n)    USBDn_BUF_MEM_SECTION_(USBD_HID##n##_DEV)

#ifdef  RTE_USB_Device_HID_0
#define USBD_HID0_BUF_MEM_SECTION       USBD_HIDn_BUF_MEM_SECTION(0)
#endif
#ifdef  RTE_USB_Device_HID_1
#define USBD_HID1_BUF_MEM_SECTION       USBD_HIDn_BUF_MEM_SECTION(1)
#endif
#ifdef  RTE_USB_Device_HID_2
#define USBD_HID2_BUF_MEM_SECTION       USBD_HIDn_BUF_MEM_SECTION(2)
#endif
#ifdef  RTE_USB_Device_HID_3
#define USBD_HID3_BUF_MEM_SECTION       USBD_HIDn_BUF_MEM_SECTION(3)
#endif
#ifdef  RTE_USB_Device_HID_4
#define USBD_HID4_BUF_MEM_SECTION       USBD_HIDn_BUF_MEM_SECTION(4)
#endif
#ifdef  RTE_USB_Device_HID_5
#define USBD_HID5_BUF_MEM_SECTION       USBD_HIDn_BUF_MEM_SECTION(5)
#endif
#ifdef  RTE_USB_Device_HID_6
#define USBD_HID6_BUF_MEM_SECTION       USBD_HIDn_BUF_MEM_SECTION(6)
#endif
#ifdef  RTE_USB_Device_HID_7
#define USBD_HID7_BUF_MEM_SECTION       USBD_HIDn_BUF_MEM_SECTION(7)
#endif

/*------------------------------------------------------------------------------
 *      USB Device MSC 0..3 section defines
 *----------------------------------------------------------------------------*/

#define USBD_MSCn_BUF_MEM_SECTION(n)    USBDn_BUF_MEM_SECTION_(USBD_MSC##n##_DEV)

#ifdef  RTE_USB_Device_MSC_0
#define USBD_MSC0_BUF_MEM_SECTION       USBD_MSCn_BUF_MEM_SECTION(0)
#endif
#ifdef  RTE_USB_Device_MSC_1
#define USBD_MSC1_BUF_MEM_SECTION       USBD_MSCn_BUF_MEM_SECTION(1)
#endif
#ifdef  RTE_USB_Device_MSC_2
#define USBD_MSC2_BUF_MEM_SECTION       USBD_MSCn_BUF_MEM_SECTION(2)
#endif
#ifdef  RTE_USB_Device_MSC_3
#define USBD_MSC3_BUF_MEM_SECTION       USBD_MSCn_BUF_MEM_SECTION(3)
#endif

#endif  // USBD_SECTION_
