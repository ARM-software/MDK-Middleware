/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_config.c
 * Purpose: USB Devices (0..3) Configuration
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "RTE_Components.h"

#ifdef    RTE_USB_Device_0
#include "USBD_Config_0.h"
#endif

#ifdef    RTE_USB_Device_1
#include "USBD_Config_1.h"
#endif

#ifdef    RTE_USB_Device_2
#include "USBD_Config_2.h"
#endif

#ifdef    RTE_USB_Device_3
#include "USBD_Config_3.h"
#endif

#if      (defined(RTE_USB_Device_CustomClass_0) && !defined(USBD_CUSTOM_CLASS0_DEV))
#include "USBD_Config_CustomClass_0.h"
#endif
#if      (defined(RTE_USB_Device_CustomClass_1) && !defined(USBD_CUSTOM_CLASS1_DEV))
#include "USBD_Config_CustomClass_1.h"
#endif
#if      (defined(RTE_USB_Device_CustomClass_2) && !defined(USBD_CUSTOM_CLASS2_DEV))
#include "USBD_Config_CustomClass_2.h"
#endif
#if      (defined(RTE_USB_Device_CustomClass_3) && !defined(USBD_CUSTOM_CLASS3_DEV))
#include "USBD_Config_CustomClass_3.h"
#endif

#if      (defined(RTE_USB_Device_HID_0) && !defined(USBD_HID0_DEV))
#include "USBD_Config_HID_0.h"
#endif
#if      (defined(RTE_USB_Device_HID_1) && !defined(USBD_HID1_DEV))
#include "USBD_Config_HID_1.h"
#endif
#if      (defined(RTE_USB_Device_HID_2) && !defined(USBD_HID2_DEV))
#include "USBD_Config_HID_2.h"
#endif
#if      (defined(RTE_USB_Device_HID_3) && !defined(USBD_HID3_DEV))
#include "USBD_Config_HID_3.h"
#endif
#if      (defined(RTE_USB_Device_HID_4) && !defined(USBD_HID4_DEV))
#include "USBD_Config_HID_4.h"
#endif
#if      (defined(RTE_USB_Device_HID_5) && !defined(USBD_HID5_DEV))
#include "USBD_Config_HID_5.h"
#endif
#if      (defined(RTE_USB_Device_HID_6) && !defined(USBD_HID6_DEV))
#include "USBD_Config_HID_6.h"
#endif
#if      (defined(RTE_USB_Device_HID_7) && !defined(USBD_HID7_DEV))
#include "USBD_Config_HID_7.h"
#endif

#if      (defined(RTE_USB_Device_MSC_0) && !defined(USBD_MSC0_DEV))
#include "USBD_Config_MSC_0.h"
#endif
#if      (defined(RTE_USB_Device_MSC_1) && !defined(USBD_MSC1_DEV))
#include "USBD_Config_MSC_1.h"
#endif
#if      (defined(RTE_USB_Device_MSC_2) && !defined(USBD_MSC2_DEV))
#include "USBD_Config_MSC_2.h"
#endif
#if      (defined(RTE_USB_Device_MSC_3) && !defined(USBD_MSC3_DEV))
#include "USBD_Config_MSC_3.h"
#endif

#if      (defined(RTE_USB_Device_CDC_0) && !defined(USBD_CDC0_DEV))
#include "USBD_Config_CDC_0.h"
#endif
#if      (defined(RTE_USB_Device_CDC_1) && !defined(USBD_CDC1_DEV))
#include "USBD_Config_CDC_1.h"
#endif
#if      (defined(RTE_USB_Device_CDC_2) && !defined(USBD_CDC2_DEV))
#include "USBD_Config_CDC_2.h"
#endif
#if      (defined(RTE_USB_Device_CDC_3) && !defined(USBD_CDC3_DEV))
#include "USBD_Config_CDC_3.h"
#endif
#if      (defined(RTE_USB_Device_CDC_4) && !defined(USBD_CDC4_DEV))
#include "USBD_Config_CDC_4.h"
#endif
#if      (defined(RTE_USB_Device_CDC_5) && !defined(USBD_CDC5_DEV))
#include "USBD_Config_CDC_5.h"
#endif
#if      (defined(RTE_USB_Device_CDC_6) && !defined(USBD_CDC6_DEV))
#include "USBD_Config_CDC_6.h"
#endif
#if      (defined(RTE_USB_Device_CDC_7) && !defined(USBD_CDC7_DEV))
#include "USBD_Config_CDC_7.h"
#endif

#if      (defined(RTE_USB_Device_ADC_0) && !defined(USBD_ADC0_DEV))
#include "USBD_Config_ADC_0.h"
#endif
#if      (defined(RTE_USB_Device_ADC_1) && !defined(USBD_ADC1_DEV))
#include "USBD_Config_ADC_1.h"
#endif
#if      (defined(RTE_USB_Device_ADC_2) && !defined(USBD_ADC2_DEV))
#include "USBD_Config_ADC_2.h"
#endif
#if      (defined(RTE_USB_Device_ADC_3) && !defined(USBD_ADC3_DEV))
#include "USBD_Config_ADC_3.h"
#endif

#include "rl_usb.h"
#include "usbd_lib.h"

#include "usbd_compatibility.h"

/*------------------------------------------------------------------------------
 *      USB Device Global Macros
 *----------------------------------------------------------------------------*/

#define MAX_OF_2(a,b)                   ((a)>(b)?(a):(b))
#define RUP32(a)                        ((((a)+31)/32)*32)

#define USBDn_DRIVER_(n)                Driver_USBD##n
#define USBDn_DRIVER(n)                 USBDn_DRIVER_(n)


/*------------------------------------------------------------------------------
 *      USB Device 0 Macros, Calculations and Constants for Library
 *----------------------------------------------------------------------------*/

#ifndef USBD0_MACROS
#define USBD0_MACROS                    1
#include "usbd_config_def_0.h"
#endif

#ifdef  USBD0_PORT
#include "usbd_config_dev_0.c"
#endif


/*------------------------------------------------------------------------------
 *      USB Device 1 Macros, Calculations and Constants for Library
 *----------------------------------------------------------------------------*/

#ifndef USBD1_MACROS
#define USBD1_MACROS                    1
#include "usbd_config_def_1.h"
#endif

#ifdef  USBD1_PORT
#include "usbd_config_dev_1.c"
#endif


/*------------------------------------------------------------------------------
 *      USB Device 2 Macros, Calculations and Constants for Library
 *----------------------------------------------------------------------------*/

#ifndef USBD2_MACROS
#define USBD2_MACROS                    1
#include "usbd_config_def_2.h"
#endif

#ifdef  USBD2_PORT
#include "usbd_config_dev_2.c"
#endif


/*------------------------------------------------------------------------------
 *      USB Device 3 Macros, Calculations and Constants for Library
 *----------------------------------------------------------------------------*/

#ifndef USBD3_MACROS
#define USBD3_MACROS                    1
#include "usbd_config_def_3.h"
#endif

#ifdef  USBD3_PORT
#include "usbd_config_dev_3.c"
#endif


/*------------------------------------------------------------------------------
 *      USB Device Global Macros
 *----------------------------------------------------------------------------*/

#define USBD_EP_LAST_NUM                USBD0_EP_MAX
#if    (USBD1_EP_MAX > USBD_EP_LAST_NUM)
#undef  USBD_EP_LAST_NUM
#define USBD_EP_LAST_NUM                USBD1_EP_MAX
#endif
#if    (USBD2_EP_MAX > USBD_EP_LAST_NUM)
#undef  USBD_EP_LAST_NUM
#define USBD_EP_LAST_NUM                USBD2_EP_MAX
#endif
#if    (USBD3_EP_MAX > USBD_EP_LAST_NUM)
#undef  USBD_EP_LAST_NUM
#define USBD_EP_LAST_NUM                USBD3_EP_MAX
#endif

#define USBD_EP_NUM                    (USBD_EP_LAST_NUM + 1)

#ifdef  RTE_USB_Device_0
#define USBD0_DEV_NUM                   1
#else
#define USBD0_DEV_NUM                   0
#endif
#ifdef  RTE_USB_Device_1
#define USBD1_DEV_NUM                   1
#else
#define USBD1_DEV_NUM                   0
#endif
#ifdef  RTE_USB_Device_2
#define USBD2_DEV_NUM                   1
#else
#define USBD2_DEV_NUM                   0
#endif
#ifdef  RTE_USB_Device_3
#define USBD3_DEV_NUM                   1
#else
#define USBD3_DEV_NUM                   0
#endif

#define USBD_DEV_NUM                   (USBD0_DEV_NUM + USBD1_DEV_NUM + USBD2_DEV_NUM + USBD3_DEV_NUM)

#ifndef USBD0_EP1
#define USBD0_EP1       0
#define USBD0_EP1_EN    0
#else
#define USBD0_EP1_EN    1
#endif
#ifndef USBD0_EP2
#define USBD0_EP2       0
#define USBD0_EP2_EN    0
#else
#define USBD0_EP2_EN    1
#endif
#ifndef USBD0_EP3
#define USBD0_EP3       0
#define USBD0_EP3_EN    0
#else
#define USBD0_EP3_EN    1
#endif
#ifndef USBD0_EP4
#define USBD0_EP4       0
#define USBD0_EP4_EN    0
#else
#define USBD0_EP4_EN    1
#endif
#ifndef USBD0_EP5
#define USBD0_EP5       0
#define USBD0_EP5_EN    0
#else
#define USBD0_EP5_EN    1
#endif
#ifndef USBD0_EP6
#define USBD0_EP6       0
#define USBD0_EP6_EN    0
#else
#define USBD0_EP6_EN    1
#endif
#ifndef USBD0_EP7
#define USBD0_EP7       0
#define USBD0_EP7_EN    0
#else
#define USBD0_EP7_EN    1
#endif
#ifndef USBD0_EP8
#define USBD0_EP8       0
#define USBD0_EP8_EN    0
#else
#define USBD0_EP8_EN    1
#endif
#ifndef USBD0_EP9
#define USBD0_EP9       0
#define USBD0_EP9_EN    0
#else
#define USBD0_EP9_EN    1
#endif
#ifndef USBD0_EP10
#define USBD0_EP10      0
#define USBD0_EP10_EN   0
#else
#define USBD0_EP10_EN   1
#endif
#ifndef USBD0_EP11
#define USBD0_EP11      0
#define USBD0_EP11_EN   0
#else
#define USBD0_EP11_EN   1
#endif
#ifndef USBD0_EP12
#define USBD0_EP12      0
#define USBD0_EP12_EN   0
#else
#define USBD0_EP12_EN   1
#endif
#ifndef USBD0_EP13
#define USBD0_EP13      0
#define USBD0_EP13_EN   0
#else
#define USBD0_EP13_EN   1
#endif
#ifndef USBD0_EP14
#define USBD0_EP14      0
#define USBD0_EP14_EN   0
#else
#define USBD0_EP14_EN   1
#endif
#ifndef USBD0_EP15
#define USBD0_EP15      0
#define USBD0_EP15_EN   0
#else
#define USBD0_EP15_EN   1
#endif

#ifndef USBD1_EP1
#define USBD1_EP1       0
#define USBD1_EP1_EN    0
#else
#define USBD1_EP1_EN    1
#endif
#ifndef USBD1_EP2
#define USBD1_EP2       0
#define USBD1_EP2_EN    0
#else
#define USBD1_EP2_EN    1
#endif
#ifndef USBD1_EP3
#define USBD1_EP3       0
#define USBD1_EP3_EN    0
#else
#define USBD1_EP3_EN    1
#endif
#ifndef USBD1_EP4
#define USBD1_EP4       0
#define USBD1_EP4_EN    0
#else
#define USBD1_EP4_EN    1
#endif
#ifndef USBD1_EP5
#define USBD1_EP5       0
#define USBD1_EP5_EN    0
#else
#define USBD1_EP5_EN    1
#endif
#ifndef USBD1_EP6
#define USBD1_EP6       0
#define USBD1_EP6_EN    0
#else
#define USBD1_EP6_EN    1
#endif
#ifndef USBD1_EP7
#define USBD1_EP7       0
#define USBD1_EP7_EN    0
#else
#define USBD1_EP7_EN    1
#endif
#ifndef USBD1_EP8
#define USBD1_EP8       0
#define USBD1_EP8_EN    0
#else
#define USBD1_EP8_EN    1
#endif
#ifndef USBD1_EP9
#define USBD1_EP9       0
#define USBD1_EP9_EN    0
#else
#define USBD1_EP9_EN    1
#endif
#ifndef USBD1_EP10
#define USBD1_EP10      0
#define USBD1_EP10_EN   0
#else
#define USBD1_EP10_EN   1
#endif
#ifndef USBD1_EP11
#define USBD1_EP11      0
#define USBD1_EP11_EN   0
#else
#define USBD1_EP11_EN   1
#endif
#ifndef USBD1_EP12
#define USBD1_EP12      0
#define USBD1_EP12_EN   0
#else
#define USBD1_EP12_EN   1
#endif
#ifndef USBD1_EP13
#define USBD1_EP13      0
#define USBD1_EP13_EN   0
#else
#define USBD1_EP13_EN   1
#endif
#ifndef USBD1_EP14
#define USBD1_EP14      0
#define USBD1_EP14_EN   0
#else
#define USBD1_EP14_EN   1
#endif
#ifndef USBD1_EP15
#define USBD1_EP15      0
#define USBD1_EP15_EN   0
#else
#define USBD1_EP15_EN   1
#endif

#ifndef USBD2_EP1
#define USBD2_EP1       0
#define USBD2_EP1_EN    0
#else
#define USBD2_EP1_EN    1
#endif
#ifndef USBD2_EP2
#define USBD2_EP2       0
#define USBD2_EP2_EN    0
#else
#define USBD2_EP2_EN    1
#endif
#ifndef USBD2_EP3
#define USBD2_EP3       0
#define USBD2_EP3_EN    0
#else
#define USBD2_EP3_EN    1
#endif
#ifndef USBD2_EP4
#define USBD2_EP4       0
#define USBD2_EP4_EN    0
#else
#define USBD2_EP4_EN    1
#endif
#ifndef USBD2_EP5
#define USBD2_EP5       0
#define USBD2_EP5_EN    0
#else
#define USBD2_EP5_EN    1
#endif
#ifndef USBD2_EP6
#define USBD2_EP6       0
#define USBD2_EP6_EN    0
#else
#define USBD2_EP6_EN    1
#endif
#ifndef USBD2_EP7
#define USBD2_EP7       0
#define USBD2_EP7_EN    0
#else
#define USBD2_EP7_EN    1
#endif
#ifndef USBD2_EP8
#define USBD2_EP8       0
#define USBD2_EP8_EN    0
#else
#define USBD2_EP8_EN    1
#endif
#ifndef USBD2_EP9
#define USBD2_EP9       0
#define USBD2_EP9_EN    0
#else
#define USBD2_EP9_EN    1
#endif
#ifndef USBD2_EP10
#define USBD2_EP10      0
#define USBD2_EP10_EN   0
#else
#define USBD2_EP10_EN   1
#endif
#ifndef USBD2_EP11
#define USBD2_EP11      0
#define USBD2_EP11_EN   0
#else
#define USBD2_EP11_EN   1
#endif
#ifndef USBD2_EP12
#define USBD2_EP12      0
#define USBD2_EP12_EN   0
#else
#define USBD2_EP12_EN   1
#endif
#ifndef USBD2_EP13
#define USBD2_EP13      0
#define USBD2_EP13_EN   0
#else
#define USBD2_EP13_EN   1
#endif
#ifndef USBD2_EP14
#define USBD2_EP14      0
#define USBD2_EP14_EN   0
#else
#define USBD2_EP14_EN   1
#endif
#ifndef USBD2_EP15
#define USBD2_EP15      0
#define USBD2_EP15_EN   0
#else
#define USBD2_EP15_EN   1
#endif

#ifndef USBD3_EP1
#define USBD3_EP1       0
#define USBD3_EP1_EN    0
#else
#define USBD3_EP1_EN    1
#endif
#ifndef USBD3_EP2
#define USBD3_EP2       0
#define USBD3_EP2_EN    0
#else
#define USBD3_EP2_EN    1
#endif
#ifndef USBD3_EP3
#define USBD3_EP3       0
#define USBD3_EP3_EN    0
#else
#define USBD3_EP3_EN    1
#endif
#ifndef USBD3_EP4
#define USBD3_EP4       0
#define USBD3_EP4_EN    0
#else
#define USBD3_EP4_EN    1
#endif
#ifndef USBD3_EP5
#define USBD3_EP5       0
#define USBD3_EP5_EN    0
#else
#define USBD3_EP5_EN    1
#endif
#ifndef USBD3_EP6
#define USBD3_EP6       0
#define USBD3_EP6_EN    0
#else
#define USBD3_EP6_EN    1
#endif
#ifndef USBD3_EP7
#define USBD3_EP7       0
#define USBD3_EP7_EN    0
#else
#define USBD3_EP7_EN    1
#endif
#ifndef USBD3_EP8
#define USBD3_EP8       0
#define USBD3_EP8_EN    0
#else
#define USBD3_EP8_EN    1
#endif
#ifndef USBD3_EP9
#define USBD3_EP9       0
#define USBD3_EP9_EN    0
#else
#define USBD3_EP9_EN    1
#endif
#ifndef USBD3_EP10
#define USBD3_EP10      0
#define USBD3_EP10_EN   0
#else
#define USBD3_EP10_EN   1
#endif
#ifndef USBD3_EP11
#define USBD3_EP11      0
#define USBD3_EP11_EN   0
#else
#define USBD3_EP11_EN   1
#endif
#ifndef USBD3_EP12
#define USBD3_EP12      0
#define USBD3_EP12_EN   0
#else
#define USBD3_EP12_EN   1
#endif
#ifndef USBD3_EP13
#define USBD3_EP13      0
#define USBD3_EP13_EN   0
#else
#define USBD3_EP13_EN   1
#endif
#ifndef USBD3_EP14
#define USBD3_EP14      0
#define USBD3_EP14_EN   0
#else
#define USBD3_EP14_EN   1
#endif
#ifndef USBD3_EP15
#define USBD3_EP15      0
#define USBD3_EP15_EN   0
#else
#define USBD3_EP15_EN   1
#endif


#if    (USBD_EP_LAST_NUM == 15)
#define EP_POINTERS(n)  (void *)&usbd_core_thread_id[n], \
                        (void *)USBD##n##_EP1 ,          \
                        (void *)USBD##n##_EP2 ,          \
                        (void *)USBD##n##_EP3 ,          \
                        (void *)USBD##n##_EP4 ,          \
                        (void *)USBD##n##_EP5 ,          \
                        (void *)USBD##n##_EP6 ,          \
                        (void *)USBD##n##_EP7 ,          \
                        (void *)USBD##n##_EP8 ,          \
                        (void *)USBD##n##_EP9 ,          \
                        (void *)USBD##n##_EP10,          \
                        (void *)USBD##n##_EP11,          \
                        (void *)USBD##n##_EP12,          \
                        (void *)USBD##n##_EP13,          \
                        (void *)USBD##n##_EP14,          \
                        (void *)USBD##n##_EP15
#elif  (USBD_EP_LAST_NUM == 14)
#define EP_POINTERS(n)  (void *)&usbd_core_thread_id[n], \
                        (void *)USBD##n##_EP1 ,          \
                        (void *)USBD##n##_EP2 ,          \
                        (void *)USBD##n##_EP3 ,          \
                        (void *)USBD##n##_EP4 ,          \
                        (void *)USBD##n##_EP5 ,          \
                        (void *)USBD##n##_EP6 ,          \
                        (void *)USBD##n##_EP7 ,          \
                        (void *)USBD##n##_EP8 ,          \
                        (void *)USBD##n##_EP9 ,          \
                        (void *)USBD##n##_EP10,          \
                        (void *)USBD##n##_EP11,          \
                        (void *)USBD##n##_EP12,          \
                        (void *)USBD##n##_EP13,          \
                        (void *)USBD##n##_EP14
#elif  (USBD_EP_LAST_NUM == 13)
#define EP_POINTERS(n)  (void *)&usbd_core_thread_id[n], \
                        (void *)USBD##n##_EP1 ,          \
                        (void *)USBD##n##_EP2 ,          \
                        (void *)USBD##n##_EP3 ,          \
                        (void *)USBD##n##_EP4 ,          \
                        (void *)USBD##n##_EP5 ,          \
                        (void *)USBD##n##_EP6 ,          \
                        (void *)USBD##n##_EP7 ,          \
                        (void *)USBD##n##_EP8 ,          \
                        (void *)USBD##n##_EP9 ,          \
                        (void *)USBD##n##_EP10,          \
                        (void *)USBD##n##_EP11,          \
                        (void *)USBD##n##_EP12,          \
                        (void *)USBD##n##_EP13
#elif  (USBD_EP_LAST_NUM == 12)
#define EP_POINTERS(n)  (void *)&usbd_core_thread_id[n], \
                        (void *)USBD##n##_EP1 ,          \
                        (void *)USBD##n##_EP2 ,          \
                        (void *)USBD##n##_EP3 ,          \
                        (void *)USBD##n##_EP4 ,          \
                        (void *)USBD##n##_EP5 ,          \
                        (void *)USBD##n##_EP6 ,          \
                        (void *)USBD##n##_EP7 ,          \
                        (void *)USBD##n##_EP8 ,          \
                        (void *)USBD##n##_EP9 ,          \
                        (void *)USBD##n##_EP10,          \
                        (void *)USBD##n##_EP11,          \
                        (void *)USBD##n##_EP12
#elif  (USBD_EP_LAST_NUM == 11)
#define EP_POINTERS(n)  (void *)&usbd_core_thread_id[n], \
                        (void *)USBD##n##_EP1 ,          \
                        (void *)USBD##n##_EP2 ,          \
                        (void *)USBD##n##_EP3 ,          \
                        (void *)USBD##n##_EP4 ,          \
                        (void *)USBD##n##_EP5 ,          \
                        (void *)USBD##n##_EP6 ,          \
                        (void *)USBD##n##_EP7 ,          \
                        (void *)USBD##n##_EP8 ,          \
                        (void *)USBD##n##_EP9 ,          \
                        (void *)USBD##n##_EP10,          \
                        (void *)USBD##n##_EP11
#elif  (USBD_EP_LAST_NUM == 10)
#define EP_POINTERS(n)  (void *)&usbd_core_thread_id[n], \
                        (void *)USBD##n##_EP1 ,          \
                        (void *)USBD##n##_EP2 ,          \
                        (void *)USBD##n##_EP3 ,          \
                        (void *)USBD##n##_EP4 ,          \
                        (void *)USBD##n##_EP5 ,          \
                        (void *)USBD##n##_EP6 ,          \
                        (void *)USBD##n##_EP7 ,          \
                        (void *)USBD##n##_EP8 ,          \
                        (void *)USBD##n##_EP9 ,          \
                        (void *)USBD##n##_EP10
#elif  (USBD_EP_LAST_NUM ==  9)
#define EP_POINTERS(n)  (void *)&usbd_core_thread_id[n], \
                        (void *)USBD##n##_EP1 ,          \
                        (void *)USBD##n##_EP2 ,          \
                        (void *)USBD##n##_EP3 ,          \
                        (void *)USBD##n##_EP4 ,          \
                        (void *)USBD##n##_EP5 ,          \
                        (void *)USBD##n##_EP6 ,          \
                        (void *)USBD##n##_EP7 ,          \
                        (void *)USBD##n##_EP8 ,          \
                        (void *)USBD##n##_EP9
#elif  (USBD_EP_LAST_NUM ==  8)
#define EP_POINTERS(n)  (void *)&usbd_core_thread_id[n], \
                        (void *)USBD##n##_EP1 ,          \
                        (void *)USBD##n##_EP2 ,          \
                        (void *)USBD##n##_EP3 ,          \
                        (void *)USBD##n##_EP4 ,          \
                        (void *)USBD##n##_EP5 ,          \
                        (void *)USBD##n##_EP6 ,          \
                        (void *)USBD##n##_EP7 ,          \
                        (void *)USBD##n##_EP8
#elif  (USBD_EP_LAST_NUM ==  7)
#define EP_POINTERS(n)  (void *)&usbd_core_thread_id[n], \
                        (void *)USBD##n##_EP1 ,          \
                        (void *)USBD##n##_EP2 ,          \
                        (void *)USBD##n##_EP3 ,          \
                        (void *)USBD##n##_EP4 ,          \
                        (void *)USBD##n##_EP5 ,          \
                        (void *)USBD##n##_EP6 ,          \
                        (void *)USBD##n##_EP7
#elif  (USBD_EP_LAST_NUM ==  6)
#define EP_POINTERS(n)  (void *)&usbd_core_thread_id[n], \
                        (void *)USBD##n##_EP1 ,          \
                        (void *)USBD##n##_EP2 ,          \
                        (void *)USBD##n##_EP3 ,          \
                        (void *)USBD##n##_EP4 ,          \
                        (void *)USBD##n##_EP5 ,          \
                        (void *)USBD##n##_EP6
#elif  (USBD_EP_LAST_NUM ==  5)
#define EP_POINTERS(n)  (void *)&usbd_core_thread_id[n], \
                        (void *)USBD##n##_EP1 ,          \
                        (void *)USBD##n##_EP2 ,          \
                        (void *)USBD##n##_EP3 ,          \
                        (void *)USBD##n##_EP4 ,          \
                        (void *)USBD##n##_EP5
#elif  (USBD_EP_LAST_NUM ==  4)
#define EP_POINTERS(n)  (void *)&usbd_core_thread_id[n], \
                        (void *)USBD##n##_EP1 ,          \
                        (void *)USBD##n##_EP2 ,          \
                        (void *)USBD##n##_EP3 ,          \
                        (void *)USBD##n##_EP4
#elif  (USBD_EP_LAST_NUM ==  3)
#define EP_POINTERS(n)  (void *)&usbd_core_thread_id[n], \
                        (void *)USBD##n##_EP1 ,          \
                        (void *)USBD##n##_EP2 ,          \
                        (void *)USBD##n##_EP3
#elif  (USBD_EP_LAST_NUM ==  2)
#define EP_POINTERS(n)  (void *)&usbd_core_thread_id[n], \
                        (void *)USBD##n##_EP1 ,          \
                        (void *)USBD##n##_EP2
#elif  (USBD_EP_LAST_NUM ==  1)
#define EP_POINTERS(n)  (void *)&usbd_core_thread_id[n], \
                        (void *)USBD##n##_EP1
#elif  (USBD_EP_LAST_NUM ==  0)
#define EP_POINTERS(n)  (void *)&usbd_core_thread_id[n]
#else
#define EP_POINTERS(n)
#endif


/*------------------------------------------------------------------------------
 *      USB Device 0..3 Configurations
 *----------------------------------------------------------------------------*/

#if    (USBD_DEV_NUM > 0)

extern void *usbd_core_thread_id         [USBD_DEV_NUM];
       void *usbd_core_thread_id         [USBD_DEV_NUM];

extern void *usbd_driver_semaphore_id    [USBD_DEV_NUM];
       void *usbd_driver_semaphore_id    [USBD_DEV_NUM];

extern void *usbd_driver_ep_semaphore_id [USBD_DEV_NUM*USBD_EP_NUM*2U];
       void *usbd_driver_ep_semaphore_id [USBD_DEV_NUM*USBD_EP_NUM*2U];

#ifdef  RTE_USB_Device_0
extern  usbd_dev_t      usbd0_dev;
#endif
#ifdef  RTE_USB_Device_1
extern  usbd_dev_t      usbd1_dev;
#endif
#ifdef  RTE_USB_Device_2
extern  usbd_dev_t      usbd2_dev;
#endif
#ifdef  RTE_USB_Device_3
extern  usbd_dev_t      usbd3_dev;
#endif

extern 
const uint8_t usbd_dev_num;
const uint8_t usbd_dev_num = USBD_DEV_NUM;

extern usbd_dev_t * const usbd_dev_ptr[USBD_DEV_NUM];
       usbd_dev_t * const usbd_dev_ptr[USBD_DEV_NUM] = {
#ifdef  RTE_USB_Device_0
       &usbd0_dev
#endif
#ifdef  RTE_USB_Device_1
     , &usbd1_dev
#endif
#ifdef  RTE_USB_Device_2
     , &usbd2_dev
#endif
#ifdef  RTE_USB_Device_3
     , &usbd3_dev
#endif
};

#endif


/*------------------------------------------------------------------------------
 *      USB Device Custom Class 0..3 Configurations
 *----------------------------------------------------------------------------*/

#ifdef  RTE_USB_Device_CustomClass_0
#define USBD_CUSTOM_CLASS0_NUM          1
#else
#define USBD_CUSTOM_CLASS0_NUM          0
#endif
#ifdef  RTE_USB_Device_CustomClass_1
#define USBD_CUSTOM_CLASS1_NUM          1
#else
#define USBD_CUSTOM_CLASS1_NUM          0
#endif
#ifdef  RTE_USB_Device_CustomClass_2
#define USBD_CUSTOM_CLASS2_NUM          1
#else
#define USBD_CUSTOM_CLASS2_NUM          0
#endif
#ifdef  RTE_USB_Device_CustomClass_3
#define USBD_CUSTOM_CLASS3_NUM          1
#else
#define USBD_CUSTOM_CLASS3_NUM          0
#endif

#define USBD_CUSTOM_CLASS_NUM          (USBD_CUSTOM_CLASS0_NUM + USBD_CUSTOM_CLASS1_NUM + USBD_CUSTOM_CLASS2_NUM + USBD_CUSTOM_CLASS3_NUM)

#if    (USBD_CUSTOM_CLASS_NUM > 0)

extern void          *usbd_custom_class_ep_thread_id [USBD_CUSTOM_CLASS_NUM][16];
       void          *usbd_custom_class_ep_thread_id [USBD_CUSTOM_CLASS_NUM][16];

extern const  uint8_t usbd_custom_class_num;
       const  uint8_t usbd_custom_class_num = USBD_CUSTOM_CLASS_NUM;
extern 
usbd_custom_class_t usbd_custom_class  [USBD_CUSTOM_CLASS_NUM];
usbd_custom_class_t usbd_custom_class  [USBD_CUSTOM_CLASS_NUM] = {
#ifdef  RTE_USB_Device_CustomClass_0
                                      {
                                        USBD_CUSTOM_CLASS0_DEV,
                                        0,
                                        USBD_CUSTOM_CLASS0_EP_MSK,
                                    { { USBD_CUSTOM_CLASS0_IF0_EN,
                                        USBD_CUSTOM_CLASS0_IF0_NUM,
                                        USBD_CUSTOM_CLASS0_IF0_EP0_EN,
                                        USBD_CUSTOM_CLASS0_IF0_EP0_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF0_EP1_EN,
                                        USBD_CUSTOM_CLASS0_IF0_EP1_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF0_EP2_EN,
                                        USBD_CUSTOM_CLASS0_IF0_EP2_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF0_EP3_EN,
                                        USBD_CUSTOM_CLASS0_IF0_EP3_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF0_EP4_EN,
                                        USBD_CUSTOM_CLASS0_IF0_EP4_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF0_EP5_EN,
                                        USBD_CUSTOM_CLASS0_IF0_EP5_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF0_EP6_EN,
                                        USBD_CUSTOM_CLASS0_IF0_EP6_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF0_EP7_EN,
                                        USBD_CUSTOM_CLASS0_IF0_EP7_BENDPOINTADDRESS },
                                      { USBD_CUSTOM_CLASS0_IF1_EN,
                                        USBD_CUSTOM_CLASS0_IF1_NUM,
                                        USBD_CUSTOM_CLASS0_IF1_EP0_EN,
                                        USBD_CUSTOM_CLASS0_IF1_EP0_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF1_EP1_EN,
                                        USBD_CUSTOM_CLASS0_IF1_EP1_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF1_EP2_EN,
                                        USBD_CUSTOM_CLASS0_IF1_EP2_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF1_EP3_EN,
                                        USBD_CUSTOM_CLASS0_IF1_EP3_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF1_EP4_EN,
                                        USBD_CUSTOM_CLASS0_IF1_EP4_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF1_EP5_EN,
                                        USBD_CUSTOM_CLASS0_IF1_EP5_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF1_EP6_EN,
                                        USBD_CUSTOM_CLASS0_IF1_EP6_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF1_EP7_EN,
                                        USBD_CUSTOM_CLASS0_IF1_EP7_BENDPOINTADDRESS },
                                      { USBD_CUSTOM_CLASS0_IF2_EN,
                                        USBD_CUSTOM_CLASS0_IF2_NUM,
                                        USBD_CUSTOM_CLASS0_IF2_EP0_EN,
                                        USBD_CUSTOM_CLASS0_IF2_EP0_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF2_EP1_EN,
                                        USBD_CUSTOM_CLASS0_IF2_EP1_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF2_EP2_EN,
                                        USBD_CUSTOM_CLASS0_IF2_EP2_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF2_EP3_EN,
                                        USBD_CUSTOM_CLASS0_IF2_EP3_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF2_EP4_EN,
                                        USBD_CUSTOM_CLASS0_IF2_EP4_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF2_EP5_EN,
                                        USBD_CUSTOM_CLASS0_IF2_EP5_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF2_EP6_EN,
                                        USBD_CUSTOM_CLASS0_IF2_EP6_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF2_EP7_EN,
                                        USBD_CUSTOM_CLASS0_IF2_EP7_BENDPOINTADDRESS },
                                      { USBD_CUSTOM_CLASS0_IF3_EN,
                                        USBD_CUSTOM_CLASS0_IF3_NUM,
                                        USBD_CUSTOM_CLASS0_IF3_EP0_EN,
                                        USBD_CUSTOM_CLASS0_IF3_EP0_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF3_EP1_EN,
                                        USBD_CUSTOM_CLASS0_IF3_EP1_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF3_EP2_EN,
                                        USBD_CUSTOM_CLASS0_IF3_EP2_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF3_EP3_EN,
                                        USBD_CUSTOM_CLASS0_IF3_EP3_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF3_EP4_EN,
                                        USBD_CUSTOM_CLASS0_IF3_EP4_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF3_EP5_EN,
                                        USBD_CUSTOM_CLASS0_IF3_EP5_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF3_EP6_EN,
                                        USBD_CUSTOM_CLASS0_IF3_EP6_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS0_IF3_EP7_EN,
                                        USBD_CUSTOM_CLASS0_IF3_EP7_BENDPOINTADDRESS } }
                                      }
#endif
#ifdef  RTE_USB_Device_CustomClass_1
                                    , {
                                        USBD_CUSTOM_CLASS1_DEV,
                                        0,
                                        USBD_CUSTOM_CLASS1_EP_MSK,
                                    { { USBD_CUSTOM_CLASS1_IF0_EN,
                                        USBD_CUSTOM_CLASS1_IF0_NUM,
                                        USBD_CUSTOM_CLASS1_IF0_EP0_EN,
                                        USBD_CUSTOM_CLASS1_IF0_EP0_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF0_EP1_EN,
                                        USBD_CUSTOM_CLASS1_IF0_EP1_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF0_EP2_EN,
                                        USBD_CUSTOM_CLASS1_IF0_EP2_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF0_EP3_EN,
                                        USBD_CUSTOM_CLASS1_IF0_EP3_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF0_EP4_EN,
                                        USBD_CUSTOM_CLASS1_IF0_EP4_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF0_EP5_EN,
                                        USBD_CUSTOM_CLASS1_IF0_EP5_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF0_EP6_EN,
                                        USBD_CUSTOM_CLASS1_IF0_EP6_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF0_EP7_EN,
                                        USBD_CUSTOM_CLASS1_IF0_EP7_BENDPOINTADDRESS },
                                      { USBD_CUSTOM_CLASS1_IF1_EN,
                                        USBD_CUSTOM_CLASS1_IF1_NUM,
                                        USBD_CUSTOM_CLASS1_IF1_EP0_EN,
                                        USBD_CUSTOM_CLASS1_IF1_EP0_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF1_EP1_EN,
                                        USBD_CUSTOM_CLASS1_IF1_EP1_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF1_EP2_EN,
                                        USBD_CUSTOM_CLASS1_IF1_EP2_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF1_EP3_EN,
                                        USBD_CUSTOM_CLASS1_IF1_EP3_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF1_EP4_EN,
                                        USBD_CUSTOM_CLASS1_IF1_EP4_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF1_EP5_EN,
                                        USBD_CUSTOM_CLASS1_IF1_EP5_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF1_EP6_EN,
                                        USBD_CUSTOM_CLASS1_IF1_EP6_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF1_EP7_EN,
                                        USBD_CUSTOM_CLASS1_IF1_EP7_BENDPOINTADDRESS },
                                      { USBD_CUSTOM_CLASS1_IF2_EN,
                                        USBD_CUSTOM_CLASS1_IF2_NUM,
                                        USBD_CUSTOM_CLASS1_IF2_EP0_EN,
                                        USBD_CUSTOM_CLASS1_IF2_EP0_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF2_EP1_EN,
                                        USBD_CUSTOM_CLASS1_IF2_EP1_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF2_EP2_EN,
                                        USBD_CUSTOM_CLASS1_IF2_EP2_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF2_EP3_EN,
                                        USBD_CUSTOM_CLASS1_IF2_EP3_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF2_EP4_EN,
                                        USBD_CUSTOM_CLASS1_IF2_EP4_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF2_EP5_EN,
                                        USBD_CUSTOM_CLASS1_IF2_EP5_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF2_EP6_EN,
                                        USBD_CUSTOM_CLASS1_IF2_EP6_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF2_EP7_EN,
                                        USBD_CUSTOM_CLASS1_IF2_EP7_BENDPOINTADDRESS },
                                      { USBD_CUSTOM_CLASS1_IF3_EN,
                                        USBD_CUSTOM_CLASS1_IF3_NUM,
                                        USBD_CUSTOM_CLASS1_IF3_EP0_EN,
                                        USBD_CUSTOM_CLASS1_IF3_EP0_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF3_EP1_EN,
                                        USBD_CUSTOM_CLASS1_IF3_EP1_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF3_EP2_EN,
                                        USBD_CUSTOM_CLASS1_IF3_EP2_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF3_EP3_EN,
                                        USBD_CUSTOM_CLASS1_IF3_EP3_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF3_EP4_EN,
                                        USBD_CUSTOM_CLASS1_IF3_EP4_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF3_EP5_EN,
                                        USBD_CUSTOM_CLASS1_IF3_EP5_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF3_EP6_EN,
                                        USBD_CUSTOM_CLASS1_IF3_EP6_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS1_IF3_EP7_EN,
                                        USBD_CUSTOM_CLASS1_IF3_EP7_BENDPOINTADDRESS } }
                                      }
#endif
#ifdef  RTE_USB_Device_CustomClass_2
                                    , {
                                        USBD_CUSTOM_CLASS2_DEV,
                                        0,
                                        USBD_CUSTOM_CLASS2_EP_MSK,
                                    { { USBD_CUSTOM_CLASS2_IF0_EN,
                                        USBD_CUSTOM_CLASS2_IF0_NUM,
                                        USBD_CUSTOM_CLASS2_IF0_EP0_EN,
                                        USBD_CUSTOM_CLASS2_IF0_EP0_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF0_EP1_EN,
                                        USBD_CUSTOM_CLASS2_IF0_EP1_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF0_EP2_EN,
                                        USBD_CUSTOM_CLASS2_IF0_EP2_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF0_EP3_EN,
                                        USBD_CUSTOM_CLASS2_IF0_EP3_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF0_EP4_EN,
                                        USBD_CUSTOM_CLASS2_IF0_EP4_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF0_EP5_EN,
                                        USBD_CUSTOM_CLASS2_IF0_EP5_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF0_EP6_EN,
                                        USBD_CUSTOM_CLASS2_IF0_EP6_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF0_EP7_EN,
                                        USBD_CUSTOM_CLASS2_IF0_EP7_BENDPOINTADDRESS },
                                      { USBD_CUSTOM_CLASS2_IF1_EN,
                                        USBD_CUSTOM_CLASS2_IF1_NUM,
                                        USBD_CUSTOM_CLASS2_IF1_EP0_EN,
                                        USBD_CUSTOM_CLASS2_IF1_EP0_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF1_EP1_EN,
                                        USBD_CUSTOM_CLASS2_IF1_EP1_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF1_EP2_EN,
                                        USBD_CUSTOM_CLASS2_IF1_EP2_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF1_EP3_EN,
                                        USBD_CUSTOM_CLASS2_IF1_EP3_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF1_EP4_EN,
                                        USBD_CUSTOM_CLASS2_IF1_EP4_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF1_EP5_EN,
                                        USBD_CUSTOM_CLASS2_IF1_EP5_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF1_EP6_EN,
                                        USBD_CUSTOM_CLASS2_IF1_EP6_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF1_EP7_EN,
                                        USBD_CUSTOM_CLASS2_IF1_EP7_BENDPOINTADDRESS },
                                      { USBD_CUSTOM_CLASS2_IF2_EN,
                                        USBD_CUSTOM_CLASS2_IF2_NUM,
                                        USBD_CUSTOM_CLASS2_IF2_EP0_EN,
                                        USBD_CUSTOM_CLASS2_IF2_EP0_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF2_EP1_EN,
                                        USBD_CUSTOM_CLASS2_IF2_EP1_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF2_EP2_EN,
                                        USBD_CUSTOM_CLASS2_IF2_EP2_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF2_EP3_EN,
                                        USBD_CUSTOM_CLASS2_IF2_EP3_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF2_EP4_EN,
                                        USBD_CUSTOM_CLASS2_IF2_EP4_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF2_EP5_EN,
                                        USBD_CUSTOM_CLASS2_IF2_EP5_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF2_EP6_EN,
                                        USBD_CUSTOM_CLASS2_IF2_EP6_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF2_EP7_EN,
                                        USBD_CUSTOM_CLASS2_IF2_EP7_BENDPOINTADDRESS },
                                      { USBD_CUSTOM_CLASS2_IF3_EN,
                                        USBD_CUSTOM_CLASS2_IF3_NUM,
                                        USBD_CUSTOM_CLASS2_IF3_EP0_EN,
                                        USBD_CUSTOM_CLASS2_IF3_EP0_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF3_EP1_EN,
                                        USBD_CUSTOM_CLASS2_IF3_EP1_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF3_EP2_EN,
                                        USBD_CUSTOM_CLASS2_IF3_EP2_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF3_EP3_EN,
                                        USBD_CUSTOM_CLASS2_IF3_EP3_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF3_EP4_EN,
                                        USBD_CUSTOM_CLASS2_IF3_EP4_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF3_EP5_EN,
                                        USBD_CUSTOM_CLASS2_IF3_EP5_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF3_EP6_EN,
                                        USBD_CUSTOM_CLASS2_IF3_EP6_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS2_IF3_EP7_EN,
                                        USBD_CUSTOM_CLASS2_IF3_EP7_BENDPOINTADDRESS } }
                                      }
#endif
#ifdef  RTE_USB_Device_CustomClass_3
                                    , {
                                        USBD_CUSTOM_CLASS3_DEV,
                                        0,
                                        USBD_CUSTOM_CLASS3_EP_MSK,
                                    { { USBD_CUSTOM_CLASS3_IF0_EN,
                                        USBD_CUSTOM_CLASS3_IF0_NUM,
                                        USBD_CUSTOM_CLASS3_IF0_EP0_EN,
                                        USBD_CUSTOM_CLASS3_IF0_EP0_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF0_EP1_EN,
                                        USBD_CUSTOM_CLASS3_IF0_EP1_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF0_EP2_EN,
                                        USBD_CUSTOM_CLASS3_IF0_EP2_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF0_EP3_EN,
                                        USBD_CUSTOM_CLASS3_IF0_EP3_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF0_EP4_EN,
                                        USBD_CUSTOM_CLASS3_IF0_EP4_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF0_EP5_EN,
                                        USBD_CUSTOM_CLASS3_IF0_EP5_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF0_EP6_EN,
                                        USBD_CUSTOM_CLASS3_IF0_EP6_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF0_EP7_EN,
                                        USBD_CUSTOM_CLASS3_IF0_EP7_BENDPOINTADDRESS },
                                      { USBD_CUSTOM_CLASS3_IF1_EN,
                                        USBD_CUSTOM_CLASS3_IF1_NUM,
                                        USBD_CUSTOM_CLASS3_IF1_EP0_EN,
                                        USBD_CUSTOM_CLASS3_IF1_EP0_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF1_EP1_EN,
                                        USBD_CUSTOM_CLASS3_IF1_EP1_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF1_EP2_EN,
                                        USBD_CUSTOM_CLASS3_IF1_EP2_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF1_EP3_EN,
                                        USBD_CUSTOM_CLASS3_IF1_EP3_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF1_EP4_EN,
                                        USBD_CUSTOM_CLASS3_IF1_EP4_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF1_EP5_EN,
                                        USBD_CUSTOM_CLASS3_IF1_EP5_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF1_EP6_EN,
                                        USBD_CUSTOM_CLASS3_IF1_EP6_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF1_EP7_EN,
                                        USBD_CUSTOM_CLASS3_IF1_EP7_BENDPOINTADDRESS },
                                      { USBD_CUSTOM_CLASS3_IF2_EN,
                                        USBD_CUSTOM_CLASS3_IF2_NUM,
                                        USBD_CUSTOM_CLASS3_IF2_EP0_EN,
                                        USBD_CUSTOM_CLASS3_IF2_EP0_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF2_EP1_EN,
                                        USBD_CUSTOM_CLASS3_IF2_EP1_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF2_EP2_EN,
                                        USBD_CUSTOM_CLASS3_IF2_EP2_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF2_EP3_EN,
                                        USBD_CUSTOM_CLASS3_IF2_EP3_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF2_EP4_EN,
                                        USBD_CUSTOM_CLASS3_IF2_EP4_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF2_EP5_EN,
                                        USBD_CUSTOM_CLASS3_IF2_EP5_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF2_EP6_EN,
                                        USBD_CUSTOM_CLASS3_IF2_EP6_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF2_EP7_EN,
                                        USBD_CUSTOM_CLASS3_IF2_EP7_BENDPOINTADDRESS },
                                      { USBD_CUSTOM_CLASS3_IF3_EN,
                                        USBD_CUSTOM_CLASS3_IF3_NUM,
                                        USBD_CUSTOM_CLASS3_IF3_EP0_EN,
                                        USBD_CUSTOM_CLASS3_IF3_EP0_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF3_EP1_EN,
                                        USBD_CUSTOM_CLASS3_IF3_EP1_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF3_EP2_EN,
                                        USBD_CUSTOM_CLASS3_IF3_EP2_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF3_EP3_EN,
                                        USBD_CUSTOM_CLASS3_IF3_EP3_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF3_EP4_EN,
                                        USBD_CUSTOM_CLASS3_IF3_EP4_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF3_EP5_EN,
                                        USBD_CUSTOM_CLASS3_IF3_EP5_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF3_EP6_EN,
                                        USBD_CUSTOM_CLASS3_IF3_EP6_BENDPOINTADDRESS,
                                        USBD_CUSTOM_CLASS3_IF3_EP7_EN,
                                        USBD_CUSTOM_CLASS3_IF3_EP7_BENDPOINTADDRESS } }
                                      }
#endif
};
extern usbd_custom_class_t * const usbd_custom_class_ptr[USBD_CUSTOM_CLASS_NUM];
       usbd_custom_class_t * const usbd_custom_class_ptr[USBD_CUSTOM_CLASS_NUM] = {
#ifdef  RTE_USB_Device_CustomClass_0
                                       &usbd_custom_class[0]
#endif
#ifdef  RTE_USB_Device_CustomClass_1
                                     , &usbd_custom_class[1]
#endif
#ifdef  RTE_USB_Device_CustomClass_2
                                     , &usbd_custom_class[2]
#endif
#ifdef  RTE_USB_Device_CustomClass_3
                                     , &usbd_custom_class[3]
#endif
};

#endif


/*------------------------------------------------------------------------------
 *      USB Device ADC 0..3 Configurations
 *----------------------------------------------------------------------------*/

#ifdef  RTE_USB_Device_ADC_0
#define USBD_ADC0_NUM                   1
#else
#define USBD_ADC0_NUM                   0
#endif
#ifdef  RTE_USB_Device_ADC_1
#define USBD_ADC1_NUM                   1
#else
#define USBD_ADC1_NUM                   0
#endif
#ifdef  RTE_USB_Device_ADC_2
#define USBD_ADC2_NUM                   1
#else
#define USBD_ADC2_NUM                   0
#endif
#ifdef  RTE_USB_Device_ADC_3
#define USBD_ADC3_NUM                   1
#else
#define USBD_ADC3_NUM                   0
#endif

#define USBD_ADC_NUM                   (USBD_ADC0_NUM + USBD_ADC1_NUM + USBD_ADC2_NUM + USBD_ADC3_NUM)

#if    (USBD_ADC_NUM > 0)

// Check settings
#ifdef  RTE_USB_Device_ADC_0
#if    (USBD_ADC0_EP_ISO_OUT_EN != 0)
#if    (USBD_ADC0_EP_ISO_OUT_WMAXPACKETSIZE != (((USBD_ADC0_OUT_TSAM_FREQ + 999) / 1000) * USBD_ADC0_OUT_CH_NUM * USBD_ADC0_OUT_BSUBFRAMESIZE))
#error Audio Device Instance 0 Isochronous Out Endpoint Maximum Packet Size should be equal to Sample Frequency in kHz (rounded up to first kHz) * Audio Subframe Size * Number of Channels!
#endif
#if    ((USBD_ADC0_OUT_BUF_SIZE * USBD_ADC0_OUT_BSUBFRAMESIZE) < (2 * USBD_ADC0_EP_ISO_OUT_WMAXPACKETSIZE))
#error Audio Device Instance 0 Maximum Audio Output Buffer Size can not be smaller then 2 Isochronous Out Endpoint Maximum Packet Size!
#endif
#endif
#if    (USBD_ADC0_EP_ISO_IN_EN != 0)
#if    (USBD_ADC0_EP_ISO_IN_WMAXPACKETSIZE != (((USBD_ADC0_IN_TSAM_FREQ + 999) / 1000) * USBD_ADC0_IN_CH_NUM * USBD_ADC0_IN_BSUBFRAMESIZE))
#error Audio Device Instance 0 Isochronous In Endpoint Maximum Packet Size should be equal to Sample Frequency in kHz (rounded up to first kHz) * Audio Subframe Size * Number of Channels!
#endif
#if    ((USBD_ADC0_IN_BUF_SIZE * USBD_ADC0_IN_BSUBFRAMESIZE) < USBD_ADC0_EP_ISO_IN_WMAXPACKETSIZE )
#error Audio Device Instance 0 Maximum Audio Input Buffer Size can not be smaller then Isochronous In Endpoint Maximum Packet Size!
#endif
#endif
#endif

#ifdef  RTE_USB_Device_ADC_1
#if    (USBD_ADC1_EP_ISO_OUT_EN != 0)
#if    (USBD_ADC1_EP_ISO_OUT_WMAXPACKETSIZE != (((USBD_ADC1_OUT_TSAM_FREQ + 999) / 1000) * USBD_ADC1_OUT_CH_NUM * USBD_ADC1_OUT_BSUBFRAMESIZE))
#error Audio Device Instance 1 Isochronous Out Endpoint Maximum Packet Size should be equal to Sample Frequency in kHz (rounded up to first kHz) * Audio Subframe Size * Number of Channels!
#endif
#if    ((USBD_ADC1_OUT_BUF_SIZE * USBD_ADC1_OUT_BSUBFRAMESIZE) < (2 * USBD_ADC1_EP_ISO_OUT_WMAXPACKETSIZE))
#error Audio Device Instance 1 Maximum Audio Output Buffer Size can not be smaller then 2 Isochronous Out Endpoint Maximum Packet Size!
#endif
#endif
#if    (USBD_ADC1_EP_ISO_IN_EN != 0)
#if    (USBD_ADC1_EP_ISO_IN_WMAXPACKETSIZE != (((USBD_ADC1_IN_TSAM_FREQ + 999) / 1000) * USBD_ADC1_IN_CH_NUM * USBD_ADC1_IN_BSUBFRAMESIZE))
#error Audio Device Instance 1 Isochronous In Endpoint Maximum Packet Size should be equal to Sample Frequency in kHz (rounded up to first kHz) * Audio Subframe Size * Number of Channels!
#endif
#if    ((USBD_ADC1_IN_BUF_SIZE * USBD_ADC1_IN_BSUBFRAMESIZE) < USBD_ADC1_EP_ISO_IN_WMAXPACKETSIZE )
#error Audio Device Instance 1 Maximum Audio Input Buffer Size can not be smaller then Isochronous In Endpoint Maximum Packet Size!
#endif
#endif
#endif

#ifdef  RTE_USB_Device_ADC_2
#if    (USBD_ADC2_EP_ISO_OUT_EN != 0)
#if    (USBD_ADC2_EP_ISO_OUT_WMAXPACKETSIZE != (((USBD_ADC2_OUT_TSAM_FREQ + 999) / 1000) * USBD_ADC2_OUT_CH_NUM * USBD_ADC2_OUT_BSUBFRAMESIZE))
#error Audio Device Instance 2 Isochronous Out Endpoint Maximum Packet Size should be equal to Sample Frequency in kHz (rounded up to first kHz) * Audio Subframe Size * Number of Channels!
#endif
#if    ((USBD_ADC2_OUT_BUF_SIZE * USBD_ADC2_OUT_BSUBFRAMESIZE) < (2 * USBD_ADC2_EP_ISO_OUT_WMAXPACKETSIZE))
#error Audio Device Instance 2 Maximum Audio Output Buffer Size can not be smaller then 2 Isochronous Out Endpoint Maximum Packet Size!
#endif
#endif
#if    (USBD_ADC2_EP_ISO_IN_EN != 0)
#if    (USBD_ADC2_EP_ISO_IN_WMAXPACKETSIZE != (((USBD_ADC2_IN_TSAM_FREQ + 999) / 1000) * USBD_ADC2_IN_CH_NUM * USBD_ADC2_IN_BSUBFRAMESIZE))
#error Audio Device Instance 2 Isochronous In Endpoint Maximum Packet Size should be equal to Sample Frequency in kHz (rounded up to first kHz) * Audio Subframe Size * Number of Channels!
#endif
#if    ((USBD_ADC2_IN_BUF_SIZE * USBD_ADC2_IN_BSUBFRAMESIZE) < USBD_ADC2_EP_ISO_IN_WMAXPACKETSIZE )
#error Audio Device Instance 2 Maximum Audio Input Buffer Size can not be smaller then Isochronous In Endpoint Maximum Packet Size!
#endif
#endif
#endif

#ifdef  RTE_USB_Device_ADC_3
#if    (USBD_ADC3_EP_ISO_OUT_EN != 0)
#if    (USBD_ADC3_EP_ISO_OUT_WMAXPACKETSIZE != (((USBD_ADC3_OUT_TSAM_FREQ + 999) / 1000) * USBD_ADC3_OUT_CH_NUM * USBD_ADC3_OUT_BSUBFRAMESIZE))
#error Audio Device Instance 3 Isochronous Out Endpoint Maximum Packet Size should be equal to Sample Frequency in kHz (rounded up to first kHz) * Audio Subframe Size * Number of Channels!
#endif
#if    ((USBD_ADC3_OUT_BUF_SIZE * USBD_ADC3_OUT_BSUBFRAMESIZE) < (2 * USBD_ADC3_EP_ISO_OUT_WMAXPACKETSIZE))
#error Audio Device Instance 3 Maximum Audio Output Buffer Size can not be smaller then 2 Isochronous Out Endpoint Maximum Packet Size!
#endif
#endif
#if    (USBD_ADC3_EP_ISO_IN_EN != 0)
#if    (USBD_ADC3_EP_ISO_IN_WMAXPACKETSIZE != (((USBD_ADC3_IN_TSAM_FREQ + 999) / 1000) * USBD_ADC3_IN_CH_NUM * USBD_ADC3_IN_BSUBFRAMESIZE))
#error Audio Device Instance 3 Isochronous In Endpoint Maximum Packet Size should be equal to Sample Frequency in kHz (rounded up to first kHz) * Audio Subframe Size * Number of Channels!
#endif
#if    ((USBD_ADC3_IN_BUF_SIZE * USBD_ADC3_IN_BSUBFRAMESIZE) < USBD_ADC3_EP_ISO_IN_WMAXPACKETSIZE )
#error Audio Device Instance 3 Maximum Audio Input Buffer Size can not be smaller then Isochronous In Endpoint Maximum Packet Size!
#endif
#endif
#endif

extern 
void                       *usbd_adc_thread_id     [USBD_ADC_NUM];
void                       *usbd_adc_thread_id     [USBD_ADC_NUM];

#ifdef  RTE_USB_Device_ADC_0
#if    (USBD_ADC0_EP_ISO_OUT_EN)
extern 
uint8_t usbd_adc0_out_data_buf [RUP32(USBD_ADC0_OUT_BUF_SIZE*USBD_ADC0_OUT_BSUBFRAMESIZE)];
uint8_t usbd_adc0_out_data_buf [RUP32(USBD_ADC0_OUT_BUF_SIZE*USBD_ADC0_OUT_BSUBFRAMESIZE)] __ALIGNED(32);
#endif
#if    (USBD_ADC0_EP_ISO_IN_EN)
extern 
uint8_t usbd_adc0_in_data_buf  [USBD_ADC0_IN_BUF_SIZE*USBD_ADC0_IN_BSUBFRAMESIZE];
uint8_t usbd_adc0_in_data_buf  [USBD_ADC0_IN_BUF_SIZE*USBD_ADC0_IN_BSUBFRAMESIZE]          __ALIGNED(4);
#endif
#endif

#ifdef  RTE_USB_Device_ADC_1
#if    (USBD_ADC1_EP_ISO_OUT_EN)
extern 
uint8_t usbd_adc1_out_data_buf [RUP32(USBD_ADC1_OUT_BUF_SIZE*USBD_ADC1_OUT_BSUBFRAMESIZE)];
uint8_t usbd_adc1_out_data_buf [RUP32(USBD_ADC1_OUT_BUF_SIZE*USBD_ADC1_OUT_BSUBFRAMESIZE)] __ALIGNED(32);
#endif
#if    (USBD_ADC1_EP_ISO_IN_EN)
extern 
uint8_t usbd_adc1_in_data_buf  [USBD_ADC1_IN_BUF_SIZE*USBD_ADC1_IN_BSUBFRAMESIZE];
uint8_t usbd_adc1_in_data_buf  [USBD_ADC1_IN_BUF_SIZE*USBD_ADC1_IN_BSUBFRAMESIZE]          __ALIGNED(4);
#endif
#endif

#ifdef  RTE_USB_Device_ADC_2
#if    (USBD_ADC2_EP_ISO_OUT_EN)
extern 
uint8_t usbd_adc2_out_data_buf [RUP32(USBD_ADC2_OUT_BUF_SIZE*USBD_ADC2_OUT_BSUBFRAMESIZE)];
uint8_t usbd_adc2_out_data_buf [RUP32(USBD_ADC2_OUT_BUF_SIZE*USBD_ADC2_OUT_BSUBFRAMESIZE)] __ALIGNED(32);
#endif
#if    (USBD_ADC2_EP_ISO_IN_EN)
extern 
uint8_t usbd_adc2_in_data_buf  [USBD_ADC2_IN_BUF_SIZE*USBD_ADC2_IN_BSUBFRAMESIZE];
uint8_t usbd_adc2_in_data_buf  [USBD_ADC2_IN_BUF_SIZE*USBD_ADC2_IN_BSUBFRAMESIZE]          __ALIGNED(4);
#endif
#endif

#ifdef  RTE_USB_Device_ADC_3
#if    (USBD_ADC3_EP_ISO_OUT_EN)
extern 
uint8_t usbd_adc3_out_data_buf [RUP32(USBD_ADC3_OUT_BUF_SIZE*USBD_ADC3_OUT_BSUBFRAMESIZE)];
uint8_t usbd_adc3_out_data_buf [RUP32(USBD_ADC3_OUT_BUF_SIZE*USBD_ADC3_OUT_BSUBFRAMESIZE)] __ALIGNED(32);
#endif
#if    (USBD_ADC3_EP_ISO_IN_EN)
extern 
uint8_t usbd_adc3_in_data_buf  [USBD_ADC3_IN_BUF_SIZE*USBD_ADC3_IN_BSUBFRAMESIZE];
uint8_t usbd_adc3_in_data_buf  [USBD_ADC3_IN_BUF_SIZE*USBD_ADC3_IN_BSUBFRAMESIZE]          __ALIGNED(4);
#endif
#endif

extern 
const uint8_t usbd_adc_num;
const uint8_t usbd_adc_num =            USBD_ADC_NUM;

#ifdef  RTE_USB_Device_ADC_0
#if    (USBD_ADC0_EP_ISO_OUT_EN)
extern 
usbd_adc_spkr_data_t usbd_adc0_spkr_data;
usbd_adc_spkr_data_t usbd_adc0_spkr_data;
extern 
usbd_adc_code_t usbd_adc0_out_data;
usbd_adc_code_t usbd_adc0_out_data = {
                                        usbd_adc0_out_data_buf,
                                        USBD_ADC0_IF_SPKR_NUM,
                                        2,
                                        USBD_ADC0_EP_ISO_OUT,
                                        USBD_ADC0_OUT_CH_NUM,
                                        USBD_ADC0_OUT_TSAM_FREQ,
                                        USBD_ADC0_OUT_BSUBFRAMESIZE,
                                        USBD_ADC0_OUT_BBITRESOLUTION,
                                        USBD_ADC0_OUT_BUF_SIZE*USBD_ADC0_OUT_BSUBFRAMESIZE,
                                        USBD_ADC0_EP_ISO_OUT_WMAXPACKETSIZE,
                                        0
};
#endif
#if    (USBD_ADC0_EP_ISO_IN_EN)
extern
usbd_adc_mic_data_t  usbd_adc0_mic_data;
usbd_adc_mic_data_t  usbd_adc0_mic_data;
extern 
usbd_adc_code_t usbd_adc0_in_data;
usbd_adc_code_t usbd_adc0_in_data = {
                                        usbd_adc0_in_data_buf,
                                        USBD_ADC0_IF_MIC_NUM,
                                        USBD_ADC0_IF_SPKR*3+2,
                                        USBD_ADC0_EP_ISO_IN,
                                        USBD_ADC0_IN_CH_NUM,
                                        USBD_ADC0_IN_TSAM_FREQ,
                                        USBD_ADC0_IN_BSUBFRAMESIZE,
                                        USBD_ADC0_IN_BBITRESOLUTION,
                                        USBD_ADC0_IN_BUF_SIZE*USBD_ADC0_IN_BSUBFRAMESIZE,
                                        USBD_ADC0_EP_ISO_IN_WMAXPACKETSIZE,
                                        0
};
#endif
#endif
#ifdef  RTE_USB_Device_ADC_1
#if    (USBD_ADC1_EP_ISO_OUT_EN)
extern 
usbd_adc_spkr_data_t usbd_adc1_spkr_data;
usbd_adc_spkr_data_t usbd_adc1_spkr_data;
extern
usbd_adc_code_t usbd_adc1_out_data;
usbd_adc_code_t usbd_adc1_out_data = {
                                        usbd_adc1_out_data_buf,
                                        USBD_ADC1_IF_SPKR_NUM,
                                        2,
                                        USBD_ADC1_EP_ISO_OUT,
                                        USBD_ADC1_OUT_CH_NUM,
                                        USBD_ADC1_OUT_TSAM_FREQ,
                                        USBD_ADC1_OUT_BSUBFRAMESIZE,
                                        USBD_ADC1_OUT_BBITRESOLUTION,
                                        USBD_ADC1_OUT_BUF_SIZE*USBD_ADC1_OUT_BSUBFRAMESIZE,
                                        USBD_ADC1_EP_ISO_OUT_WMAXPACKETSIZE,
                                        0
};
#endif
#if    (USBD_ADC1_EP_ISO_IN_EN)
extern 
usbd_adc_mic_data_t  usbd_adc1_mic_data;
usbd_adc_mic_data_t  usbd_adc1_mic_data;
extern 
usbd_adc_code_t usbd_adc1_in_data;
usbd_adc_code_t usbd_adc1_in_data = {
                                        usbd_adc1_in_data_buf,
                                        USBD_ADC1_IF_MIC_NUM,
                                        USBD_ADC1_IF_SPKR*3+2,
                                        USBD_ADC1_EP_ISO_IN,
                                        USBD_ADC1_IN_CH_NUM,
                                        USBD_ADC1_IN_TSAM_FREQ,
                                        USBD_ADC1_IN_BSUBFRAMESIZE,
                                        USBD_ADC1_IN_BBITRESOLUTION,
                                        USBD_ADC1_IN_BUF_SIZE*USBD_ADC1_IN_BSUBFRAMESIZE,
                                        USBD_ADC1_EP_ISO_IN_WMAXPACKETSIZE,
                                        0
};
#endif
#endif
#ifdef  RTE_USB_Device_ADC_2
#if    (USBD_ADC2_EP_ISO_OUT_EN)
extern 
usbd_adc_spkr_data_t usbd_adc2_spkr_data;
usbd_adc_spkr_data_t usbd_adc2_spkr_data;
extern 
usbd_adc_code_t usbd_adc2_out_data;
usbd_adc_code_t usbd_adc2_out_data = {
                                        usbd_adc2_out_data_buf,
                                        USBD_ADC2_IF_SPKR_NUM,
                                        2,
                                        USBD_ADC2_EP_ISO_OUT,
                                        USBD_ADC2_OUT_CH_NUM,
                                        USBD_ADC2_OUT_TSAM_FREQ,
                                        USBD_ADC2_OUT_BSUBFRAMESIZE,
                                        USBD_ADC2_OUT_BBITRESOLUTION,
                                        USBD_ADC2_OUT_BUF_SIZE*USBD_ADC2_OUT_BSUBFRAMESIZE,
                                        USBD_ADC2_EP_ISO_OUT_WMAXPACKETSIZE,
                                        0
};
#endif
#if    (USBD_ADC2_EP_ISO_IN_EN)
extern 
usbd_adc_mic_data_t  usbd_adc2_mic_data;
usbd_adc_mic_data_t  usbd_adc2_mic_data;
extern 
usbd_adc_code_t usbd_adc2_in_data;
usbd_adc_code_t usbd_adc2_in_data = {
                                        usbd_adc2_in_data_buf,
                                        USBD_ADC2_IF_MIC_NUM,
                                        USBD_ADC2_IF_SPKR*3+2,
                                        USBD_ADC2_EP_ISO_IN,
                                        USBD_ADC2_IN_CH_NUM,
                                        USBD_ADC2_IN_TSAM_FREQ,
                                        USBD_ADC2_IN_BSUBFRAMESIZE,
                                        USBD_ADC2_IN_BBITRESOLUTION,
                                        USBD_ADC2_IN_BUF_SIZE*USBD_ADC2_IN_BSUBFRAMESIZE,
                                        USBD_ADC2_EP_ISO_IN_WMAXPACKETSIZE,
                                        0
};
#endif
#endif
#ifdef  RTE_USB_Device_ADC_3
#if    (USBD_ADC3_EP_ISO_OUT_EN)
extern 
usbd_adc_spkr_data_t usbd_adc3_spkr_data;
usbd_adc_spkr_data_t usbd_adc3_spkr_data;
extern 
usbd_adc_code_t usbd_adc3_out_data;
usbd_adc_code_t usbd_adc3_out_data = {
                                        usbd_adc3_out_data_buf,
                                        USBD_ADC3_IF_SPKR_NUM,
                                        2,
                                        USBD_ADC3_EP_ISO_OUT,
                                        USBD_ADC3_OUT_CH_NUM,
                                        USBD_ADC3_OUT_TSAM_FREQ,
                                        USBD_ADC3_OUT_BSUBFRAMESIZE,
                                        USBD_ADC3_OUT_BBITRESOLUTION,
                                        USBD_ADC3_OUT_BUF_SIZE*USBD_ADC3_OUT_BSUBFRAMESIZE,
                                        USBD_ADC3_EP_ISO_OUT_WMAXPACKETSIZE,
                                        0
};
#endif
#if    (USBD_ADC3_EP_ISO_IN_EN)
extern 
usbd_adc_mic_data_t  usbd_adc3_mic_data;
usbd_adc_mic_data_t  usbd_adc3_mic_data;
extern 
usbd_adc_code_t usbd_adc3_in_data;
usbd_adc_code_t usbd_adc3_in_data = {
                                        usbd_adc3_in_data_buf,
                                        USBD_ADC3_IF_MIC_NUM,
                                        USBD_ADC3_IF_SPKR*3+2,
                                        USBD_ADC3_EP_ISO_IN,
                                        USBD_ADC3_IN_CH_NUM,
                                        USBD_ADC3_IN_TSAM_FREQ,
                                        USBD_ADC3_IN_BSUBFRAMESIZE,
                                        USBD_ADC3_IN_BBITRESOLUTION,
                                        USBD_ADC3_IN_BUF_SIZE*USBD_ADC3_IN_BSUBFRAMESIZE,
                                        USBD_ADC3_EP_ISO_IN_WMAXPACKETSIZE,
                                        0
};
#endif
#endif

extern 
usbd_adc_t usbd_adc                    [USBD_ADC_NUM];
usbd_adc_t usbd_adc                    [USBD_ADC_NUM] = {
#ifdef  RTE_USB_Device_ADC_0
                                      {
#if    (USBD_ADC0_EP_ISO_OUT_EN)
                                       &usbd_adc0_spkr_data,
                                       &usbd_adc0_out_data,
#else
                                        NULL,
                                        NULL,
#endif
#if    (USBD_ADC0_EP_ISO_IN_EN)
                                       &usbd_adc0_mic_data,
                                       &usbd_adc0_in_data,
#else
                                        NULL,
                                        NULL,
#endif
                                        USBD_ADC0_DEV,
                                        USBD_ADC0_IF_CTRL_NUM,
                                        0
                                      }
#endif
#ifdef  RTE_USB_Device_ADC_1
                                    , {
#if    (USBD_ADC1_EP_ISO_OUT_EN)
                                       &usbd_adc1_spkr_data,
                                       &usbd_adc1_out_data,
#else
                                        NULL,
                                        NULL,
#endif
#if    (USBD_ADC1_EP_ISO_IN_EN)
                                       &usbd_adc1_mic_data,
                                       &usbd_adc1_in_data,
#else
                                        NULL,
                                        NULL,
#endif
                                        USBD_ADC1_DEV,
                                        USBD_ADC1_IF_CTRL_NUM,
                                        0
                                      }
#endif

#ifdef  RTE_USB_Device_ADC_2
                                    , {
#if    (USBD_ADC2_EP_ISO_OUT_EN)
                                       &usbd_adc2_spkr_data,
                                       &usbd_adc2_out_data,
#else
                                        NULL,
                                        NULL,
#endif
#if    (USBD_ADC2_EP_ISO_IN_EN)
                                       &usbd_adc2_mic_data,
                                       &usbd_adc2_in_data,
#else
                                        NULL,
                                        NULL,
#endif
                                        USBD_ADC2_DEV,
                                        USBD_ADC2_IF_CTRL_NUM,
                                        0
                                      }
#endif


#ifdef  RTE_USB_Device_ADC_3
                                    , {
#if    (USBD_ADC3_EP_ISO_OUT_EN)
                                       &usbd_adc3_spkr_data,
                                       &usbd_adc3_out_data,
#else
                                        NULL,
                                        NULL,
#endif
#if    (USBD_ADC3_EP_ISO_IN_EN)
                                       &usbd_adc3_mic_data,
                                       &usbd_adc3_in_data,
#else
                                        NULL,
                                        NULL,
#endif
                                        USBD_ADC3_DEV,
                                        USBD_ADC3_IF_CTRL_NUM,
                                        0
                                      }
#endif
};
extern usbd_adc_t * const usbd_adc_ptr  [USBD_ADC_NUM];
       usbd_adc_t * const usbd_adc_ptr  [USBD_ADC_NUM] = {
#ifdef  RTE_USB_Device_ADC_0
       &usbd_adc[0]
#endif
#ifdef  RTE_USB_Device_ADC_1
     , &usbd_adc[1]
#endif
#ifdef  RTE_USB_Device_ADC_2
     , &usbd_adc[2]
#endif
#ifdef  RTE_USB_Device_ADC_3
     , &usbd_adc[3]
#endif
};

#endif


/*------------------------------------------------------------------------------
 *      USB Device CDC 0..7 Configurations
 *----------------------------------------------------------------------------*/

#define USBD_CDC0_MAX_PACKET           (MAX_OF_2(USBD_CDC0_WMAXPACKETSIZE,  USBD_CDC0_HS_WMAXPACKETSIZE))
#define USBD_CDC1_MAX_PACKET           (MAX_OF_2(USBD_CDC1_WMAXPACKETSIZE,  USBD_CDC1_HS_WMAXPACKETSIZE))
#define USBD_CDC2_MAX_PACKET           (MAX_OF_2(USBD_CDC2_WMAXPACKETSIZE,  USBD_CDC2_HS_WMAXPACKETSIZE))
#define USBD_CDC3_MAX_PACKET           (MAX_OF_2(USBD_CDC3_WMAXPACKETSIZE,  USBD_CDC3_HS_WMAXPACKETSIZE))
#define USBD_CDC4_MAX_PACKET           (MAX_OF_2(USBD_CDC4_WMAXPACKETSIZE,  USBD_CDC4_HS_WMAXPACKETSIZE))
#define USBD_CDC5_MAX_PACKET           (MAX_OF_2(USBD_CDC5_WMAXPACKETSIZE,  USBD_CDC5_HS_WMAXPACKETSIZE))
#define USBD_CDC6_MAX_PACKET           (MAX_OF_2(USBD_CDC6_WMAXPACKETSIZE,  USBD_CDC6_HS_WMAXPACKETSIZE))
#define USBD_CDC7_MAX_PACKET           (MAX_OF_2(USBD_CDC7_WMAXPACKETSIZE,  USBD_CDC7_HS_WMAXPACKETSIZE))
#define USBD_CDC0_MAX_PACKET1          (MAX_OF_2(USBD_CDC0_WMAXPACKETSIZE1, USBD_CDC0_HS_WMAXPACKETSIZE1))
#define USBD_CDC1_MAX_PACKET1          (MAX_OF_2(USBD_CDC1_WMAXPACKETSIZE1, USBD_CDC1_HS_WMAXPACKETSIZE1))
#define USBD_CDC2_MAX_PACKET1          (MAX_OF_2(USBD_CDC2_WMAXPACKETSIZE1, USBD_CDC2_HS_WMAXPACKETSIZE1))
#define USBD_CDC3_MAX_PACKET1          (MAX_OF_2(USBD_CDC3_WMAXPACKETSIZE1, USBD_CDC3_HS_WMAXPACKETSIZE1))
#define USBD_CDC4_MAX_PACKET1          (MAX_OF_2(USBD_CDC4_WMAXPACKETSIZE1, USBD_CDC4_HS_WMAXPACKETSIZE1))
#define USBD_CDC5_MAX_PACKET1          (MAX_OF_2(USBD_CDC5_WMAXPACKETSIZE1, USBD_CDC5_HS_WMAXPACKETSIZE1))
#define USBD_CDC6_MAX_PACKET1          (MAX_OF_2(USBD_CDC6_WMAXPACKETSIZE1, USBD_CDC6_HS_WMAXPACKETSIZE1))
#define USBD_CDC7_MAX_PACKET1          (MAX_OF_2(USBD_CDC7_WMAXPACKETSIZE1, USBD_CDC7_HS_WMAXPACKETSIZE1))

#ifdef  RTE_USB_Device_CDC_0
#define USBD_CDC0_NUM                   1
#else
#define USBD_CDC0_NUM                   0
#endif
#ifdef  RTE_USB_Device_CDC_1
#define USBD_CDC1_NUM                   1
#else
#define USBD_CDC1_NUM                   0
#endif
#ifdef  RTE_USB_Device_CDC_2
#define USBD_CDC2_NUM                   1
#else
#define USBD_CDC2_NUM                   0
#endif
#ifdef  RTE_USB_Device_CDC_3
#define USBD_CDC3_NUM                   1
#else
#define USBD_CDC3_NUM                   0
#endif
#ifdef  RTE_USB_Device_CDC_4
#define USBD_CDC4_NUM                   1
#else
#define USBD_CDC4_NUM                   0
#endif
#ifdef  RTE_USB_Device_CDC_5
#define USBD_CDC5_NUM                   1
#else
#define USBD_CDC5_NUM                   0
#endif
#ifdef  RTE_USB_Device_CDC_6
#define USBD_CDC6_NUM                   1
#else
#define USBD_CDC6_NUM                   0
#endif
#ifdef  RTE_USB_Device_CDC_7
#define USBD_CDC7_NUM                   1
#else
#define USBD_CDC7_NUM                   0
#endif

#define USBD_CDC_NUM                   (USBD_CDC0_NUM + USBD_CDC1_NUM + USBD_CDC2_NUM + USBD_CDC3_NUM + USBD_CDC4_NUM + USBD_CDC5_NUM + USBD_CDC6_NUM + USBD_CDC7_NUM)

#if    (USBD_CDC_NUM > 0)

extern 
void                             *usbd_cdc_int_thread_id      [USBD_CDC_NUM];
void                             *usbd_cdc_int_thread_id      [USBD_CDC_NUM];

extern 
void                             *usbd_cdc_bulk_thread_id     [USBD_CDC_NUM];
void                             *usbd_cdc_bulk_thread_id     [USBD_CDC_NUM];


#ifdef  RTE_USB_Device_CDC_0
#if     USBD_CDC0_ACM
extern 
uint8_t usbd_cdc0_send_buf       [USBD_CDC0_ACM_SEND_BUF_SIZE];
uint8_t usbd_cdc0_send_buf       [USBD_CDC0_ACM_SEND_BUF_SIZE]           __ALIGNED(4);
extern 
uint8_t usbd_cdc0_receive_buf    [USBD_CDC0_ACM_RECEIVE_BUF_SIZE];
uint8_t usbd_cdc0_receive_buf    [USBD_CDC0_ACM_RECEIVE_BUF_SIZE]        __ALIGNED(4);
extern 
uint8_t usbd_cdc0_notify_buf     [10];
uint8_t usbd_cdc0_notify_buf     [10]                                    __ALIGNED(4);
extern 
uint8_t usbd_cdc0_send_mps_buf   [USBD_CDC0_MAX_PACKET1];
uint8_t usbd_cdc0_send_mps_buf   [USBD_CDC0_MAX_PACKET1]                 __ALIGNED(4);
extern 
uint8_t usbd_cdc0_receive_mps_buf[RUP32(USBD_CDC0_MAX_PACKET1)];
uint8_t usbd_cdc0_receive_mps_buf[RUP32(USBD_CDC0_MAX_PACKET1)]          __ALIGNED(32);
#elif   USBD_CDC0_NCM
#if    (USBD_CDC0_NCM_RAW_ENABLE == 0)
#if    (USBD_CDC0_NCM_NTB_IN_BUF_CNT == 0)
#error Maximum CDC0 NCM IN NTB Data Buffers can not be 0!
#elif  (USBD_CDC0_NCM_NTB_IN_BUF_CNT > 2)
#error Maximum CDC0 NCM IN NTB Data Buffers can not be more than 2!
#endif
#if    (USBD_CDC0_NCM_NTB_OUT_BUF_CNT == 0)
#error Maximum CDC0 NCM OUT NTB Data Buffers can not be 0!
#elif  (USBD_CDC0_NCM_NTB_OUT_BUF_CNT > 2)
#error Maximum CDC0 NCM OUT NTB Data Buffers can not be more than 2!
#endif
#define USBD_CDC0_NCM_NTB_IN_BUF_SIZE    (USBD_CDC0_NCM_NTB_IN_BUF_CNT *USBD_CDC0_NCM_DW_NTB_IN_MAX_SIZE)
#define USBD_CDC0_NCM_NTB_OUT_BUF_SIZE   (USBD_CDC0_NCM_NTB_OUT_BUF_CNT*USBD_CDC0_NCM_DW_NTB_OUT_MAX_SIZE)
extern 
uint8_t usbd_cdc0_send_buf       [USBD_CDC0_NCM_NTB_IN_BUF_SIZE];
uint8_t usbd_cdc0_send_buf       [USBD_CDC0_NCM_NTB_IN_BUF_SIZE]         __ALIGNED(4);
extern
uint8_t usbd_cdc0_receive_buf    [RUP32(USBD_CDC0_NCM_NTB_OUT_BUF_SIZE)];
uint8_t usbd_cdc0_receive_buf    [RUP32(USBD_CDC0_NCM_NTB_OUT_BUF_SIZE)] __ALIGNED(32);
#else
#define USBD_CDC0_NCM_NTB_IN_BUF_SIZE    (0)
#define USBD_CDC0_NCM_NTB_OUT_BUF_SIZE   (0)
#endif
extern 
uint8_t usbd_cdc0_notify_buf     [16];
uint8_t usbd_cdc0_notify_buf     [16]                                    __ALIGNED(4);
#endif
#endif

#ifdef  RTE_USB_Device_CDC_1
#if     USBD_CDC1_ACM
extern 
uint8_t usbd_cdc1_send_buf       [USBD_CDC1_ACM_SEND_BUF_SIZE];
uint8_t usbd_cdc1_send_buf       [USBD_CDC1_ACM_SEND_BUF_SIZE]           __ALIGNED(4);
extern 
uint8_t usbd_cdc1_receive_buf    [USBD_CDC1_ACM_RECEIVE_BUF_SIZE];
uint8_t usbd_cdc1_receive_buf    [USBD_CDC1_ACM_RECEIVE_BUF_SIZE]        __ALIGNED(4);
extern 
uint8_t usbd_cdc1_notify_buf     [10];
uint8_t usbd_cdc1_notify_buf     [10]                                    __ALIGNED(4);
extern 
uint8_t usbd_cdc1_send_mps_buf   [USBD_CDC1_MAX_PACKET1];
uint8_t usbd_cdc1_send_mps_buf   [USBD_CDC1_MAX_PACKET1]                 __ALIGNED(4);
extern 
uint8_t usbd_cdc1_receive_mps_buf[RUP32(USBD_CDC1_MAX_PACKET1)];
uint8_t usbd_cdc1_receive_mps_buf[RUP32(USBD_CDC1_MAX_PACKET1)]          __ALIGNED(32);
#elif   USBD_CDC1_NCM
#if    (USBD_CDC1_NCM_RAW_ENABLE == 0)
#if    (USBD_CDC1_NCM_NTB_IN_BUF_CNT == 0)
#error Maximum CDC1 NCM IN NTB Data Buffers can not be 0!
#elif  (USBD_CDC1_NCM_NTB_IN_BUF_CNT > 2)
#error Maximum CDC1 NCM IN NTB Data Buffers can not be more than 2!
#endif
#if    (USBD_CDC1_NCM_NTB_OUT_BUF_CNT == 0)
#error Maximum CDC1 NCM OUT NTB Data Buffers can not be 0!
#elif  (USBD_CDC1_NCM_NTB_OUT_BUF_CNT > 2)
#error Maximum CDC1 NCM OUT NTB Data Buffers can not be more than 2!
#endif
#define USBD_CDC1_NCM_NTB_IN_BUF_SIZE    (USBD_CDC1_NCM_NTB_IN_BUF_CNT *USBD_CDC1_NCM_DW_NTB_IN_MAX_SIZE)
#define USBD_CDC1_NCM_NTB_OUT_BUF_SIZE   (USBD_CDC1_NCM_NTB_OUT_BUF_CNT*USBD_CDC1_NCM_DW_NTB_OUT_MAX_SIZE)
extern 
uint8_t usbd_cdc1_send_buf       [USBD_CDC1_NCM_NTB_IN_BUF_SIZE];
uint8_t usbd_cdc1_send_buf       [USBD_CDC1_NCM_NTB_IN_BUF_SIZE]         __ALIGNED(4);
extern 
uint8_t usbd_cdc1_receive_buf    [RUP32(USBD_CDC1_NCM_NTB_OUT_BUF_SIZE)];
uint8_t usbd_cdc1_receive_buf    [RUP32(USBD_CDC1_NCM_NTB_OUT_BUF_SIZE)] __ALIGNED(32);
#else
#define USBD_CDC1_NCM_NTB_IN_BUF_SIZE    (0)
#define USBD_CDC1_NCM_NTB_OUT_BUF_SIZE   (0)
#endif
extern 
uint8_t usbd_cdc1_notify_buf     [16];
uint8_t usbd_cdc1_notify_buf     [16]                                    __ALIGNED(4);
#endif
#endif

#ifdef  RTE_USB_Device_CDC_2
#if     USBD_CDC2_ACM
extern 
uint8_t usbd_cdc2_send_buf       [USBD_CDC2_ACM_SEND_BUF_SIZE];
uint8_t usbd_cdc2_send_buf       [USBD_CDC2_ACM_SEND_BUF_SIZE]           __ALIGNED(4);
extern 
uint8_t usbd_cdc2_receive_buf    [USBD_CDC2_ACM_RECEIVE_BUF_SIZE];
uint8_t usbd_cdc2_receive_buf    [USBD_CDC2_ACM_RECEIVE_BUF_SIZE]        __ALIGNED(4);
extern 
uint8_t usbd_cdc2_notify_buf     [10];
uint8_t usbd_cdc2_notify_buf     [10]                                    __ALIGNED(4);
extern 
uint8_t usbd_cdc2_send_mps_buf   [USBD_CDC2_MAX_PACKET1];
uint8_t usbd_cdc2_send_mps_buf   [USBD_CDC2_MAX_PACKET1]                 __ALIGNED(4);
extern 
uint8_t usbd_cdc2_receive_mps_buf[RUP32(USBD_CDC2_MAX_PACKET1)];
uint8_t usbd_cdc2_receive_mps_buf[RUP32(USBD_CDC2_MAX_PACKET1)]          __ALIGNED(32);
#elif   USBD_CDC2_NCM
#if    (USBD_CDC2_NCM_RAW_ENABLE == 0)
#if    (USBD_CDC2_NCM_NTB_IN_BUF_CNT == 0)
#error Maximum CDC2 NCM IN NTB Data Buffers can not be 0!
#elif  (USBD_CDC2_NCM_NTB_IN_BUF_CNT > 2)
#error Maximum CDC2 NCM IN NTB Data Buffers can not be more than 2!
#endif
#if    (USBD_CDC2_NCM_NTB_OUT_BUF_CNT == 0)
#error Maximum CDC2 NCM OUT NTB Data Buffers can not be 0!
#elif  (USBD_CDC2_NCM_NTB_OUT_BUF_CNT > 2)
#error Maximum CDC2 NCM OUT NTB Data Buffers can not be more than 2!
#endif
#define USBD_CDC2_NCM_NTB_IN_BUF_SIZE    (USBD_CDC2_NCM_NTB_IN_BUF_CNT *USBD_CDC2_NCM_DW_NTB_IN_MAX_SIZE)
#define USBD_CDC2_NCM_NTB_OUT_BUF_SIZE   (USBD_CDC2_NCM_NTB_OUT_BUF_CNT*USBD_CDC2_NCM_DW_NTB_OUT_MAX_SIZE)
extern 
uint8_t usbd_cdc2_send_buf       [USBD_CDC2_NCM_NTB_IN_BUF_SIZE];
uint8_t usbd_cdc2_send_buf       [USBD_CDC2_NCM_NTB_IN_BUF_SIZE]         __ALIGNED(4);
extern 
uint8_t usbd_cdc2_receive_buf    [RUP32(USBD_CDC2_NCM_NTB_OUT_BUF_SIZE)];
uint8_t usbd_cdc2_receive_buf    [RUP32(USBD_CDC2_NCM_NTB_OUT_BUF_SIZE)] __ALIGNED(32);
#else
#define USBD_CDC2_NCM_NTB_IN_BUF_SIZE    (0)
#define USBD_CDC2_NCM_NTB_OUT_BUF_SIZE   (0)
#endif
extern 
uint8_t usbd_cdc2_notify_buf     [16];
uint8_t usbd_cdc2_notify_buf     [16]                                    __ALIGNED(4);
#endif
#endif

#ifdef  RTE_USB_Device_CDC_3
#if     USBD_CDC3_ACM
extern 
uint8_t usbd_cdc3_send_buf       [USBD_CDC3_ACM_SEND_BUF_SIZE];
uint8_t usbd_cdc3_send_buf       [USBD_CDC3_ACM_SEND_BUF_SIZE]           __ALIGNED(4);
extern 
uint8_t usbd_cdc3_receive_buf    [USBD_CDC3_ACM_RECEIVE_BUF_SIZE];
uint8_t usbd_cdc3_receive_buf    [USBD_CDC3_ACM_RECEIVE_BUF_SIZE]        __ALIGNED(4);
extern 
uint8_t usbd_cdc3_notify_buf     [10];
uint8_t usbd_cdc3_notify_buf     [10]                                    __ALIGNED(4);
extern 
uint8_t usbd_cdc3_send_mps_buf   [USBD_CDC3_MAX_PACKET1];
uint8_t usbd_cdc3_send_mps_buf   [USBD_CDC3_MAX_PACKET1]                 __ALIGNED(4);
extern 
uint8_t usbd_cdc3_receive_mps_buf[RUP32(USBD_CDC3_MAX_PACKET1)];
uint8_t usbd_cdc3_receive_mps_buf[RUP32(USBD_CDC3_MAX_PACKET1)]          __ALIGNED(32);
#elif   USBD_CDC3_NCM
#if    (USBD_CDC3_NCM_RAW_ENABLE == 0)
#if    (USBD_CDC3_NCM_NTB_IN_BUF_CNT == 0)
#error Maximum CDC3 NCM IN NTB Data Buffers can not be 0!
#elif  (USBD_CDC3_NCM_NTB_IN_BUF_CNT > 2)
#error Maximum CDC3 NCM IN NTB Data Buffers can not be more than 2!
#endif
#if    (USBD_CDC3_NCM_NTB_OUT_BUF_CNT == 0)
#error Maximum CDC3 NCM OUT NTB Data Buffers can not be 0!
#elif  (USBD_CDC3_NCM_NTB_OUT_BUF_CNT > 2)
#error Maximum CDC3 NCM OUT NTB Data Buffers can not be more than 2!
#endif
#define USBD_CDC3_NCM_NTB_IN_BUF_SIZE    (USBD_CDC3_NCM_NTB_IN_BUF_CNT *USBD_CDC3_NCM_DW_NTB_IN_MAX_SIZE)
#define USBD_CDC3_NCM_NTB_OUT_BUF_SIZE   (USBD_CDC3_NCM_NTB_OUT_BUF_CNT*USBD_CDC3_NCM_DW_NTB_OUT_MAX_SIZE)
extern 
uint8_t usbd_cdc3_send_buf       [USBD_CDC3_NCM_NTB_IN_BUF_SIZE];
uint8_t usbd_cdc3_send_buf       [USBD_CDC3_NCM_NTB_IN_BUF_SIZE]         __ALIGNED(4);
extern 
uint8_t usbd_cdc3_receive_buf    [RUP32(USBD_CDC3_NCM_NTB_OUT_BUF_SIZE)];
uint8_t usbd_cdc3_receive_buf    [RUP32(USBD_CDC3_NCM_NTB_OUT_BUF_SIZE)] __ALIGNED(32);
#else
#define USBD_CDC3_NCM_NTB_IN_BUF_SIZE    (0)
#define USBD_CDC3_NCM_NTB_OUT_BUF_SIZE   (0)
#endif
extern 
uint8_t usbd_cdc3_notify_buf     [16];
uint8_t usbd_cdc3_notify_buf     [16]                                    __ALIGNED(4);
#endif
#endif

#ifdef  RTE_USB_Device_CDC_4
#if     USBD_CDC4_ACM
extern 
uint8_t usbd_cdc4_send_buf       [USBD_CDC4_ACM_SEND_BUF_SIZE];
uint8_t usbd_cdc4_send_buf       [USBD_CDC4_ACM_SEND_BUF_SIZE]           __ALIGNED(4);
extern 
uint8_t usbd_cdc4_receive_buf    [USBD_CDC4_ACM_RECEIVE_BUF_SIZE];
uint8_t usbd_cdc4_receive_buf    [USBD_CDC4_ACM_RECEIVE_BUF_SIZE]        __ALIGNED(4);
extern 
uint8_t usbd_cdc4_notify_buf     [10];
uint8_t usbd_cdc4_notify_buf     [10]                                    __ALIGNED(4);
extern 
uint8_t usbd_cdc4_send_mps_buf   [USBD_CDC4_MAX_PACKET1];
uint8_t usbd_cdc4_send_mps_buf   [USBD_CDC4_MAX_PACKET1]                 __ALIGNED(4);
extern 
uint8_t usbd_cdc4_receive_mps_buf[RUP32(USBD_CDC4_MAX_PACKET1)];
uint8_t usbd_cdc4_receive_mps_buf[RUP32(USBD_CDC4_MAX_PACKET1)]          __ALIGNED(32);
#elif   USBD_CDC4_NCM
#if    (USBD_CDC4_NCM_RAW_ENABLE == 0)
#if    (USBD_CDC4_NCM_NTB_IN_BUF_CNT == 0)
#error Maximum CDC4 NCM IN NTB Data Buffers can not be 0!
#elif  (USBD_CDC4_NCM_NTB_IN_BUF_CNT > 2)
#error Maximum CDC4 NCM IN NTB Data Buffers can not be more than 2!
#endif
#if    (USBD_CDC4_NCM_NTB_OUT_BUF_CNT == 0)
#error Maximum CDC4 NCM OUT NTB Data Buffers can not be 0!
#elif  (USBD_CDC4_NCM_NTB_OUT_BUF_CNT > 2)
#error Maximum CDC4 NCM OUT NTB Data Buffers can not be more than 2!
#endif
#define USBD_CDC4_NCM_NTB_IN_BUF_SIZE    (USBD_CDC4_NCM_NTB_IN_BUF_CNT *USBD_CDC4_NCM_DW_NTB_IN_MAX_SIZE)
#define USBD_CDC4_NCM_NTB_OUT_BUF_SIZE   (USBD_CDC4_NCM_NTB_OUT_BUF_CNT*USBD_CDC4_NCM_DW_NTB_OUT_MAX_SIZE)
extern 
uint8_t usbd_cdc4_send_buf       [USBD_CDC4_NCM_NTB_IN_BUF_SIZE];
uint8_t usbd_cdc4_send_buf       [USBD_CDC4_NCM_NTB_IN_BUF_SIZE]         __ALIGNED(4);
extern 
uint8_t usbd_cdc4_receive_buf    [RUP32(USBD_CDC4_NCM_NTB_OUT_BUF_SIZE)];
uint8_t usbd_cdc4_receive_buf    [RUP32(USBD_CDC4_NCM_NTB_OUT_BUF_SIZE)] __ALIGNED(32);
#else
#define USBD_CDC4_NCM_NTB_IN_BUF_SIZE    (0)
#define USBD_CDC4_NCM_NTB_OUT_BUF_SIZE   (0)
#endif
extern 
uint8_t usbd_cdc4_notify_buf     [16];
uint8_t usbd_cdc4_notify_buf     [16]                                    __ALIGNED(4);
#endif
#endif

#ifdef  RTE_USB_Device_CDC_5
#if     USBD_CDC5_ACM
extern 
uint8_t usbd_cdc5_send_buf       [USBD_CDC5_ACM_SEND_BUF_SIZE];
uint8_t usbd_cdc5_send_buf       [USBD_CDC5_ACM_SEND_BUF_SIZE]           __ALIGNED(4);
extern 
uint8_t usbd_cdc5_receive_buf    [USBD_CDC5_ACM_RECEIVE_BUF_SIZE];
uint8_t usbd_cdc5_receive_buf    [USBD_CDC5_ACM_RECEIVE_BUF_SIZE]        __ALIGNED(4);
extern 
uint8_t usbd_cdc5_notify_buf     [10];
uint8_t usbd_cdc5_notify_buf     [10]                                    __ALIGNED(4);
extern 
uint8_t usbd_cdc5_send_mps_buf   [USBD_CDC5_MAX_PACKET1];
uint8_t usbd_cdc5_send_mps_buf   [USBD_CDC5_MAX_PACKET1]                 __ALIGNED(4);
extern 
uint8_t usbd_cdc5_receive_mps_buf[RUP32(USBD_CDC5_MAX_PACKET1)];
uint8_t usbd_cdc5_receive_mps_buf[RUP32(USBD_CDC5_MAX_PACKET1)]          __ALIGNED(32);
#elif   USBD_CDC5_NCM
#if    (USBD_CDC5_NCM_RAW_ENABLE == 0)
#if    (USBD_CDC5_NCM_NTB_IN_BUF_CNT == 0)
#error Maximum CDC5 NCM IN NTB Data Buffers can not be 0!
#elif  (USBD_CDC5_NCM_NTB_IN_BUF_CNT > 2)
#error Maximum CDC5 NCM IN NTB Data Buffers can not be more than 2!
#endif
#if    (USBD_CDC5_NCM_NTB_OUT_BUF_CNT == 0)
#error Maximum CDC5 NCM OUT NTB Data Buffers can not be 0!
#elif  (USBD_CDC5_NCM_NTB_OUT_BUF_CNT > 2)
#error Maximum CDC5 NCM OUT NTB Data Buffers can not be more than 2!
#endif
#define USBD_CDC5_NCM_NTB_IN_BUF_SIZE    (USBD_CDC5_NCM_NTB_IN_BUF_CNT *USBD_CDC5_NCM_DW_NTB_IN_MAX_SIZE)
#define USBD_CDC5_NCM_NTB_OUT_BUF_SIZE   (USBD_CDC5_NCM_NTB_OUT_BUF_CNT*USBD_CDC5_NCM_DW_NTB_OUT_MAX_SIZE)
extern 
uint8_t usbd_cdc5_send_buf       [USBD_CDC5_NCM_NTB_IN_BUF_SIZE];
uint8_t usbd_cdc5_send_buf       [USBD_CDC5_NCM_NTB_IN_BUF_SIZE]         __ALIGNED(4);
extern 
uint8_t usbd_cdc5_receive_buf    [RUP32(USBD_CDC5_NCM_NTB_OUT_BUF_SIZE)];
uint8_t usbd_cdc5_receive_buf    [RUP32(USBD_CDC5_NCM_NTB_OUT_BUF_SIZE)] __ALIGNED(32);
#else
#define USBD_CDC5_NCM_NTB_IN_BUF_SIZE    (0)
#define USBD_CDC5_NCM_NTB_OUT_BUF_SIZE   (0)
#endif
extern 
uint8_t usbd_cdc5_notify_buf     [16];
uint8_t usbd_cdc5_notify_buf     [16]                                    __ALIGNED(4);
#endif
#endif

#ifdef  RTE_USB_Device_CDC_6
#if     USBD_CDC6_ACM
extern 
uint8_t usbd_cdc6_send_buf       [USBD_CDC6_ACM_SEND_BUF_SIZE];
uint8_t usbd_cdc6_send_buf       [USBD_CDC6_ACM_SEND_BUF_SIZE]           __ALIGNED(4);
extern 
uint8_t usbd_cdc6_receive_buf    [USBD_CDC6_ACM_RECEIVE_BUF_SIZE];
uint8_t usbd_cdc6_receive_buf    [USBD_CDC6_ACM_RECEIVE_BUF_SIZE]        __ALIGNED(4);
extern 
uint8_t usbd_cdc6_notify_buf     [10];
uint8_t usbd_cdc6_notify_buf     [10]                                    __ALIGNED(4);
extern 
uint8_t usbd_cdc6_send_mps_buf   [USBD_CDC6_MAX_PACKET1];
uint8_t usbd_cdc6_send_mps_buf   [USBD_CDC6_MAX_PACKET1]                 __ALIGNED(4);
extern 
uint8_t usbd_cdc6_receive_mps_buf[RUP32(USBD_CDC6_MAX_PACKET1)];
uint8_t usbd_cdc6_receive_mps_buf[RUP32(USBD_CDC6_MAX_PACKET1)]          __ALIGNED(32);
#elif   USBD_CDC6_NCM
#if    (USBD_CDC6_NCM_RAW_ENABLE == 0)
#if    (USBD_CDC6_NCM_NTB_IN_BUF_CNT == 0)
#error Maximum CDC6 NCM IN NTB Data Buffers can not be 0!
#elif  (USBD_CDC6_NCM_NTB_IN_BUF_CNT > 2)
#error Maximum CDC6 NCM IN NTB Data Buffers can not be more than 2!
#endif
#if    (USBD_CDC6_NCM_NTB_OUT_BUF_CNT == 0)
#error Maximum CDC6 NCM OUT NTB Data Buffers can not be 0!
#elif  (USBD_CDC6_NCM_NTB_OUT_BUF_CNT > 2)
#error Maximum CDC6 NCM OUT NTB Data Buffers can not be more than 2!
#endif
#define USBD_CDC6_NCM_NTB_IN_BUF_SIZE    (USBD_CDC6_NCM_NTB_IN_BUF_CNT *USBD_CDC6_NCM_DW_NTB_IN_MAX_SIZE)
#define USBD_CDC6_NCM_NTB_OUT_BUF_SIZE   (USBD_CDC6_NCM_NTB_OUT_BUF_CNT*USBD_CDC6_NCM_DW_NTB_OUT_MAX_SIZE)
extern 
uint8_t usbd_cdc6_send_buf       [USBD_CDC6_NCM_NTB_IN_BUF_SIZE];
uint8_t usbd_cdc6_send_buf       [USBD_CDC6_NCM_NTB_IN_BUF_SIZE]         __ALIGNED(4);
extern 
uint8_t usbd_cdc6_receive_buf    [RUP32(USBD_CDC6_NCM_NTB_OUT_BUF_SIZE)];
uint8_t usbd_cdc6_receive_buf    [RUP32(USBD_CDC6_NCM_NTB_OUT_BUF_SIZE)] __ALIGNED(32);
#else
#define USBD_CDC6_NCM_NTB_IN_BUF_SIZE    (0)
#define USBD_CDC6_NCM_NTB_OUT_BUF_SIZE   (0)
#endif
extern 
uint8_t usbd_cdc6_notify_buf     [16];
uint8_t usbd_cdc6_notify_buf     [16]                                    __ALIGNED(4);
#endif
#endif

#ifdef  RTE_USB_Device_CDC_7
#if     USBD_CDC7_ACM
extern 
uint8_t usbd_cdc7_send_buf       [USBD_CDC7_ACM_SEND_BUF_SIZE];
uint8_t usbd_cdc7_send_buf       [USBD_CDC7_ACM_SEND_BUF_SIZE]           __ALIGNED(4);
extern 
uint8_t usbd_cdc7_receive_buf    [USBD_CDC7_ACM_RECEIVE_BUF_SIZE];
uint8_t usbd_cdc7_receive_buf    [USBD_CDC7_ACM_RECEIVE_BUF_SIZE]        __ALIGNED(4);
extern 
uint8_t usbd_cdc7_notify_buf     [10];
uint8_t usbd_cdc7_notify_buf     [10]                                    __ALIGNED(4);
extern 
uint8_t usbd_cdc7_send_mps_buf   [USBD_CDC7_MAX_PACKET1];
uint8_t usbd_cdc7_send_mps_buf   [USBD_CDC7_MAX_PACKET1]                 __ALIGNED(4);
extern 
uint8_t usbd_cdc7_receive_mps_buf[RUP32(USBD_CDC7_MAX_PACKET1)];
uint8_t usbd_cdc7_receive_mps_buf[RUP32(USBD_CDC7_MAX_PACKET1)]          __ALIGNED(32);
#elif   USBD_CDC7_NCM
#if    (USBD_CDC7_NCM_RAW_ENABLE == 0)
#if    (USBD_CDC7_NCM_NTB_IN_BUF_CNT == 0)
#error Maximum CDC7 NCM IN NTB Data Buffers can not be 0!
#elif  (USBD_CDC7_NCM_NTB_IN_BUF_CNT > 2)
#error Maximum CDC7 NCM IN NTB Data Buffers can not be more than 2!
#endif
#if    (USBD_CDC7_NCM_NTB_OUT_BUF_CNT == 0)
#error Maximum CDC7 NCM OUT NTB Data Buffers can not be 0!
#elif  (USBD_CDC7_NCM_NTB_OUT_BUF_CNT > 2)
#error Maximum CDC7 NCM OUT NTB Data Buffers can not be more than 2!
#endif
#define USBD_CDC7_NCM_NTB_IN_BUF_SIZE    (USBD_CDC7_NCM_NTB_IN_BUF_CNT *USBD_CDC7_NCM_DW_NTB_IN_MAX_SIZE)
#define USBD_CDC7_NCM_NTB_OUT_BUF_SIZE   (USBD_CDC7_NCM_NTB_OUT_BUF_CNT*USBD_CDC7_NCM_DW_NTB_OUT_MAX_SIZE)
extern 
uint8_t usbd_cdc7_send_buf       [USBD_CDC7_NCM_NTB_IN_BUF_SIZE];
uint8_t usbd_cdc7_send_buf       [USBD_CDC7_NCM_NTB_IN_BUF_SIZE]         __ALIGNED(4);
extern 
uint8_t usbd_cdc7_receive_buf    [RUP32(USBD_CDC7_NCM_NTB_OUT_BUF_SIZE)];
uint8_t usbd_cdc7_receive_buf    [RUP32(USBD_CDC7_NCM_NTB_OUT_BUF_SIZE)] __ALIGNED(32);
#else
#define USBD_CDC7_NCM_NTB_IN_BUF_SIZE    (0)
#define USBD_CDC7_NCM_NTB_OUT_BUF_SIZE   (0)
#endif
extern 
uint8_t usbd_cdc7_notify_buf     [16];
uint8_t usbd_cdc7_notify_buf     [16]                                    __ALIGNED(4);
#endif
#endif

extern 
const uint8_t   usbd_cdc_num;
const uint8_t   usbd_cdc_num =          USBD_CDC_NUM;
extern 
usbd_cdc_data_t usbd_cdc_data          [USBD_CDC_NUM];
usbd_cdc_data_t usbd_cdc_data          [USBD_CDC_NUM];
extern 
usbd_cdc_t      usbd_cdc               [USBD_CDC_NUM];
usbd_cdc_t      usbd_cdc               [USBD_CDC_NUM] = {
#ifdef  RTE_USB_Device_CDC_0
                                      {
#if   ((USBD_CDC0_NCM != 0) && (USBD_CDC0_NCM_RAW_ENABLE == 1))
                                        NULL,
                                        NULL,
#else
                                        usbd_cdc0_send_buf,
                                        usbd_cdc0_receive_buf,
#endif
                                        usbd_cdc0_notify_buf,
#if    (USBD_CDC0_NCM != 0)
                                        NULL,
                                        NULL,
#else
                                        usbd_cdc0_send_mps_buf,
                                        usbd_cdc0_receive_mps_buf,
#endif
                                       &usbd_cdc_data[0],
                                        USBD_CDC0_DEV,
                                        USBD_CDC0_ACM,
                                        USBD_CDC0_RNDIS,
                                        USBD_CDC0_IF0_NUM,
                                        USBD_CDC0_IF1_NUM,
                                        USBD_CDC0_EP_INT_IN,
                                        USBD_CDC0_EP_BULK_IN,
                                        USBD_CDC0_EP_BULK_OUT,
#if     USBD_CDC0_ACM
                                        USBD_CDC0_ACM_SEND_BUF_SIZE,
                                        USBD_CDC0_ACM_RECEIVE_BUF_SIZE,
#elif   USBD_CDC0_NCM
                                        USBD_CDC0_NCM_NTB_IN_BUF_SIZE,
                                        USBD_CDC0_NCM_NTB_OUT_BUF_SIZE,
#endif
                                      { USBD_CDC0_WMAXPACKETSIZE,  USBD_CDC0_HS_WMAXPACKETSIZE  },
                                      { USBD_CDC0_WMAXPACKETSIZE1, USBD_CDC0_HS_WMAXPACKETSIZE1 },
#if     USBD_CDC0_ACM
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0
#elif   USBD_CDC0_NCM
                                        USBD_CDC0_NCM_RAW_ENABLE,
                                        USBD_CDC0_NCM_NTB_IN_BUF_CNT,
                                        USBD_CDC0_NCM_NTB_OUT_BUF_CNT,
                                        USBD_CDC0_NCM_BM_NTB_FORMATS_SUPPORTED,
                                        USBD_CDC0_NCM_DW_NTB_IN_MAX_SIZE,
                                        USBD_CDC0_NCM_DW_NTB_OUT_MAX_SIZE,
                                        USBD_CDC0_NCM_W_MAX_SEGMENT_SIZE,
                                        USBD_CDC0_NCM_W_NDP_IN_DIVISOR,
                                        USBD_CDC0_NCM_W_NDP_IN_PAYLOAD_REMINDER,
                                        USBD_CDC0_NCM_W_NDP_IN_ALIGNMENT,
                                        USBD_CDC0_NCM_W_NDP_OUT_DIVISOR,
                                        USBD_CDC0_NCM_W_NDP_OUT_PAYLOAD_REMINDER,
                                        USBD_CDC0_NCM_W_NDP_OUT_ALIGNMENT,
                                        0
#endif
                                      }
#endif
#ifdef  RTE_USB_Device_CDC_1
                                    , {
#if   ((USBD_CDC1_NCM != 0) && (USBD_CDC1_NCM_RAW_ENABLE == 1))
                                        NULL,
                                        NULL,
#else
                                        usbd_cdc1_send_buf,
                                        usbd_cdc1_receive_buf,
#endif
                                        usbd_cdc1_notify_buf,
#if    (USBD_CDC1_NCM != 0)
                                        NULL,
                                        NULL,
#else
                                        usbd_cdc1_send_mps_buf,
                                        usbd_cdc1_receive_mps_buf,
#endif
                                       &usbd_cdc_data[1],
                                        USBD_CDC1_DEV,
                                        USBD_CDC1_ACM,
                                        USBD_CDC1_RNDIS,
                                        USBD_CDC1_IF0_NUM,
                                        USBD_CDC1_IF1_NUM,
                                        USBD_CDC1_EP_INT_IN,
                                        USBD_CDC1_EP_BULK_IN,
                                        USBD_CDC1_EP_BULK_OUT,
#if     USBD_CDC1_ACM
                                        USBD_CDC1_ACM_SEND_BUF_SIZE,
                                        USBD_CDC1_ACM_RECEIVE_BUF_SIZE,
#elif   USBD_CDC1_NCM
                                        USBD_CDC1_NCM_NTB_IN_BUF_SIZE,
                                        USBD_CDC1_NCM_NTB_OUT_BUF_SIZE,
#endif
                                      { USBD_CDC1_WMAXPACKETSIZE,  USBD_CDC1_HS_WMAXPACKETSIZE  },
                                      { USBD_CDC1_WMAXPACKETSIZE1, USBD_CDC1_HS_WMAXPACKETSIZE1 },
#if     USBD_CDC1_ACM
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0
#elif   USBD_CDC1_NCM
                                        USBD_CDC1_NCM_RAW_ENABLE,
                                        USBD_CDC1_NCM_NTB_IN_BUF_CNT,
                                        USBD_CDC1_NCM_NTB_OUT_BUF_CNT,
                                        USBD_CDC1_NCM_BM_NTB_FORMATS_SUPPORTED,
                                        USBD_CDC1_NCM_DW_NTB_IN_MAX_SIZE,
                                        USBD_CDC1_NCM_DW_NTB_OUT_MAX_SIZE,
                                        USBD_CDC1_NCM_W_MAX_SEGMENT_SIZE,
                                        USBD_CDC1_NCM_W_NDP_IN_DIVISOR,
                                        USBD_CDC1_NCM_W_NDP_IN_PAYLOAD_REMINDER,
                                        USBD_CDC1_NCM_W_NDP_IN_ALIGNMENT,
                                        USBD_CDC1_NCM_W_NDP_OUT_DIVISOR,
                                        USBD_CDC1_NCM_W_NDP_OUT_PAYLOAD_REMINDER,
                                        USBD_CDC1_NCM_W_NDP_OUT_ALIGNMENT,
                                        0
#endif
                                      }
#endif
#ifdef  RTE_USB_Device_CDC_2
                                    , {
#if   ((USBD_CDC2_NCM != 0) && (USBD_CDC2_NCM_RAW_ENABLE == 1))
                                        NULL,
                                        NULL,
#else
                                        usbd_cdc2_send_buf,
                                        usbd_cdc2_receive_buf,
#endif
                                        usbd_cdc2_notify_buf,
#if    (USBD_CDC2_NCM != 0)
                                        NULL,
                                        NULL,
#else
                                        usbd_cdc2_send_mps_buf,
                                        usbd_cdc2_receive_mps_buf,
#endif
                                       &usbd_cdc_data[2],
                                        USBD_CDC2_DEV,
                                        USBD_CDC2_ACM,
                                        USBD_CDC2_RNDIS,
                                        USBD_CDC2_IF0_NUM,
                                        USBD_CDC2_IF1_NUM,
                                        USBD_CDC2_EP_INT_IN,
                                        USBD_CDC2_EP_BULK_IN,
                                        USBD_CDC2_EP_BULK_OUT,
#if     USBD_CDC2_ACM
                                        USBD_CDC2_ACM_SEND_BUF_SIZE,
                                        USBD_CDC2_ACM_RECEIVE_BUF_SIZE,
#elif   USBD_CDC2_NCM
                                        USBD_CDC2_NCM_NTB_IN_BUF_SIZE,
                                        USBD_CDC2_NCM_NTB_OUT_BUF_SIZE,
#endif
                                      { USBD_CDC2_WMAXPACKETSIZE,  USBD_CDC2_HS_WMAXPACKETSIZE  },
                                      { USBD_CDC2_WMAXPACKETSIZE1, USBD_CDC2_HS_WMAXPACKETSIZE1 },
#if     USBD_CDC2_ACM
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0
#elif   USBD_CDC2_NCM
                                        USBD_CDC2_NCM_RAW_ENABLE,
                                        USBD_CDC2_NCM_NTB_IN_BUF_CNT,
                                        USBD_CDC2_NCM_NTB_OUT_BUF_CNT,
                                        USBD_CDC2_NCM_BM_NTB_FORMATS_SUPPORTED,
                                        USBD_CDC2_NCM_DW_NTB_IN_MAX_SIZE,
                                        USBD_CDC2_NCM_DW_NTB_OUT_MAX_SIZE,
                                        USBD_CDC2_NCM_W_MAX_SEGMENT_SIZE,
                                        USBD_CDC2_NCM_W_NDP_IN_DIVISOR,
                                        USBD_CDC2_NCM_W_NDP_IN_PAYLOAD_REMINDER,
                                        USBD_CDC2_NCM_W_NDP_IN_ALIGNMENT,
                                        USBD_CDC2_NCM_W_NDP_OUT_DIVISOR,
                                        USBD_CDC2_NCM_W_NDP_OUT_PAYLOAD_REMINDER,
                                        USBD_CDC2_NCM_W_NDP_OUT_ALIGNMENT,
                                        0
#endif
                                      }
#endif
#ifdef  RTE_USB_Device_CDC_3
                                    , {
#if   ((USBD_CDC3_NCM != 0) && (USBD_CDC3_NCM_RAW_ENABLE == 1))
                                        NULL,
                                        NULL,
#else
                                        usbd_cdc3_send_buf,
                                        usbd_cdc3_receive_buf,
#endif
                                        usbd_cdc3_notify_buf,
#if    (USBD_CDC3_NCM != 0)
                                        NULL,
                                        NULL,
#else
                                        usbd_cdc3_send_mps_buf,
                                        usbd_cdc3_receive_mps_buf,
#endif
                                       &usbd_cdc_data[3],
                                        USBD_CDC3_DEV,
                                        USBD_CDC3_ACM,
                                        USBD_CDC3_RNDIS,
                                        USBD_CDC3_IF0_NUM,
                                        USBD_CDC3_IF1_NUM,
                                        USBD_CDC3_EP_INT_IN,
                                        USBD_CDC3_EP_BULK_IN,
                                        USBD_CDC3_EP_BULK_OUT,
#if     USBD_CDC3_ACM
                                        USBD_CDC3_ACM_SEND_BUF_SIZE,
                                        USBD_CDC3_ACM_RECEIVE_BUF_SIZE,
#elif   USBD_CDC3_NCM
                                        USBD_CDC3_NCM_NTB_IN_BUF_SIZE,
                                        USBD_CDC3_NCM_NTB_OUT_BUF_SIZE,
#endif
                                      { USBD_CDC3_WMAXPACKETSIZE,  USBD_CDC3_HS_WMAXPACKETSIZE  },
                                      { USBD_CDC3_WMAXPACKETSIZE1, USBD_CDC3_HS_WMAXPACKETSIZE1 },
#if     USBD_CDC3_ACM
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0
#elif   USBD_CDC3_NCM
                                        USBD_CDC3_NCM_RAW_ENABLE,
                                        USBD_CDC3_NCM_NTB_IN_BUF_CNT,
                                        USBD_CDC3_NCM_NTB_OUT_BUF_CNT,
                                        USBD_CDC3_NCM_BM_NTB_FORMATS_SUPPORTED,
                                        USBD_CDC3_NCM_DW_NTB_IN_MAX_SIZE,
                                        USBD_CDC3_NCM_DW_NTB_OUT_MAX_SIZE,
                                        USBD_CDC3_NCM_W_MAX_SEGMENT_SIZE,
                                        USBD_CDC3_NCM_W_NDP_IN_DIVISOR,
                                        USBD_CDC3_NCM_W_NDP_IN_PAYLOAD_REMINDER,
                                        USBD_CDC3_NCM_W_NDP_IN_ALIGNMENT,
                                        USBD_CDC3_NCM_W_NDP_OUT_DIVISOR,
                                        USBD_CDC3_NCM_W_NDP_OUT_PAYLOAD_REMINDER,
                                        USBD_CDC3_NCM_W_NDP_OUT_ALIGNMENT,
                                        0
#endif
                                      }
#endif
#ifdef  RTE_USB_Device_CDC_4
                                    , {
#if   ((USBD_CDC4_NCM != 0) && (USBD_CDC4_NCM_RAW_ENABLE == 1))
                                        NULL,
                                        NULL,
#else
                                        usbd_cdc4_send_buf,
                                        usbd_cdc4_receive_buf,
#endif
                                        usbd_cdc4_notify_buf,
#if    (USBD_CDC4_NCM != 0)
                                        NULL,
                                        NULL,
#else
                                        usbd_cdc4_send_mps_buf,
                                        usbd_cdc4_receive_mps_buf,
#endif
                                       &usbd_cdc_data[4],
                                        USBD_CDC4_DEV,
                                        USBD_CDC4_ACM,
                                        USBD_CDC4_RNDIS,
                                        USBD_CDC4_IF0_NUM,
                                        USBD_CDC4_IF1_NUM,
                                        USBD_CDC4_EP_INT_IN,
                                        USBD_CDC4_EP_BULK_IN,
                                        USBD_CDC4_EP_BULK_OUT,
#if     USBD_CDC4_ACM
                                        USBD_CDC4_ACM_SEND_BUF_SIZE,
                                        USBD_CDC4_ACM_RECEIVE_BUF_SIZE,
#elif   USBD_CDC4_NCM
                                        USBD_CDC4_NCM_NTB_IN_BUF_SIZE,
                                        USBD_CDC4_NCM_NTB_OUT_BUF_SIZE,
#endif
                                      { USBD_CDC4_WMAXPACKETSIZE,  USBD_CDC4_HS_WMAXPACKETSIZE  },
                                      { USBD_CDC4_WMAXPACKETSIZE1, USBD_CDC4_HS_WMAXPACKETSIZE1 },
#if     USBD_CDC4_ACM
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0
#elif   USBD_CDC4_NCM
                                        USBD_CDC4_NCM_RAW_ENABLE,
                                        USBD_CDC4_NCM_NTB_IN_BUF_CNT,
                                        USBD_CDC4_NCM_NTB_OUT_BUF_CNT,
                                        USBD_CDC4_NCM_BM_NTB_FORMATS_SUPPORTED,
                                        USBD_CDC4_NCM_DW_NTB_IN_MAX_SIZE,
                                        USBD_CDC4_NCM_DW_NTB_OUT_MAX_SIZE,
                                        USBD_CDC4_NCM_W_MAX_SEGMENT_SIZE,
                                        USBD_CDC4_NCM_W_NDP_IN_DIVISOR,
                                        USBD_CDC4_NCM_W_NDP_IN_PAYLOAD_REMINDER,
                                        USBD_CDC4_NCM_W_NDP_IN_ALIGNMENT,
                                        USBD_CDC4_NCM_W_NDP_OUT_DIVISOR,
                                        USBD_CDC4_NCM_W_NDP_OUT_PAYLOAD_REMINDER,
                                        USBD_CDC4_NCM_W_NDP_OUT_ALIGNMENT,
                                        0
#endif
                                      }
#endif
#ifdef  RTE_USB_Device_CDC_5
                                    , {
#if   ((USBD_CDC5_NCM != 0) && (USBD_CDC5_NCM_RAW_ENABLE == 1))
                                        NULL,
                                        NULL,
#else
                                        usbd_cdc5_send_buf,
                                        usbd_cdc5_receive_buf,
#endif
                                        usbd_cdc5_notify_buf,
#if    (USBD_CDC5_NCM != 0)
                                        NULL,
                                        NULL,
#else
                                        usbd_cdc5_send_mps_buf,
                                        usbd_cdc5_receive_mps_buf,
#endif
                                       &usbd_cdc_data[5],
                                        USBD_CDC5_DEV,
                                        USBD_CDC5_ACM,
                                        USBD_CDC5_RNDIS,
                                        USBD_CDC5_IF0_NUM,
                                        USBD_CDC5_IF1_NUM,
                                        USBD_CDC5_EP_INT_IN,
                                        USBD_CDC5_EP_BULK_IN,
                                        USBD_CDC5_EP_BULK_OUT,
#if     USBD_CDC5_ACM
                                        USBD_CDC5_ACM_SEND_BUF_SIZE,
                                        USBD_CDC5_ACM_RECEIVE_BUF_SIZE,
#elif   USBD_CDC5_NCM
                                        USBD_CDC5_NCM_NTB_IN_BUF_SIZE,
                                        USBD_CDC5_NCM_NTB_OUT_BUF_SIZE,
#endif
                                      { USBD_CDC5_WMAXPACKETSIZE,  USBD_CDC5_HS_WMAXPACKETSIZE  },
                                      { USBD_CDC5_WMAXPACKETSIZE1, USBD_CDC5_HS_WMAXPACKETSIZE1 },
#if     USBD_CDC5_ACM
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0
#elif   USBD_CDC5_NCM
                                        USBD_CDC5_NCM_RAW_ENABLE,
                                        USBD_CDC5_NCM_NTB_IN_BUF_CNT,
                                        USBD_CDC5_NCM_NTB_OUT_BUF_CNT,
                                        USBD_CDC5_NCM_BM_NTB_FORMATS_SUPPORTED,
                                        USBD_CDC5_NCM_DW_NTB_IN_MAX_SIZE,
                                        USBD_CDC5_NCM_DW_NTB_OUT_MAX_SIZE,
                                        USBD_CDC5_NCM_W_MAX_SEGMENT_SIZE,
                                        USBD_CDC5_NCM_W_NDP_IN_DIVISOR,
                                        USBD_CDC5_NCM_W_NDP_IN_PAYLOAD_REMINDER,
                                        USBD_CDC5_NCM_W_NDP_IN_ALIGNMENT,
                                        USBD_CDC5_NCM_W_NDP_OUT_DIVISOR,
                                        USBD_CDC5_NCM_W_NDP_OUT_PAYLOAD_REMINDER,
                                        USBD_CDC5_NCM_W_NDP_OUT_ALIGNMENT,
                                        0
#endif
                                      }
#endif
#ifdef  RTE_USB_Device_CDC_6
                                    , {
#if   ((USBD_CDC6_NCM != 0) && (USBD_CDC6_NCM_RAW_ENABLE == 1))
                                        NULL,
                                        NULL,
#else
                                        usbd_cdc6_send_buf,
                                        usbd_cdc6_receive_buf,
#endif
                                        usbd_cdc6_notify_buf,
#if    (USBD_CDC6_NCM != 0)
                                        NULL,
                                        NULL,
#else
                                        usbd_cdc6_send_mps_buf,
                                        usbd_cdc6_receive_mps_buf,
#endif
                                       &usbd_cdc_data[6],
                                        USBD_CDC6_DEV,
                                        USBD_CDC6_ACM,
                                        USBD_CDC6_RNDIS,
                                        USBD_CDC6_IF0_NUM,
                                        USBD_CDC6_IF1_NUM,
                                        USBD_CDC6_EP_INT_IN,
                                        USBD_CDC6_EP_BULK_IN,
                                        USBD_CDC6_EP_BULK_OUT,
#if     USBD_CDC6_ACM
                                        USBD_CDC6_ACM_SEND_BUF_SIZE,
                                        USBD_CDC6_ACM_RECEIVE_BUF_SIZE,
#elif   USBD_CDC6_NCM
                                        USBD_CDC6_NCM_NTB_IN_BUF_SIZE,
                                        USBD_CDC6_NCM_NTB_OUT_BUF_SIZE,
#endif
                                      { USBD_CDC6_WMAXPACKETSIZE,  USBD_CDC6_HS_WMAXPACKETSIZE  },
                                      { USBD_CDC6_WMAXPACKETSIZE1, USBD_CDC6_HS_WMAXPACKETSIZE1 },
#if     USBD_CDC6_ACM
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0
#elif   USBD_CDC6_NCM
                                        USBD_CDC6_NCM_RAW_ENABLE,
                                        USBD_CDC6_NCM_NTB_IN_BUF_CNT,
                                        USBD_CDC6_NCM_NTB_OUT_BUF_CNT,
                                        USBD_CDC6_NCM_BM_NTB_FORMATS_SUPPORTED,
                                        USBD_CDC6_NCM_DW_NTB_IN_MAX_SIZE,
                                        USBD_CDC6_NCM_DW_NTB_OUT_MAX_SIZE,
                                        USBD_CDC6_NCM_W_MAX_SEGMENT_SIZE,
                                        USBD_CDC6_NCM_W_NDP_IN_DIVISOR,
                                        USBD_CDC6_NCM_W_NDP_IN_PAYLOAD_REMINDER,
                                        USBD_CDC6_NCM_W_NDP_IN_ALIGNMENT,
                                        USBD_CDC6_NCM_W_NDP_OUT_DIVISOR,
                                        USBD_CDC6_NCM_W_NDP_OUT_PAYLOAD_REMINDER,
                                        USBD_CDC6_NCM_W_NDP_OUT_ALIGNMENT,
                                        0
#endif
                                      }
#endif
#ifdef  RTE_USB_Device_CDC_7
                                    , {
#if   ((USBD_CDC7_NCM != 0) && (USBD_CDC7_NCM_RAW_ENABLE == 1))
                                        NULL,
                                        NULL,
#else
                                        usbd_cdc7_send_buf,
                                        usbd_cdc7_receive_buf,
#endif
                                        usbd_cdc7_notify_buf,
#if    (USBD_CDC7_NCM != 0)
                                        NULL,
                                        NULL,
#else
                                        usbd_cdc7_send_mps_buf,
                                        usbd_cdc7_receive_mps_buf,
#endif
                                       &usbd_cdc_data[7],
                                        USBD_CDC7_DEV,
                                        USBD_CDC7_ACM,
                                        USBD_CDC7_RNDIS,
                                        USBD_CDC7_IF0_NUM,
                                        USBD_CDC7_IF1_NUM,
                                        USBD_CDC7_EP_INT_IN,
                                        USBD_CDC7_EP_BULK_IN,
                                        USBD_CDC7_EP_BULK_OUT,
#if     USBD_CDC7_ACM
                                        USBD_CDC7_ACM_SEND_BUF_SIZE,
                                        USBD_CDC7_ACM_RECEIVE_BUF_SIZE,
#elif   USBD_CDC7_NCM
                                        USBD_CDC7_NCM_NTB_IN_BUF_SIZE,
                                        USBD_CDC7_NCM_NTB_OUT_BUF_SIZE,
#endif
                                      { USBD_CDC7_WMAXPACKETSIZE,  USBD_CDC7_HS_WMAXPACKETSIZE  },
                                      { USBD_CDC7_WMAXPACKETSIZE1, USBD_CDC7_HS_WMAXPACKETSIZE1 },
#if     USBD_CDC7_ACM
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0
#elif   USBD_CDC7_NCM
                                        USBD_CDC7_NCM_RAW_ENABLE,
                                        USBD_CDC7_NCM_NTB_IN_BUF_CNT,
                                        USBD_CDC7_NCM_NTB_OUT_BUF_CNT,
                                        USBD_CDC7_NCM_BM_NTB_FORMATS_SUPPORTED,
                                        USBD_CDC7_NCM_DW_NTB_IN_MAX_SIZE,
                                        USBD_CDC7_NCM_DW_NTB_OUT_MAX_SIZE,
                                        USBD_CDC7_NCM_W_MAX_SEGMENT_SIZE,
                                        USBD_CDC7_NCM_W_NDP_IN_DIVISOR,
                                        USBD_CDC7_NCM_W_NDP_IN_PAYLOAD_REMINDER,
                                        USBD_CDC7_NCM_W_NDP_IN_ALIGNMENT,
                                        USBD_CDC7_NCM_W_NDP_OUT_DIVISOR,
                                        USBD_CDC7_NCM_W_NDP_OUT_PAYLOAD_REMINDER,
                                        USBD_CDC7_NCM_W_NDP_OUT_ALIGNMENT,
                                        0
#endif
                                      }
#endif
};
extern usbd_cdc_t * const usbd_cdc_ptr[USBD_CDC_NUM];
       usbd_cdc_t * const usbd_cdc_ptr[USBD_CDC_NUM] = {
#ifdef  RTE_USB_Device_CDC_0
       &usbd_cdc[0]
#endif
#ifdef  RTE_USB_Device_CDC_1
     , &usbd_cdc[1]
#endif
#ifdef  RTE_USB_Device_CDC_2
     , &usbd_cdc[2]
#endif
#ifdef  RTE_USB_Device_CDC_3
     , &usbd_cdc[3]
#endif
#ifdef  RTE_USB_Device_CDC_4
     , &usbd_cdc[4]
#endif
#ifdef  RTE_USB_Device_CDC_5
     , &usbd_cdc[5]
#endif
#ifdef  RTE_USB_Device_CDC_6
     , &usbd_cdc[6]
#endif
#ifdef  RTE_USB_Device_CDC_7
     , &usbd_cdc[7]
#endif
};

#endif


/*------------------------------------------------------------------------------
 *      USB Device HID 0..7 Configurations
 *----------------------------------------------------------------------------*/

#define USBD_HID0_MAX_PACKET           (MAX_OF_2(USBD_HID0_WMAXPACKETSIZE, USBD_HID0_HS_WMAXPACKETSIZE))
#define USBD_HID1_MAX_PACKET           (MAX_OF_2(USBD_HID1_WMAXPACKETSIZE, USBD_HID1_HS_WMAXPACKETSIZE))
#define USBD_HID2_MAX_PACKET           (MAX_OF_2(USBD_HID2_WMAXPACKETSIZE, USBD_HID2_HS_WMAXPACKETSIZE))
#define USBD_HID3_MAX_PACKET           (MAX_OF_2(USBD_HID3_WMAXPACKETSIZE, USBD_HID3_HS_WMAXPACKETSIZE))
#define USBD_HID4_MAX_PACKET           (MAX_OF_2(USBD_HID4_WMAXPACKETSIZE, USBD_HID4_HS_WMAXPACKETSIZE))
#define USBD_HID5_MAX_PACKET           (MAX_OF_2(USBD_HID5_WMAXPACKETSIZE, USBD_HID5_HS_WMAXPACKETSIZE))
#define USBD_HID6_MAX_PACKET           (MAX_OF_2(USBD_HID6_WMAXPACKETSIZE, USBD_HID6_HS_WMAXPACKETSIZE))
#define USBD_HID7_MAX_PACKET           (MAX_OF_2(USBD_HID7_WMAXPACKETSIZE, USBD_HID7_HS_WMAXPACKETSIZE))

#ifdef  RTE_USB_Device_HID_0
#define USBD_HID0_NUM                   1
#else
#define USBD_HID0_NUM                   0
#endif
#ifdef  RTE_USB_Device_HID_1
#define USBD_HID1_NUM                   1
#else
#define USBD_HID1_NUM                   0
#endif
#ifdef  RTE_USB_Device_HID_2
#define USBD_HID2_NUM                   1
#else
#define USBD_HID2_NUM                   0
#endif
#ifdef  RTE_USB_Device_HID_3
#define USBD_HID3_NUM                   1
#else
#define USBD_HID3_NUM                   0
#endif
#ifdef  RTE_USB_Device_HID_4
#define USBD_HID4_NUM                   1
#else
#define USBD_HID4_NUM                   0
#endif
#ifdef  RTE_USB_Device_HID_5
#define USBD_HID5_NUM                   1
#else
#define USBD_HID5_NUM                   0
#endif
#ifdef  RTE_USB_Device_HID_6
#define USBD_HID6_NUM                   1
#else
#define USBD_HID6_NUM                   0
#endif
#ifdef  RTE_USB_Device_HID_7
#define USBD_HID7_NUM                   1
#else
#define USBD_HID7_NUM                   0
#endif

#define USBD_HID_NUM                   (USBD_HID0_NUM + USBD_HID1_NUM + USBD_HID2_NUM + USBD_HID3_NUM + USBD_HID4_NUM + USBD_HID5_NUM + USBD_HID6_NUM + USBD_HID7_NUM)

#if    (USBD_HID_NUM > 0)

extern void *usbd_hid_thread_id   [USBD_HID_NUM];
       void *usbd_hid_thread_id   [USBD_HID_NUM];

extern void *usbd_hid_timer_id    [USBD_HID_NUM];
       void *usbd_hid_timer_id    [USBD_HID_NUM];

extern void *usbd_hid_semaphore_id[USBD_HID_NUM];
       void *usbd_hid_semaphore_id[USBD_HID_NUM];

#ifdef  RTE_USB_Device_HID_0
extern 
uint16_t usbd_hid0_idle_count [USBD_HID0_IN_REPORT_NUM];
uint16_t usbd_hid0_idle_count [USBD_HID0_IN_REPORT_NUM];
extern 
uint16_t usbd_hid0_idle_reload[USBD_HID0_IN_REPORT_NUM];
uint16_t usbd_hid0_idle_reload[USBD_HID0_IN_REPORT_NUM];
extern 
uint8_t  usbd_hid0_idle_set   [USBD_HID0_IN_REPORT_NUM];
uint8_t  usbd_hid0_idle_set   [USBD_HID0_IN_REPORT_NUM];
extern 
uint8_t  usbd_hid0_in_report  [USBD_HID0_IN_REPORT_MAX_SZ  +(USBD_HID0_IN_REPORT_MAX_SZ>1)];
uint8_t  usbd_hid0_in_report  [USBD_HID0_IN_REPORT_MAX_SZ  +(USBD_HID0_IN_REPORT_MAX_SZ>1)]         __ALIGNED(4);
extern 
uint8_t  usbd_hid0_out_report [RUP32(USBD_HID0_OUT_REPORT_MAX_SZ +(USBD_HID0_OUT_REPORT_MAX_SZ>1))];
uint8_t  usbd_hid0_out_report [RUP32(USBD_HID0_OUT_REPORT_MAX_SZ +(USBD_HID0_OUT_REPORT_MAX_SZ>1))] __ALIGNED(32);
extern 
uint8_t  usbd_hid0_feat_report[USBD_HID0_FEAT_REPORT_MAX_SZ+(USBD_HID0_FEAT_REPORT_MAX_SZ>1)];
uint8_t  usbd_hid0_feat_report[USBD_HID0_FEAT_REPORT_MAX_SZ+(USBD_HID0_FEAT_REPORT_MAX_SZ>1)]       __ALIGNED(4);
#endif

#ifdef  RTE_USB_Device_HID_1
extern 
uint16_t usbd_hid1_idle_count [USBD_HID1_IN_REPORT_NUM];
uint16_t usbd_hid1_idle_count [USBD_HID1_IN_REPORT_NUM];
extern 
uint16_t usbd_hid1_idle_reload[USBD_HID1_IN_REPORT_NUM];
uint16_t usbd_hid1_idle_reload[USBD_HID1_IN_REPORT_NUM];
extern 
uint8_t  usbd_hid1_idle_set   [USBD_HID1_IN_REPORT_NUM];
uint8_t  usbd_hid1_idle_set   [USBD_HID1_IN_REPORT_NUM];
extern 
uint8_t  usbd_hid1_in_report  [USBD_HID1_IN_REPORT_MAX_SZ  +(USBD_HID1_IN_REPORT_MAX_SZ>1)];
uint8_t  usbd_hid1_in_report  [USBD_HID1_IN_REPORT_MAX_SZ  +(USBD_HID1_IN_REPORT_MAX_SZ>1)]         __ALIGNED(4);
extern 
uint8_t  usbd_hid1_out_report [RUP32(USBD_HID1_OUT_REPORT_MAX_SZ +(USBD_HID1_OUT_REPORT_MAX_SZ>1))];
uint8_t  usbd_hid1_out_report [RUP32(USBD_HID1_OUT_REPORT_MAX_SZ +(USBD_HID1_OUT_REPORT_MAX_SZ>1))] __ALIGNED(32);
extern 
uint8_t  usbd_hid1_feat_report[USBD_HID1_FEAT_REPORT_MAX_SZ+(USBD_HID1_FEAT_REPORT_MAX_SZ>1)];
uint8_t  usbd_hid1_feat_report[USBD_HID1_FEAT_REPORT_MAX_SZ+(USBD_HID1_FEAT_REPORT_MAX_SZ>1)]       __ALIGNED(4);
#endif

#ifdef  RTE_USB_Device_HID_2
extern 
uint16_t usbd_hid2_idle_count [USBD_HID2_IN_REPORT_NUM];
uint16_t usbd_hid2_idle_count [USBD_HID2_IN_REPORT_NUM];
extern 
uint16_t usbd_hid2_idle_reload[USBD_HID2_IN_REPORT_NUM];
uint16_t usbd_hid2_idle_reload[USBD_HID2_IN_REPORT_NUM];
extern 
uint8_t  usbd_hid2_idle_set   [USBD_HID2_IN_REPORT_NUM];
uint8_t  usbd_hid2_idle_set   [USBD_HID2_IN_REPORT_NUM];
extern 
uint8_t  usbd_hid2_in_report  [USBD_HID2_IN_REPORT_MAX_SZ  +(USBD_HID2_IN_REPORT_MAX_SZ>1)];
uint8_t  usbd_hid2_in_report  [USBD_HID2_IN_REPORT_MAX_SZ  +(USBD_HID2_IN_REPORT_MAX_SZ>1)]         __ALIGNED(4);
extern 
uint8_t  usbd_hid2_out_report [RUP32(USBD_HID2_OUT_REPORT_MAX_SZ +(USBD_HID2_OUT_REPORT_MAX_SZ>1))];
uint8_t  usbd_hid2_out_report [RUP32(USBD_HID2_OUT_REPORT_MAX_SZ +(USBD_HID2_OUT_REPORT_MAX_SZ>1))] __ALIGNED(32);
extern 
uint8_t  usbd_hid2_feat_report[USBD_HID2_FEAT_REPORT_MAX_SZ+(USBD_HID2_FEAT_REPORT_MAX_SZ>1)];
uint8_t  usbd_hid2_feat_report[USBD_HID2_FEAT_REPORT_MAX_SZ+(USBD_HID2_FEAT_REPORT_MAX_SZ>1)]       __ALIGNED(4);
#endif

#ifdef  RTE_USB_Device_HID_3
extern 
uint16_t usbd_hid3_idle_count [USBD_HID3_IN_REPORT_NUM];
uint16_t usbd_hid3_idle_count [USBD_HID3_IN_REPORT_NUM];
extern 
uint16_t usbd_hid3_idle_reload[USBD_HID3_IN_REPORT_NUM];
uint16_t usbd_hid3_idle_reload[USBD_HID3_IN_REPORT_NUM];
extern 
uint8_t  usbd_hid3_idle_set   [USBD_HID3_IN_REPORT_NUM];
uint8_t  usbd_hid3_idle_set   [USBD_HID3_IN_REPORT_NUM];
extern 
uint8_t  usbd_hid3_in_report  [USBD_HID3_IN_REPORT_MAX_SZ  +(USBD_HID3_IN_REPORT_MAX_SZ>1)];
uint8_t  usbd_hid3_in_report  [USBD_HID3_IN_REPORT_MAX_SZ  +(USBD_HID3_IN_REPORT_MAX_SZ>1)]         __ALIGNED(4);
extern 
uint8_t  usbd_hid3_out_report [RUP32(USBD_HID3_OUT_REPORT_MAX_SZ +(USBD_HID3_OUT_REPORT_MAX_SZ>1))];
uint8_t  usbd_hid3_out_report [RUP32(USBD_HID3_OUT_REPORT_MAX_SZ +(USBD_HID3_OUT_REPORT_MAX_SZ>1))] __ALIGNED(32);
extern 
uint8_t  usbd_hid3_feat_report[USBD_HID3_FEAT_REPORT_MAX_SZ+(USBD_HID3_FEAT_REPORT_MAX_SZ>1)];
uint8_t  usbd_hid3_feat_report[USBD_HID3_FEAT_REPORT_MAX_SZ+(USBD_HID3_FEAT_REPORT_MAX_SZ>1)]       __ALIGNED(4);
#endif

#ifdef  RTE_USB_Device_HID_4
extern 
uint16_t usbd_hid4_idle_count [USBD_HID4_IN_REPORT_NUM];
uint16_t usbd_hid4_idle_count [USBD_HID4_IN_REPORT_NUM];
extern 
uint16_t usbd_hid4_idle_reload[USBD_HID4_IN_REPORT_NUM];
uint16_t usbd_hid4_idle_reload[USBD_HID4_IN_REPORT_NUM];
extern 
uint8_t  usbd_hid4_idle_set   [USBD_HID4_IN_REPORT_NUM];
uint8_t  usbd_hid4_idle_set   [USBD_HID4_IN_REPORT_NUM];
extern 
uint8_t  usbd_hid4_in_report  [USBD_HID4_IN_REPORT_MAX_SZ  +(USBD_HID4_IN_REPORT_MAX_SZ>1)];
uint8_t  usbd_hid4_in_report  [USBD_HID4_IN_REPORT_MAX_SZ  +(USBD_HID4_IN_REPORT_MAX_SZ>1)]         __ALIGNED(4);
extern 
uint8_t  usbd_hid4_out_report [RUP32(USBD_HID4_OUT_REPORT_MAX_SZ +(USBD_HID4_OUT_REPORT_MAX_SZ>1))];
uint8_t  usbd_hid4_out_report [RUP32(USBD_HID4_OUT_REPORT_MAX_SZ +(USBD_HID4_OUT_REPORT_MAX_SZ>1))] __ALIGNED(32);
extern 
uint8_t  usbd_hid4_feat_report[USBD_HID4_FEAT_REPORT_MAX_SZ+(USBD_HID4_FEAT_REPORT_MAX_SZ>1)];
uint8_t  usbd_hid4_feat_report[USBD_HID4_FEAT_REPORT_MAX_SZ+(USBD_HID4_FEAT_REPORT_MAX_SZ>1)]       __ALIGNED(4);
#endif

#ifdef  RTE_USB_Device_HID_5
extern 
uint16_t usbd_hid5_idle_count [USBD_HID5_IN_REPORT_NUM];
uint16_t usbd_hid5_idle_count [USBD_HID5_IN_REPORT_NUM];
extern 
uint16_t usbd_hid5_idle_reload[USBD_HID5_IN_REPORT_NUM];
uint16_t usbd_hid5_idle_reload[USBD_HID5_IN_REPORT_NUM];
extern 
uint8_t  usbd_hid5_idle_set   [USBD_HID5_IN_REPORT_NUM];
uint8_t  usbd_hid5_idle_set   [USBD_HID5_IN_REPORT_NUM];
extern 
uint8_t  usbd_hid5_in_report  [USBD_HID5_IN_REPORT_MAX_SZ  +(USBD_HID5_IN_REPORT_MAX_SZ>1)];
uint8_t  usbd_hid5_in_report  [USBD_HID5_IN_REPORT_MAX_SZ  +(USBD_HID5_IN_REPORT_MAX_SZ>1)]         __ALIGNED(4);
extern 
uint8_t  usbd_hid5_out_report [RUP32(USBD_HID5_OUT_REPORT_MAX_SZ +(USBD_HID5_OUT_REPORT_MAX_SZ>1))];
uint8_t  usbd_hid5_out_report [RUP32(USBD_HID5_OUT_REPORT_MAX_SZ +(USBD_HID5_OUT_REPORT_MAX_SZ>1))] __ALIGNED(32);
extern 
uint8_t  usbd_hid5_feat_report[USBD_HID5_FEAT_REPORT_MAX_SZ+(USBD_HID5_FEAT_REPORT_MAX_SZ>1)];
uint8_t  usbd_hid5_feat_report[USBD_HID5_FEAT_REPORT_MAX_SZ+(USBD_HID5_FEAT_REPORT_MAX_SZ>1)]       __ALIGNED(4);
#endif

#ifdef  RTE_USB_Device_HID_6
extern 
uint16_t usbd_hid6_idle_count [USBD_HID6_IN_REPORT_NUM];
uint16_t usbd_hid6_idle_count [USBD_HID6_IN_REPORT_NUM];
extern 
uint16_t usbd_hid6_idle_reload[USBD_HID6_IN_REPORT_NUM];
uint16_t usbd_hid6_idle_reload[USBD_HID6_IN_REPORT_NUM];
extern 
uint8_t  usbd_hid6_idle_set   [USBD_HID6_IN_REPORT_NUM];
uint8_t  usbd_hid6_idle_set   [USBD_HID6_IN_REPORT_NUM];
extern 
uint8_t  usbd_hid6_in_report  [USBD_HID6_IN_REPORT_MAX_SZ  +(USBD_HID6_IN_REPORT_MAX_SZ>1)];
uint8_t  usbd_hid6_in_report  [USBD_HID6_IN_REPORT_MAX_SZ  +(USBD_HID6_IN_REPORT_MAX_SZ>1)]         __ALIGNED(4);
extern 
uint8_t  usbd_hid6_out_report [RUP32(USBD_HID6_OUT_REPORT_MAX_SZ +(USBD_HID6_OUT_REPORT_MAX_SZ>1))];
uint8_t  usbd_hid6_out_report [RUP32(USBD_HID6_OUT_REPORT_MAX_SZ +(USBD_HID6_OUT_REPORT_MAX_SZ>1))] __ALIGNED(32);
extern 
uint8_t  usbd_hid6_feat_report[USBD_HID6_FEAT_REPORT_MAX_SZ+(USBD_HID6_FEAT_REPORT_MAX_SZ>1)];
uint8_t  usbd_hid6_feat_report[USBD_HID6_FEAT_REPORT_MAX_SZ+(USBD_HID6_FEAT_REPORT_MAX_SZ>1)]       __ALIGNED(4);
#endif

#ifdef  RTE_USB_Device_HID_7
extern 
uint16_t usbd_hid7_idle_count [USBD_HID7_IN_REPORT_NUM];
uint16_t usbd_hid7_idle_count [USBD_HID7_IN_REPORT_NUM];
extern 
uint16_t usbd_hid7_idle_reload[USBD_HID7_IN_REPORT_NUM];
uint16_t usbd_hid7_idle_reload[USBD_HID7_IN_REPORT_NUM];
extern 
uint8_t  usbd_hid7_idle_set   [USBD_HID7_IN_REPORT_NUM];
uint8_t  usbd_hid7_idle_set   [USBD_HID7_IN_REPORT_NUM];
extern 
uint8_t  usbd_hid7_in_report  [USBD_HID7_IN_REPORT_MAX_SZ  +(USBD_HID7_IN_REPORT_MAX_SZ>1)];
uint8_t  usbd_hid7_in_report  [USBD_HID7_IN_REPORT_MAX_SZ  +(USBD_HID7_IN_REPORT_MAX_SZ>1)]         __ALIGNED(4);
extern 
uint8_t  usbd_hid7_out_report [RUP32(USBD_HID7_OUT_REPORT_MAX_SZ +(USBD_HID7_OUT_REPORT_MAX_SZ>1))];
uint8_t  usbd_hid7_out_report [RUP32(USBD_HID7_OUT_REPORT_MAX_SZ +(USBD_HID7_OUT_REPORT_MAX_SZ>1))] __ALIGNED(32);
extern 
uint8_t  usbd_hid7_feat_report[USBD_HID7_FEAT_REPORT_MAX_SZ+(USBD_HID7_FEAT_REPORT_MAX_SZ>1)];
uint8_t  usbd_hid7_feat_report[USBD_HID7_FEAT_REPORT_MAX_SZ+(USBD_HID7_FEAT_REPORT_MAX_SZ>1)]       __ALIGNED(4);
#endif

extern 
const   uint8_t     usbd_hid_num;
const   uint8_t     usbd_hid_num  =     USBD_HID_NUM;
extern 
usbd_hid_data_t     usbd_hid_data      [USBD_HID_NUM];
usbd_hid_data_t     usbd_hid_data      [USBD_HID_NUM];
extern
usbd_hid_t          usbd_hid           [USBD_HID_NUM];
usbd_hid_t          usbd_hid           [USBD_HID_NUM] = {
#ifdef  RTE_USB_Device_HID_0
                                      {
                                        usbd_hid0_idle_count,
                                        usbd_hid0_idle_reload,
                                        usbd_hid0_idle_set,
                                        usbd_hid0_in_report,
                                        usbd_hid0_out_report,
                                        usbd_hid0_feat_report,
                                       &usbd_hid_data[0],
                                        USBD_HID0_DEV,
                                        USBD_HID0_IF0_NUM,
                                        USBD_HID0_EP_INT_IN,
                                        USBD_HID0_EP_INT_OUT,
                                      { USBD_HID0_EP_INT_IN_BINTERVAL,       (2 << ((USBD_HID0_EP_INT_IN_HS_BINTERVAL & 0x0F)-1)) },
                                      { USBD_HID0_EP_INT_IN_WMAXPACKETSIZE,          USBD_HID0_EP_INT_IN_HS_WMAXPACKETSIZE },
                                      { USBD_HID0_EP_INT_OUT_BINTERVAL,      (2 << ((USBD_HID0_EP_INT_OUT_HS_BINTERVAL & 0x0F)-1)) },
                                      { USBD_HID0_EP_INT_OUT_WMAXPACKETSIZE,         USBD_HID0_EP_INT_OUT_HS_WMAXPACKETSIZE },
                                        USBD_HID0_IN_REPORT_NUM,
                                        USBD_HID0_OUT_REPORT_NUM,
                                        USBD_HID0_IN_REPORT_MAX_SZ,
                                        USBD_HID0_OUT_REPORT_MAX_SZ,
                                        USBD_HID0_FEAT_REPORT_MAX_SZ,
                                      }
#endif
#ifdef  RTE_USB_Device_HID_1
                                    , {
                                        usbd_hid1_idle_count,
                                        usbd_hid1_idle_reload,
                                        usbd_hid1_idle_set,
                                        usbd_hid1_in_report,
                                        usbd_hid1_out_report,
                                        usbd_hid1_feat_report,
                                       &usbd_hid_data[1],
                                        USBD_HID1_DEV,
                                        USBD_HID1_IF0_NUM,
                                        USBD_HID1_EP_INT_IN,
                                        USBD_HID1_EP_INT_OUT,
                                      { USBD_HID1_EP_INT_IN_BINTERVAL,       (2 << ((USBD_HID1_EP_INT_IN_HS_BINTERVAL & 0x0F)-1)) },
                                      { USBD_HID1_EP_INT_IN_WMAXPACKETSIZE,          USBD_HID1_EP_INT_IN_HS_WMAXPACKETSIZE },
                                      { USBD_HID1_EP_INT_OUT_BINTERVAL,      (2 << ((USBD_HID1_EP_INT_OUT_HS_BINTERVAL & 0x0F)-1)) },
                                      { USBD_HID1_EP_INT_OUT_WMAXPACKETSIZE,         USBD_HID1_EP_INT_OUT_HS_WMAXPACKETSIZE },
                                        USBD_HID1_IN_REPORT_NUM,
                                        USBD_HID1_OUT_REPORT_NUM,
                                        USBD_HID1_IN_REPORT_MAX_SZ,
                                        USBD_HID1_OUT_REPORT_MAX_SZ,
                                        USBD_HID1_FEAT_REPORT_MAX_SZ,
                                      }
#endif
#ifdef  RTE_USB_Device_HID_2
                                    , {
                                        usbd_hid2_idle_count,
                                        usbd_hid2_idle_reload,
                                        usbd_hid2_idle_set,
                                        usbd_hid2_in_report,
                                        usbd_hid2_out_report,
                                        usbd_hid2_feat_report,
                                       &usbd_hid_data[2],
                                        USBD_HID2_DEV,
                                        USBD_HID2_IF0_NUM,
                                        USBD_HID2_EP_INT_IN,
                                        USBD_HID2_EP_INT_OUT,
                                      { USBD_HID2_EP_INT_IN_BINTERVAL,       (2 << ((USBD_HID2_EP_INT_IN_HS_BINTERVAL & 0x0F)-1)) },
                                      { USBD_HID2_EP_INT_IN_WMAXPACKETSIZE,          USBD_HID2_EP_INT_IN_HS_WMAXPACKETSIZE },
                                      { USBD_HID2_EP_INT_OUT_BINTERVAL,      (2 << ((USBD_HID2_EP_INT_OUT_HS_BINTERVAL & 0x0F)-1)) },
                                      { USBD_HID2_EP_INT_OUT_WMAXPACKETSIZE,         USBD_HID2_EP_INT_OUT_HS_WMAXPACKETSIZE },
                                        USBD_HID2_IN_REPORT_NUM,
                                        USBD_HID2_OUT_REPORT_NUM,
                                        USBD_HID2_IN_REPORT_MAX_SZ,
                                        USBD_HID2_OUT_REPORT_MAX_SZ,
                                        USBD_HID2_FEAT_REPORT_MAX_SZ,
                                      }
#endif
#ifdef  RTE_USB_Device_HID_3
                                    , {
                                        usbd_hid3_idle_count,
                                        usbd_hid3_idle_reload,
                                        usbd_hid3_idle_set,
                                        usbd_hid3_in_report,
                                        usbd_hid3_out_report,
                                        usbd_hid3_feat_report,
                                       &usbd_hid_data[3],
                                        USBD_HID3_DEV,
                                        USBD_HID3_IF0_NUM,
                                        USBD_HID3_EP_INT_IN,
                                        USBD_HID3_EP_INT_OUT,
                                      { USBD_HID3_EP_INT_IN_BINTERVAL,       (2 << ((USBD_HID3_EP_INT_IN_HS_BINTERVAL & 0x0F)-1)) },
                                      { USBD_HID3_EP_INT_IN_WMAXPACKETSIZE,          USBD_HID3_EP_INT_IN_HS_WMAXPACKETSIZE },
                                      { USBD_HID3_EP_INT_OUT_BINTERVAL,      (2 << ((USBD_HID3_EP_INT_OUT_HS_BINTERVAL & 0x0F)-1)) },
                                      { USBD_HID3_EP_INT_OUT_WMAXPACKETSIZE,         USBD_HID3_EP_INT_OUT_HS_WMAXPACKETSIZE },
                                        USBD_HID3_IN_REPORT_NUM,
                                        USBD_HID3_OUT_REPORT_NUM,
                                        USBD_HID3_IN_REPORT_MAX_SZ,
                                        USBD_HID3_OUT_REPORT_MAX_SZ,
                                        USBD_HID3_FEAT_REPORT_MAX_SZ,
                                      }
#endif
#ifdef  RTE_USB_Device_HID_4
                                    , {
                                        usbd_hid4_idle_count,
                                        usbd_hid4_idle_reload,
                                        usbd_hid4_idle_set,
                                        usbd_hid4_in_report,
                                        usbd_hid4_out_report,
                                        usbd_hid4_feat_report,
                                       &usbd_hid_data[4],
                                        USBD_HID4_DEV,
                                        USBD_HID4_IF0_NUM,
                                        USBD_HID4_EP_INT_IN,
                                        USBD_HID4_EP_INT_OUT,
                                      { USBD_HID4_EP_INT_IN_BINTERVAL,       (2 << ((USBD_HID4_EP_INT_IN_HS_BINTERVAL & 0x0F)-1)) },
                                      { USBD_HID4_EP_INT_IN_WMAXPACKETSIZE,          USBD_HID4_EP_INT_IN_HS_WMAXPACKETSIZE },
                                      { USBD_HID4_EP_INT_OUT_BINTERVAL,      (2 << ((USBD_HID4_EP_INT_OUT_HS_BINTERVAL & 0x0F)-1)) },
                                      { USBD_HID4_EP_INT_OUT_WMAXPACKETSIZE,         USBD_HID4_EP_INT_OUT_HS_WMAXPACKETSIZE },
                                        USBD_HID4_IN_REPORT_NUM,
                                        USBD_HID4_OUT_REPORT_NUM,
                                        USBD_HID4_IN_REPORT_MAX_SZ,
                                        USBD_HID4_OUT_REPORT_MAX_SZ,
                                        USBD_HID4_FEAT_REPORT_MAX_SZ,
                                      }
#endif
#ifdef  RTE_USB_Device_HID_5
                                    , {
                                        usbd_hid5_idle_count,
                                        usbd_hid5_idle_reload,
                                        usbd_hid5_idle_set,
                                        usbd_hid5_in_report,
                                        usbd_hid5_out_report,
                                        usbd_hid5_feat_report,
                                       &usbd_hid_data[5],
                                        USBD_HID5_DEV,
                                        USBD_HID5_IF0_NUM,
                                        USBD_HID5_EP_INT_IN,
                                        USBD_HID5_EP_INT_OUT,
                                      { USBD_HID5_EP_INT_IN_BINTERVAL,       (2 << ((USBD_HID5_EP_INT_IN_HS_BINTERVAL & 0x0F)-1)) },
                                      { USBD_HID5_EP_INT_IN_WMAXPACKETSIZE,          USBD_HID5_EP_INT_IN_HS_WMAXPACKETSIZE },
                                      { USBD_HID5_EP_INT_OUT_BINTERVAL,      (2 << ((USBD_HID5_EP_INT_OUT_HS_BINTERVAL & 0x0F)-1)) },
                                      { USBD_HID5_EP_INT_OUT_WMAXPACKETSIZE,         USBD_HID5_EP_INT_OUT_HS_WMAXPACKETSIZE },
                                        USBD_HID5_IN_REPORT_NUM,
                                        USBD_HID5_OUT_REPORT_NUM,
                                        USBD_HID5_IN_REPORT_MAX_SZ,
                                        USBD_HID5_OUT_REPORT_MAX_SZ,
                                        USBD_HID5_FEAT_REPORT_MAX_SZ,
                                      }
#endif
#ifdef  RTE_USB_Device_HID_6
                                    , {
                                        usbd_hid6_idle_count,
                                        usbd_hid6_idle_reload,
                                        usbd_hid6_idle_set,
                                        usbd_hid6_in_report,
                                        usbd_hid6_out_report,
                                        usbd_hid6_feat_report,
                                       &usbd_hid_data[6],
                                        USBD_HID6_DEV,
                                        USBD_HID6_IF0_NUM,
                                        USBD_HID6_EP_INT_IN,
                                        USBD_HID6_EP_INT_OUT,
                                      { USBD_HID6_EP_INT_IN_BINTERVAL,       (2 << ((USBD_HID6_EP_INT_IN_HS_BINTERVAL & 0x0F)-1)) },
                                      { USBD_HID6_EP_INT_IN_WMAXPACKETSIZE,          USBD_HID6_EP_INT_IN_HS_WMAXPACKETSIZE },
                                      { USBD_HID6_EP_INT_OUT_BINTERVAL,      (2 << ((USBD_HID6_EP_INT_OUT_HS_BINTERVAL & 0x0F)-1)) },
                                      { USBD_HID6_EP_INT_OUT_WMAXPACKETSIZE,         USBD_HID6_EP_INT_OUT_HS_WMAXPACKETSIZE },
                                        USBD_HID6_IN_REPORT_NUM,
                                        USBD_HID6_OUT_REPORT_NUM,
                                        USBD_HID6_IN_REPORT_MAX_SZ,
                                        USBD_HID6_OUT_REPORT_MAX_SZ,
                                        USBD_HID6_FEAT_REPORT_MAX_SZ,
                                      }
#endif
#ifdef  RTE_USB_Device_HID_7
                                    , {
                                        usbd_hid7_idle_count,
                                        usbd_hid7_idle_reload,
                                        usbd_hid7_idle_set,
                                        usbd_hid7_in_report,
                                        usbd_hid7_out_report,
                                        usbd_hid7_feat_report,
                                       &usbd_hid_data[7],
                                        USBD_HID7_DEV,
                                        USBD_HID7_IF0_NUM,
                                        USBD_HID7_EP_INT_IN,
                                        USBD_HID7_EP_INT_OUT,
                                      { USBD_HID7_EP_INT_IN_BINTERVAL,       (2 << ((USBD_HID7_EP_INT_IN_HS_BINTERVAL & 0x0F)-1)) },
                                      { USBD_HID7_EP_INT_IN_WMAXPACKETSIZE,          USBD_HID7_EP_INT_IN_HS_WMAXPACKETSIZE },
                                      { USBD_HID7_EP_INT_OUT_BINTERVAL,      (2 << ((USBD_HID7_EP_INT_OUT_HS_BINTERVAL & 0x0F)-1)) },
                                      { USBD_HID7_EP_INT_OUT_WMAXPACKETSIZE,         USBD_HID7_EP_INT_OUT_HS_WMAXPACKETSIZE },
                                        USBD_HID7_IN_REPORT_NUM,
                                        USBD_HID7_OUT_REPORT_NUM,
                                        USBD_HID7_IN_REPORT_MAX_SZ,
                                        USBD_HID7_OUT_REPORT_MAX_SZ,
                                        USBD_HID7_FEAT_REPORT_MAX_SZ,
                                      }
#endif
};
extern usbd_hid_t *usbd_hid_ptr       [USBD_HID_NUM];
       usbd_hid_t *usbd_hid_ptr       [USBD_HID_NUM] = {
#ifdef  RTE_USB_Device_HID_0
       &usbd_hid[0]
#endif
#ifdef  RTE_USB_Device_HID_1
     , &usbd_hid[1]
#endif
#ifdef  RTE_USB_Device_HID_2
     , &usbd_hid[2]
#endif
#ifdef  RTE_USB_Device_HID_3
     , &usbd_hid[3]
#endif
#ifdef  RTE_USB_Device_HID_4
     , &usbd_hid[4]
#endif
#ifdef  RTE_USB_Device_HID_5
     , &usbd_hid[5]
#endif
#ifdef  RTE_USB_Device_HID_6
     , &usbd_hid[6]
#endif
#ifdef  RTE_USB_Device_HID_7
     , &usbd_hid[7]
#endif
};

#endif


/*------------------------------------------------------------------------------
 *      USB Device MSC 0..3 Configurations
 *----------------------------------------------------------------------------*/

#define USBD_MSC0_MAX_PACKET           (MAX_OF_2(USBD_MSC0_WMAXPACKETSIZE, USBD_MSC0_HS_WMAXPACKETSIZE))
#define USBD_MSC1_MAX_PACKET           (MAX_OF_2(USBD_MSC1_WMAXPACKETSIZE, USBD_MSC1_HS_WMAXPACKETSIZE))
#define USBD_MSC2_MAX_PACKET           (MAX_OF_2(USBD_MSC2_WMAXPACKETSIZE, USBD_MSC2_HS_WMAXPACKETSIZE))
#define USBD_MSC3_MAX_PACKET           (MAX_OF_2(USBD_MSC3_WMAXPACKETSIZE, USBD_MSC3_HS_WMAXPACKETSIZE))

#ifdef  RTE_USB_Device_MSC_0
#define USBD_MSC0_NUM                   1
#else
#define USBD_MSC0_NUM                   0
#endif
#ifdef  RTE_USB_Device_MSC_1
#define USBD_MSC1_NUM                   1
#else
#define USBD_MSC1_NUM                   0
#endif
#ifdef  RTE_USB_Device_MSC_2
#define USBD_MSC2_NUM                   1
#else
#define USBD_MSC2_NUM                   0
#endif
#ifdef  RTE_USB_Device_MSC_3
#define USBD_MSC3_NUM                   1
#else
#define USBD_MSC3_NUM                   0
#endif

#define USBD_MSC_NUM                   (USBD_MSC0_NUM + USBD_MSC1_NUM + USBD_MSC2_NUM + USBD_MSC3_NUM)

#if    (USBD_MSC_NUM > 0)

// Check settings
#ifdef  RTE_USB_Device_MSC_0
#if    (USBD_MSC0_BULK_BUF_SIZE < USBD_MSC0_MAX_PACKET)
#error Maximum Bulk Buffer Size for Mass Storage Instance 0 can not be smaller then Bulk Endpoint Maximum Packet Size!
#endif
#endif

#ifdef  RTE_USB_Device_MSC_1
#if    (USBD_MSC1_BULK_BUF_SIZE < USBD_MSC1_MAX_PACKET)
#error Maximum Bulk Buffer Size for Mass Storage Instance 1 can not be smaller then Bulk Endpoint Maximum Packet Size!
#endif
#endif

#ifdef  RTE_USB_Device_MSC_2
#if    (USBD_MSC2_BULK_BUF_SIZE < USBD_MSC2_MAX_PACKET)
#error Maximum Bulk Buffer Size for Mass Storage Instance 2 can not be smaller then Bulk Endpoint Maximum Packet Size!
#endif
#endif

#ifdef  RTE_USB_Device_MSC_3
#if    (USBD_MSC3_BULK_BUF_SIZE < USBD_MSC3_MAX_PACKET)
#error Maximum Bulk Buffer Size for Mass Storage Instance 3 can not be smaller then Bulk Endpoint Maximum Packet Size!
#endif
#endif

extern 
void                       *usbd_msc_thread_id     [USBD_MSC_NUM];
void                       *usbd_msc_thread_id     [USBD_MSC_NUM];

#ifdef  RTE_USB_Device_MSC_0
extern  void USBD_MSC0_Thread (void const *arg);
extern 
uint8_t usbd_msc0_bulk_buf [RUP32(USBD_MSC0_BULK_BUF_SIZE)];
uint8_t usbd_msc0_bulk_buf [RUP32(USBD_MSC0_BULK_BUF_SIZE)] __ALIGNED(32);
#endif

#ifdef  RTE_USB_Device_MSC_1
extern  void USBD_MSC1_Thread (void const *arg);
extern 
uint8_t usbd_msc1_bulk_buf [RUP32(USBD_MSC1_BULK_BUF_SIZE)];
uint8_t usbd_msc1_bulk_buf [RUP32(USBD_MSC1_BULK_BUF_SIZE)] __ALIGNED(32);
#endif

#ifdef  RTE_USB_Device_MSC_2
extern  void USBD_MSC2_Thread (void const *arg);
extern 
uint8_t usbd_msc2_bulk_buf [RUP32(USBD_MSC2_BULK_BUF_SIZE)];
uint8_t usbd_msc2_bulk_buf [RUP32(USBD_MSC2_BULK_BUF_SIZE)] __ALIGNED(32);
#endif

#ifdef  RTE_USB_Device_MSC_3
extern  void USBD_MSC3_Thread (void const *arg);
extern 
uint8_t usbd_msc3_bulk_buf [RUP32(USBD_MSC3_BULK_BUF_SIZE)];
uint8_t usbd_msc3_bulk_buf [RUP32(USBD_MSC3_BULK_BUF_SIZE)] __ALIGNED(32);
#endif

extern 
const   uint8_t     usbd_msc_num;
const   uint8_t     usbd_msc_num  =     USBD_MSC_NUM;
extern 
usbd_msc_data_t     usbd_msc_data      [USBD_MSC_NUM];
usbd_msc_data_t     usbd_msc_data      [USBD_MSC_NUM];
extern 
usbd_msc_t          usbd_msc           [USBD_MSC_NUM];
usbd_msc_t          usbd_msc           [USBD_MSC_NUM] = {
#ifdef  RTE_USB_Device_MSC_0
                                      {
                                        usbd_msc0_bulk_buf,
                                       &usbd_msc_data[0],
                                        USBD_MSC0_DEV,
                                        USBD_MSC0_IF0_NUM,
                                        USBD_MSC0_EP_BULK_IN,
                                        USBD_MSC0_EP_BULK_OUT,
                                        USBD_MSC0_BULK_BUF_SIZE,
                                      { USBD_MSC0_WMAXPACKETSIZE,
                                        USBD_MSC0_HS_WMAXPACKETSIZE },
                                        USBD_MSC0_MAX_LUN,
                                      { 0U, 0U, 0U },
                                      { (uint8_t *)(USBD_MSC0_INQUIRY_DATA)      ,
#if (USBD_MSC0_MAX_LUN > 0)
                                        (uint8_t *)(USBD_MSC0_LUN1_INQUIRY_DATA) ,
#else
                                        (uint8_t *)(USBD_MSC0_INQUIRY_DATA)      ,
#endif
#if (USBD_MSC0_MAX_LUN > 1)
                                        (uint8_t *)(USBD_MSC0_LUN2_INQUIRY_DATA) ,
#else
                                        (uint8_t *)(USBD_MSC0_INQUIRY_DATA)      ,
#endif
#if (USBD_MSC0_MAX_LUN > 2)
                                        (uint8_t *)(USBD_MSC0_LUN3_INQUIRY_DATA) }
#else
                                        (uint8_t *)(USBD_MSC0_INQUIRY_DATA)      }
#endif
                                      }
#endif
#ifdef  RTE_USB_Device_MSC_1
                                    , {
                                        usbd_msc1_bulk_buf,
                                       &usbd_msc_data[1],
                                        USBD_MSC1_DEV,
                                        USBD_MSC1_IF0_NUM,
                                        USBD_MSC1_EP_BULK_IN,
                                        USBD_MSC1_EP_BULK_OUT,
                                        USBD_MSC1_BULK_BUF_SIZE,
                                      { USBD_MSC1_WMAXPACKETSIZE,
                                        USBD_MSC1_HS_WMAXPACKETSIZE },
                                        USBD_MSC1_MAX_LUN,
                                      { 0U, 0U, 0U },
                                      { (uint8_t *)(USBD_MSC1_INQUIRY_DATA)      ,
#if (USBD_MSC1_MAX_LUN > 0)
                                        (uint8_t *)(USBD_MSC1_LUN1_INQUIRY_DATA) ,
#else
                                        (uint8_t *)(USBD_MSC1_INQUIRY_DATA)      ,
#endif
#if (USBD_MSC1_MAX_LUN > 1)
                                        (uint8_t *)(USBD_MSC1_LUN2_INQUIRY_DATA) ,
#else
                                        (uint8_t *)(USBD_MSC1_INQUIRY_DATA)      ,
#endif
#if (USBD_MSC1_MAX_LUN > 2)
                                        (uint8_t *)(USBD_MSC1_LUN3_INQUIRY_DATA) }
#else
                                        (uint8_t *)(USBD_MSC1_INQUIRY_DATA)      }
#endif
                                      }
#endif
#ifdef  RTE_USB_Device_MSC_2
                                    , {
                                        usbd_msc2_bulk_buf,
                                       &usbd_msc_data[2],
                                        USBD_MSC2_DEV,
                                        USBD_MSC2_IF0_NUM,
                                        USBD_MSC2_EP_BULK_IN,
                                        USBD_MSC2_EP_BULK_OUT,
                                        USBD_MSC2_BULK_BUF_SIZE,
                                      { USBD_MSC2_WMAXPACKETSIZE,
                                        USBD_MSC2_HS_WMAXPACKETSIZE },
                                        USBD_MSC2_MAX_LUN,
                                      { 0U, 0U, 0U },
                                      { (uint8_t *)(USBD_MSC2_INQUIRY_DATA)      ,
#if (USBD_MSC2_MAX_LUN > 0)
                                        (uint8_t *)(USBD_MSC2_LUN1_INQUIRY_DATA) ,
#else
                                        (uint8_t *)(USBD_MSC2_INQUIRY_DATA)      ,
#endif
#if (USBD_MSC2_MAX_LUN > 1)
                                        (uint8_t *)(USBD_MSC2_LUN2_INQUIRY_DATA) ,
#else
                                        (uint8_t *)(USBD_MSC2_INQUIRY_DATA)      ,
#endif
#if (USBD_MSC2_MAX_LUN > 2)
                                        (uint8_t *)(USBD_MSC2_LUN3_INQUIRY_DATA) }
#else
                                        (uint8_t *)(USBD_MSC2_INQUIRY_DATA)      }
#endif
                                      }
#endif
#ifdef  RTE_USB_Device_MSC_3
                                    , {
                                        usbd_msc3_bulk_buf,
                                       &usbd_msc_data[3],
                                        USBD_MSC3_DEV,
                                        USBD_MSC3_IF0_NUM,
                                        USBD_MSC3_EP_BULK_IN,
                                        USBD_MSC3_EP_BULK_OUT,
                                        USBD_MSC3_BULK_BUF_SIZE,
                                      { USBD_MSC3_WMAXPACKETSIZE,
                                        USBD_MSC3_HS_WMAXPACKETSIZE },
                                        USBD_MSC3_MAX_LUN,
                                      { 0U, 0U, 0U },
                                      { (uint8_t *)(USBD_MSC3_INQUIRY_DATA)      ,
#if (USBD_MSC3_MAX_LUN > 0)
                                        (uint8_t *)(USBD_MSC3_LUN1_INQUIRY_DATA) ,
#else
                                        (uint8_t *)(USBD_MSC3_INQUIRY_DATA)      ,
#endif
#if (USBD_MSC3_MAX_LUN > 1)
                                        (uint8_t *)(USBD_MSC3_LUN2_INQUIRY_DATA) ,
#else
                                        (uint8_t *)(USBD_MSC3_INQUIRY_DATA)      ,
#endif
#if (USBD_MSC3_MAX_LUN > 2)
                                        (uint8_t *)(USBD_MSC3_LUN3_INQUIRY_DATA) }
#else
                                        (uint8_t *)(USBD_MSC3_INQUIRY_DATA)      }
#endif
                                      }
#endif
};

extern usbd_msc_t * const usbd_msc_ptr[USBD_MSC_NUM];
       usbd_msc_t * const usbd_msc_ptr[USBD_MSC_NUM] = {
#ifdef  RTE_USB_Device_MSC_0
       &usbd_msc[0]
#endif
#ifdef  RTE_USB_Device_MSC_1
     , &usbd_msc[1]
#endif
#ifdef  RTE_USB_Device_MSC_2
     , &usbd_msc[2]
#endif
#ifdef  RTE_USB_Device_MSC_3
     , &usbd_msc[3]
#endif
};

#endif


/*------------------------------------------------------------------------------
 *      USB Device OS abstraction
 *----------------------------------------------------------------------------*/

#ifdef RTE_CMSIS_RTOS2
  #include "usbd_cmsis_rtos2.c"
#else
  #error USB Device component requires CMSIS-RTOS2!
#endif


/*------------------------------------------------------------------------------
 *      USB Device Event Handler Functions
 *----------------------------------------------------------------------------*/

#if    (USBD_DEV_NUM)

extern 
const   uint8_t     usbd_ep_max_num;
const   uint8_t     usbd_ep_max_num =                    USBD_EP_NUM;
extern
const   void **usbd_ep_thread_id_ptr[USBD_DEV_NUM * USBD_EP_NUM];
const   void **usbd_ep_thread_id_ptr[USBD_DEV_NUM * USBD_EP_NUM] = {
#if    (USBD_DEV_NUM > 0)
        EP_POINTERS(0)
#endif
#if    (USBD_DEV_NUM > 1)
      , EP_POINTERS(1)
#endif
#if    (USBD_DEV_NUM > 2)
      , EP_POINTERS(2)
#endif
#if    (USBD_DEV_NUM > 3)
      , EP_POINTERS(3)
#endif
};

#if    (defined(RTE_USB_Device_0) || defined(RTE_USB_Device_1) || defined(RTE_USB_Device_2) || defined(RTE_USB_Device_3))

extern  void    USBD_SignalDeviceEvent   (uint8_t  device, uint32_t event);
extern  void    USBD_SignalEndpointEvent (uint8_t  device, uint8_t ep_addr, uint32_t event);
#ifdef  RTE_USB_Device_0
        void    USBD0_SignalDeviceEvent  (uint32_t event);
        void    USBD0_SignalDeviceEvent  (uint32_t event)                    { USBD_SignalDeviceEvent  (0, event);            }
        void    USBD0_SignalEndpointEvent(uint8_t  ep_addr, uint32_t event);
        void    USBD0_SignalEndpointEvent(uint8_t  ep_addr, uint32_t event)  { USBD_SignalEndpointEvent(0, ep_addr, event);   }
#endif
#ifdef  RTE_USB_Device_1
        void    USBD1_SignalDeviceEvent  (uint32_t event);
        void    USBD1_SignalDeviceEvent  (uint32_t event)                    { USBD_SignalDeviceEvent  (1, event);            }
        void    USBD1_SignalEndpointEvent(uint8_t  ep_addr, uint32_t event);
        void    USBD1_SignalEndpointEvent(uint8_t  ep_addr, uint32_t event)  { USBD_SignalEndpointEvent(1, ep_addr, event);   }
#endif
#ifdef  RTE_USB_Device_2
        void    USBD2_SignalDeviceEvent  (uint32_t event);
        void    USBD2_SignalDeviceEvent  (uint32_t event)                    { USBD_SignalDeviceEvent  (2, event);            }
        void    USBD2_SignalEndpointEvent(uint8_t  ep_addr, uint32_t event);
        void    USBD2_SignalEndpointEvent(uint8_t  ep_addr, uint32_t event)  { USBD_SignalEndpointEvent(2, ep_addr, event);   }
#endif
#ifdef  RTE_USB_Device_3
        void    USBD3_SignalDeviceEvent  (uint32_t event);
        void    USBD3_SignalDeviceEvent  (uint32_t event)                    { USBD_SignalDeviceEvent  (3, event);            }
        void    USBD3_SignalEndpointEvent(uint8_t  ep_addr, uint32_t event);
        void    USBD3_SignalEndpointEvent(uint8_t  ep_addr, uint32_t event)  { USBD_SignalEndpointEvent(3, ep_addr, event);   }
#endif

extern 
ARM_USBD_SignalDeviceEvent_t const usbd_cb_device_event_ptr[USBD_DEV_NUM];
ARM_USBD_SignalDeviceEvent_t const usbd_cb_device_event_ptr[USBD_DEV_NUM]   = {
#if    (USBD_DEV_NUM > 0)
        USBD0_SignalDeviceEvent
#endif
#if    (USBD_DEV_NUM > 1)
      , USBD1_SignalDeviceEvent
#endif
#if    (USBD_DEV_NUM > 2)
      , USBD2_SignalDeviceEvent
#endif
#if    (USBD_DEV_NUM > 3)
      , USBD3_SignalDeviceEvent
#endif
};

extern 
ARM_USBD_SignalEndpointEvent_t const usbd_cb_endpoint_event_ptr[USBD_DEV_NUM];
ARM_USBD_SignalEndpointEvent_t const usbd_cb_endpoint_event_ptr[USBD_DEV_NUM] = {
#if    (USBD_DEV_NUM > 0)
        USBD0_SignalEndpointEvent
#endif
#if    (USBD_DEV_NUM > 1)
      , USBD1_SignalEndpointEvent
#endif
#if    (USBD_DEV_NUM > 2)
      , USBD2_SignalEndpointEvent
#endif
#if    (USBD_DEV_NUM > 3)
      , USBD3_SignalEndpointEvent
#endif
};

extern 
ARM_DRIVER_VERSION    usbd_drv_version [USBD_DEV_NUM];
ARM_DRIVER_VERSION    usbd_drv_version [USBD_DEV_NUM];
extern 
ARM_USBD_CAPABILITIES usbd_capabilities[USBD_DEV_NUM];
ARM_USBD_CAPABILITIES usbd_capabilities[USBD_DEV_NUM];
#endif

#if    (USBD_CUSTOM_CLASS_NUM == 0)
        bool              USBD_Endpoint0_ReqGetExtProp_CC                 (uint8_t device, const uint8_t **pD);
        bool              USBD_Endpoint0_ReqGetExtProp_CC                 (uint8_t device, const uint8_t **pD)                                 { (void)device; (void)pD; return false; }
        usbdRequestStatus USBD_Class_Endpoint0_SetupPacketReceived        (uint8_t device);
        usbdRequestStatus USBD_Class_Endpoint0_SetupPacketReceived        (uint8_t device)                                                     { (void)device; return usbdRequestNotProcessed; }
        void              USBD_Class_Endpoint0_SetupPacketProcessed       (uint8_t device);
        void              USBD_Class_Endpoint0_SetupPacketProcessed       (uint8_t device)                                                     { (void)device;                                 }
        usbdRequestStatus USBD_Class_Endpoint0_OutDataReceived            (uint8_t device);
        usbdRequestStatus USBD_Class_Endpoint0_OutDataReceived            (uint8_t device)                                                     { (void)device; return usbdRequestNotProcessed; }
        usbdRequestStatus USBD_Class_Endpoint0_InDataSent                 (uint8_t device);
        usbdRequestStatus USBD_Class_Endpoint0_InDataSent                 (uint8_t device)                                                     { (void)device; return usbdRequestNotProcessed; }
#else
__WEAK  void              USBD_CustomClass0_Initialize                    (void)                                                               { }
__WEAK  void              USBD_CustomClass0_Uninitialize                  (void)                                                               { }
extern  void              USBD_CustomClass0_EndpointReset                 (void);
__WEAK  void              USBD_CustomClass0_EndpointReset                 (void)            /* Deprecated */                                   { }
extern  void              USBD_CustomClass0_EventEndpointStart            (uint8_t ep_addr);
__WEAK  void              USBD_CustomClass0_EventEndpointStart            (uint8_t ep_addr) /* Deprecated */                                   { (void)ep_addr; }
extern  void              USBD_CustomClass0_EventEndpointStop             (uint8_t ep_addr);
__WEAK  void              USBD_CustomClass0_EventEndpointStop             (uint8_t ep_addr) /* Deprecated */                                   { (void)ep_addr; }
__WEAK  void              USBD_CustomClass0_Reset                         (void)                                                               { USBD_CustomClass0_EndpointReset      ();        }
__WEAK  void              USBD_CustomClass0_EndpointStart                 (uint8_t ep_addr)                                                    { USBD_CustomClass0_EventEndpointStart (ep_addr); }
__WEAK  void              USBD_CustomClass0_EndpointStop                  (uint8_t ep_addr)                                                    { USBD_CustomClass0_EventEndpointStop  (ep_addr); }
__WEAK  usbdRequestStatus USBD_CustomClass0_Endpoint0_SetupPacketReceived (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len) { (void)setup_packet; (void)buf; (void)len; return usbdRequestNotProcessed; }
__WEAK  void              USBD_CustomClass0_Endpoint0_SetupPacketProcessed(const USB_SETUP_PACKET *setup_packet)                               { (void)setup_packet;                                                       }
__WEAK  usbdRequestStatus USBD_CustomClass0_Endpoint0_OutDataReceived     (uint32_t len)                                                       { (void)len; return usbdRequestNotProcessed; }
__WEAK  usbdRequestStatus USBD_CustomClass0_Endpoint0_InDataSent          (uint32_t len)                                                       { (void)len; return usbdRequestNotProcessed; }
__WEAK  void              USBD_CustomClass0_Endpoint1_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass0_Endpoint2_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass0_Endpoint3_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass0_Endpoint4_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass0_Endpoint5_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass0_Endpoint6_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass0_Endpoint7_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass0_Endpoint8_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass0_Endpoint9_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass0_Endpoint10_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass0_Endpoint11_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass0_Endpoint12_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass0_Endpoint13_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass0_Endpoint14_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass0_Endpoint15_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
#ifdef  RTE_USB_Device_CustomClass_1
__WEAK  void              USBD_CustomClass1_Initialize                    (void)                                                               { }
__WEAK  void              USBD_CustomClass1_Uninitialize                  (void)                                                               { }
extern  void              USBD_CustomClass1_EndpointReset                 (void);
__WEAK  void              USBD_CustomClass1_EndpointReset                 (void)            /* Deprecated */                                   { }
extern  void              USBD_CustomClass1_EventEndpointStart            (uint8_t ep_addr);
__WEAK  void              USBD_CustomClass1_EventEndpointStart            (uint8_t ep_addr) /* Deprecated */                                   { (void)ep_addr; }
extern  void              USBD_CustomClass1_EventEndpointStop             (uint8_t ep_addr);
__WEAK  void              USBD_CustomClass1_EventEndpointStop             (uint8_t ep_addr) /* Deprecated */                                   { (void)ep_addr; }
__WEAK  void              USBD_CustomClass1_Reset                         (void)                                                               { USBD_CustomClass1_EndpointReset      ();        }
__WEAK  void              USBD_CustomClass1_EndpointStart                 (uint8_t ep_addr)                                                    { USBD_CustomClass1_EventEndpointStart (ep_addr); }
__WEAK  void              USBD_CustomClass1_EndpointStop                  (uint8_t ep_addr)                                                    { USBD_CustomClass1_EventEndpointStop  (ep_addr); }
__WEAK  usbdRequestStatus USBD_CustomClass1_Endpoint0_SetupPacketReceived (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len) { (void)setup_packet; (void)buf; (void)len; return usbdRequestNotProcessed; }
__WEAK  void              USBD_CustomClass1_Endpoint0_SetupPacketProcessed(const USB_SETUP_PACKET *setup_packet)                               { (void)setup_packet;                                                       }
__WEAK  usbdRequestStatus USBD_CustomClass1_Endpoint0_OutDataReceived     (uint32_t len)                                                       { (void)len; return usbdRequestNotProcessed; }
__WEAK  usbdRequestStatus USBD_CustomClass1_Endpoint0_InDataSent          (uint32_t len)                                                       { (void)len; return usbdRequestNotProcessed; }
__WEAK  void              USBD_CustomClass1_Endpoint1_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass1_Endpoint2_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass1_Endpoint3_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass1_Endpoint4_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass1_Endpoint5_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass1_Endpoint6_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass1_Endpoint7_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass1_Endpoint8_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass1_Endpoint9_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass1_Endpoint10_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass1_Endpoint11_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass1_Endpoint12_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass1_Endpoint13_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass1_Endpoint14_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass1_Endpoint15_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
#endif
#ifdef  RTE_USB_Device_CustomClass_2
__WEAK  void              USBD_CustomClass2_Initialize                    (void)                                                               { }
__WEAK  void              USBD_CustomClass2_Uninitialize                  (void)                                                               { }
extern  void              USBD_CustomClass2_EndpointReset                 (void);
__WEAK  void              USBD_CustomClass2_EndpointReset                 (void)            /* Deprecated */                                   { }
extern  void              USBD_CustomClass2_EventEndpointStart            (uint8_t ep_addr);
__WEAK  void              USBD_CustomClass2_EventEndpointStart            (uint8_t ep_addr) /* Deprecated */                                   { (void)ep_addr; }
extern  void              USBD_CustomClass2_EventEndpointStop             (uint8_t ep_addr);
__WEAK  void              USBD_CustomClass2_EventEndpointStop             (uint8_t ep_addr) /* Deprecated */                                   { (void)ep_addr; }
__WEAK  void              USBD_CustomClass2_Reset                         (void)                                                               { USBD_CustomClass2_EndpointReset      ();        }
__WEAK  void              USBD_CustomClass2_EndpointStart                 (uint8_t ep_addr)                                                    { USBD_CustomClass2_EventEndpointStart (ep_addr); }
__WEAK  void              USBD_CustomClass2_EndpointStop                  (uint8_t ep_addr)                                                    { USBD_CustomClass2_EventEndpointStop  (ep_addr); }
__WEAK  usbdRequestStatus USBD_CustomClass2_Endpoint0_SetupPacketReceived (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len) { (void)setup_packet; (void)buf; (void)len; return usbdRequestNotProcessed; }
__WEAK  void              USBD_CustomClass2_Endpoint0_SetupPacketProcessed(const USB_SETUP_PACKET *setup_packet)                               { (void)setup_packet;                                                       }
__WEAK  usbdRequestStatus USBD_CustomClass2_Endpoint0_OutDataReceived     (uint32_t len)                                                       { (void)len; return usbdRequestNotProcessed; }
__WEAK  usbdRequestStatus USBD_CustomClass2_Endpoint0_InDataSent          (uint32_t len)                                                       { (void)len; return usbdRequestNotProcessed; }
__WEAK  void              USBD_CustomClass2_Endpoint1_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass2_Endpoint2_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass2_Endpoint3_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass2_Endpoint4_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass2_Endpoint5_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass2_Endpoint6_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass2_Endpoint7_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass2_Endpoint8_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass2_Endpoint9_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass2_Endpoint10_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass2_Endpoint11_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass2_Endpoint12_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass2_Endpoint13_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass2_Endpoint14_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass2_Endpoint15_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
#endif
#ifdef  RTE_USB_Device_CustomClass_3
__WEAK  void              USBD_CustomClass3_Initialize                    (void)                                                               { }
__WEAK  void              USBD_CustomClass3_Uninitialize                  (void)                                                               { }
extern  void              USBD_CustomClass3_EndpointReset                 (void);
__WEAK  void              USBD_CustomClass3_EndpointReset                 (void)            /* Deprecated */                                   { }
extern  void              USBD_CustomClass3_EventEndpointStart            (uint8_t ep_addr);
__WEAK  void              USBD_CustomClass3_EventEndpointStart            (uint8_t ep_addr) /* Deprecated */                                   { (void)ep_addr; }
extern  void              USBD_CustomClass3_EventEndpointStop             (uint8_t ep_addr);
__WEAK  void              USBD_CustomClass3_EventEndpointStop             (uint8_t ep_addr) /* Deprecated */                                   { (void)ep_addr; }
__WEAK  void              USBD_CustomClass3_Reset                         (void)                                                               { USBD_CustomClass3_EndpointReset      ();        }
__WEAK  void              USBD_CustomClass3_EndpointStart                 (uint8_t ep_addr)                                                    { USBD_CustomClass3_EventEndpointStart (ep_addr); }
__WEAK  void              USBD_CustomClass3_EndpointStop                  (uint8_t ep_addr)                                                    { USBD_CustomClass3_EventEndpointStop  (ep_addr); }
__WEAK  usbdRequestStatus USBD_CustomClass3_Endpoint0_SetupPacketReceived (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len) { (void)setup_packet; (void)buf; (void)len; return usbdRequestNotProcessed; }
__WEAK  void              USBD_CustomClass3_Endpoint0_SetupPacketProcessed(const USB_SETUP_PACKET *setup_packet)                               { (void)setup_packet;                                                       }
__WEAK  usbdRequestStatus USBD_CustomClass3_Endpoint0_OutDataReceived     (uint32_t len)                                                       { (void)len; return usbdRequestNotProcessed; }
__WEAK  usbdRequestStatus USBD_CustomClass3_Endpoint0_InDataSent          (uint32_t len)                                                       { (void)len; return usbdRequestNotProcessed; }
__WEAK  void              USBD_CustomClass3_Endpoint1_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass3_Endpoint2_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass3_Endpoint3_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass3_Endpoint4_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass3_Endpoint5_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass3_Endpoint6_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass3_Endpoint7_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass3_Endpoint8_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass3_Endpoint9_Event               (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass3_Endpoint10_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass3_Endpoint11_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass3_Endpoint12_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass3_Endpoint13_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass3_Endpoint14_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
__WEAK  void              USBD_CustomClass3_Endpoint15_Event              (uint32_t event)                                                     { (void)event; USBD_ThreadFlagsWait(0xFFFFFFFFU); }
#endif

extern 
void (* const fpUSBD_CustomClass_Initialize[USBD_CUSTOM_CLASS_NUM]) (void);
void (* const fpUSBD_CustomClass_Initialize[USBD_CUSTOM_CLASS_NUM]) (void) = {
        USBD_CustomClass0_Initialize
#ifdef  RTE_USB_Device_CustomClass_1
      , USBD_CustomClass1_Initialize
#endif
#ifdef  RTE_USB_Device_CustomClass_2
      , USBD_CustomClass2_Initialize
#endif
#ifdef  RTE_USB_Device_CustomClass_3
      , USBD_CustomClass3_Initialize
#endif
};

extern 
void (* const fpUSBD_CustomClass_Uninitialize[USBD_CUSTOM_CLASS_NUM]) (void);
void (* const fpUSBD_CustomClass_Uninitialize[USBD_CUSTOM_CLASS_NUM]) (void) = {
        USBD_CustomClass0_Uninitialize
#ifdef  RTE_USB_Device_CustomClass_1
      , USBD_CustomClass1_Uninitialize
#endif
#ifdef  RTE_USB_Device_CustomClass_2
      , USBD_CustomClass2_Uninitialize
#endif
#ifdef  RTE_USB_Device_CustomClass_3
      , USBD_CustomClass3_Uninitialize
#endif
};

extern 
void (* const fpUSBD_CustomClass_Reset[USBD_CUSTOM_CLASS_NUM]) (void);
void (* const fpUSBD_CustomClass_Reset[USBD_CUSTOM_CLASS_NUM]) (void) = {
        USBD_CustomClass0_Reset
#ifdef  RTE_USB_Device_CustomClass_1
      , USBD_CustomClass1_Reset
#endif
#ifdef  RTE_USB_Device_CustomClass_2
      , USBD_CustomClass2_Reset
#endif
#ifdef  RTE_USB_Device_CustomClass_3
      , USBD_CustomClass3_Reset
#endif
};

extern 
void (* const fpUSBD_CustomClass_EndpointStart[USBD_CUSTOM_CLASS_NUM]) (uint8_t ep_addr);
void (* const fpUSBD_CustomClass_EndpointStart[USBD_CUSTOM_CLASS_NUM]) (uint8_t ep_addr) = {
        USBD_CustomClass0_EndpointStart
#ifdef  RTE_USB_Device_CustomClass_1
      , USBD_CustomClass1_EndpointStart
#endif
#ifdef  RTE_USB_Device_CustomClass_2
      , USBD_CustomClass2_EndpointStart
#endif
#ifdef  RTE_USB_Device_CustomClass_3
      , USBD_CustomClass3_EndpointStart
#endif
};

extern 
void (* const fpUSBD_CustomClass_EndpointStop[USBD_CUSTOM_CLASS_NUM]) (uint8_t ep_addr);
void (* const fpUSBD_CustomClass_EndpointStop[USBD_CUSTOM_CLASS_NUM]) (uint8_t ep_addr) = {
        USBD_CustomClass0_EndpointStop
#ifdef  RTE_USB_Device_CustomClass_1
      , USBD_CustomClass1_EndpointStop
#endif
#ifdef  RTE_USB_Device_CustomClass_2
      , USBD_CustomClass2_EndpointStop
#endif
#ifdef  RTE_USB_Device_CustomClass_3
      , USBD_CustomClass3_EndpointStop
#endif
};

extern 
usbdRequestStatus (* const fpUSBD_CustomClass_Endpoint0_SetupPacketReceived[USBD_CUSTOM_CLASS_NUM]) (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len);
usbdRequestStatus (* const fpUSBD_CustomClass_Endpoint0_SetupPacketReceived[USBD_CUSTOM_CLASS_NUM]) (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len) = {
        USBD_CustomClass0_Endpoint0_SetupPacketReceived
#ifdef  RTE_USB_Device_CustomClass_1
      , USBD_CustomClass1_Endpoint0_SetupPacketReceived
#endif
#ifdef  RTE_USB_Device_CustomClass_2
      , USBD_CustomClass2_Endpoint0_SetupPacketReceived
#endif
#ifdef  RTE_USB_Device_CustomClass_3
      , USBD_CustomClass3_Endpoint0_SetupPacketReceived
#endif
};

extern 
void (* const fpUSBD_CustomClass_Endpoint0_SetupPacketProcessed[USBD_CUSTOM_CLASS_NUM]) (const USB_SETUP_PACKET *setup_packet);
void (* const fpUSBD_CustomClass_Endpoint0_SetupPacketProcessed[USBD_CUSTOM_CLASS_NUM]) (const USB_SETUP_PACKET *setup_packet) = {
        USBD_CustomClass0_Endpoint0_SetupPacketProcessed
#ifdef  RTE_USB_Device_CustomClass_1
      , USBD_CustomClass1_Endpoint0_SetupPacketProcessed
#endif
#ifdef  RTE_USB_Device_CustomClass_2
      , USBD_CustomClass2_Endpoint0_SetupPacketProcessed
#endif
#ifdef  RTE_USB_Device_CustomClass_3
      , USBD_CustomClass3_Endpoint0_SetupPacketProcessed
#endif
};

extern 
usbdRequestStatus (* const fpUSBD_CustomClass_Endpoint0_OutDataReceived[USBD_CUSTOM_CLASS_NUM]) (uint32_t len);
usbdRequestStatus (* const fpUSBD_CustomClass_Endpoint0_OutDataReceived[USBD_CUSTOM_CLASS_NUM]) (uint32_t len) = {
        USBD_CustomClass0_Endpoint0_OutDataReceived
#ifdef  RTE_USB_Device_CustomClass_1
      , USBD_CustomClass1_Endpoint0_OutDataReceived
#endif
#ifdef  RTE_USB_Device_CustomClass_2
      , USBD_CustomClass2_Endpoint0_OutDataReceived
#endif
#ifdef  RTE_USB_Device_CustomClass_3
      , USBD_CustomClass3_Endpoint0_OutDataReceived
#endif
};

extern 
usbdRequestStatus (* const fpUSBD_CustomClass_Endpoint0_InDataSent[USBD_CUSTOM_CLASS_NUM]) (uint32_t len);
usbdRequestStatus (* const fpUSBD_CustomClass_Endpoint0_InDataSent[USBD_CUSTOM_CLASS_NUM]) (uint32_t len) = {
        USBD_CustomClass0_Endpoint0_InDataSent
#ifdef  RTE_USB_Device_CustomClass_1
      , USBD_CustomClass1_Endpoint0_InDataSent
#endif
#ifdef  RTE_USB_Device_CustomClass_2
      , USBD_CustomClass2_Endpoint0_InDataSent
#endif
#ifdef  RTE_USB_Device_CustomClass_3
      , USBD_CustomClass3_Endpoint0_InDataSent
#endif
};

#if    (USBD_EP_LAST_NUM >= 15)
#define CUSTOM_CLASS_EP_EVENT_FUNC(n)  0, USBD_CustomClass##n##_Endpoint1_Event , USBD_CustomClass##n##_Endpoint2_Event , USBD_CustomClass##n##_Endpoint3_Event , USBD_CustomClass##n##_Endpoint4_Event,  \
                                          USBD_CustomClass##n##_Endpoint5_Event , USBD_CustomClass##n##_Endpoint6_Event , USBD_CustomClass##n##_Endpoint7_Event , USBD_CustomClass##n##_Endpoint8_Event,  \
                                          USBD_CustomClass##n##_Endpoint9_Event , USBD_CustomClass##n##_Endpoint10_Event, USBD_CustomClass##n##_Endpoint11_Event, USBD_CustomClass##n##_Endpoint12_Event, \
                                          USBD_CustomClass##n##_Endpoint13_Event, USBD_CustomClass##n##_Endpoint14_Event, USBD_CustomClass##n##_Endpoint15_Event
#elif  (USBD_EP_LAST_NUM == 14)
#define CUSTOM_CLASS_EP_EVENT_FUNC(n)  0, USBD_CustomClass##n##_Endpoint1_Event , USBD_CustomClass##n##_Endpoint2_Event , USBD_CustomClass##n##_Endpoint3_Event , USBD_CustomClass##n##_Endpoint4_Event,  \
                                          USBD_CustomClass##n##_Endpoint5_Event , USBD_CustomClass##n##_Endpoint6_Event , USBD_CustomClass##n##_Endpoint7_Event , USBD_CustomClass##n##_Endpoint8_Event,  \
                                          USBD_CustomClass##n##_Endpoint9_Event , USBD_CustomClass##n##_Endpoint10_Event, USBD_CustomClass##n##_Endpoint11_Event, USBD_CustomClass##n##_Endpoint12_Event, \
                                          USBD_CustomClass##n##_Endpoint13_Event, USBD_CustomClass##n##_Endpoint14_Event
#elif  (USBD_EP_LAST_NUM == 13)
#define CUSTOM_CLASS_EP_EVENT_FUNC(n)  0, USBD_CustomClass##n##_Endpoint1_Event , USBD_CustomClass##n##_Endpoint2_Event , USBD_CustomClass##n##_Endpoint3_Event , USBD_CustomClass##n##_Endpoint4_Event,  \
                                          USBD_CustomClass##n##_Endpoint5_Event , USBD_CustomClass##n##_Endpoint6_Event , USBD_CustomClass##n##_Endpoint7_Event , USBD_CustomClass##n##_Endpoint8_Event,  \
                                          USBD_CustomClass##n##_Endpoint9_Event , USBD_CustomClass##n##_Endpoint10_Event, USBD_CustomClass##n##_Endpoint11_Event, USBD_CustomClass##n##_Endpoint12_Event, \
                                          USBD_CustomClass##n##_Endpoint13_Event
#elif  (USBD_EP_LAST_NUM == 12)
#define CUSTOM_CLASS_EP_EVENT_FUNC(n)  0, USBD_CustomClass##n##_Endpoint1_Event , USBD_CustomClass##n##_Endpoint2_Event , USBD_CustomClass##n##_Endpoint3_Event , USBD_CustomClass##n##_Endpoint4_Event,  \
                                          USBD_CustomClass##n##_Endpoint5_Event , USBD_CustomClass##n##_Endpoint6_Event , USBD_CustomClass##n##_Endpoint7_Event , USBD_CustomClass##n##_Endpoint8_Event,  \
                                          USBD_CustomClass##n##_Endpoint9_Event , USBD_CustomClass##n##_Endpoint10_Event, USBD_CustomClass##n##_Endpoint11_Event, USBD_CustomClass##n##_Endpoint12_Event
#elif  (USBD_EP_LAST_NUM == 11)
#define CUSTOM_CLASS_EP_EVENT_FUNC(n)  0, USBD_CustomClass##n##_Endpoint1_Event , USBD_CustomClass##n##_Endpoint2_Event , USBD_CustomClass##n##_Endpoint3_Event , USBD_CustomClass##n##_Endpoint4_Event,  \
                                          USBD_CustomClass##n##_Endpoint5_Event , USBD_CustomClass##n##_Endpoint6_Event , USBD_CustomClass##n##_Endpoint7_Event , USBD_CustomClass##n##_Endpoint8_Event,  \
                                          USBD_CustomClass##n##_Endpoint9_Event , USBD_CustomClass##n##_Endpoint10_Event, USBD_CustomClass##n##_Endpoint11_Event
#elif  (USBD_EP_LAST_NUM == 10)
#define CUSTOM_CLASS_EP_EVENT_FUNC(n)  0, USBD_CustomClass##n##_Endpoint1_Event , USBD_CustomClass##n##_Endpoint2_Event , USBD_CustomClass##n##_Endpoint3_Event , USBD_CustomClass##n##_Endpoint4_Event,  \
                                          USBD_CustomClass##n##_Endpoint5_Event , USBD_CustomClass##n##_Endpoint6_Event , USBD_CustomClass##n##_Endpoint7_Event , USBD_CustomClass##n##_Endpoint8_Event,  \
                                          USBD_CustomClass##n##_Endpoint9_Event , USBD_CustomClass##n##_Endpoint10_Event
#elif  (USBD_EP_LAST_NUM ==  9)
#define CUSTOM_CLASS_EP_EVENT_FUNC(n)  0, USBD_CustomClass##n##_Endpoint1_Event , USBD_CustomClass##n##_Endpoint2_Event , USBD_CustomClass##n##_Endpoint3_Event , USBD_CustomClass##n##_Endpoint4_Event,  \
                                          USBD_CustomClass##n##_Endpoint5_Event , USBD_CustomClass##n##_Endpoint6_Event , USBD_CustomClass##n##_Endpoint7_Event , USBD_CustomClass##n##_Endpoint8_Event,  \
                                          USBD_CustomClass##n##_Endpoint9_Event
#elif  (USBD_EP_LAST_NUM ==  8)
#define CUSTOM_CLASS_EP_EVENT_FUNC(n)  0, USBD_CustomClass##n##_Endpoint1_Event , USBD_CustomClass##n##_Endpoint2_Event , USBD_CustomClass##n##_Endpoint3_Event , USBD_CustomClass##n##_Endpoint4_Event,  \
                                          USBD_CustomClass##n##_Endpoint5_Event , USBD_CustomClass##n##_Endpoint6_Event , USBD_CustomClass##n##_Endpoint7_Event , USBD_CustomClass##n##_Endpoint8_Event
#elif  (USBD_EP_LAST_NUM ==  7)
#define CUSTOM_CLASS_EP_EVENT_FUNC(n)  0, USBD_CustomClass##n##_Endpoint1_Event , USBD_CustomClass##n##_Endpoint2_Event , USBD_CustomClass##n##_Endpoint3_Event , USBD_CustomClass##n##_Endpoint4_Event,  \
                                          USBD_CustomClass##n##_Endpoint5_Event , USBD_CustomClass##n##_Endpoint6_Event , USBD_CustomClass##n##_Endpoint7_Event
#elif  (USBD_EP_LAST_NUM ==  6)
#define CUSTOM_CLASS_EP_EVENT_FUNC(n)  0, USBD_CustomClass##n##_Endpoint1_Event , USBD_CustomClass##n##_Endpoint2_Event , USBD_CustomClass##n##_Endpoint3_Event , USBD_CustomClass##n##_Endpoint4_Event,  \
                                          USBD_CustomClass##n##_Endpoint5_Event , USBD_CustomClass##n##_Endpoint6_Event
#elif  (USBD_EP_LAST_NUM ==  5)
#define CUSTOM_CLASS_EP_EVENT_FUNC(n)  0, USBD_CustomClass##n##_Endpoint1_Event , USBD_CustomClass##n##_Endpoint2_Event , USBD_CustomClass##n##_Endpoint3_Event , USBD_CustomClass##n##_Endpoint4_Event,  \
                                          USBD_CustomClass##n##_Endpoint5_Event
#elif  (USBD_EP_LAST_NUM ==  4)
#define CUSTOM_CLASS_EP_EVENT_FUNC(n)  0, USBD_CustomClass##n##_Endpoint1_Event , USBD_CustomClass##n##_Endpoint2_Event , USBD_CustomClass##n##_Endpoint3_Event , USBD_CustomClass##n##_Endpoint4_Event
#elif  (USBD_EP_LAST_NUM ==  3)
#define CUSTOM_CLASS_EP_EVENT_FUNC(n)  0, USBD_CustomClass##n##_Endpoint1_Event , USBD_CustomClass##n##_Endpoint2_Event , USBD_CustomClass##n##_Endpoint3_Event
#elif  (USBD_EP_LAST_NUM ==  2)
#define CUSTOM_CLASS_EP_EVENT_FUNC(n)  0, USBD_CustomClass##n##_Endpoint1_Event , USBD_CustomClass##n##_Endpoint2_Event
#elif  (USBD_EP_LAST_NUM ==  1)
#define CUSTOM_CLASS_EP_EVENT_FUNC(n)  0, USBD_CustomClass##n##_Endpoint1_Event
#elif  (USBD_EP_LAST_NUM ==  0)
#define CUSTOM_CLASS_EP_EVENT_FUNC(n)  0
#else
#define CUSTOM_CLASS_EP_EVENT_FUNC(n)
#endif

extern 
void (* const fpUSBD_Endpoint_Event[USBD_CUSTOM_CLASS_NUM * USBD_EP_NUM]) (uint32_t event);
void (* const fpUSBD_Endpoint_Event[USBD_CUSTOM_CLASS_NUM * USBD_EP_NUM]) (uint32_t event) = {
        CUSTOM_CLASS_EP_EVENT_FUNC(0)
#ifdef  RTE_USB_Device_CustomClass_1
    ,   CUSTOM_CLASS_EP_EVENT_FUNC(1)
#endif
#ifdef  RTE_USB_Device_CustomClass_2
    ,   CUSTOM_CLASS_EP_EVENT_FUNC(2)
#endif
#ifdef  RTE_USB_Device_CustomClass_3
    ,   CUSTOM_CLASS_EP_EVENT_FUNC(3)
#endif
};
#endif

#if    (USBD_ADC_NUM == 0)
        bool      USBD_Endpoint0_Setup_ADC_ReqToIF    (uint8_t device);
        bool      USBD_Endpoint0_Setup_ADC_ReqToIF    (uint8_t device)               { (void)device; return false; }
        bool      USBD_Endpoint0_Out_ADC_ReqToIF      (uint8_t device);
        bool      USBD_Endpoint0_Out_ADC_ReqToIF      (uint8_t device)               { (void)device; return false; }
        bool      USBD_Endpoint0_Setup_ADC_ReqToEP    (uint8_t device);
        bool      USBD_Endpoint0_Setup_ADC_ReqToEP    (uint8_t device)               { (void)device; return false; }
        bool      USBD_Endpoint0_Out_ADC_ReqToEP      (uint8_t device);
        bool      USBD_Endpoint0_Out_ADC_ReqToEP      (uint8_t device)               { (void)device; return false; }
#else
__WEAK  void      USBD_ADC0_Initialize                (void)                         {}
__WEAK  void      USBD_ADC0_Uninitialize              (void)                         {}
extern  void      USBD_ADC0_SpeakerStatusEvent        (bool active);
__WEAK  void      USBD_ADC0_SpeakerStatusEvent        (bool active)                  { (void)active; } /* Deprecated */
extern  void      USBD_ADC0_SpeakerMuteEvent          (uint8_t ch, bool     cur);
__WEAK  void      USBD_ADC0_SpeakerMuteEvent          (uint8_t ch, bool     cur)     { (void)ch; (void)cur; } /* Deprecated */
extern  void      USBD_ADC0_SpeakerVolumeEvent        (uint8_t ch, uint16_t cur);
__WEAK  void      USBD_ADC0_SpeakerVolumeEvent        (uint8_t ch, uint16_t cur)     { (void)ch; (void)cur; } /* Deprecated */
extern  void      USBD_ADC0_MicrophoneStatusEvent     (bool active);
__WEAK  void      USBD_ADC0_MicrophoneStatusEvent     (bool active)                  { (void)active; } /* Deprecated */
extern  void      USBD_ADC0_MicrophoneMuteEvent       (uint8_t ch, bool     cur);
__WEAK  void      USBD_ADC0_MicrophoneMuteEvent       (uint8_t ch, bool     cur)     { (void)ch; (void)cur; } /* Deprecated */
extern  void      USBD_ADC0_MicrophoneVolumeEvent     (uint8_t ch, uint16_t cur);
__WEAK  void      USBD_ADC0_MicrophoneVolumeEvent     (uint8_t ch, uint16_t cur)     { (void)ch; (void)cur; } /* Deprecated */
__WEAK  void      USBD_ADC0_PlayStart                 (void)                         { USBD_ADC0_SpeakerStatusEvent    (true );   }
__WEAK  void      USBD_ADC0_PlayStop                  (void)                         { USBD_ADC0_SpeakerStatusEvent    (false);   }
__WEAK  void      USBD_ADC0_SetSpeakerMute            (uint8_t ch, bool     on)      { USBD_ADC0_SpeakerMuteEvent      (ch, on);  }
__WEAK  void      USBD_ADC0_SetSpeakerVolume          (uint8_t ch, uint16_t vol)     { USBD_ADC0_SpeakerVolumeEvent    (ch, vol); }
__WEAK  void      USBD_ADC0_RecordStart               (void)                         { USBD_ADC0_MicrophoneStatusEvent (true );   }
__WEAK  void      USBD_ADC0_RecordStop                (void)                         { USBD_ADC0_MicrophoneStatusEvent (false);   }
__WEAK  void      USBD_ADC0_SetMicrophoneMute         (uint8_t ch, bool     on)      { USBD_ADC0_MicrophoneMuteEvent   (ch, on);  }
__WEAK  void      USBD_ADC0_SetMicrophoneVolume       (uint8_t ch, uint16_t vol)     { USBD_ADC0_MicrophoneVolumeEvent (ch, vol); }
__WEAK  void      USBD_ADC0_ReceivedSamples           (void)                         {}
#ifdef  RTE_USB_Device_ADC_1
__WEAK  void      USBD_ADC1_Initialize                (void)                         {}
__WEAK  void      USBD_ADC1_Uninitialize              (void)                         {}
extern  void      USBD_ADC1_SpeakerStatusEvent        (bool active);
__WEAK  void      USBD_ADC1_SpeakerStatusEvent        (bool active)                  { (void)active; } /* Deprecated */
extern  void      USBD_ADC1_SpeakerMuteEvent          (uint8_t ch, bool     cur);
__WEAK  void      USBD_ADC1_SpeakerMuteEvent          (uint8_t ch, bool     cur)     { (void)ch; (void)cur; } /* Deprecated */
extern  void      USBD_ADC1_SpeakerVolumeEvent        (uint8_t ch, uint16_t cur);
__WEAK  void      USBD_ADC1_SpeakerVolumeEvent        (uint8_t ch, uint16_t cur)     { (void)ch; (void)cur; } /* Deprecated */
extern  void      USBD_ADC1_MicrophoneStatusEvent     (bool active);
__WEAK  void      USBD_ADC1_MicrophoneStatusEvent     (bool active)                  { (void)active; } /* Deprecated */
extern  void      USBD_ADC1_MicrophoneMuteEvent       (uint8_t ch, bool     cur);
__WEAK  void      USBD_ADC1_MicrophoneMuteEvent       (uint8_t ch, bool     cur)     { (void)ch; (void)cur; } /* Deprecated */
extern  void      USBD_ADC1_MicrophoneVolumeEvent     (uint8_t ch, uint16_t cur);
__WEAK  void      USBD_ADC1_MicrophoneVolumeEvent     (uint8_t ch, uint16_t cur)     { (void)ch; (void)cur; } /* Deprecated */
__WEAK  void      USBD_ADC1_PlayStart                 (void)                         { USBD_ADC1_SpeakerStatusEvent    (true );   }
__WEAK  void      USBD_ADC1_PlayStop                  (void)                         { USBD_ADC1_SpeakerStatusEvent    (false);   }
__WEAK  void      USBD_ADC1_SetSpeakerMute            (uint8_t ch, bool     on)      { USBD_ADC1_SpeakerMuteEvent      (ch, on);  }
__WEAK  void      USBD_ADC1_SetSpeakerVolume          (uint8_t ch, uint16_t vol)     { USBD_ADC1_SpeakerVolumeEvent    (ch, vol); }
__WEAK  void      USBD_ADC1_RecordStart               (void)                         { USBD_ADC1_MicrophoneStatusEvent (true );   }
__WEAK  void      USBD_ADC1_RecordStop                (void)                         { USBD_ADC1_MicrophoneStatusEvent (false);   }
__WEAK  void      USBD_ADC1_SetMicrophoneMute         (uint8_t ch, bool     on)      { USBD_ADC1_MicrophoneMuteEvent   (ch, on);  }
__WEAK  void      USBD_ADC1_SetMicrophoneVolume       (uint8_t ch, uint16_t vol)     { USBD_ADC1_MicrophoneVolumeEvent (ch, vol); }
__WEAK  void      USBD_ADC1_ReceivedSamples           (void)                         {}
#endif
#ifdef  RTE_USB_Device_ADC_2
__WEAK  void      USBD_ADC2_Initialize                (void)                         {}
__WEAK  void      USBD_ADC2_Uninitialize              (void)                         {}
extern  void      USBD_ADC2_SpeakerStatusEvent        (bool active);
__WEAK  void      USBD_ADC2_SpeakerStatusEvent        (bool active)                  { (void)active; } /* Deprecated */
extern  void      USBD_ADC2_SpeakerMuteEvent          (uint8_t ch, bool     cur);
__WEAK  void      USBD_ADC2_SpeakerMuteEvent          (uint8_t ch, bool     cur)     { (void)ch; (void)cur; } /* Deprecated */
extern  void      USBD_ADC2_SpeakerVolumeEvent        (uint8_t ch, uint16_t cur);
__WEAK  void      USBD_ADC2_SpeakerVolumeEvent        (uint8_t ch, uint16_t cur)     { (void)ch; (void)cur; } /* Deprecated */
extern  void      USBD_ADC2_MicrophoneStatusEvent     (bool active);
__WEAK  void      USBD_ADC2_MicrophoneStatusEvent     (bool active)                  { (void)active; } /* Deprecated */
extern  void      USBD_ADC2_MicrophoneMuteEvent       (uint8_t ch, bool     cur);
__WEAK  void      USBD_ADC2_MicrophoneMuteEvent       (uint8_t ch, bool     cur)     { (void)ch; (void)cur; } /* Deprecated */
extern  void      USBD_ADC2_MicrophoneVolumeEvent     (uint8_t ch, uint16_t cur);
__WEAK  void      USBD_ADC2_MicrophoneVolumeEvent     (uint8_t ch, uint16_t cur)     { (void)ch; (void)cur; } /* Deprecated */
__WEAK  void      USBD_ADC2_PlayStart                 (void)                         { USBD_ADC2_SpeakerStatusEvent    (true );   }
__WEAK  void      USBD_ADC2_PlayStop                  (void)                         { USBD_ADC2_SpeakerStatusEvent    (false);   }
__WEAK  void      USBD_ADC2_SetSpeakerMute            (uint8_t ch, bool     on)      { USBD_ADC2_SpeakerMuteEvent      (ch, on);  }
__WEAK  void      USBD_ADC2_SetSpeakerVolume          (uint8_t ch, uint16_t vol)     { USBD_ADC2_SpeakerVolumeEvent    (ch, vol); }
__WEAK  void      USBD_ADC2_RecordStart               (void)                         { USBD_ADC2_MicrophoneStatusEvent (true );   }
__WEAK  void      USBD_ADC2_RecordStop                (void)                         { USBD_ADC2_MicrophoneStatusEvent (false);   }
__WEAK  void      USBD_ADC2_SetMicrophoneMute         (uint8_t ch, bool     on)      { USBD_ADC2_MicrophoneMuteEvent   (ch, on);  }
__WEAK  void      USBD_ADC2_SetMicrophoneVolume       (uint8_t ch, uint16_t vol)     { USBD_ADC2_MicrophoneVolumeEvent (ch, vol); }
__WEAK  void      USBD_ADC2_ReceivedSamples           (void)                         {}
#endif
#ifdef  RTE_USB_Device_ADC_3
__WEAK  void      USBD_ADC3_Initialize                (void)                         {}
__WEAK  void      USBD_ADC3_Uninitialize              (void)                         {}
extern  void      USBD_ADC3_SpeakerStatusEvent        (bool active);
__WEAK  void      USBD_ADC3_SpeakerStatusEvent        (bool active)                  { (void)active; } /* Deprecated */
extern  void      USBD_ADC3_SpeakerMuteEvent          (uint8_t ch, bool     cur);
__WEAK  void      USBD_ADC3_SpeakerMuteEvent          (uint8_t ch, bool     cur)     { (void)ch; (void)cur; } /* Deprecated */
extern  void      USBD_ADC3_SpeakerVolumeEvent        (uint8_t ch, uint16_t cur);
__WEAK  void      USBD_ADC3_SpeakerVolumeEvent        (uint8_t ch, uint16_t cur)     { (void)ch; (void)cur; } /* Deprecated */
extern  void      USBD_ADC3_MicrophoneStatusEvent     (bool active);
__WEAK  void      USBD_ADC3_MicrophoneStatusEvent     (bool active)                  { (void)active; } /* Deprecated */
extern  void      USBD_ADC3_MicrophoneMuteEvent       (uint8_t ch, bool     cur);
__WEAK  void      USBD_ADC3_MicrophoneMuteEvent       (uint8_t ch, bool     cur)     { (void)ch; (void)cur; } /* Deprecated */
extern  void      USBD_ADC3_MicrophoneVolumeEvent     (uint8_t ch, uint16_t cur);
__WEAK  void      USBD_ADC3_MicrophoneVolumeEvent     (uint8_t ch, uint16_t cur)     { (void)ch; (void)cur; } /* Deprecated */
__WEAK  void      USBD_ADC3_PlayStart                 (void)                         { USBD_ADC3_SpeakerStatusEvent    (true );   }
__WEAK  void      USBD_ADC3_PlayStop                  (void)                         { USBD_ADC3_SpeakerStatusEvent    (false);   }
__WEAK  void      USBD_ADC3_SetSpeakerMute            (uint8_t ch, bool     on)      { USBD_ADC3_SpeakerMuteEvent      (ch, on);  }
__WEAK  void      USBD_ADC3_SetSpeakerVolume          (uint8_t ch, uint16_t vol)     { USBD_ADC3_SpeakerVolumeEvent    (ch, vol); }
__WEAK  void      USBD_ADC3_RecordStart               (void)                         { USBD_ADC3_MicrophoneStatusEvent (true );   }
__WEAK  void      USBD_ADC3_RecordStop                (void)                         { USBD_ADC3_MicrophoneStatusEvent (false);   }
__WEAK  void      USBD_ADC3_SetMicrophoneMute         (uint8_t ch, bool     on)      { USBD_ADC3_MicrophoneMuteEvent   (ch, on);  }
__WEAK  void      USBD_ADC3_SetMicrophoneVolume       (uint8_t ch, uint16_t vol)     { USBD_ADC3_MicrophoneVolumeEvent (ch, vol); }
__WEAK  void      USBD_ADC3_ReceivedSamples           (void)                         {}
#endif

extern 
void (* const fpUSBD_ADC_Initialize[USBD_ADC_NUM]) (void);
void (* const fpUSBD_ADC_Initialize[USBD_ADC_NUM]) (void) = {
        USBD_ADC0_Initialize
#ifdef  RTE_USB_Device_ADC_1
      , USBD_ADC1_Initialize
#endif
#ifdef  RTE_USB_Device_ADC_2
      , USBD_ADC2_Initialize
#endif
#ifdef  RTE_USB_Device_ADC_3
      , USBD_ADC3_Initialize
#endif
};
extern 
void (* const fpUSBD_ADC_Uninitialize[USBD_ADC_NUM]) (void);
void (* const fpUSBD_ADC_Uninitialize[USBD_ADC_NUM]) (void) = {
        USBD_ADC0_Uninitialize
#ifdef  RTE_USB_Device_ADC_1
      , USBD_ADC1_Uninitialize
#endif
#ifdef  RTE_USB_Device_ADC_2
      , USBD_ADC2_Uninitialize
#endif
#ifdef  RTE_USB_Device_ADC_3
      , USBD_ADC3_Uninitialize
#endif
};
extern 
void (* const fpUSBD_ADC_PlayStart[USBD_ADC_NUM]) (void);
void (* const fpUSBD_ADC_PlayStart[USBD_ADC_NUM]) (void) = {
        USBD_ADC0_PlayStart
#ifdef  RTE_USB_Device_ADC_1
      , USBD_ADC1_PlayStart
#endif
#ifdef  RTE_USB_Device_ADC_2
      , USBD_ADC2_PlayStart
#endif
#ifdef  RTE_USB_Device_ADC_3
      , USBD_ADC3_PlayStart
#endif
};
extern 
void (* const fpUSBD_ADC_PlayStop[USBD_ADC_NUM]) (void);
void (* const fpUSBD_ADC_PlayStop[USBD_ADC_NUM]) (void) = {
        USBD_ADC0_PlayStop
#ifdef  RTE_USB_Device_ADC_1
      , USBD_ADC1_PlayStop
#endif
#ifdef  RTE_USB_Device_ADC_2
      , USBD_ADC2_PlayStop
#endif
#ifdef  RTE_USB_Device_ADC_3
      , USBD_ADC3_PlayStop
#endif
};
extern 
void (* const fpUSBD_ADC_SetSpeakerMute[USBD_ADC_NUM]) (uint8_t ch, bool on);
void (* const fpUSBD_ADC_SetSpeakerMute[USBD_ADC_NUM]) (uint8_t ch, bool on) = {
        USBD_ADC0_SetSpeakerMute
#ifdef  RTE_USB_Device_ADC_1
      , USBD_ADC1_SetSpeakerMute
#endif
#ifdef  RTE_USB_Device_ADC_2
      , USBD_ADC2_SetSpeakerMute
#endif
#ifdef  RTE_USB_Device_ADC_3
      , USBD_ADC3_SetSpeakerMute
#endif
};
extern 
void (* const fpUSBD_ADC_SetSpeakerVolume[USBD_ADC_NUM]) (uint8_t ch, uint16_t vol);
void (* const fpUSBD_ADC_SetSpeakerVolume[USBD_ADC_NUM]) (uint8_t ch, uint16_t vol) = {
        USBD_ADC0_SetSpeakerVolume
#ifdef  RTE_USB_Device_ADC_1
      , USBD_ADC1_SetSpeakerVolume
#endif
#ifdef  RTE_USB_Device_ADC_2
      , USBD_ADC2_SetSpeakerVolume
#endif
#ifdef  RTE_USB_Device_ADC_3
      , USBD_ADC3_SetSpeakerVolume
#endif
};
extern 
void (* const fpUSBD_ADC_RecordStart[USBD_ADC_NUM]) (void);
void (* const fpUSBD_ADC_RecordStart[USBD_ADC_NUM]) (void) = {
        USBD_ADC0_RecordStart
#ifdef  RTE_USB_Device_ADC_1
      , USBD_ADC1_RecordStart
#endif
#ifdef  RTE_USB_Device_ADC_2
      , USBD_ADC2_RecordStart
#endif
#ifdef  RTE_USB_Device_ADC_3
      , USBD_ADC3_RecordStart
#endif
};
extern 
void (* const fpUSBD_ADC_RecordStop[USBD_ADC_NUM]) (void);
void (* const fpUSBD_ADC_RecordStop[USBD_ADC_NUM]) (void) = {
        USBD_ADC0_RecordStop
#ifdef  RTE_USB_Device_ADC_1
      , USBD_ADC1_RecordStop
#endif
#ifdef  RTE_USB_Device_ADC_2
      , USBD_ADC2_RecordStop
#endif
#ifdef  RTE_USB_Device_ADC_3
      , USBD_ADC3_RecordStop
#endif
};
extern 
void (* const fpUSBD_ADC_SetMicrophoneMute[USBD_ADC_NUM]) (uint8_t ch, bool on);
void (* const fpUSBD_ADC_SetMicrophoneMute[USBD_ADC_NUM]) (uint8_t ch, bool on) = {
        USBD_ADC0_SetMicrophoneMute
#ifdef  RTE_USB_Device_ADC_1
      , USBD_ADC1_SetMicrophoneMute
#endif
#ifdef  RTE_USB_Device_ADC_2
      , USBD_ADC2_SetMicrophoneMute
#endif
#ifdef  RTE_USB_Device_ADC_3
      , USBD_ADC3_SetMicrophoneMute
#endif
};
extern 
void (* const fpUSBD_ADC_SetMicrophoneVolume[USBD_ADC_NUM]) (uint8_t ch, uint16_t vol);
void (* const fpUSBD_ADC_SetMicrophoneVolume[USBD_ADC_NUM]) (uint8_t ch, uint16_t vol) = {
        USBD_ADC0_SetMicrophoneVolume
#ifdef  RTE_USB_Device_ADC_1
      , USBD_ADC1_SetMicrophoneVolume
#endif
#ifdef  RTE_USB_Device_ADC_2
      , USBD_ADC2_SetMicrophoneVolume
#endif
#ifdef  RTE_USB_Device_ADC_3
      , USBD_ADC3_SetMicrophoneVolume
#endif
};
extern 
void (* const fpUSBD_ADC_ReceivedSamples[USBD_ADC_NUM]) (void);
void (* const fpUSBD_ADC_ReceivedSamples[USBD_ADC_NUM]) (void) = {
        USBD_ADC0_ReceivedSamples
#ifdef  RTE_USB_Device_ADC_1
      , USBD_ADC1_ReceivedSamples
#endif
#ifdef  RTE_USB_Device_ADC_2
      , USBD_ADC2_ReceivedSamples
#endif
#ifdef  RTE_USB_Device_ADC_3
      , USBD_ADC3_ReceivedSamples
#endif
};
#endif

#if    (USBD_CDC_NUM == 0)
        bool      USBD_Endpoint0_Setup_CDC_ReqToIF (uint8_t device);
        bool      USBD_Endpoint0_Setup_CDC_ReqToIF (uint8_t device)               { (void)device; return false; }
        bool      USBD_Endpoint0_Out_CDC_ReqToIF   (uint8_t device);
        bool      USBD_Endpoint0_Out_CDC_ReqToIF   (uint8_t device)               { (void)device; return false; }
#else
__WEAK  void      USBD_CDC0_ACM_Initialize         (void)                         { }
__WEAK  void      USBD_CDC0_ACM_Uninitialize       (void)                         { }
__WEAK  void      USBD_CDC0_ACM_Reset              (void)                         { }
__WEAK  bool      USBD_CDC0_ACM_SendEncapsulatedCommand (            const uint8_t  *buf, uint16_t  len)  {                (void)buf; (void)len; return false; }
__WEAK  bool      USBD_CDC0_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len)  { (void)max_len; (void)buf; (void)len; return false; }
__WEAK  bool      USBD_CDC0_ACM_SetLineCoding      (const CDC_LINE_CODING *line_coding) { (void)line_coding; return false; }
__WEAK  bool      USBD_CDC0_ACM_GetLineCoding      (      CDC_LINE_CODING *line_coding) { (void)line_coding; return false; }
__WEAK  bool      USBD_CDC0_ACM_SetControlLineState(uint16_t state)               { (void)state; return false; }
__WEAK  void      USBD_CDC0_ACM_DataReceived       (uint32_t len)                 { (void)len; }
__WEAK  void      USBD_CDC0_ACM_DataSent           (void)                         { }
__WEAK  void      USBD_CDC0_NCM_Initialize         (void)                         { }
__WEAK  void      USBD_CDC0_NCM_Uninitialize       (void)                         { }
__WEAK  void      USBD_CDC0_NCM_Reset              (void)                         { }
__WEAK  void      USBD_CDC0_NCM_Start              (void)                         { }
__WEAK  void      USBD_CDC0_NCM_Stop               (void)                         { }
__WEAK  bool      USBD_CDC0_NCM_SetEthernetMulticastFilters             (const uint8_t *addr_list, uint16_t num_of_filters)                                   { (void)addr_list; (void)num_of_filters; return false; }
__WEAK  bool      USBD_CDC0_NCM_SetEthernetPowerManagementPatternFilter (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size) { (void)filter_number; (void)pattern_filter; (void)pattern_filter_size; return false; }
__WEAK  bool      USBD_CDC0_NCM_GetEthernetPowerManagementPatternFilter (uint16_t filter_number, uint16_t *pattern_active)                                    { (void)filter_number; (void)pattern_active; return false; }
__WEAK  bool      USBD_CDC0_NCM_SetEthernetPacketFilter                 (uint16_t packet_filter_bitmap)                                                       { (void)packet_filter_bitmap; return false; }
__WEAK  bool      USBD_CDC0_NCM_GetEthernetStatistic                    (uint16_t feature_selector, uint32_t *data)                                           { (void)feature_selector, (void)data; return false; }
__WEAK  bool      USBD_CDC0_NCM_GetNtbParameters   (CDC_NCM_NTB_PARAM *ntb_params){ (void)ntb_params; return false; }
__WEAK  bool      USBD_CDC0_NCM_GetNetAddress      (uint8_t *addr)                { (void)addr; return false; }
__WEAK  bool      USBD_CDC0_NCM_SetNetAddress      (const uint8_t *addr)          { (void)addr; return false; }
__WEAK  bool      USBD_CDC0_NCM_GetNtbFormat       (uint16_t *ntb_format)         { (void)ntb_format; return false; }
__WEAK  bool      USBD_CDC0_NCM_SetNtbFormat       (uint16_t  ntb_format)         { (void)ntb_format; return false; }
__WEAK  bool      USBD_CDC0_NCM_GetNtbInputSize    (uint32_t *ntb_input_size)     { (void)ntb_input_size; return false; }
__WEAK  bool      USBD_CDC0_NCM_SetNtbInputSize    (uint32_t  ntb_input_size)     { (void)ntb_input_size; return false; }
__WEAK  bool      USBD_CDC0_NCM_GetMaxDatagramSize (uint16_t *max_datagram_size)  { (void)max_datagram_size; return false; }
__WEAK  bool      USBD_CDC0_NCM_SetMaxDatagramSize (uint16_t  max_datagram_size)  { (void)max_datagram_size; return false; }
__WEAK  bool      USBD_CDC0_NCM_GetCrcMode         (uint16_t *mode)               { (void)mode; return false; }
__WEAK  bool      USBD_CDC0_NCM_SetCrcMode         (uint16_t  mode)               { (void)mode; return false; }
__WEAK  void      USBD_CDC0_NCM_NTB_IN_Sent        (void)                         { }
__WEAK  void      USBD_CDC0_NCM_NTB_OUT_Received   (void)                         { }
#ifdef  RTE_USB_Device_CDC_1
__WEAK  void      USBD_CDC1_ACM_Initialize         (void)                         { }
__WEAK  void      USBD_CDC1_ACM_Uninitialize       (void)                         { }
__WEAK  void      USBD_CDC1_ACM_Reset              (void)                         { }
__WEAK  bool      USBD_CDC1_ACM_SendEncapsulatedCommand (            const uint8_t  *buf, uint16_t  len)  {                (void)buf; (void)len; return false; }
__WEAK  bool      USBD_CDC1_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len)  { (void)max_len; (void)buf; (void)len; return false; }
__WEAK  bool      USBD_CDC1_ACM_SetLineCoding      (const CDC_LINE_CODING *line_coding) { (void)line_coding; return false; }
__WEAK  bool      USBD_CDC1_ACM_GetLineCoding      (      CDC_LINE_CODING *line_coding) { (void)line_coding; return false; }
__WEAK  bool      USBD_CDC1_ACM_SetControlLineState(uint16_t state)               { (void)state; return false; }
__WEAK  void      USBD_CDC1_ACM_DataReceived       (uint32_t len)                 { (void)len; }
__WEAK  void      USBD_CDC1_ACM_DataSent           (void)                         { }
__WEAK  void      USBD_CDC1_NCM_Initialize         (void)                         { }
__WEAK  void      USBD_CDC1_NCM_Uninitialize       (void)                         { }
__WEAK  void      USBD_CDC1_NCM_Reset              (void)                         { }
__WEAK  void      USBD_CDC1_NCM_Start              (void)                         { }
__WEAK  void      USBD_CDC1_NCM_Stop               (void)                         { }
__WEAK  bool      USBD_CDC1_NCM_SetEthernetMulticastFilters             (const uint8_t *addr_list, uint16_t num_of_filters)                                   { (void)addr_list; (void)num_of_filters; return false; }
__WEAK  bool      USBD_CDC1_NCM_SetEthernetPowerManagementPatternFilter (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size) { (void)filter_number; (void)pattern_filter; (void)pattern_filter_size; return false; }
__WEAK  bool      USBD_CDC1_NCM_GetEthernetPowerManagementPatternFilter (uint16_t filter_number, uint16_t *pattern_active)                                    { (void)filter_number; (void)pattern_active; return false; }
__WEAK  bool      USBD_CDC1_NCM_SetEthernetPacketFilter                 (uint16_t packet_filter_bitmap)                                                       { (void)packet_filter_bitmap; return false; }
__WEAK  bool      USBD_CDC1_NCM_GetEthernetStatistic                    (uint16_t feature_selector, uint32_t *data)                                           { (void)feature_selector, (void)data; return false; }
__WEAK  bool      USBD_CDC1_NCM_GetNtbParameters   (CDC_NCM_NTB_PARAM *ntb_params){ (void)ntb_params; return false; }
__WEAK  bool      USBD_CDC1_NCM_GetNetAddress      (uint8_t *addr)                { (void)addr; return false; }
__WEAK  bool      USBD_CDC1_NCM_SetNetAddress      (const uint8_t *addr)          { (void)addr; return false; }
__WEAK  bool      USBD_CDC1_NCM_GetNtbFormat       (uint16_t *ntb_format)         { (void)ntb_format; return false; }
__WEAK  bool      USBD_CDC1_NCM_SetNtbFormat       (uint16_t  ntb_format)         { (void)ntb_format; return false; }
__WEAK  bool      USBD_CDC1_NCM_GetNtbInputSize    (uint32_t *ntb_input_size)     { (void)ntb_input_size; return false; }
__WEAK  bool      USBD_CDC1_NCM_SetNtbInputSize    (uint32_t  ntb_input_size)     { (void)ntb_input_size; return false; }
__WEAK  bool      USBD_CDC1_NCM_GetMaxDatagramSize (uint16_t *max_datagram_size)  { (void)max_datagram_size; return false; }
__WEAK  bool      USBD_CDC1_NCM_SetMaxDatagramSize (uint16_t  max_datagram_size)  { (void)max_datagram_size; return false; }
__WEAK  bool      USBD_CDC1_NCM_GetCrcMode         (uint16_t *mode)               { (void)mode; return false; }
__WEAK  bool      USBD_CDC1_NCM_SetCrcMode         (uint16_t  mode)               { (void)mode; return false; }
__WEAK  void      USBD_CDC1_NCM_NTB_IN_Sent        (void)                         { }
__WEAK  void      USBD_CDC1_NCM_NTB_OUT_Received   (void)                         { }
#endif
#ifdef  RTE_USB_Device_CDC_2
__WEAK  void      USBD_CDC2_ACM_Initialize         (void)                         { }
__WEAK  void      USBD_CDC2_ACM_Uninitialize       (void)                         { }
__WEAK  void      USBD_CDC2_ACM_Reset              (void)                         { }
__WEAK  bool      USBD_CDC2_ACM_SendEncapsulatedCommand (            const uint8_t  *buf, uint16_t  len)  {                (void)buf; (void)len; return false; }
__WEAK  bool      USBD_CDC2_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len)  { (void)max_len; (void)buf; (void)len; return false; }
__WEAK  bool      USBD_CDC2_ACM_SetLineCoding      (const CDC_LINE_CODING *line_coding) { (void)line_coding; return false; }
__WEAK  bool      USBD_CDC2_ACM_GetLineCoding      (      CDC_LINE_CODING *line_coding) { (void)line_coding; return false; }
__WEAK  bool      USBD_CDC2_ACM_SetControlLineState(uint16_t state)               { (void)state; return false; }
__WEAK  void      USBD_CDC2_ACM_DataReceived       (uint32_t len)                 { (void)len; }
__WEAK  void      USBD_CDC2_ACM_DataSent           (void)                         { }
__WEAK  void      USBD_CDC2_NCM_Initialize         (void)                         { }
__WEAK  void      USBD_CDC2_NCM_Uninitialize       (void)                         { }
__WEAK  void      USBD_CDC2_NCM_Reset              (void)                         { }
__WEAK  void      USBD_CDC2_NCM_Start              (void)                         { }
__WEAK  void      USBD_CDC2_NCM_Stop               (void)                         { }
__WEAK  bool      USBD_CDC2_NCM_SetEthernetMulticastFilters             (const uint8_t *addr_list, uint16_t num_of_filters)                                   { (void)addr_list; (void)num_of_filters; return false; }
__WEAK  bool      USBD_CDC2_NCM_SetEthernetPowerManagementPatternFilter (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size) { (void)filter_number; (void)pattern_filter; (void)pattern_filter_size; return false; }
__WEAK  bool      USBD_CDC2_NCM_GetEthernetPowerManagementPatternFilter (uint16_t filter_number, uint16_t *pattern_active)                                    { (void)filter_number; (void)pattern_active; return false; }
__WEAK  bool      USBD_CDC2_NCM_SetEthernetPacketFilter                 (uint16_t packet_filter_bitmap)                                                       { (void)packet_filter_bitmap; return false; }
__WEAK  bool      USBD_CDC2_NCM_GetEthernetStatistic                    (uint16_t feature_selector, uint32_t *data)                                           { (void)feature_selector, (void)data; return false; }
__WEAK  bool      USBD_CDC2_NCM_GetNtbParameters   (CDC_NCM_NTB_PARAM *ntb_params){ (void)ntb_params; return false; }
__WEAK  bool      USBD_CDC2_NCM_GetNetAddress      (uint8_t *addr)                { (void)addr; return false; }
__WEAK  bool      USBD_CDC2_NCM_SetNetAddress      (const uint8_t *addr)          { (void)addr; return false; }
__WEAK  bool      USBD_CDC2_NCM_GetNtbFormat       (uint16_t *ntb_format)         { (void)ntb_format; return false; }
__WEAK  bool      USBD_CDC2_NCM_SetNtbFormat       (uint16_t  ntb_format)         { (void)ntb_format; return false; }
__WEAK  bool      USBD_CDC2_NCM_GetNtbInputSize    (uint32_t *ntb_input_size)     { (void)ntb_input_size; return false; }
__WEAK  bool      USBD_CDC2_NCM_SetNtbInputSize    (uint32_t  ntb_input_size)     { (void)ntb_input_size; return false; }
__WEAK  bool      USBD_CDC2_NCM_GetMaxDatagramSize (uint16_t *max_datagram_size)  { (void)max_datagram_size; return false; }
__WEAK  bool      USBD_CDC2_NCM_SetMaxDatagramSize (uint16_t  max_datagram_size)  { (void)max_datagram_size; return false; }
__WEAK  bool      USBD_CDC2_NCM_GetCrcMode         (uint16_t *mode)               { (void)mode; return false; }
__WEAK  bool      USBD_CDC2_NCM_SetCrcMode         (uint16_t  mode)               { (void)mode; return false; }
__WEAK  void      USBD_CDC2_NCM_NTB_IN_Sent        (void)                         { }
__WEAK  void      USBD_CDC2_NCM_NTB_OUT_Received   (void)                         { }
#endif
#ifdef  RTE_USB_Device_CDC_3
__WEAK  void      USBD_CDC3_ACM_Initialize         (void)                         { }
__WEAK  void      USBD_CDC3_ACM_Uninitialize       (void)                         { }
__WEAK  void      USBD_CDC3_ACM_Reset              (void)                         { }
__WEAK  bool      USBD_CDC3_ACM_SendEncapsulatedCommand (            const uint8_t  *buf, uint16_t  len)  {                (void)buf; (void)len; return false; }
__WEAK  bool      USBD_CDC3_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len)  { (void)max_len; (void)buf; (void)len; return false; }
__WEAK  bool      USBD_CDC3_ACM_SetLineCoding      (const CDC_LINE_CODING *line_coding) { (void)line_coding; return false; }
__WEAK  bool      USBD_CDC3_ACM_GetLineCoding      (      CDC_LINE_CODING *line_coding) { (void)line_coding; return false; }
__WEAK  bool      USBD_CDC3_ACM_SetControlLineState(uint16_t state)               { (void)state; return false; }
__WEAK  void      USBD_CDC3_ACM_DataReceived       (uint32_t len)                 { (void)len; }
__WEAK  void      USBD_CDC3_ACM_DataSent           (void)                         { }
__WEAK  void      USBD_CDC3_NCM_Initialize         (void)                         { }
__WEAK  void      USBD_CDC3_NCM_Uninitialize       (void)                         { }
__WEAK  void      USBD_CDC3_NCM_Reset              (void)                         { }
__WEAK  void      USBD_CDC3_NCM_Start              (void)                         { }
__WEAK  void      USBD_CDC3_NCM_Stop               (void)                         { }
__WEAK  bool      USBD_CDC3_NCM_SetEthernetMulticastFilters             (const uint8_t *addr_list, uint16_t num_of_filters)                                   { (void)addr_list; (void)num_of_filters; return false; }
__WEAK  bool      USBD_CDC3_NCM_SetEthernetPowerManagementPatternFilter (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size) { (void)filter_number; (void)pattern_filter; (void)pattern_filter_size; return false; }
__WEAK  bool      USBD_CDC3_NCM_GetEthernetPowerManagementPatternFilter (uint16_t filter_number, uint16_t *pattern_active)                                    { (void)filter_number; (void)pattern_active; return false; }
__WEAK  bool      USBD_CDC3_NCM_SetEthernetPacketFilter                 (uint16_t packet_filter_bitmap)                                                       { (void)packet_filter_bitmap; return false; }
__WEAK  bool      USBD_CDC3_NCM_GetEthernetStatistic                    (uint16_t feature_selector, uint32_t *data)                                           { (void)feature_selector, (void)data; return false; }
__WEAK  bool      USBD_CDC3_NCM_GetNtbParameters   (CDC_NCM_NTB_PARAM *ntb_params){ (void)ntb_params; return false; }
__WEAK  bool      USBD_CDC3_NCM_GetNetAddress      (uint8_t *addr)                { (void)addr; return false; }
__WEAK  bool      USBD_CDC3_NCM_SetNetAddress      (const uint8_t *addr)          { (void)addr; return false; }
__WEAK  bool      USBD_CDC3_NCM_GetNtbFormat       (uint16_t *ntb_format)         { (void)ntb_format; return false; }
__WEAK  bool      USBD_CDC3_NCM_SetNtbFormat       (uint16_t  ntb_format)         { (void)ntb_format; return false; }
__WEAK  bool      USBD_CDC3_NCM_GetNtbInputSize    (uint32_t *ntb_input_size)     { (void)ntb_input_size; return false; }
__WEAK  bool      USBD_CDC3_NCM_SetNtbInputSize    (uint32_t  ntb_input_size)     { (void)ntb_input_size; return false; }
__WEAK  bool      USBD_CDC3_NCM_GetMaxDatagramSize (uint16_t *max_datagram_size)  { (void)max_datagram_size; return false; }
__WEAK  bool      USBD_CDC3_NCM_SetMaxDatagramSize (uint16_t  max_datagram_size)  { (void)max_datagram_size; return false; }
__WEAK  bool      USBD_CDC3_NCM_GetCrcMode         (uint16_t *mode)               { (void)mode; return false; }
__WEAK  bool      USBD_CDC3_NCM_SetCrcMode         (uint16_t  mode)               { (void)mode; return false; }
__WEAK  void      USBD_CDC3_NCM_NTB_IN_Sent        (void)                         { }
__WEAK  void      USBD_CDC3_NCM_NTB_OUT_Received   (void)                         { }
#endif
#ifdef  RTE_USB_Device_CDC_4
__WEAK  void      USBD_CDC4_ACM_Initialize         (void)                         { }
__WEAK  void      USBD_CDC4_ACM_Uninitialize       (void)                         { }
__WEAK  void      USBD_CDC4_ACM_Reset              (void)                         { }
__WEAK  bool      USBD_CDC4_ACM_SendEncapsulatedCommand (            const uint8_t  *buf, uint16_t  len)  {                (void)buf; (void)len; return false; }
__WEAK  bool      USBD_CDC4_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len)  { (void)max_len; (void)buf; (void)len; return false; }
__WEAK  bool      USBD_CDC4_ACM_SetLineCoding      (const CDC_LINE_CODING *line_coding) { (void)line_coding; return false; }
__WEAK  bool      USBD_CDC4_ACM_GetLineCoding      (      CDC_LINE_CODING *line_coding) { (void)line_coding; return false; }
__WEAK  bool      USBD_CDC4_ACM_SetControlLineState(uint16_t state)               { (void)state; return false; }
__WEAK  void      USBD_CDC4_ACM_DataReceived       (uint32_t len)                 { (void)len; }
__WEAK  void      USBD_CDC4_ACM_DataSent           (void)                         { }
__WEAK  void      USBD_CDC4_NCM_Initialize         (void)                         { }
__WEAK  void      USBD_CDC4_NCM_Uninitialize       (void)                         { }
__WEAK  void      USBD_CDC4_NCM_Reset              (void)                         { }
__WEAK  void      USBD_CDC4_NCM_Start              (void)                         { }
__WEAK  void      USBD_CDC4_NCM_Stop               (void)                         { }
__WEAK  bool      USBD_CDC4_NCM_SetEthernetMulticastFilters             (const uint8_t *addr_list, uint16_t num_of_filters)                                   { (void)addr_list; (void)num_of_filters; return false; }
__WEAK  bool      USBD_CDC4_NCM_SetEthernetPowerManagementPatternFilter (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size) { (void)filter_number; (void)pattern_filter; (void)pattern_filter_size; return false; }
__WEAK  bool      USBD_CDC4_NCM_GetEthernetPowerManagementPatternFilter (uint16_t filter_number, uint16_t *pattern_active)                                    { (void)filter_number; (void)pattern_active; return false; }
__WEAK  bool      USBD_CDC4_NCM_SetEthernetPacketFilter                 (uint16_t packet_filter_bitmap)                                                       { (void)packet_filter_bitmap; return false; }
__WEAK  bool      USBD_CDC4_NCM_GetEthernetStatistic                    (uint16_t feature_selector, uint32_t *data)                                           { (void)feature_selector, (void)data; return false; }
__WEAK  bool      USBD_CDC4_NCM_GetNtbParameters   (CDC_NCM_NTB_PARAM *ntb_params){ (void)ntb_params; return false; }
__WEAK  bool      USBD_CDC4_NCM_GetNetAddress      (uint8_t *addr)                { (void)addr; return false; }
__WEAK  bool      USBD_CDC4_NCM_SetNetAddress      (const uint8_t *addr)          { (void)addr; return false; }
__WEAK  bool      USBD_CDC4_NCM_GetNtbFormat       (uint16_t *ntb_format)         { (void)ntb_format; return false; }
__WEAK  bool      USBD_CDC4_NCM_SetNtbFormat       (uint16_t  ntb_format)         { (void)ntb_format; return false; }
__WEAK  bool      USBD_CDC4_NCM_GetNtbInputSize    (uint32_t *ntb_input_size)     { (void)ntb_input_size; return false; }
__WEAK  bool      USBD_CDC4_NCM_SetNtbInputSize    (uint32_t  ntb_input_size)     { (void)ntb_input_size; return false; }
__WEAK  bool      USBD_CDC4_NCM_GetMaxDatagramSize (uint16_t *max_datagram_size)  { (void)max_datagram_size; return false; }
__WEAK  bool      USBD_CDC4_NCM_SetMaxDatagramSize (uint16_t  max_datagram_size)  { (void)max_datagram_size; return false; }
__WEAK  bool      USBD_CDC4_NCM_GetCrcMode         (uint16_t *mode)               { (void)mode; return false; }
__WEAK  bool      USBD_CDC4_NCM_SetCrcMode         (uint16_t  mode)               { (void)mode; return false; }
__WEAK  void      USBD_CDC4_NCM_NTB_IN_Sent        (void)                         { }
__WEAK  void      USBD_CDC4_NCM_NTB_OUT_Received   (void)                         { }
#endif
#ifdef  RTE_USB_Device_CDC_5
__WEAK  void      USBD_CDC5_ACM_Initialize         (void)                         { }
__WEAK  void      USBD_CDC5_ACM_Uninitialize       (void)                         { }
__WEAK  void      USBD_CDC5_ACM_Reset              (void)                         { }
__WEAK  bool      USBD_CDC5_ACM_SendEncapsulatedCommand (            const uint8_t  *buf, uint16_t  len)  {                (void)buf; (void)len; return false; }
__WEAK  bool      USBD_CDC5_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len)  { (void)max_len; (void)buf; (void)len; return false; }
__WEAK  bool      USBD_CDC5_ACM_SetLineCoding      (const CDC_LINE_CODING *line_coding) { (void)line_coding; return false; }
__WEAK  bool      USBD_CDC5_ACM_GetLineCoding      (      CDC_LINE_CODING *line_coding) { (void)line_coding; return false; }
__WEAK  bool      USBD_CDC5_ACM_SetControlLineState(uint16_t state)               { (void)state; return false; }
__WEAK  void      USBD_CDC5_ACM_DataReceived       (uint32_t len)                 { (void)len; }
__WEAK  void      USBD_CDC5_ACM_DataSent           (void)                         { }
__WEAK  void      USBD_CDC5_NCM_Initialize         (void)                         { }
__WEAK  void      USBD_CDC5_NCM_Uninitialize       (void)                         { }
__WEAK  void      USBD_CDC5_NCM_Reset              (void)                         { }
__WEAK  void      USBD_CDC5_NCM_Start              (void)                         { }
__WEAK  void      USBD_CDC5_NCM_Stop               (void)                         { }
__WEAK  bool      USBD_CDC5_NCM_SetEthernetMulticastFilters             (const uint8_t *addr_list, uint16_t num_of_filters)                                   { (void)addr_list; (void)num_of_filters; return false; }
__WEAK  bool      USBD_CDC5_NCM_SetEthernetPowerManagementPatternFilter (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size) { (void)filter_number; (void)pattern_filter; (void)pattern_filter_size; return false; }
__WEAK  bool      USBD_CDC5_NCM_GetEthernetPowerManagementPatternFilter (uint16_t filter_number, uint16_t *pattern_active)                                    { (void)filter_number; (void)pattern_active; return false; }
__WEAK  bool      USBD_CDC5_NCM_SetEthernetPacketFilter                 (uint16_t packet_filter_bitmap)                                                       { (void)packet_filter_bitmap; return false; }
__WEAK  bool      USBD_CDC5_NCM_GetEthernetStatistic                    (uint16_t feature_selector, uint32_t *data)                                           { (void)feature_selector, (void)data; return false; }
__WEAK  bool      USBD_CDC5_NCM_GetNtbParameters   (CDC_NCM_NTB_PARAM *ntb_params){ (void)ntb_params; return false; }
__WEAK  bool      USBD_CDC5_NCM_GetNetAddress      (uint8_t *addr)                { (void)addr; return false; }
__WEAK  bool      USBD_CDC5_NCM_SetNetAddress      (const uint8_t *addr)          { (void)addr; return false; }
__WEAK  bool      USBD_CDC5_NCM_GetNtbFormat       (uint16_t *ntb_format)         { (void)ntb_format; return false; }
__WEAK  bool      USBD_CDC5_NCM_SetNtbFormat       (uint16_t  ntb_format)         { (void)ntb_format; return false; }
__WEAK  bool      USBD_CDC5_NCM_GetNtbInputSize    (uint32_t *ntb_input_size)     { (void)ntb_input_size; return false; }
__WEAK  bool      USBD_CDC5_NCM_SetNtbInputSize    (uint32_t  ntb_input_size)     { (void)ntb_input_size; return false; }
__WEAK  bool      USBD_CDC5_NCM_GetMaxDatagramSize (uint16_t *max_datagram_size)  { (void)max_datagram_size; return false; }
__WEAK  bool      USBD_CDC5_NCM_SetMaxDatagramSize (uint16_t  max_datagram_size)  { (void)max_datagram_size; return false; }
__WEAK  bool      USBD_CDC5_NCM_GetCrcMode         (uint16_t *mode)               { (void)mode; return false; }
__WEAK  bool      USBD_CDC5_NCM_SetCrcMode         (uint16_t  mode)               { (void)mode; return false; }
__WEAK  void      USBD_CDC5_NCM_NTB_IN_Sent        (void)                         { }
__WEAK  void      USBD_CDC5_NCM_NTB_OUT_Received   (void)                         { }
#endif
#ifdef  RTE_USB_Device_CDC_6
__WEAK  void      USBD_CDC6_ACM_Initialize         (void)                         { }
__WEAK  void      USBD_CDC6_ACM_Uninitialize       (void)                         { }
__WEAK  void      USBD_CDC6_ACM_Reset              (void)                         { }
__WEAK  bool      USBD_CDC6_ACM_SendEncapsulatedCommand (            const uint8_t  *buf, uint16_t  len)  {                (void)buf; (void)len; return false; }
__WEAK  bool      USBD_CDC6_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len)  { (void)max_len; (void)buf; (void)len; return false; }
__WEAK  bool      USBD_CDC6_ACM_SetLineCoding      (const CDC_LINE_CODING *line_coding) { (void)line_coding; return false; }
__WEAK  bool      USBD_CDC6_ACM_GetLineCoding      (      CDC_LINE_CODING *line_coding) { (void)line_coding; return false; }
__WEAK  bool      USBD_CDC6_ACM_SetControlLineState(uint16_t state)               { (void)state; return false; }
__WEAK  void      USBD_CDC6_ACM_DataReceived       (uint32_t len)                 { (void)len; }
__WEAK  void      USBD_CDC6_ACM_DataSent           (void)                         { }
__WEAK  void      USBD_CDC6_NCM_Initialize         (void)                         { }
__WEAK  void      USBD_CDC6_NCM_Uninitialize       (void)                         { }
__WEAK  void      USBD_CDC6_NCM_Reset              (void)                         { }
__WEAK  void      USBD_CDC6_NCM_Start              (void)                         { }
__WEAK  void      USBD_CDC6_NCM_Stop               (void)                         { }
__WEAK  bool      USBD_CDC6_NCM_SetEthernetMulticastFilters             (const uint8_t *addr_list, uint16_t num_of_filters)                                   { (void)addr_list; (void)num_of_filters; return false; }
__WEAK  bool      USBD_CDC6_NCM_SetEthernetPowerManagementPatternFilter (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size) { (void)filter_number; (void)pattern_filter; (void)pattern_filter_size; return false; }
__WEAK  bool      USBD_CDC6_NCM_GetEthernetPowerManagementPatternFilter (uint16_t filter_number, uint16_t *pattern_active)                                    { (void)filter_number; (void)pattern_active; return false; }
__WEAK  bool      USBD_CDC6_NCM_SetEthernetPacketFilter                 (uint16_t packet_filter_bitmap)                                                       { (void)packet_filter_bitmap; return false; }
__WEAK  bool      USBD_CDC6_NCM_GetEthernetStatistic                    (uint16_t feature_selector, uint32_t *data)                                           { (void)feature_selector, (void)data; return false; }
__WEAK  bool      USBD_CDC6_NCM_GetNtbParameters   (CDC_NCM_NTB_PARAM *ntb_params){ (void)ntb_params; return false; }
__WEAK  bool      USBD_CDC6_NCM_GetNetAddress      (uint8_t *addr)                { (void)addr; return false; }
__WEAK  bool      USBD_CDC6_NCM_SetNetAddress      (const uint8_t *addr)          { (void)addr; return false; }
__WEAK  bool      USBD_CDC6_NCM_GetNtbFormat       (uint16_t *ntb_format)         { (void)ntb_format; return false; }
__WEAK  bool      USBD_CDC6_NCM_SetNtbFormat       (uint16_t  ntb_format)         { (void)ntb_format; return false; }
__WEAK  bool      USBD_CDC6_NCM_GetNtbInputSize    (uint32_t *ntb_input_size)     { (void)ntb_input_size; return false; }
__WEAK  bool      USBD_CDC6_NCM_SetNtbInputSize    (uint32_t  ntb_input_size)     { (void)ntb_input_size; return false; }
__WEAK  bool      USBD_CDC6_NCM_GetMaxDatagramSize (uint16_t *max_datagram_size)  { (void)max_datagram_size; return false; }
__WEAK  bool      USBD_CDC6_NCM_SetMaxDatagramSize (uint16_t  max_datagram_size)  { (void)max_datagram_size; return false; }
__WEAK  bool      USBD_CDC6_NCM_GetCrcMode         (uint16_t *mode)               { (void)mode; return false; }
__WEAK  bool      USBD_CDC6_NCM_SetCrcMode         (uint16_t  mode)               { (void)mode; return false; }
__WEAK  void      USBD_CDC6_NCM_NTB_IN_Sent        (void)                         { }
__WEAK  void      USBD_CDC6_NCM_NTB_OUT_Received   (void)                         { }
#endif
#ifdef  RTE_USB_Device_CDC_7
__WEAK  void      USBD_CDC7_ACM_Initialize         (void)                         { }
__WEAK  void      USBD_CDC7_ACM_Uninitialize       (void)                         { }
__WEAK  void      USBD_CDC7_ACM_Reset              (void)                         { }
__WEAK  bool      USBD_CDC7_ACM_SendEncapsulatedCommand (            const uint8_t  *buf, uint16_t  len)  {                (void)buf; (void)len; return false; }
__WEAK  bool      USBD_CDC7_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len)  { (void)max_len; (void)buf; (void)len; return false; }
__WEAK  bool      USBD_CDC7_ACM_SetLineCoding      (const CDC_LINE_CODING *line_coding) { (void)line_coding; return false; }
__WEAK  bool      USBD_CDC7_ACM_GetLineCoding      (      CDC_LINE_CODING *line_coding) { (void)line_coding; return false; }
__WEAK  bool      USBD_CDC7_ACM_SetControlLineState(uint16_t state)               { (void)state; return false; }
__WEAK  void      USBD_CDC7_ACM_DataReceived       (uint32_t len)                 { (void)len; }
__WEAK  void      USBD_CDC7_ACM_DataSent           (void)                         { }
__WEAK  void      USBD_CDC7_NCM_Initialize         (void)                         { }
__WEAK  void      USBD_CDC7_NCM_Uninitialize       (void)                         { }
__WEAK  void      USBD_CDC7_NCM_Reset              (void)                         { }
__WEAK  void      USBD_CDC7_NCM_Start              (void)                         { }
__WEAK  void      USBD_CDC7_NCM_Stop               (void)                         { }
__WEAK  bool      USBD_CDC7_NCM_SetEthernetMulticastFilters             (const uint8_t *addr_list, uint16_t num_of_filters)                                   { (void)addr_list; (void)num_of_filters; return false; }
__WEAK  bool      USBD_CDC7_NCM_SetEthernetPowerManagementPatternFilter (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size) { (void)filter_number; (void)pattern_filter; (void)pattern_filter_size; return false; }
__WEAK  bool      USBD_CDC7_NCM_GetEthernetPowerManagementPatternFilter (uint16_t filter_number, uint16_t *pattern_active)                                    { (void)filter_number; (void)pattern_active; return false; }
__WEAK  bool      USBD_CDC7_NCM_SetEthernetPacketFilter                 (uint16_t packet_filter_bitmap)                                                       { (void)packet_filter_bitmap; return false; }
__WEAK  bool      USBD_CDC7_NCM_GetEthernetStatistic                    (uint16_t feature_selector, uint32_t *data)                                           { (void)feature_selector, (void)data; return false; }
__WEAK  bool      USBD_CDC7_NCM_GetNtbParameters   (CDC_NCM_NTB_PARAM *ntb_params){ (void)ntb_params; return false; }
__WEAK  bool      USBD_CDC7_NCM_GetNetAddress      (uint8_t *addr)                { (void)addr; return false; }
__WEAK  bool      USBD_CDC7_NCM_SetNetAddress      (const uint8_t *addr)          { (void)addr; return false; }
__WEAK  bool      USBD_CDC7_NCM_GetNtbFormat       (uint16_t *ntb_format)         { (void)ntb_format; return false; }
__WEAK  bool      USBD_CDC7_NCM_SetNtbFormat       (uint16_t  ntb_format)         { (void)ntb_format; return false; }
__WEAK  bool      USBD_CDC7_NCM_GetNtbInputSize    (uint32_t *ntb_input_size)     { (void)ntb_input_size; return false; }
__WEAK  bool      USBD_CDC7_NCM_SetNtbInputSize    (uint32_t  ntb_input_size)     { (void)ntb_input_size; return false; }
__WEAK  bool      USBD_CDC7_NCM_GetMaxDatagramSize (uint16_t *max_datagram_size)  { (void)max_datagram_size; return false; }
__WEAK  bool      USBD_CDC7_NCM_SetMaxDatagramSize (uint16_t  max_datagram_size)  { (void)max_datagram_size; return false; }
__WEAK  bool      USBD_CDC7_NCM_GetCrcMode         (uint16_t *mode)               { (void)mode; return false; }
__WEAK  bool      USBD_CDC7_NCM_SetCrcMode         (uint16_t  mode)               { (void)mode; return false; }
__WEAK  void      USBD_CDC7_NCM_NTB_IN_Sent        (void)                         { }
__WEAK  void      USBD_CDC7_NCM_NTB_OUT_Received   (void)                         { }
#endif

extern 
void (* const fpUSBD_CDC_ACM_Initialize[USBD_CDC_NUM]) (void);
void (* const fpUSBD_CDC_ACM_Initialize[USBD_CDC_NUM]) (void) = {
        USBD_CDC0_ACM_Initialize
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_ACM_Initialize
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_ACM_Initialize
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_ACM_Initialize
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_ACM_Initialize
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_ACM_Initialize
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_ACM_Initialize
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_ACM_Initialize
#endif
};
extern 
void (* const fpUSBD_CDC_ACM_Uninitialize[USBD_CDC_NUM]) (void);
void (* const fpUSBD_CDC_ACM_Uninitialize[USBD_CDC_NUM]) (void) = {
        USBD_CDC0_ACM_Uninitialize
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_ACM_Uninitialize
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_ACM_Uninitialize
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_ACM_Uninitialize
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_ACM_Uninitialize
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_ACM_Uninitialize
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_ACM_Uninitialize
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_ACM_Uninitialize
#endif
};
extern 
void (* const fpUSBD_CDC_ACM_Reset[USBD_CDC_NUM]) (void);
void (* const fpUSBD_CDC_ACM_Reset[USBD_CDC_NUM]) (void) = {
        USBD_CDC0_ACM_Reset
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_ACM_Reset
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_ACM_Reset
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_ACM_Reset
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_ACM_Reset
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_ACM_Reset
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_ACM_Reset
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_ACM_Reset
#endif
};
extern 
bool (* const fpUSBD_CDC_ACM_SendEncapsulatedCommand[USBD_CDC_NUM]) (const uint8_t *buf, uint16_t len);
bool (* const fpUSBD_CDC_ACM_SendEncapsulatedCommand[USBD_CDC_NUM]) (const uint8_t *buf, uint16_t len) = {
        USBD_CDC0_ACM_SendEncapsulatedCommand
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_ACM_SendEncapsulatedCommand
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_ACM_SendEncapsulatedCommand
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_ACM_SendEncapsulatedCommand
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_ACM_SendEncapsulatedCommand
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_ACM_SendEncapsulatedCommand
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_ACM_SendEncapsulatedCommand
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_ACM_SendEncapsulatedCommand
#endif
};
extern 
bool (* const fpUSBD_CDC_ACM_GetEncapsulatedResponse[USBD_CDC_NUM]) (uint16_t max_len, uint8_t **buf, uint16_t *len);
bool (* const fpUSBD_CDC_ACM_GetEncapsulatedResponse[USBD_CDC_NUM]) (uint16_t max_len, uint8_t **buf, uint16_t *len) = {
        USBD_CDC0_ACM_GetEncapsulatedResponse
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_ACM_GetEncapsulatedResponse
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_ACM_GetEncapsulatedResponse
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_ACM_GetEncapsulatedResponse
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_ACM_GetEncapsulatedResponse
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_ACM_GetEncapsulatedResponse
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_ACM_GetEncapsulatedResponse
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_ACM_GetEncapsulatedResponse
#endif
};
extern 
bool (* const fpUSBD_CDC_ACM_SetLineCoding[USBD_CDC_NUM]) (const CDC_LINE_CODING *line_coding);
bool (* const fpUSBD_CDC_ACM_SetLineCoding[USBD_CDC_NUM]) (const CDC_LINE_CODING *line_coding) = {
        USBD_CDC0_ACM_SetLineCoding
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_ACM_SetLineCoding
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_ACM_SetLineCoding
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_ACM_SetLineCoding
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_ACM_SetLineCoding
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_ACM_SetLineCoding
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_ACM_SetLineCoding
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_ACM_SetLineCoding
#endif
};
extern 
bool (* const fpUSBD_CDC_ACM_GetLineCoding[USBD_CDC_NUM]) (CDC_LINE_CODING *line_coding);
bool (* const fpUSBD_CDC_ACM_GetLineCoding[USBD_CDC_NUM]) (CDC_LINE_CODING *line_coding) = {
        USBD_CDC0_ACM_GetLineCoding
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_ACM_GetLineCoding
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_ACM_GetLineCoding
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_ACM_GetLineCoding
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_ACM_GetLineCoding
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_ACM_GetLineCoding
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_ACM_GetLineCoding
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_ACM_GetLineCoding
#endif
};
extern 
bool (* const fpUSBD_CDC_ACM_SetControlLineState[USBD_CDC_NUM]) (uint16_t ctrl_bmp);
bool (* const fpUSBD_CDC_ACM_SetControlLineState[USBD_CDC_NUM]) (uint16_t ctrl_bmp) = {
        USBD_CDC0_ACM_SetControlLineState
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_ACM_SetControlLineState
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_ACM_SetControlLineState
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_ACM_SetControlLineState
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_ACM_SetControlLineState
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_ACM_SetControlLineState
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_ACM_SetControlLineState
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_ACM_SetControlLineState
#endif
};
extern 
void (* const fpUSBD_CDC_ACM_DataReceived[USBD_CDC_NUM]) (uint32_t len);
void (* const fpUSBD_CDC_ACM_DataReceived[USBD_CDC_NUM]) (uint32_t len) = {
        USBD_CDC0_ACM_DataReceived
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_ACM_DataReceived
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_ACM_DataReceived
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_ACM_DataReceived
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_ACM_DataReceived
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_ACM_DataReceived
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_ACM_DataReceived
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_ACM_DataReceived
#endif
};
extern 
void (* const fpUSBD_CDC_ACM_DataSent[USBD_CDC_NUM]) (void);
void (* const fpUSBD_CDC_ACM_DataSent[USBD_CDC_NUM]) (void) = {
        USBD_CDC0_ACM_DataSent
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_ACM_DataSent
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_ACM_DataSent
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_ACM_DataSent
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_ACM_DataSent
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_ACM_DataSent
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_ACM_DataSent
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_ACM_DataSent
#endif
};
extern 
void (* const fpUSBD_CDC_NCM_Initialize[USBD_CDC_NUM]) (void);
void (* const fpUSBD_CDC_NCM_Initialize[USBD_CDC_NUM]) (void) = {
        USBD_CDC0_NCM_Initialize
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_Initialize
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_Initialize
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_Initialize
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_Initialize
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_Initialize
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_Initialize
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_Initialize
#endif
};
extern 
void (* const fpUSBD_CDC_NCM_Uninitialize[USBD_CDC_NUM]) (void);
void (* const fpUSBD_CDC_NCM_Uninitialize[USBD_CDC_NUM]) (void) = {
        USBD_CDC0_NCM_Uninitialize
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_Uninitialize
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_Uninitialize
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_Uninitialize
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_Uninitialize
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_Uninitialize
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_Uninitialize
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_Uninitialize
#endif
};
extern 
void (* const fpUSBD_CDC_NCM_Reset[USBD_CDC_NUM]) (void);
void (* const fpUSBD_CDC_NCM_Reset[USBD_CDC_NUM]) (void) = {
        USBD_CDC0_NCM_Reset
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_Reset
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_Reset
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_Reset
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_Reset
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_Reset
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_Reset
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_Reset
#endif
};
extern 
void (* const fpUSBD_CDC_NCM_Start[USBD_CDC_NUM]) (void);
void (* const fpUSBD_CDC_NCM_Start[USBD_CDC_NUM]) (void) = {
        USBD_CDC0_NCM_Start
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_Start
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_Start
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_Start
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_Start
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_Start
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_Start
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_Start
#endif
};
extern 
void (* const fpUSBD_CDC_NCM_Stop[USBD_CDC_NUM]) (void);
void (* const fpUSBD_CDC_NCM_Stop[USBD_CDC_NUM]) (void) = {
        USBD_CDC0_NCM_Stop
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_Stop
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_Stop
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_Stop
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_Stop
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_Stop
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_Stop
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_Stop
#endif
};
extern 
bool (* const fpUSBD_CDC_NCM_SetEthernetMulticastFilters[USBD_CDC_NUM]) (const uint8_t *addr_list, uint16_t num_of_filters);
bool (* const fpUSBD_CDC_NCM_SetEthernetMulticastFilters[USBD_CDC_NUM]) (const uint8_t *addr_list, uint16_t num_of_filters) = {
        USBD_CDC0_NCM_SetEthernetMulticastFilters
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_SetEthernetMulticastFilters
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_SetEthernetMulticastFilters
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_SetEthernetMulticastFilters
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_SetEthernetMulticastFilters
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_SetEthernetMulticastFilters
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_SetEthernetMulticastFilters
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_SetEthernetMulticastFilters
#endif
};
extern 
bool (* const fpUSBD_CDC_NCM_SetEthernetPowerManagementPatternFilter[USBD_CDC_NUM]) (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size);
bool (* const fpUSBD_CDC_NCM_SetEthernetPowerManagementPatternFilter[USBD_CDC_NUM]) (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size) = {
        USBD_CDC0_NCM_SetEthernetPowerManagementPatternFilter
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_SetEthernetPowerManagementPatternFilter
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_SetEthernetPowerManagementPatternFilter
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_SetEthernetPowerManagementPatternFilter
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_SetEthernetPowerManagementPatternFilter
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_SetEthernetPowerManagementPatternFilter
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_SetEthernetPowerManagementPatternFilter
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_SetEthernetPowerManagementPatternFilter
#endif
};
extern 
bool (* const fpUSBD_CDC_NCM_GetEthernetPowerManagementPatternFilter[USBD_CDC_NUM]) (uint16_t filter_number, uint16_t *pattern_active);
bool (* const fpUSBD_CDC_NCM_GetEthernetPowerManagementPatternFilter[USBD_CDC_NUM]) (uint16_t filter_number, uint16_t *pattern_active) = {
        USBD_CDC0_NCM_GetEthernetPowerManagementPatternFilter
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_GetEthernetPowerManagementPatternFilter
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_GetEthernetPowerManagementPatternFilter
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_GetEthernetPowerManagementPatternFilter
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_GetEthernetPowerManagementPatternFilter
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_GetEthernetPowerManagementPatternFilter
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_GetEthernetPowerManagementPatternFilter
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_GetEthernetPowerManagementPatternFilter
#endif
};
extern 
bool (* const fpUSBD_CDC_NCM_SetEthernetPacketFilter[USBD_CDC_NUM]) (uint16_t packet_filter_bitmap);
bool (* const fpUSBD_CDC_NCM_SetEthernetPacketFilter[USBD_CDC_NUM]) (uint16_t packet_filter_bitmap) = {
        USBD_CDC0_NCM_SetEthernetPacketFilter
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_SetEthernetPacketFilter
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_SetEthernetPacketFilter
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_SetEthernetPacketFilter
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_SetEthernetPacketFilter
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_SetEthernetPacketFilter
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_SetEthernetPacketFilter
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_SetEthernetPacketFilter
#endif
};
extern 
bool (* const fpUSBD_CDC_NCM_GetEthernetStatistic[USBD_CDC_NUM]) (uint16_t feature_selector, uint32_t *data);
bool (* const fpUSBD_CDC_NCM_GetEthernetStatistic[USBD_CDC_NUM]) (uint16_t feature_selector, uint32_t *data) = {
        USBD_CDC0_NCM_GetEthernetStatistic
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_GetEthernetStatistic
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_GetEthernetStatistic
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_GetEthernetStatistic
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_GetEthernetStatistic
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_GetEthernetStatistic
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_GetEthernetStatistic
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_GetEthernetStatistic
#endif
};
extern 
bool (* const fpUSBD_CDC_NCM_GetNtbParameters[USBD_CDC_NUM]) (CDC_NCM_NTB_PARAM *ntb_params);
bool (* const fpUSBD_CDC_NCM_GetNtbParameters[USBD_CDC_NUM]) (CDC_NCM_NTB_PARAM *ntb_params) = {
        USBD_CDC0_NCM_GetNtbParameters
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_GetNtbParameters
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_GetNtbParameters
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_GetNtbParameters
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_GetNtbParameters
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_GetNtbParameters
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_GetNtbParameters
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_GetNtbParameters
#endif
};
extern 
bool (* const fpUSBD_CDC_NCM_GetNetAddress[USBD_CDC_NUM]) (uint8_t *addr);
bool (* const fpUSBD_CDC_NCM_GetNetAddress[USBD_CDC_NUM]) (uint8_t *addr) = {
        USBD_CDC0_NCM_GetNetAddress
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_GetNetAddress
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_GetNetAddress
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_GetNetAddress
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_GetNetAddress
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_GetNetAddress
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_GetNetAddress
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_GetNetAddress
#endif
};
extern 
bool (* const fpUSBD_CDC_NCM_SetNetAddress[USBD_CDC_NUM]) (const uint8_t *addr);
bool (* const fpUSBD_CDC_NCM_SetNetAddress[USBD_CDC_NUM]) (const uint8_t *addr) = {
        USBD_CDC0_NCM_SetNetAddress
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_SetNetAddress
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_SetNetAddress
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_SetNetAddress
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_SetNetAddress
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_SetNetAddress
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_SetNetAddress
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_SetNetAddress
#endif
};
extern 
bool (* const fpUSBD_CDC_NCM_GetNtbFormat[USBD_CDC_NUM]) (uint16_t *ntb_format);
bool (* const fpUSBD_CDC_NCM_GetNtbFormat[USBD_CDC_NUM]) (uint16_t *ntb_format) = {
        USBD_CDC0_NCM_GetNtbFormat
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_GetNtbFormat
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_GetNtbFormat
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_GetNtbFormat
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_GetNtbFormat
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_GetNtbFormat
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_GetNtbFormat
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_GetNtbFormat
#endif
};
extern 
bool (* const fpUSBD_CDC_NCM_SetNtbFormat[USBD_CDC_NUM]) (uint16_t ntb_format);
bool (* const fpUSBD_CDC_NCM_SetNtbFormat[USBD_CDC_NUM]) (uint16_t ntb_format) = {
        USBD_CDC0_NCM_SetNtbFormat
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_SetNtbFormat
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_SetNtbFormat
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_SetNtbFormat
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_SetNtbFormat
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_SetNtbFormat
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_SetNtbFormat
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_SetNtbFormat
#endif
};
extern 
bool (* const fpUSBD_CDC_NCM_GetNtbInputSize[USBD_CDC_NUM]) (uint32_t *ntb_input_size);
bool (* const fpUSBD_CDC_NCM_GetNtbInputSize[USBD_CDC_NUM]) (uint32_t *ntb_input_size) = {
        USBD_CDC0_NCM_GetNtbInputSize
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_GetNtbInputSize
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_GetNtbInputSize
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_GetNtbInputSize
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_GetNtbInputSize
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_GetNtbInputSize
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_GetNtbInputSize
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_GetNtbInputSize
#endif
};
extern 
bool (* const fpUSBD_CDC_NCM_SetNtbInputSize[USBD_CDC_NUM]) (uint32_t ntb_input_size);
bool (* const fpUSBD_CDC_NCM_SetNtbInputSize[USBD_CDC_NUM]) (uint32_t ntb_input_size) = {
        USBD_CDC0_NCM_SetNtbInputSize
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_SetNtbInputSize
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_SetNtbInputSize
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_SetNtbInputSize
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_SetNtbInputSize
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_SetNtbInputSize
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_SetNtbInputSize
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_SetNtbInputSize
#endif
};
extern 
bool (* const fpUSBD_CDC_NCM_GetMaxDatagramSize[USBD_CDC_NUM]) (uint16_t *max_datagram_size);
bool (* const fpUSBD_CDC_NCM_GetMaxDatagramSize[USBD_CDC_NUM]) (uint16_t *max_datagram_size) = {
        USBD_CDC0_NCM_GetMaxDatagramSize
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_GetMaxDatagramSize
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_GetMaxDatagramSize
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_GetMaxDatagramSize
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_GetMaxDatagramSize
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_GetMaxDatagramSize
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_GetMaxDatagramSize
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_GetMaxDatagramSize
#endif
};
extern 
bool (* const fpUSBD_CDC_NCM_SetMaxDatagramSize[USBD_CDC_NUM]) (uint16_t max_datagram_size);
bool (* const fpUSBD_CDC_NCM_SetMaxDatagramSize[USBD_CDC_NUM]) (uint16_t max_datagram_size) = {
        USBD_CDC0_NCM_SetMaxDatagramSize
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_SetMaxDatagramSize
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_SetMaxDatagramSize
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_SetMaxDatagramSize
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_SetMaxDatagramSize
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_SetMaxDatagramSize
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_SetMaxDatagramSize
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_SetMaxDatagramSize
#endif
};
extern 
bool (* const fpUSBD_CDC_NCM_GetCrcMode[USBD_CDC_NUM]) (uint16_t *mode);
bool (* const fpUSBD_CDC_NCM_GetCrcMode[USBD_CDC_NUM]) (uint16_t *mode) = {
        USBD_CDC0_NCM_GetCrcMode
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_GetCrcMode
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_GetCrcMode
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_GetCrcMode
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_GetCrcMode
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_GetCrcMode
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_GetCrcMode
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_GetCrcMode
#endif
};
extern 
bool (* const fpUSBD_CDC_NCM_SetCrcMode[USBD_CDC_NUM]) (uint16_t mode);
bool (* const fpUSBD_CDC_NCM_SetCrcMode[USBD_CDC_NUM]) (uint16_t mode) = {
        USBD_CDC0_NCM_SetCrcMode
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_SetCrcMode
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_SetCrcMode
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_SetCrcMode
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_SetCrcMode
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_SetCrcMode
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_SetCrcMode
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_SetCrcMode
#endif
};
extern 
void (* const fpUSBD_CDC_NCM_NTB_IN_Sent[USBD_CDC_NUM]) (void);
void (* const fpUSBD_CDC_NCM_NTB_IN_Sent[USBD_CDC_NUM]) (void) = {
        USBD_CDC0_NCM_NTB_IN_Sent
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_NTB_IN_Sent
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_NTB_IN_Sent
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_NTB_IN_Sent
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_NTB_IN_Sent
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_NTB_IN_Sent
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_NTB_IN_Sent
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_NTB_IN_Sent
#endif
};
extern 
void (* const fpUSBD_CDC_NCM_NTB_OUT_Received[USBD_CDC_NUM]) (void);
void (* const fpUSBD_CDC_NCM_NTB_OUT_Received[USBD_CDC_NUM]) (void) = {
        USBD_CDC0_NCM_NTB_OUT_Received
#ifdef  RTE_USB_Device_CDC_1
      , USBD_CDC1_NCM_NTB_OUT_Received
#endif
#ifdef  RTE_USB_Device_CDC_2
      , USBD_CDC2_NCM_NTB_OUT_Received
#endif
#ifdef  RTE_USB_Device_CDC_3
      , USBD_CDC3_NCM_NTB_OUT_Received
#endif
#ifdef  RTE_USB_Device_CDC_4
      , USBD_CDC4_NCM_NTB_OUT_Received
#endif
#ifdef  RTE_USB_Device_CDC_5
      , USBD_CDC5_NCM_NTB_OUT_Received
#endif
#ifdef  RTE_USB_Device_CDC_6
      , USBD_CDC6_NCM_NTB_OUT_Received
#endif
#ifdef  RTE_USB_Device_CDC_7
      , USBD_CDC7_NCM_NTB_OUT_Received
#endif
};
#endif

#if    (USBD_HID_NUM == 0)
        bool      USBD_Endpoint0_ReqGetDescriptor_HID (uint8_t device, uint8_t **pD, uint32_t *len);
        bool      USBD_Endpoint0_ReqGetDescriptor_HID (uint8_t device, uint8_t **pD, uint32_t *len)                                { (void)device; (void)pD; (void)len; return false; }
        bool      USBD_Endpoint0_Setup_HID_ReqToIF    (uint8_t device);
        bool      USBD_Endpoint0_Setup_HID_ReqToIF    (uint8_t device)                                                             { (void)device; return false; }
        bool      USBD_Endpoint0_Out_HID_ReqToIF      (uint8_t device);
        bool      USBD_Endpoint0_Out_HID_ReqToIF      (uint8_t device)                                                             { (void)device; return false; }
#else
__WEAK  void      USBD_HID0_Initialize                (void)                                                                       { }
__WEAK  void      USBD_HID0_Uninitialize              (void)                                                                       { }
__WEAK  int32_t   USBD_HID0_GetReport                 (uint8_t rtype, uint8_t  req, uint8_t rid,       uint8_t *buf)               { (void)rtype; (void)req; (void)rid; (void)buf; return -1;    }
__WEAK  bool      USBD_HID0_SetReport                 (uint8_t rtype, uint8_t  req, uint8_t rid, const uint8_t *buf, int32_t len)  { (void)rtype; (void)req; (void)rid; (void)buf; (void)len; return false; }
extern  int8_t    USBD_HID0_GetProtocol               (void);
__WEAK  int8_t    USBD_HID0_GetProtocol               (void)                                                                       { return -1;    }
extern  bool      USBD_HID0_SetProtocol               (uint8_t protocol);
__WEAK  bool      USBD_HID0_SetProtocol               (uint8_t protocol)                                                           { (void)protocol; return false; }
#ifdef  RTE_USB_Device_HID_1
__WEAK  void      USBD_HID1_Initialize                (void)                                                                       { }
__WEAK  void      USBD_HID1_Uninitialize              (void)                                                                       { }
__WEAK  int32_t   USBD_HID1_GetReport                 (uint8_t rtype, uint8_t  req, uint8_t rid,       uint8_t *buf)               { (void)rtype; (void)req; (void)rid; (void)buf; return -1;    }
__WEAK  bool      USBD_HID1_SetReport                 (uint8_t rtype, uint8_t  req, uint8_t rid, const uint8_t *buf, int32_t len)  { (void)rtype; (void)req; (void)rid; (void)buf; (void)len; return false; }
extern  int8_t    USBD_HID1_GetProtocol               (void);
__WEAK  int8_t    USBD_HID1_GetProtocol               (void)                                                                       { return -1;    }
extern  bool      USBD_HID1_SetProtocol               (uint8_t protocol);
__WEAK  bool      USBD_HID1_SetProtocol               (uint8_t protocol)                                                           { (void)protocol; return false; }
#endif
#ifdef  RTE_USB_Device_HID_2
__WEAK  void      USBD_HID2_Initialize                (void)                                                                       { }
__WEAK  void      USBD_HID2_Uninitialize              (void)                                                                       { }
__WEAK  int32_t   USBD_HID2_GetReport                 (uint8_t rtype, uint8_t  req, uint8_t rid,       uint8_t *buf)               { (void)rtype; (void)req; (void)rid; (void)buf; return -1;    }
__WEAK  bool      USBD_HID2_SetReport                 (uint8_t rtype, uint8_t  req, uint8_t rid, const uint8_t *buf, int32_t len)  { (void)rtype; (void)req; (void)rid; (void)buf; (void)len; return false; }
extern  int8_t    USBD_HID2_GetProtocol               (void);
__WEAK  int8_t    USBD_HID2_GetProtocol               (void)                                                                       { return -1;    }
extern  bool      USBD_HID2_SetProtocol               (uint8_t protocol);
__WEAK  bool      USBD_HID2_SetProtocol               (uint8_t protocol)                                                           { (void)protocol; return false; }
#endif
#ifdef  RTE_USB_Device_HID_3
__WEAK  void      USBD_HID3_Initialize                (void)                                                                       { }
__WEAK  void      USBD_HID3_Uninitialize              (void)                                                                       { }
__WEAK  int32_t   USBD_HID3_GetReport                 (uint8_t rtype, uint8_t  req, uint8_t rid,       uint8_t *buf)               { (void)rtype; (void)req; (void)rid; (void)buf; return -1;    }
__WEAK  bool      USBD_HID3_SetReport                 (uint8_t rtype, uint8_t  req, uint8_t rid, const uint8_t *buf, int32_t len)  { (void)rtype; (void)req; (void)rid; (void)buf; (void)len; return false; }
extern  int8_t    USBD_HID3_GetProtocol               (void);
__WEAK  int8_t    USBD_HID3_GetProtocol               (void)                                                                       { return -1;    }
extern  bool      USBD_HID3_SetProtocol               (uint8_t protocol);
__WEAK  bool      USBD_HID3_SetProtocol               (uint8_t protocol)                                                           { (void)protocol; return false; }
#endif
#ifdef  RTE_USB_Device_HID_4
__WEAK  void      USBD_HID4_Initialize                (void)                                                                       { }
__WEAK  void      USBD_HID4_Uninitialize              (void)                                                                       { }
__WEAK  int32_t   USBD_HID4_GetReport                 (uint8_t rtype, uint8_t  req, uint8_t rid,       uint8_t *buf)               { (void)rtype; (void)req; (void)rid; (void)buf; return -1;    }
__WEAK  bool      USBD_HID4_SetReport                 (uint8_t rtype, uint8_t  req, uint8_t rid, const uint8_t *buf, int32_t len)  { (void)rtype; (void)req; (void)rid; (void)buf; (void)len; return false; }
extern  int8_t    USBD_HID4_GetProtocol               (void);
__WEAK  int8_t    USBD_HID4_GetProtocol               (void)                                                                       { return -1;    }
extern  bool      USBD_HID4_SetProtocol               (uint8_t protocol);
__WEAK  bool      USBD_HID4_SetProtocol               (uint8_t protocol)                                                           { (void)protocol; return false; }
#endif
#ifdef  RTE_USB_Device_HID_5
__WEAK  void      USBD_HID5_Initialize                (void)                                                                       { }
__WEAK  void      USBD_HID5_Uninitialize              (void)                                                                       { }
__WEAK  int32_t   USBD_HID5_GetReport                 (uint8_t rtype, uint8_t  req, uint8_t rid,       uint8_t *buf)               { (void)rtype; (void)req; (void)rid; (void)buf; return -1;    }
__WEAK  bool      USBD_HID5_SetReport                 (uint8_t rtype, uint8_t  req, uint8_t rid, const uint8_t *buf, int32_t len)  { (void)rtype; (void)req; (void)rid; (void)buf; (void)len; return false; }
extern  int8_t    USBD_HID5_GetProtocol               (void);
__WEAK  int8_t    USBD_HID5_GetProtocol               (void)                                                                       { return -1;    }
extern  bool      USBD_HID5_SetProtocol               (uint8_t protocol);
__WEAK  bool      USBD_HID5_SetProtocol               (uint8_t protocol)                                                           { (void)protocol; return false; }
#endif
#ifdef  RTE_USB_Device_HID_6
__WEAK  void      USBD_HID6_Initialize                (void)                                                                       { }
__WEAK  void      USBD_HID6_Uninitialize              (void)                                                                       { }
__WEAK  int32_t   USBD_HID6_GetReport                 (uint8_t rtype, uint8_t  req, uint8_t rid,       uint8_t *buf)               { (void)rtype; (void)req; (void)rid; (void)buf; return -1;    }
__WEAK  bool      USBD_HID6_SetReport                 (uint8_t rtype, uint8_t  req, uint8_t rid, const uint8_t *buf, int32_t len)  { (void)rtype; (void)req; (void)rid; (void)buf; (void)len; return false; }
extern  int8_t    USBD_HID6_GetProtocol               (void);
__WEAK  int8_t    USBD_HID6_GetProtocol               (void)                                                                       { return -1;    }
extern  bool      USBD_HID6_SetProtocol               (uint8_t protocol);
__WEAK  bool      USBD_HID6_SetProtocol               (uint8_t protocol)                                                           { (void)protocol; return false; }
#endif
#ifdef  RTE_USB_Device_HID_7
__WEAK  void      USBD_HID7_Initialize                (void)                                                                       { }
__WEAK  void      USBD_HID7_Uninitialize              (void)                                                                       { }
__WEAK  int32_t   USBD_HID7_GetReport                 (uint8_t rtype, uint8_t  req, uint8_t rid,       uint8_t *buf)               { (void)rtype; (void)req; (void)rid; (void)buf; return -1;    }
__WEAK  bool      USBD_HID7_SetReport                 (uint8_t rtype, uint8_t  req, uint8_t rid, const uint8_t *buf, int32_t len)  { (void)rtype; (void)req; (void)rid; (void)buf; (void)len; return false; }
extern  int8_t    USBD_HID7_GetProtocol               (void);
__WEAK  int8_t    USBD_HID7_GetProtocol               (void)                                                                       { return -1;    }
extern  bool      USBD_HID7_SetProtocol               (uint8_t protocol);
__WEAK  bool      USBD_HID7_SetProtocol               (uint8_t protocol)                                                           { (void)protocol; return false; }
#endif

extern 
void (* const fpUSBD_HID_Initialize[USBD_HID_NUM]) (void);
void (* const fpUSBD_HID_Initialize[USBD_HID_NUM]) (void) = {
        USBD_HID0_Initialize
#ifdef  RTE_USB_Device_HID_1
      , USBD_HID1_Initialize
#endif
#ifdef  RTE_USB_Device_HID_2
      , USBD_HID2_Initialize
#endif
#ifdef  RTE_USB_Device_HID_3
      , USBD_HID3_Initialize
#endif
#ifdef  RTE_USB_Device_HID_4
      , USBD_HID4_Initialize
#endif
#ifdef  RTE_USB_Device_HID_5
      , USBD_HID5_Initialize
#endif
#ifdef  RTE_USB_Device_HID_6
      , USBD_HID6_Initialize
#endif
#ifdef  RTE_USB_Device_HID_7
      , USBD_HID7_Initialize
#endif
};
extern 
void (* const fpUSBD_HID_Uninitialize[USBD_HID_NUM]) (void);
void (* const fpUSBD_HID_Uninitialize[USBD_HID_NUM]) (void) = {
        USBD_HID0_Uninitialize
#ifdef  RTE_USB_Device_HID_1
      , USBD_HID1_Uninitialize
#endif
#ifdef  RTE_USB_Device_HID_2
      , USBD_HID2_Uninitialize
#endif
#ifdef  RTE_USB_Device_HID_3
      , USBD_HID3_Uninitialize
#endif
#ifdef  RTE_USB_Device_HID_4
      , USBD_HID4_Uninitialize
#endif
#ifdef  RTE_USB_Device_HID_5
      , USBD_HID5_Uninitialize
#endif
#ifdef  RTE_USB_Device_HID_6
      , USBD_HID6_Uninitialize
#endif
#ifdef  RTE_USB_Device_HID_7
      , USBD_HID7_Uninitialize
#endif
};
extern 
int32_t  (* const fpUSBD_HID_GetReport[USBD_HID_NUM]) (uint8_t rtype, uint8_t req, uint8_t  rid, uint8_t *buf);
int32_t  (* const fpUSBD_HID_GetReport[USBD_HID_NUM]) (uint8_t rtype, uint8_t req, uint8_t  rid, uint8_t *buf) = {
        USBD_HID0_GetReport
#ifdef  RTE_USB_Device_HID_1
      , USBD_HID1_GetReport
#endif
#ifdef  RTE_USB_Device_HID_2
      , USBD_HID2_GetReport
#endif
#ifdef  RTE_USB_Device_HID_3
      , USBD_HID3_GetReport
#endif
#ifdef  RTE_USB_Device_HID_4
      , USBD_HID4_GetReport
#endif
#ifdef  RTE_USB_Device_HID_5
      , USBD_HID5_GetReport
#endif
#ifdef  RTE_USB_Device_HID_6
      , USBD_HID6_GetReport
#endif
#ifdef  RTE_USB_Device_HID_7
      , USBD_HID7_GetReport
#endif
};
extern 
bool (* const fpUSBD_HID_SetReport[USBD_HID_NUM]) (uint8_t rtype, uint8_t req, uint8_t rid, const uint8_t *buf, int32_t len);
bool (* const fpUSBD_HID_SetReport[USBD_HID_NUM]) (uint8_t rtype, uint8_t req, uint8_t rid, const uint8_t *buf, int32_t len) = {
        USBD_HID0_SetReport
#ifdef  RTE_USB_Device_HID_1
      , USBD_HID1_SetReport
#endif
#ifdef  RTE_USB_Device_HID_2
      , USBD_HID2_SetReport
#endif
#ifdef  RTE_USB_Device_HID_3
      , USBD_HID3_SetReport
#endif
#ifdef  RTE_USB_Device_HID_4
      , USBD_HID4_SetReport
#endif
#ifdef  RTE_USB_Device_HID_5
      , USBD_HID5_SetReport
#endif
#ifdef  RTE_USB_Device_HID_6
      , USBD_HID6_SetReport
#endif
#ifdef  RTE_USB_Device_HID_7
      , USBD_HID7_SetReport
#endif
};
extern 
int8_t (* const fpUSBD_HID_GetProtocol[USBD_HID_NUM]) (void);
int8_t (* const fpUSBD_HID_GetProtocol[USBD_HID_NUM]) (void) = {
        USBD_HID0_GetProtocol
#ifdef  RTE_USB_Device_HID_1
      , USBD_HID1_GetProtocol
#endif
#ifdef  RTE_USB_Device_HID_2
      , USBD_HID2_GetProtocol
#endif
#ifdef  RTE_USB_Device_HID_3
      , USBD_HID3_GetProtocol
#endif
#ifdef  RTE_USB_Device_HID_4
      , USBD_HID4_GetProtocol
#endif
#ifdef  RTE_USB_Device_HID_5
      , USBD_HID5_GetProtocol
#endif
#ifdef  RTE_USB_Device_HID_6
      , USBD_HID6_GetProtocol
#endif
#ifdef  RTE_USB_Device_HID_7
      , USBD_HID7_GetProtocol
#endif
};
extern 
bool (* const fpUSBD_HID_SetProtocol[USBD_HID_NUM]) (uint8_t protocol);
bool (* const fpUSBD_HID_SetProtocol[USBD_HID_NUM]) (uint8_t protocol) = {
        USBD_HID0_SetProtocol
#ifdef  RTE_USB_Device_HID_1
      , USBD_HID1_SetProtocol
#endif
#ifdef  RTE_USB_Device_HID_2
      , USBD_HID2_SetProtocol
#endif
#ifdef  RTE_USB_Device_HID_3
      , USBD_HID3_SetProtocol
#endif
#ifdef  RTE_USB_Device_HID_4
      , USBD_HID4_SetProtocol
#endif
#ifdef  RTE_USB_Device_HID_5
      , USBD_HID5_SetProtocol
#endif
#ifdef  RTE_USB_Device_HID_6
      , USBD_HID6_SetProtocol
#endif
#ifdef  RTE_USB_Device_HID_7
      , USBD_HID7_SetProtocol
#endif
};
#endif

#if    (USBD_MSC_NUM == 0)
        void      USBD_Endpoint0_ClearFeatureStall_MSC    (uint8_t device, uint8_t ep_addr);
        void      USBD_Endpoint0_ClearFeatureStall_MSC    (uint8_t device, uint8_t ep_addr)                             { (void)device; (void)ep_addr; }
        bool      USBD_Endpoint0_Setup_MSC_ReqToIF        (uint8_t device);
        bool      USBD_Endpoint0_Setup_MSC_ReqToIF        (uint8_t device)                                              { (void)device; return false; }
#else
#ifdef  RTE_USB_Device_MSC_0
__WEAK  void      USBD_MSC0_Initialize                    (void)                                                        { }
__WEAK  void      USBD_MSC0_Uninitialize                  (void)                                                        { }
__WEAK  bool      USBD_MSC0_GetCacheInfo                  (uint32_t *buffer, uint32_t *size)                            { (void)buffer; (void) size; return false; }
__WEAK  bool      USBD_MSC0_GetMediaCapacity              (uint32_t *block_count, uint32_t *block_size)                 { (void)block_count; (void)block_size; return false; }
__WEAK  bool      USBD_MSC0_Read                          (uint32_t lba, uint32_t cnt,       uint8_t *buf)              { (void)lba; (void)cnt; (void)buf; return false; }
__WEAK  bool      USBD_MSC0_Write                         (uint32_t lba, uint32_t cnt, const uint8_t *buf)              { (void)lba; (void)cnt; (void)buf; return false; }
__WEAK  bool      USBD_MSC0_StartStop                     (bool start)                                                  { (void)start; return false; }
__WEAK  uint32_t  USBD_MSC0_CheckMedia                    (void)                                                        { return USBD_MSC_MEDIA_READY; }
__WEAK  uint8_t   USBD_MSC0_GetMaxLUN                     (void)                                                        { return 0U; }

__WEAK  bool      USBD_MSC0_LUN_GetMediaCapacity          (uint8_t lun, uint32_t *block_count, uint32_t *block_size)    { (void)lun; return (USBD_MSC0_GetMediaCapacity (block_count, block_size)); }
__WEAK  bool      USBD_MSC0_LUN_Read                      (uint8_t lun, uint32_t lba, uint32_t cnt,       uint8_t *buf) { (void)lun; return (USBD_MSC0_Read             (lba, cnt, buf));           }
__WEAK  bool      USBD_MSC0_LUN_Write                     (uint8_t lun, uint32_t lba, uint32_t cnt, const uint8_t *buf) { (void)lun; return (USBD_MSC0_Write            (lba, cnt, buf));           }
__WEAK  bool      USBD_MSC0_LUN_StartStop                 (uint8_t lun, bool start)                                     { (void)lun; return (USBD_MSC0_StartStop        (start));                   }
__WEAK  uint32_t  USBD_MSC0_LUN_CheckMedia                (uint8_t lun)                                                 { (void)lun; return (USBD_MSC0_CheckMedia       ());                        }
#endif
#ifdef  RTE_USB_Device_MSC_1
__WEAK  void      USBD_MSC1_Initialize                    (void)                                                        { }
__WEAK  void      USBD_MSC1_Uninitialize                  (void)                                                        { }
__WEAK  bool      USBD_MSC1_GetCacheInfo                  (uint32_t *buffer, uint32_t *size)                            { (void)buffer; (void) size; return false; }
__WEAK  bool      USBD_MSC1_GetMediaCapacity              (uint32_t *block_count, uint32_t *block_size)                 { (void)block_count; (void)block_size; return false; }
__WEAK  bool      USBD_MSC1_Read                          (uint32_t lba, uint32_t cnt,       uint8_t *buf)              { (void)lba; (void)cnt; (void)buf; return false; }
__WEAK  bool      USBD_MSC1_Write                         (uint32_t lba, uint32_t cnt, const uint8_t *buf)              { (void)lba; (void)cnt; (void)buf; return false; }
__WEAK  bool      USBD_MSC1_StartStop                     (bool start)                                                  { (void)start; return false; }
__WEAK  uint32_t  USBD_MSC1_CheckMedia                    (void)                                                        { return USBD_MSC_MEDIA_READY; }
__WEAK  uint8_t   USBD_MSC1_GetMaxLUN                     (void)                                                        { return 0U; }

__WEAK  bool      USBD_MSC1_LUN_GetMediaCapacity          (uint8_t lun, uint32_t *block_count, uint32_t *block_size)    { (void)lun; return (USBD_MSC1_GetMediaCapacity (block_count, block_size)); }
__WEAK  bool      USBD_MSC1_LUN_Read                      (uint8_t lun, uint32_t lba, uint32_t cnt,       uint8_t *buf) { (void)lun; return (USBD_MSC1_Read             (lba, cnt, buf));           }
__WEAK  bool      USBD_MSC1_LUN_Write                     (uint8_t lun, uint32_t lba, uint32_t cnt, const uint8_t *buf) { (void)lun; return (USBD_MSC1_Write            (lba, cnt, buf));           }
__WEAK  bool      USBD_MSC1_LUN_StartStop                 (uint8_t lun, bool start)                                     { (void)lun; return (USBD_MSC1_StartStop        (start));                   }
__WEAK  uint32_t  USBD_MSC1_LUN_CheckMedia                (uint8_t lun)                                                 { (void)lun; return (USBD_MSC1_CheckMedia       ());                        }
#endif
#ifdef  RTE_USB_Device_MSC_2
__WEAK  void      USBD_MSC2_Initialize                    (void)                                                        { }
__WEAK  void      USBD_MSC2_Uninitialize                  (void)                                                        { }
__WEAK  bool      USBD_MSC2_GetCacheInfo                  (uint32_t *buffer, uint32_t *size)                            { (void)buffer; (void) size; return false; }
__WEAK  bool      USBD_MSC2_GetMediaCapacity              (uint32_t *block_count, uint32_t *block_size)                 { (void)block_count; (void)block_size; return false; }
__WEAK  bool      USBD_MSC2_Read                          (uint32_t lba, uint32_t cnt,       uint8_t *buf)              { (void)lba; (void)cnt; (void)buf; return false; }
__WEAK  bool      USBD_MSC2_Write                         (uint32_t lba, uint32_t cnt, const uint8_t *buf)              { (void)lba; (void)cnt; (void)buf; return false; }
__WEAK  bool      USBD_MSC2_StartStop                     (bool start)                                                  { (void)start; return false; }
__WEAK  uint32_t  USBD_MSC2_CheckMedia                    (void)                                                        { return USBD_MSC_MEDIA_READY; }
__WEAK  uint8_t   USBD_MSC2_GetMaxLUN                     (void)                                                        { return 0U; }

__WEAK  bool      USBD_MSC2_LUN_GetMediaCapacity          (uint8_t lun, uint32_t *block_count, uint32_t *block_size)    { (void)lun; return (USBD_MSC2_GetMediaCapacity (block_count, block_size)); }
__WEAK  bool      USBD_MSC2_LUN_Read                      (uint8_t lun, uint32_t lba, uint32_t cnt,       uint8_t *buf) { (void)lun; return (USBD_MSC2_Read             (lba, cnt, buf));           }
__WEAK  bool      USBD_MSC2_LUN_Write                     (uint8_t lun, uint32_t lba, uint32_t cnt, const uint8_t *buf) { (void)lun; return (USBD_MSC2_Write            (lba, cnt, buf));           }
__WEAK  bool      USBD_MSC2_LUN_StartStop                 (uint8_t lun, bool start)                                     { (void)lun; return (USBD_MSC2_StartStop        (start));                   }
__WEAK  uint32_t  USBD_MSC2_LUN_CheckMedia                (uint8_t lun)                                                 { (void)lun; return (USBD_MSC2_CheckMedia       ());                        }
#endif
#ifdef  RTE_USB_Device_MSC_3
__WEAK  void      USBD_MSC3_Initialize                    (void)                                                        { }
__WEAK  void      USBD_MSC3_Uninitialize                  (void)                                                        { }
__WEAK  bool      USBD_MSC3_GetCacheInfo                  (uint32_t *buffer, uint32_t *size)                            { (void)buffer; (void) size; return false; }
__WEAK  bool      USBD_MSC3_GetMediaCapacity              (uint32_t *block_count, uint32_t *block_size)                 { (void)block_count; (void)block_size; return false; }
__WEAK  bool      USBD_MSC3_Read                          (uint32_t lba, uint32_t cnt,       uint8_t *buf)              { (void)lba; (void)cnt; (void)buf; return false; }
__WEAK  bool      USBD_MSC3_Write                         (uint32_t lba, uint32_t cnt, const uint8_t *buf)              { (void)lba; (void)cnt; (void)buf; return false; }
__WEAK  bool      USBD_MSC3_StartStop                     (bool start)                                                  { (void)start; return false; }
__WEAK  uint32_t  USBD_MSC3_CheckMedia                    (void)                                                        { return USBD_MSC_MEDIA_READY; }
__WEAK  uint8_t   USBD_MSC3_GetMaxLUN                     (void)                                                        { return 0U; }

__WEAK  bool      USBD_MSC3_LUN_GetMediaCapacity          (uint8_t lun, uint32_t *block_count, uint32_t *block_size)    { (void)lun; return (USBD_MSC3_GetMediaCapacity (block_count, block_size)); }
__WEAK  bool      USBD_MSC3_LUN_Read                      (uint8_t lun, uint32_t lba, uint32_t cnt,       uint8_t *buf) { (void)lun; return (USBD_MSC3_Read             (lba, cnt, buf));           }
__WEAK  bool      USBD_MSC3_LUN_Write                     (uint8_t lun, uint32_t lba, uint32_t cnt, const uint8_t *buf) { (void)lun; return (USBD_MSC3_Write            (lba, cnt, buf));           }
__WEAK  bool      USBD_MSC3_LUN_StartStop                 (uint8_t lun, bool start)                                     { (void)lun; return (USBD_MSC3_StartStop        (start));                   }
__WEAK  uint32_t  USBD_MSC3_LUN_CheckMedia                (uint8_t lun)                                                 { (void)lun; return (USBD_MSC3_CheckMedia       ());                        }
#endif

extern 
void (* const fpUSBD_MSC_Initialize[USBD_MSC_NUM]) (void);
void (* const fpUSBD_MSC_Initialize[USBD_MSC_NUM]) (void) = {
        USBD_MSC0_Initialize
#ifdef  RTE_USB_Device_MSC_1
      , USBD_MSC1_Initialize
#endif
#ifdef  RTE_USB_Device_MSC_2
      , USBD_MSC2_Initialize
#endif
#ifdef  RTE_USB_Device_MSC_3
      , USBD_MSC3_Initialize
#endif
};
extern 
void (* const fpUSBD_MSC_Uninitialize[USBD_MSC_NUM]) (void);
void (* const fpUSBD_MSC_Uninitialize[USBD_MSC_NUM]) (void) = {
        USBD_MSC0_Uninitialize
#ifdef  RTE_USB_Device_MSC_1
      , USBD_MSC1_Uninitialize
#endif
#ifdef  RTE_USB_Device_MSC_2
      , USBD_MSC2_Uninitialize
#endif
#ifdef  RTE_USB_Device_MSC_3
      , USBD_MSC3_Uninitialize
#endif
};
extern 
bool (* const fpUSBD_MSC_GetCacheInfo[USBD_MSC_NUM]) (uint32_t *buffer, uint32_t *size);
bool (* const fpUSBD_MSC_GetCacheInfo[USBD_MSC_NUM]) (uint32_t *buffer, uint32_t *size) = {
        USBD_MSC0_GetCacheInfo
#ifdef  RTE_USB_Device_MSC_1
      , USBD_MSC1_GetCacheInfo
#endif
#ifdef  RTE_USB_Device_MSC_2
      , USBD_MSC2_GetCacheInfo
#endif
#ifdef  RTE_USB_Device_MSC_3
      , USBD_MSC3_GetCacheInfo
#endif
};
extern 
uint8_t (* const fpUSBD_MSC_GetMaxLUN[USBD_MSC_NUM]) (void);
uint8_t (* const fpUSBD_MSC_GetMaxLUN[USBD_MSC_NUM]) (void) = {
        USBD_MSC0_GetMaxLUN
#ifdef  RTE_USB_Device_MSC_1
      , USBD_MSC1_GetMaxLUN
#endif
#ifdef  RTE_USB_Device_MSC_2
      , USBD_MSC2_GetMaxLUN
#endif
#ifdef  RTE_USB_Device_MSC_3
      , USBD_MSC3_GetMaxLUN
#endif
};
extern 
bool (* const fpUSBD_MSC_LUN_GetMediaCapacity[USBD_MSC_NUM]) (uint8_t lun, uint32_t *block_count, uint32_t *block_size);
bool (* const fpUSBD_MSC_LUN_GetMediaCapacity[USBD_MSC_NUM]) (uint8_t lun, uint32_t *block_count, uint32_t *block_size) = {
        USBD_MSC0_LUN_GetMediaCapacity
#ifdef  RTE_USB_Device_MSC_1
      , USBD_MSC1_LUN_GetMediaCapacity
#endif
#ifdef  RTE_USB_Device_MSC_2
      , USBD_MSC2_LUN_GetMediaCapacity
#endif
#ifdef  RTE_USB_Device_MSC_3
      , USBD_MSC3_LUN_GetMediaCapacity
#endif
};
extern 
bool (* const fpUSBD_MSC_LUN_Read[USBD_MSC_NUM]) (uint8_t lun, uint32_t lba, uint32_t cnt, uint8_t *buf);
bool (* const fpUSBD_MSC_LUN_Read[USBD_MSC_NUM]) (uint8_t lun, uint32_t lba, uint32_t cnt, uint8_t *buf) = {
        USBD_MSC0_LUN_Read
#ifdef  RTE_USB_Device_MSC_1
      , USBD_MSC1_LUN_Read
#endif
#ifdef  RTE_USB_Device_MSC_2
      , USBD_MSC2_LUN_Read
#endif
#ifdef  RTE_USB_Device_MSC_3
      , USBD_MSC3_LUN_Read
#endif
};
extern 
bool (* const fpUSBD_MSC_LUN_Write[USBD_MSC_NUM]) (uint8_t lun, uint32_t lba, uint32_t cnt, const uint8_t *buf);
bool (* const fpUSBD_MSC_LUN_Write[USBD_MSC_NUM]) (uint8_t lun, uint32_t lba, uint32_t cnt, const uint8_t *buf) = {
        USBD_MSC0_LUN_Write
#ifdef  RTE_USB_Device_MSC_1
      , USBD_MSC1_LUN_Write
#endif
#ifdef  RTE_USB_Device_MSC_2
      , USBD_MSC2_LUN_Write
#endif
#ifdef  RTE_USB_Device_MSC_3
      , USBD_MSC3_LUN_Write
#endif
};
extern 
bool (* const fpUSBD_MSC_LUN_StartStop[USBD_MSC_NUM]) (uint8_t lun, bool start);
bool (* const fpUSBD_MSC_LUN_StartStop[USBD_MSC_NUM]) (uint8_t lun, bool start) = {
        USBD_MSC0_LUN_StartStop
#ifdef  RTE_USB_Device_MSC_1
      , USBD_MSC1_LUN_StartStop
#endif
#ifdef  RTE_USB_Device_MSC_2
      , USBD_MSC2_LUN_StartStop
#endif
#ifdef  RTE_USB_Device_MSC_3
      , USBD_MSC3_LUN_StartStop
#endif
};
extern 
uint32_t (* const fpUSBD_MSC_LUN_CheckMedia[USBD_MSC_NUM]) (uint8_t lun);
uint32_t (* const fpUSBD_MSC_LUN_CheckMedia[USBD_MSC_NUM]) (uint8_t lun) = {
        USBD_MSC0_LUN_CheckMedia
#ifdef  RTE_USB_Device_MSC_1
      , USBD_MSC1_LUN_CheckMedia
#endif
#ifdef  RTE_USB_Device_MSC_2
      , USBD_MSC2_LUN_CheckMedia
#endif
#ifdef  RTE_USB_Device_MSC_3
      , USBD_MSC3_LUN_CheckMedia
#endif
};
#endif

__WEAK  void              USBD_Device0_Initialize                    (void)                                                               { }
__WEAK  void              USBD_Device0_Uninitialize                  (void)                                                               { }
__WEAK  void              USBD_Device0_VbusChanged                   (bool level)                                                         { (void)level; }
extern  void              USBD_Device0_EventReset                    (void);
__WEAK  void              USBD_Device0_EventReset                    (void)         /* Deprecated */                                      { }
__WEAK  void              USBD_Device0_Reset                         (void)                                                               { USBD_Device0_EventReset (); }
__WEAK  void              USBD_Device0_HighSpeedActivated            (void)                                                               { }
__WEAK  void              USBD_Device0_Suspended                     (void)                                                               { }
__WEAK  void              USBD_Device0_Resumed                       (void)                                                               { }
__WEAK  void              USBD_Device0_ConfigurationChanged          (uint8_t val)                                                        { (void)val; }
__WEAK  void              USBD_Device0_EnableRemoteWakeup            (void)                                                               { }
__WEAK  void              USBD_Device0_DisableRemoteWakeup           (void)                                                               { }
__WEAK  usbdRequestStatus USBD_Device0_Endpoint0_SetupPacketReceived (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len) { (void)setup_packet; (void)buf; (void)len; return usbdRequestNotProcessed; }
__WEAK  void              USBD_Device0_Endpoint0_SetupPacketProcessed(const USB_SETUP_PACKET *setup_packet)                               { (void)setup_packet;                                                       }
__WEAK  usbdRequestStatus USBD_Device0_Endpoint0_OutDataReceived     (uint32_t len)                                                       { (void)len; return usbdRequestNotProcessed; }
__WEAK  usbdRequestStatus USBD_Device0_Endpoint0_InDataSent          (uint32_t len)                                                       { (void)len; return usbdRequestNotProcessed; }
#ifdef  RTE_USB_Device_1
__WEAK  void              USBD_Device1_Initialize                    (void)                                                               { }
__WEAK  void              USBD_Device1_Uninitialize                  (void)                                                               { }
__WEAK  void              USBD_Device1_VbusChanged                   (bool level)                                                         { (void)level; }
extern  void              USBD_Device1_EventReset                    (void);
__WEAK  void              USBD_Device1_EventReset                    (void)         /* Deprecated */                                      { }
__WEAK  void              USBD_Device1_Reset                         (void)                                                               { USBD_Device1_EventReset (); }
__WEAK  void              USBD_Device1_HighSpeedActivated            (void)                                                               { }
__WEAK  void              USBD_Device1_Suspended                     (void)                                                               { }
__WEAK  void              USBD_Device1_Resumed                       (void)                                                               { }
__WEAK  void              USBD_Device1_ConfigurationChanged          (uint8_t val)                                                        { (void)val; }
__WEAK  void              USBD_Device1_EnableRemoteWakeup            (void)                                                               { }
__WEAK  void              USBD_Device1_DisableRemoteWakeup           (void)                                                               { }
__WEAK  usbdRequestStatus USBD_Device1_Endpoint0_SetupPacketReceived (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len) { (void)setup_packet; (void)buf; (void)len; return usbdRequestNotProcessed; }
__WEAK  void              USBD_Device1_Endpoint0_SetupPacketProcessed(const USB_SETUP_PACKET *setup_packet)                               { (void)setup_packet;                                                       }
__WEAK  usbdRequestStatus USBD_Device1_Endpoint0_OutDataReceived     (uint32_t len)                                                       { (void)len; return usbdRequestNotProcessed; }
__WEAK  usbdRequestStatus USBD_Device1_Endpoint0_InDataSent          (uint32_t len)                                                       { (void)len; return usbdRequestNotProcessed; }
#endif
#ifdef  RTE_USB_Device_2
__WEAK  void              USBD_Device2_Initialize                    (void)                                                               { }
__WEAK  void              USBD_Device2_Uninitialize                  (void)                                                               { }
__WEAK  void              USBD_Device2_VbusChanged                   (bool level)                                                         { (void)level; }
extern  void              USBD_Device2_EventReset                    (void);
__WEAK  void              USBD_Device2_EventReset                    (void)         /* Deprecated */                                      { }
__WEAK  void              USBD_Device2_Reset                         (void)                                                               { USBD_Device2_EventReset (); }
__WEAK  void              USBD_Device2_HighSpeedActivated            (void)                                                               { }
__WEAK  void              USBD_Device2_Suspended                     (void)                                                               { }
__WEAK  void              USBD_Device2_Resumed                       (void)                                                               { }
__WEAK  void              USBD_Device2_ConfigurationChanged          (uint8_t val)                                                        { (void)val; }
__WEAK  void              USBD_Device2_EnableRemoteWakeup            (void)                                                               { }
__WEAK  void              USBD_Device2_DisableRemoteWakeup           (void)                                                               { }
__WEAK  usbdRequestStatus USBD_Device2_Endpoint0_SetupPacketReceived (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len) { (void)setup_packet; (void)buf; (void)len; return usbdRequestNotProcessed; }
__WEAK  void              USBD_Device2_Endpoint0_SetupPacketProcessed(const USB_SETUP_PACKET *setup_packet)                               { (void)setup_packet;                                                       }
__WEAK  usbdRequestStatus USBD_Device2_Endpoint0_OutDataReceived     (uint32_t len)                                                       { (void)len; return usbdRequestNotProcessed; }
__WEAK  usbdRequestStatus USBD_Device2_Endpoint0_InDataSent          (uint32_t len)                                                       { (void)len; return usbdRequestNotProcessed; }
#endif
#ifdef  RTE_USB_Device_3
__WEAK  void              USBD_Device3_Initialize                    (void)                                                               { }
__WEAK  void              USBD_Device3_Uninitialize                  (void)                                                               { }
__WEAK  void              USBD_Device3_VbusChanged                   (bool level)                                                         { (void)level; }
extern  void              USBD_Device3_EventReset                    (void);
__WEAK  void              USBD_Device3_EventReset                    (void)         /* Deprecated */                                      { }
__WEAK  void              USBD_Device3_Reset                         (void)                                                               { USBD_Device3_EventReset (); }
__WEAK  void              USBD_Device3_HighSpeedActivated            (void)                                                               { }
__WEAK  void              USBD_Device3_Suspended                     (void)                                                               { }
__WEAK  void              USBD_Device3_Resumed                       (void)                                                               { }
__WEAK  void              USBD_Device3_ConfigurationChanged          (uint8_t val)                                                        { (void)val; }
__WEAK  void              USBD_Device3_EnableRemoteWakeup            (void)                                                               { }
__WEAK  void              USBD_Device3_DisableRemoteWakeup           (void)                                                               { }
__WEAK  usbdRequestStatus USBD_Device3_Endpoint0_SetupPacketReceived (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len) { (void)setup_packet; (void)buf; (void)len; return usbdRequestNotProcessed; }
__WEAK  void              USBD_Device3_Endpoint0_SetupPacketProcessed(const USB_SETUP_PACKET *setup_packet)                               { (void)setup_packet;                                                       }
__WEAK  usbdRequestStatus USBD_Device3_Endpoint0_OutDataReceived     (uint32_t len)                                                       { (void)len; return usbdRequestNotProcessed; }
__WEAK  usbdRequestStatus USBD_Device3_Endpoint0_InDataSent          (uint32_t len)                                                       { (void)len; return usbdRequestNotProcessed; }
#endif

extern 
void (* const fpUSBD_Device_Initialize[USBD_DEV_NUM]) (void);
void (* const fpUSBD_Device_Initialize[USBD_DEV_NUM]) (void) = {
        USBD_Device0_Initialize
#ifdef  RTE_USB_Device_1
      , USBD_Device1_Initialize
#endif
#ifdef  RTE_USB_Device_2
      , USBD_Device2_Initialize
#endif
#ifdef  RTE_USB_Device_3
      , USBD_Device3_Initialize
#endif
};
extern 
void (* const fpUSBD_Device_Uninitialize[USBD_DEV_NUM]) (void);
void (* const fpUSBD_Device_Uninitialize[USBD_DEV_NUM]) (void) = {
        USBD_Device0_Uninitialize
#ifdef  RTE_USB_Device_1
      , USBD_Device1_Uninitialize
#endif
#ifdef  RTE_USB_Device_2
      , USBD_Device2_Uninitialize
#endif
#ifdef  RTE_USB_Device_3
      , USBD_Device3_Uninitialize
#endif
};
extern 
void (* const fpUSBD_Device_VbusChanged[USBD_DEV_NUM]) (bool level);
void (* const fpUSBD_Device_VbusChanged[USBD_DEV_NUM]) (bool level) = {
        USBD_Device0_VbusChanged
#ifdef  RTE_USB_Device_1
      , USBD_Device1_VbusChanged
#endif
#ifdef  RTE_USB_Device_2
      , USBD_Device2_VbusChanged
#endif
#ifdef  RTE_USB_Device_3
      , USBD_Device3_VbusChanged
#endif
};
extern 
void (* const fpUSBD_Device_Reset[USBD_DEV_NUM]) (void);
void (* const fpUSBD_Device_Reset[USBD_DEV_NUM]) (void) = {
        USBD_Device0_Reset
#ifdef  RTE_USB_Device_1
      , USBD_Device1_Reset
#endif
#ifdef  RTE_USB_Device_2
      , USBD_Device2_Reset
#endif
#ifdef  RTE_USB_Device_3
      , USBD_Device3_Reset
#endif
};
extern 
void (* const fpUSBD_Device_HighSpeedActivated[USBD_DEV_NUM]) (void);
void (* const fpUSBD_Device_HighSpeedActivated[USBD_DEV_NUM]) (void) = {
        USBD_Device0_HighSpeedActivated
#ifdef  RTE_USB_Device_1
      , USBD_Device1_HighSpeedActivated
#endif
#ifdef  RTE_USB_Device_2
      , USBD_Device2_HighSpeedActivated
#endif
#ifdef  RTE_USB_Device_3
      , USBD_Device3_HighSpeedActivated
#endif
};
extern 
void (* const fpUSBD_Device_Suspended[USBD_DEV_NUM]) (void);
void (* const fpUSBD_Device_Suspended[USBD_DEV_NUM]) (void) = {
        USBD_Device0_Suspended
#ifdef  RTE_USB_Device_1
      , USBD_Device1_Suspended
#endif
#ifdef  RTE_USB_Device_2
      , USBD_Device2_Suspended
#endif
#ifdef  RTE_USB_Device_3
      , USBD_Device3_Suspended
#endif
};
extern 
void (* const fpUSBD_Device_Resumed[USBD_DEV_NUM]) (void);
void (* const fpUSBD_Device_Resumed[USBD_DEV_NUM]) (void) = {
        USBD_Device0_Resumed
#ifdef  RTE_USB_Device_1
      , USBD_Device1_Resumed
#endif
#ifdef  RTE_USB_Device_2
      , USBD_Device2_Resumed
#endif
#ifdef  RTE_USB_Device_3
      , USBD_Device3_Resumed
#endif
};
extern 
void (* const fpUSBD_Device_ConfigurationChanged[USBD_DEV_NUM]) (uint8_t val);
void (* const fpUSBD_Device_ConfigurationChanged[USBD_DEV_NUM]) (uint8_t val) = {
        USBD_Device0_ConfigurationChanged
#ifdef  RTE_USB_Device_1
      , USBD_Device1_ConfigurationChanged
#endif
#ifdef  RTE_USB_Device_2
      , USBD_Device2_ConfigurationChanged
#endif
#ifdef  RTE_USB_Device_3
      , USBD_Device3_ConfigurationChanged
#endif
};
extern 
void (* const fpUSBD_Device_EnableRemoteWakeup[USBD_DEV_NUM]) (void);
void (* const fpUSBD_Device_EnableRemoteWakeup[USBD_DEV_NUM]) (void) = {
        USBD_Device0_EnableRemoteWakeup
#ifdef  RTE_USB_Device_1
      , USBD_Device1_EnableRemoteWakeup
#endif
#ifdef  RTE_USB_Device_2
      , USBD_Device2_EnableRemoteWakeup
#endif
#ifdef  RTE_USB_Device_3
      , USBD_Device3_EnableRemoteWakeup
#endif
};
extern 
void (* const fpUSBD_Device_DisableRemoteWakeup[USBD_DEV_NUM]) (void);
void (* const fpUSBD_Device_DisableRemoteWakeup[USBD_DEV_NUM]) (void) = {
        USBD_Device0_DisableRemoteWakeup
#ifdef  RTE_USB_Device_1
      , USBD_Device1_DisableRemoteWakeup
#endif
#ifdef  RTE_USB_Device_2
      , USBD_Device2_DisableRemoteWakeup
#endif
#ifdef  RTE_USB_Device_3
      , USBD_Device3_DisableRemoteWakeup
#endif
};
extern 
usbdRequestStatus (* const fpUSBD_Device_Endpoint0_SetupPacketReceived[USBD_DEV_NUM]) (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len);
usbdRequestStatus (* const fpUSBD_Device_Endpoint0_SetupPacketReceived[USBD_DEV_NUM]) (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len) = {
        USBD_Device0_Endpoint0_SetupPacketReceived
#ifdef  RTE_USB_Device_1
      , USBD_Device1_Endpoint0_SetupPacketReceived
#endif
#ifdef  RTE_USB_Device_2
      , USBD_Device2_Endpoint0_SetupPacketReceived
#endif
#ifdef  RTE_USB_Device_3
      , USBD_Device3_Endpoint0_SetupPacketReceived
#endif
};
extern 
void (* const fpUSBD_Device_Endpoint0_SetupPacketProcessed[USBD_DEV_NUM]) (const USB_SETUP_PACKET *setup_packet);
void (* const fpUSBD_Device_Endpoint0_SetupPacketProcessed[USBD_DEV_NUM]) (const USB_SETUP_PACKET *setup_packet) = {
        USBD_Device0_Endpoint0_SetupPacketProcessed
#ifdef  RTE_USB_Device_1
      , USBD_Device1_Endpoint0_SetupPacketProcessed
#endif
#ifdef  RTE_USB_Device_2
      , USBD_Device2_Endpoint0_SetupPacketProcessed
#endif
#ifdef  RTE_USB_Device_3
      , USBD_Device3_Endpoint0_SetupPacketProcessed
#endif
};
extern 
usbdRequestStatus (* const fpUSBD_Device_Endpoint0_OutDataReceived[USBD_DEV_NUM]) (uint32_t len);
usbdRequestStatus (* const fpUSBD_Device_Endpoint0_OutDataReceived[USBD_DEV_NUM]) (uint32_t len) = {
        USBD_Device0_Endpoint0_OutDataReceived
#ifdef  RTE_USB_Device_1
      , USBD_Device1_Endpoint0_OutDataReceived
#endif
#ifdef  RTE_USB_Device_2
      , USBD_Device2_Endpoint0_OutDataReceived
#endif
#ifdef  RTE_USB_Device_3
      , USBD_Device3_Endpoint0_OutDataReceived
#endif
};
extern 
usbdRequestStatus (* const fpUSBD_Device_Endpoint0_InDataSent[USBD_DEV_NUM]) (uint32_t len);
usbdRequestStatus (* const fpUSBD_Device_Endpoint0_InDataSent[USBD_DEV_NUM]) (uint32_t len) = {
        USBD_Device0_Endpoint0_InDataSent
#ifdef  RTE_USB_Device_1
      , USBD_Device1_Endpoint0_InDataSent
#endif
#ifdef  RTE_USB_Device_2
      , USBD_Device2_Endpoint0_InDataSent
#endif
#ifdef  RTE_USB_Device_3
      , USBD_Device3_Endpoint0_InDataSent
#endif
};

usbStatus USBD_ClassInitialize (uint8_t device) {
  uint8_t   instance;
  usbStatus status_ret, status;

  status_ret = usbOK;
#if (USBD_CUSTOM_CLASS_NUM > 0)
  for (instance = 0; instance < usbd_custom_class_num; instance++) {
    if (usbd_custom_class_ptr[instance] != NULL) {
      if (usbd_custom_class_ptr[instance]->dev_num == device) {
        status = USBD_CustomClass_Initialize (instance);
        if (status) status_ret = status;
      }
    }
  }
#endif

#if (USBD_ADC_NUM > 0)
  for (instance = 0; instance < usbd_adc_num; instance++) {
    if ((usbd_adc_ptr[instance]->spkr_data_ptr != NULL) || (usbd_adc_ptr[instance]->mic_data_ptr != NULL)) {
      if (usbd_adc_ptr[instance]->dev_num == device) {
        status = USBD_ADC_Initialize (instance);
        if (status) status_ret = status;
      }
    }
  }
#endif

#if (USBD_CDC_NUM > 0)
  for (instance = 0; instance < usbd_cdc_num; instance++) {
    if (usbd_cdc_ptr[instance]->data_ptr != NULL) {
      if (usbd_cdc_ptr[instance]->dev_num == device) {
        status = USBD_CDC_Initialize (instance);
        if (status) status_ret = status;
      }
    }
  }
#endif

#if (USBD_HID_NUM > 0)
  for (instance = 0; instance < usbd_hid_num; instance++) {
    if (usbd_hid_ptr[instance]->data_ptr != NULL) {
      if (usbd_hid_ptr[instance]->dev_num == device) {
        status = USBD_HID_Initialize (instance);
        if (status) status_ret = status;
      }
    }
  }
#endif

#if (USBD_MSC_NUM > 0)
  for (instance = 0; instance < usbd_msc_num; instance++) {
    if (usbd_msc_ptr[instance]->data_ptr != NULL) {
      if (usbd_msc_ptr[instance]->dev_num == device) {
        status = USBD_MSC_Initialize (instance);
        if (status) status_ret = status;
      }
    }
  }
#endif

  return status_ret;
}

usbStatus USBD_ClassUninitialize (uint8_t device) {
  uint8_t   instance;
  usbStatus status_ret, status;

  status_ret = usbOK;
#if (USBD_CUSTOM_CLASS_NUM > 0)
  for (instance = 0; instance < usbd_custom_class_num; instance++) {
    if (usbd_custom_class_ptr[instance] != NULL) {
      if (usbd_custom_class_ptr[instance]->dev_num == device) {
        status = USBD_CustomClass_Uninitialize (instance);
        if (status) status_ret = status;
      }
    }
  }
#endif

#if (USBD_ADC_NUM > 0)
  for (instance = 0; instance < usbd_adc_num; instance++) {
    if ((usbd_adc_ptr[instance]->spkr_data_ptr != NULL) || (usbd_adc_ptr[instance]->mic_data_ptr != NULL)) {
      if (usbd_adc_ptr[instance]->dev_num == device) {
        status = USBD_ADC_Uninitialize (instance);
        if (status) status_ret = status;
      }
    }
  }
#endif

#if (USBD_CDC_NUM > 0)
  for (instance = 0; instance < usbd_cdc_num; instance++) {
    if (usbd_cdc_ptr[instance]->data_ptr != NULL) {
      if (usbd_cdc_ptr[instance]->dev_num == device) {
        status = USBD_CDC_Uninitialize (instance);
        if (status) status_ret = status;
      }
    }
  }
#endif

#if (USBD_HID_NUM > 0)
  for (instance = 0; instance < usbd_hid_num; instance++) {
    if (usbd_hid_ptr[instance]->data_ptr != NULL) {
      if (usbd_hid_ptr[instance]->dev_num == device) {
        status = USBD_HID_Uninitialize (instance);
        if (status) status_ret = status;
      }
    }
  }
#endif

#if (USBD_MSC_NUM > 0)
  for (instance = 0; instance < usbd_msc_num; instance++) {
    if (usbd_msc_ptr[instance]->data_ptr != NULL) {
      if (usbd_msc_ptr[instance]->dev_num == device) {
        status = USBD_MSC_Uninitialize (instance);
        if (status) status_ret = status;
      }
    }
  }
#endif

  return status_ret;
}

void USBD_Reset (uint8_t device) {
  uint8_t instance;

#if (USBD_CUSTOM_CLASS_NUM > 0)
  for (instance = 0; instance < usbd_custom_class_num; instance++) {
    if (usbd_custom_class_ptr[instance] != NULL) {
      if (usbd_custom_class_ptr[instance]->dev_num == device) {
        USBD_CustomClass_Reset (instance);
      }
    }
  }
#endif

#if (USBD_ADC_NUM > 0)
  for (instance = 0; instance < usbd_adc_num; instance++) {
    if ((usbd_adc_ptr[instance]->spkr_data_ptr != NULL) || (usbd_adc_ptr[instance]->mic_data_ptr != NULL)){
      if (usbd_adc_ptr[instance]->dev_num == device) {
        USBD_ADC_Reset (instance);
      }
    }
  }
#endif

#if (USBD_CDC_NUM > 0)
  for (instance = 0; instance < usbd_cdc_num; instance++) {
    if (usbd_cdc_ptr[instance]->data_ptr != NULL) {
      if (usbd_cdc_ptr[instance]->dev_num == device) {
        USBD_CDC_Reset (instance);
      }
    }
  }
#endif

#if (USBD_HID_NUM > 0)
  for (instance = 0; instance < usbd_hid_num; instance++) {
    if (usbd_hid_ptr[instance]->data_ptr != NULL) {
      if (usbd_hid_ptr[instance]->dev_num == device) {
        USBD_HID_Reset (instance);
      }
    }
  }
#endif

#if (USBD_MSC_NUM > 0)
  for (instance = 0; instance < usbd_msc_num; instance++) {
    if (usbd_msc_ptr[instance]->data_ptr != NULL) {
      if (usbd_msc_ptr[instance]->dev_num == device) {
        USBD_MSC_Reset (instance);
      }
    }
  }
#endif
}

void USBD_SetConfiguration (uint8_t device, uint8_t configuration) {

#if (USBD_HID_NUM > 0)
  uint8_t instance;

  if (configuration != 0U) {
    for (instance = 0; instance < usbd_hid_num; instance++) {
      if (usbd_hid_ptr[instance]->data_ptr != NULL) {
        if (usbd_hid_ptr[instance]->dev_num == device) {
          USBD_HID_SetConfiguration (instance);
        }
      }
    }
  }
#else
  (void)device;
  (void)configuration;
#endif
}

void USBD_EndpointStart (uint8_t device, uint8_t ep_addr) {
  uint8_t instance;

#if (USBD_CUSTOM_CLASS_NUM > 0)
  uint8_t i;

  for (instance = 0; instance < usbd_custom_class_num; instance++) {
    if (usbd_custom_class_ptr[instance] != NULL) {
      if (usbd_custom_class_ptr[instance]->dev_num == device) {
        for (i = 0U; i < 4U; i++) {
          if (usbd_custom_class_ptr[instance]->cc[i].if_en) {
            if (((usbd_custom_class_ptr[instance]->cc[i].if_ep0_en   != 0U)       &&
                 (usbd_custom_class_ptr[instance]->cc[i].if_ep0_addr == ep_addr)) ||
                ((usbd_custom_class_ptr[instance]->cc[i].if_ep1_en   != 0U)       &&
                 (usbd_custom_class_ptr[instance]->cc[i].if_ep1_addr == ep_addr)) ||
                ((usbd_custom_class_ptr[instance]->cc[i].if_ep2_en   != 0U)       &&
                 (usbd_custom_class_ptr[instance]->cc[i].if_ep2_addr == ep_addr)) ||
                ((usbd_custom_class_ptr[instance]->cc[i].if_ep3_en   != 0U)       &&
                 (usbd_custom_class_ptr[instance]->cc[i].if_ep3_addr == ep_addr)) ||
                ((usbd_custom_class_ptr[instance]->cc[i].if_ep4_en   != 0U)       &&
                 (usbd_custom_class_ptr[instance]->cc[i].if_ep4_addr == ep_addr)) ||
                ((usbd_custom_class_ptr[instance]->cc[i].if_ep5_en   != 0U)       &&
                 (usbd_custom_class_ptr[instance]->cc[i].if_ep5_addr == ep_addr)) ||
                ((usbd_custom_class_ptr[instance]->cc[i].if_ep6_en   != 0U)       &&
                 (usbd_custom_class_ptr[instance]->cc[i].if_ep6_addr == ep_addr)) ||
                ((usbd_custom_class_ptr[instance]->cc[i].if_ep7_en   != 0U)       &&
                 (usbd_custom_class_ptr[instance]->cc[i].if_ep7_addr == ep_addr))) {
              USBD_CustomClass_EndpointStart (instance, ep_addr);
            }
          }
        }
      }
    }
  }
#endif

#if (USBD_ADC_NUM > 0)
  for (instance = 0; instance < usbd_adc_num; instance++) {
    if (usbd_adc_ptr[instance]->dev_num == device) {
      if ((usbd_adc_ptr[instance]->spkr_data_ptr != NULL) && (usbd_adc_ptr[instance]->out_cfg != NULL)) {
        if ((usbd_adc_ptr[instance]->out_cfg->ep_iso) == ep_addr) {
          USBD_ADC_EndpointStart (instance, ep_addr);
        }
      }
      if ((usbd_adc_ptr[instance]->mic_data_ptr  != NULL) && (usbd_adc_ptr[instance]->in_cfg  != NULL)) {
        if ((usbd_adc_ptr[instance]->in_cfg->ep_iso | 0x80) == ep_addr) {
          USBD_ADC_EndpointStart (instance, ep_addr);
        }
      }
    }
  }
#endif

#if (USBD_CDC_NUM > 0)
  for (instance = 0; instance < usbd_cdc_num; instance++) {
    if (usbd_cdc_ptr[instance]->data_ptr != NULL) {
      if (usbd_cdc_ptr[instance]->dev_num == device) {
        if (((usbd_cdc_ptr[instance]->ep_int_in  | 0x80) == ep_addr) ||
            ((usbd_cdc_ptr[instance]->ep_bulk_in | 0x80) == ep_addr) ||
            ((usbd_cdc_ptr[instance]->ep_bulk_out      ) == ep_addr)) {
          USBD_CDC_EndpointStart (instance, ep_addr);
        }
      }
    }
  }
#endif

#if (USBD_HID_NUM > 0)
  for (instance = 0; instance < usbd_hid_num; instance++) {
    if (usbd_hid_ptr[instance]->data_ptr != NULL) {
      if (usbd_hid_ptr[instance]->dev_num == device) {
        if (((usbd_hid_ptr[instance]->ep_int_in | 0x80) == ep_addr) ||
            ((usbd_hid_ptr[instance]->ep_int_out      ) == ep_addr)) {
          USBD_HID_EndpointStart (instance, ep_addr);
        }
      }
    }
  }
#endif

#if (USBD_MSC_NUM > 0)
  for (instance = 0; instance < usbd_msc_num; instance++) {
    if (usbd_msc_ptr[instance]->data_ptr != NULL) {
      if (usbd_msc_ptr[instance]->dev_num == device) {
        if (((usbd_msc_ptr[instance]->ep_bulk_in | 0x80) == ep_addr) ||
            ((usbd_msc_ptr[instance]->ep_bulk_out      ) == ep_addr)) {
          USBD_MSC_EndpointStart (instance, ep_addr);
        }
      }
    }
  }
#endif
}

void USBD_EndpointStop (uint8_t device, uint8_t ep_addr) {
#if ((USBD_CUSTOM_CLASS_NUM > 0) || (USBD_ADC_NUM > 0)  || (USBD_CDC_NUM > 0))
  uint8_t instance;
#else
  (void)device;
  (void)ep_addr;
#endif

#if (USBD_CUSTOM_CLASS_NUM > 0)
  uint8_t i;

  for (instance = 0; instance < usbd_custom_class_num; instance++) {
    if (usbd_custom_class_ptr[instance] != NULL) {
      if (usbd_custom_class_ptr[instance]->dev_num == device) {
        for (i = 0U; i < 4U; i++) {
          if (usbd_custom_class_ptr[instance]->cc[i].if_en) {
            if (((usbd_custom_class_ptr[instance]->cc[i].if_ep0_en   != 0U)       &&
                 (usbd_custom_class_ptr[instance]->cc[i].if_ep0_addr == ep_addr)) ||
                ((usbd_custom_class_ptr[instance]->cc[i].if_ep1_en   != 0U)       &&
                 (usbd_custom_class_ptr[instance]->cc[i].if_ep1_addr == ep_addr)) ||
                ((usbd_custom_class_ptr[instance]->cc[i].if_ep2_en   != 0U)       &&
                 (usbd_custom_class_ptr[instance]->cc[i].if_ep2_addr == ep_addr)) ||
                ((usbd_custom_class_ptr[instance]->cc[i].if_ep3_en   != 0U)       &&
                 (usbd_custom_class_ptr[instance]->cc[i].if_ep3_addr == ep_addr)) ||
                ((usbd_custom_class_ptr[instance]->cc[i].if_ep4_en   != 0U)       &&
                 (usbd_custom_class_ptr[instance]->cc[i].if_ep4_addr == ep_addr)) ||
                ((usbd_custom_class_ptr[instance]->cc[i].if_ep5_en   != 0U)       &&
                 (usbd_custom_class_ptr[instance]->cc[i].if_ep5_addr == ep_addr)) ||
                ((usbd_custom_class_ptr[instance]->cc[i].if_ep6_en   != 0U)       &&
                 (usbd_custom_class_ptr[instance]->cc[i].if_ep6_addr == ep_addr)) ||
                ((usbd_custom_class_ptr[instance]->cc[i].if_ep7_en   != 0U)       &&
                 (usbd_custom_class_ptr[instance]->cc[i].if_ep7_addr == ep_addr))) {
              USBD_CustomClass_EndpointStop (instance, ep_addr);
            }
          }
        }
      }
    }
  }
#endif

#if (USBD_ADC_NUM > 0)
  for (instance = 0; instance < usbd_adc_num; instance++) {
    if (usbd_adc_ptr[instance]->dev_num == device) {
      if ((usbd_adc_ptr[instance]->spkr_data_ptr != NULL) && (usbd_adc_ptr[instance]->out_cfg != NULL)) {
        if ((usbd_adc_ptr[instance]->out_cfg->ep_iso) == ep_addr) {
          USBD_ADC_EndpointStop (instance, ep_addr);
        }
      }
      if ((usbd_adc_ptr[instance]->mic_data_ptr  != NULL) && (usbd_adc_ptr[instance]->in_cfg  != NULL)) {
        if ((usbd_adc_ptr[instance]->in_cfg->ep_iso | 0x80) == ep_addr) {
          USBD_ADC_EndpointStop (instance, ep_addr);
        }
      }
    }
  }
#endif

#if (USBD_CDC_NUM > 0)
  for (instance = 0; instance < usbd_cdc_num; instance++) {
    if (usbd_cdc_ptr[instance]->data_ptr != NULL) {
      if (usbd_cdc_ptr[instance]->dev_num == device) {
        if (((usbd_cdc_ptr[instance]->ep_int_in  | 0x80) == ep_addr) ||
            ((usbd_cdc_ptr[instance]->ep_bulk_in | 0x80) == ep_addr) ||
            ((usbd_cdc_ptr[instance]->ep_bulk_out      ) == ep_addr)) {
          USBD_CDC_EndpointStop (instance, ep_addr);
        }
      }
    }
  }
#endif
}
#endif


/*------------------------------------------------------------------------------
 *      USB Device Global Macros for Descriptors
 *----------------------------------------------------------------------------*/

#ifndef USBD_DESC_MACROS
#define USBD_DESC_MACROS                1
#include "usbd_config_desc_macros.h"
#endif


/*------------------------------------------------------------------------------
 *      USB Custom Class 0..3 Specific Descriptors
 *----------------------------------------------------------------------------*/

#if    (USBD_CUSTOM_CLASS_NUM > 0)

#ifdef  RTE_USB_Device_CustomClass_0
#if   ((defined(USBD0_PORT) && (USBD0_CUSTOM_CLASS0 == 1)) || \
       (defined(USBD1_PORT) && (USBD1_CUSTOM_CLASS0 == 1)) || \
       (defined(USBD2_PORT) && (USBD2_CUSTOM_CLASS0 == 1)) || \
       (defined(USBD3_PORT) && (USBD3_CUSTOM_CLASS0 == 1)))
#include "usbd_config_def_cc_0.h"
#include "usbd_config_desc_cc_0.c"
#endif
#endif

#ifdef  RTE_USB_Device_CustomClass_1
#if   ((defined(USBD0_PORT) && (USBD0_CUSTOM_CLASS1 == 1)) || \
       (defined(USBD1_PORT) && (USBD1_CUSTOM_CLASS1 == 1)) || \
       (defined(USBD2_PORT) && (USBD2_CUSTOM_CLASS1 == 1)) || \
       (defined(USBD3_PORT) && (USBD3_CUSTOM_CLASS1 == 1)))
#include "usbd_config_def_cc_1.h"
#include "usbd_config_desc_cc_1.c"
#endif
#endif

#ifdef  RTE_USB_Device_CustomClass_2
#if   ((defined(USBD0_PORT) && (USBD0_CUSTOM_CLASS2 == 1)) || \
       (defined(USBD1_PORT) && (USBD1_CUSTOM_CLASS2 == 1)) || \
       (defined(USBD2_PORT) && (USBD2_CUSTOM_CLASS2 == 1)) || \
       (defined(USBD3_PORT) && (USBD3_CUSTOM_CLASS2 == 1)))
#include "usbd_config_def_cc_2.h"
#include "usbd_config_desc_cc_2.c"
#endif
#endif

#ifdef  RTE_USB_Device_CustomClass_3
#if   ((defined(USBD0_PORT) && (USBD0_CUSTOM_CLASS3 == 1)) || \
       (defined(USBD1_PORT) && (USBD1_CUSTOM_CLASS3 == 1)) || \
       (defined(USBD2_PORT) && (USBD2_CUSTOM_CLASS3 == 1)) || \
       (defined(USBD3_PORT) && (USBD3_CUSTOM_CLASS3 == 1)))
#include "usbd_config_def_cc_3.h"
#include "usbd_config_desc_cc_3.c"
#endif
#endif

extern  usbd_cc_desc_t usbd_cc0_desc;
extern  usbd_cc_desc_t usbd_cc1_desc;
extern  usbd_cc_desc_t usbd_cc2_desc;
extern  usbd_cc_desc_t usbd_cc3_desc;

extern 
usbd_cc_desc_t * const usbd_cc_desc_ptr[USBD_CUSTOM_CLASS_NUM];
usbd_cc_desc_t * const usbd_cc_desc_ptr[USBD_CUSTOM_CLASS_NUM] = {
#ifdef  RTE_USB_Device_CustomClass_0
       &usbd_cc0_desc
#endif
#ifdef  RTE_USB_Device_CustomClass_1
     , &usbd_cc1_desc
#endif
#ifdef  RTE_USB_Device_CustomClass_2
     , &usbd_cc2_desc
#endif
#ifdef  RTE_USB_Device_CustomClass_3
     , &usbd_cc3_desc
#endif
};

#endif


/*------------------------------------------------------------------------------
 *      USB HID 0..7 Specific Descriptors
 *----------------------------------------------------------------------------*/

#if    (USBD_HID_NUM > 0)

#ifdef  RTE_USB_Device_HID_0
#if   ((defined(USBD0_PORT) && (USBD0_HID0 == 1)) || \
       (defined(USBD1_PORT) && (USBD1_HID0 == 1)) || \
       (defined(USBD2_PORT) && (USBD2_HID0 == 1)) || \
       (defined(USBD3_PORT) && (USBD3_HID0 == 1)))
#include "usbd_config_desc_hid_0.c"
#endif
#endif

#ifdef  RTE_USB_Device_HID_1
#if   ((defined(USBD0_PORT) && (USBD0_HID1 == 1)) || \
       (defined(USBD1_PORT) && (USBD1_HID1 == 1)) || \
       (defined(USBD2_PORT) && (USBD2_HID1 == 1)) || \
       (defined(USBD3_PORT) && (USBD3_HID1 == 1)))
#include "usbd_config_desc_hid_1.c"
#endif
#endif

#ifdef  RTE_USB_Device_HID_2
#if   ((defined(USBD0_PORT) && (USBD0_HID2 == 1)) || \
       (defined(USBD1_PORT) && (USBD1_HID2 == 1)) || \
       (defined(USBD2_PORT) && (USBD2_HID2 == 1)) || \
       (defined(USBD3_PORT) && (USBD3_HID2 == 1)))
#include "usbd_config_desc_hid_2.c"
#endif
#endif

#ifdef  RTE_USB_Device_HID_3
#if   ((defined(USBD0_PORT) && (USBD0_HID3 == 1)) || \
       (defined(USBD1_PORT) && (USBD1_HID3 == 1)) || \
       (defined(USBD2_PORT) && (USBD2_HID3 == 1)) || \
       (defined(USBD3_PORT) && (USBD3_HID3 == 1)))
#include "usbd_config_desc_hid_3.c"
#endif
#endif

#ifdef  RTE_USB_Device_HID_4
#if   ((defined(USBD0_PORT) && (USBD0_HID4 == 1)) || \
       (defined(USBD1_PORT) && (USBD1_HID4 == 1)) || \
       (defined(USBD2_PORT) && (USBD2_HID4 == 1)) || \
       (defined(USBD3_PORT) && (USBD3_HID4 == 1)))
#include "usbd_config_desc_hid_4.c"
#endif
#endif

#ifdef  RTE_USB_Device_HID_5
#if   ((defined(USBD0_PORT) && (USBD0_HID5 == 1)) || \
       (defined(USBD1_PORT) && (USBD1_HID5 == 1)) || \
       (defined(USBD2_PORT) && (USBD2_HID5 == 1)) || \
       (defined(USBD3_PORT) && (USBD3_HID5 == 1)))
#include "usbd_config_desc_hid_5.c"
#endif
#endif

#ifdef  RTE_USB_Device_HID_6
#if   ((defined(USBD0_PORT) && (USBD0_HID6 == 1)) || \
       (defined(USBD1_PORT) && (USBD1_HID6 == 1)) || \
       (defined(USBD2_PORT) && (USBD2_HID6 == 1)) || \
       (defined(USBD3_PORT) && (USBD3_HID6 == 1)))
#include "usbd_config_desc_hid_6.c"
#endif
#endif

#ifdef  RTE_USB_Device_HID_7
#if   ((defined(USBD0_PORT) && (USBD0_HID7 == 1)) || \
       (defined(USBD1_PORT) && (USBD1_HID7 == 1)) || \
       (defined(USBD2_PORT) && (USBD2_HID7 == 1)) || \
       (defined(USBD3_PORT) && (USBD3_HID7 == 1)))
#include "usbd_config_desc_hid_7.c"
#endif
#endif

extern  usbd_hid_desc_t usbd_hid0_desc;
extern  usbd_hid_desc_t usbd_hid1_desc;
extern  usbd_hid_desc_t usbd_hid2_desc;
extern  usbd_hid_desc_t usbd_hid3_desc;
extern  usbd_hid_desc_t usbd_hid4_desc;
extern  usbd_hid_desc_t usbd_hid5_desc;
extern  usbd_hid_desc_t usbd_hid6_desc;
extern  usbd_hid_desc_t usbd_hid7_desc;

extern 
usbd_hid_desc_t * const usbd_hid_desc_ptr[USBD_HID_NUM];
usbd_hid_desc_t * const usbd_hid_desc_ptr[USBD_HID_NUM] = {
#ifdef  RTE_USB_Device_HID_0
       &usbd_hid0_desc
#endif
#ifdef  RTE_USB_Device_HID_1
     , &usbd_hid1_desc
#endif
#ifdef  RTE_USB_Device_HID_2
     , &usbd_hid2_desc
#endif
#ifdef  RTE_USB_Device_HID_3
     , &usbd_hid3_desc
#endif
#ifdef  RTE_USB_Device_HID_4
     , &usbd_hid4_desc
#endif
#ifdef  RTE_USB_Device_HID_5
     , &usbd_hid5_desc
#endif
#ifdef  RTE_USB_Device_HID_6
     , &usbd_hid6_desc
#endif
#ifdef  RTE_USB_Device_HID_7
     , &usbd_hid7_desc
#endif
};

#endif


/*------------------------------------------------------------------------------
 *      USB Device 0 Descriptors
 *----------------------------------------------------------------------------*/

#ifdef  USBD0_PORT
#include "usbd_config_desc_0.c"
#endif


/*------------------------------------------------------------------------------
 *      USB Device 1 Descriptors
 *----------------------------------------------------------------------------*/

#ifdef  USBD1_PORT
#include "usbd_config_desc_1.c"
#endif


/*------------------------------------------------------------------------------
 *      USB Device 2 Descriptors
 *----------------------------------------------------------------------------*/

#ifdef  USBD2_PORT
#include "usbd_config_desc_2.c"
#endif


/*------------------------------------------------------------------------------
 *      USB Device 0 Descriptors
 *----------------------------------------------------------------------------*/

#ifdef  USBD3_PORT
#include "usbd_config_desc_3.c"
#endif


/*------------------------------------------------------------------------------
 *      USB Device 0..3 Pointers to Descriptors
 *----------------------------------------------------------------------------*/

#if    (USBD_DEV_NUM > 0)

extern usbd_desc_t usbd0_desc;
extern usbd_desc_t usbd1_desc;
extern usbd_desc_t usbd2_desc;
extern usbd_desc_t usbd3_desc;

extern 
usbd_desc_t *usbd_desc_ptr [USBD_DEV_NUM];
usbd_desc_t *usbd_desc_ptr [USBD_DEV_NUM]  = {
#ifdef  RTE_USB_Device_0
       &usbd0_desc
#endif
#ifdef  RTE_USB_Device_1
     , &usbd1_desc
#endif
#ifdef  RTE_USB_Device_2
     , &usbd2_desc
#endif
#ifdef  RTE_USB_Device_3
     , &usbd3_desc
#endif
};

#endif
