/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_config_def_cc_$x.h
 * Purpose: USB Device Custom Class $x Specific Definitions
 *----------------------------------------------------------------------------*/

// USB Device Custom Class $x Interface 0 Microsoft Extended Properties OS Feature Descriptor calculations
#define USBD_CC$x_IF0_MS_EXT_PROP_CNT      ((USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP0_EN != 0) + \
                                           (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP1_EN != 0) + \
                                           (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP2_EN != 0) + \
                                           (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP3_EN != 0) )

#if    (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP0_EN != 0)
#if   ((USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP0_DATA_TYP == 1) || (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP0_DATA_TYP == 2) || (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP0_DATA_TYP == 6) )
#ifdef  USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP0_NAME_RAW
#define USBD_CC$x_IF0_MS_EXT_PROP0_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP0_NAME_RAW)) + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP0_DATA_STR_RAW)))
#else
#define USBD_CC$x_IF0_MS_EXT_PROP0_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP0_NAME) + sizeof(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP0_DATA_STR))
#endif
#elif ((USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP0_DATA_TYP == 4) || (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP0_DATA_TYP == 5) )
#ifdef  USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP0_NAME_RAW
#define USBD_CC$x_IF0_MS_EXT_PROP0_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP0_NAME_RAW)) + 4U)
#else
#define USBD_CC$x_IF0_MS_EXT_PROP0_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP0_NAME) + 4U)
#endif
#else
#error Custom Class $x Interface 0 Extended Properties OS Feature Descriptor Custom Property Section 0 Data Type is not allowed!
#endif
#else
#define USBD_CC$x_IF0_MS_EXT_PROP0_LEN      (0U)
#endif

#if    (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP1_EN != 0)
#if   ((USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP1_DATA_TYP == 1) || (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP1_DATA_TYP == 2) || (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP1_DATA_TYP == 6) )
#ifdef  USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP1_NAME_RAW
#define USBD_CC$x_IF0_MS_EXT_PROP1_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP1_NAME_RAW)) + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP1_DATA_STR_RAW)))
#else
#define USBD_CC$x_IF0_MS_EXT_PROP1_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP1_NAME) + sizeof(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP1_DATA_STR))
#endif
#elif ((USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP1_DATA_TYP == 4) || (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP1_DATA_TYP == 5) )
#ifdef  USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP1_NAME_RAW
#define USBD_CC$x_IF0_MS_EXT_PROP1_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP1_NAME_RAW)) + 4U)
#else
#define USBD_CC$x_IF0_MS_EXT_PROP1_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP1_NAME) + 4U)
#endif
#else
#error Custom Class $x Interface 0 Extended Properties OS Feature Descriptor Custom Property Section 1 Data Type is not allowed!
#endif
#else
#define USBD_CC$x_IF0_MS_EXT_PROP1_LEN      (0U)
#endif

#if    (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP2_EN != 0)
#if   ((USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP2_DATA_TYP == 1) || (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP2_DATA_TYP == 2) || (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP2_DATA_TYP == 6) )
#ifdef  USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP2_NAME_RAW
#define USBD_CC$x_IF0_MS_EXT_PROP2_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP2_NAME_RAW)) + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP2_DATA_STR_RAW)))
#else
#define USBD_CC$x_IF0_MS_EXT_PROP2_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP2_NAME) + sizeof(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP2_DATA_STR))
#endif
#elif ((USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP2_DATA_TYP == 4) || (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP2_DATA_TYP == 5) )
#ifdef  USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP2_NAME_RAW
#define USBD_CC$x_IF0_MS_EXT_PROP2_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP2_NAME_RAW)) + 4U)
#else
#define USBD_CC$x_IF0_MS_EXT_PROP2_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP2_NAME) + 4U)
#endif
#else
#error Custom Class $x Interface 0 Extended Properties OS Feature Descriptor Custom Property Section 2 Data Type is not allowed!
#endif
#else
#define USBD_CC$x_IF0_MS_EXT_PROP2_LEN      (0U)
#endif

#if    (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP3_EN != 0)
#if   ((USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP3_DATA_TYP == 1) || (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP3_DATA_TYP == 2) || (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP3_DATA_TYP == 6) )
#ifdef  USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP3_NAME_RAW
#define USBD_CC$x_IF0_MS_EXT_PROP3_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP3_NAME_RAW)) + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP3_DATA_STR_RAW)))
#else
#define USBD_CC$x_IF0_MS_EXT_PROP3_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP3_NAME) + sizeof(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP3_DATA_STR))
#endif
#elif ((USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP3_DATA_TYP == 4) || (USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP3_DATA_TYP == 5) )
#ifdef  USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP3_NAME_RAW
#define USBD_CC$x_IF0_MS_EXT_PROP3_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP3_NAME_RAW)) + 4U)
#else
#define USBD_CC$x_IF0_MS_EXT_PROP3_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF0_OS_EXT_PROP3_NAME) + 4U)
#endif
#else
#error Custom Class $x Interface 0 Extended Properties OS Feature Descriptor Custom Property Section 3 Data Type is not allowed!
#endif
#else
#define USBD_CC$x_IF0_MS_EXT_PROP3_LEN      (0U)
#endif

#define USBD_CC$x_IF0_MS_EXT_PROP_TOT_LEN   (USBD_CC$x_IF0_MS_EXT_PROP0_LEN + \
                                            USBD_CC$x_IF0_MS_EXT_PROP1_LEN + \
                                            USBD_CC$x_IF0_MS_EXT_PROP2_LEN + \
                                            USBD_CC$x_IF0_MS_EXT_PROP3_LEN )


// USB Device Custom Class $x Interface 1 Microsoft Extended Properties OS Feature Descriptor calculations
#define USBD_CC$x_IF1_MS_EXT_PROP_CNT      ((USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP0_EN != 0) + \
                                           (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP1_EN != 0) + \
                                           (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP2_EN != 0) + \
                                           (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP3_EN != 0) )

#if    (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP0_EN != 0)
#if   ((USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP0_DATA_TYP == 1) || (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP0_DATA_TYP == 2) || (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP0_DATA_TYP == 6) )
#ifdef  USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP0_NAME_RAW
#define USBD_CC$x_IF1_MS_EXT_PROP0_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP0_NAME_RAW)) + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP0_DATA_STR_RAW)))
#else
#define USBD_CC$x_IF1_MS_EXT_PROP0_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP0_NAME) + sizeof(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP0_DATA_STR))
#endif
#elif ((USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP0_DATA_TYP == 4) || (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP0_DATA_TYP == 5) )
#ifdef  USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP0_NAME_RAW
#define USBD_CC$x_IF1_MS_EXT_PROP0_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP0_NAME_RAW)) + 4U)
#else
#define USBD_CC$x_IF1_MS_EXT_PROP0_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP0_NAME) + 4U)
#endif
#else
#error Custom Class $x Interface 1 Extended Properties OS Feature Descriptor Custom Property Section 0 Data Type is not allowed!
#endif
#else
#define USBD_CC$x_IF1_MS_EXT_PROP0_LEN      (0U)
#endif

#if    (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP1_EN != 0)
#if   ((USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP1_DATA_TYP == 1) || (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP1_DATA_TYP == 2) || (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP1_DATA_TYP == 6) )
#ifdef  USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP1_NAME_RAW
#define USBD_CC$x_IF1_MS_EXT_PROP1_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP1_NAME_RAW)) + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP1_DATA_STR_RAW)))
#else
#define USBD_CC$x_IF1_MS_EXT_PROP1_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP1_NAME) + sizeof(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP1_DATA_STR))
#endif
#elif ((USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP1_DATA_TYP == 4) || (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP1_DATA_TYP == 5) )
#ifdef  USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP1_NAME_RAW
#define USBD_CC$x_IF1_MS_EXT_PROP1_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP1_NAME_RAW)) + 4U)
#else
#define USBD_CC$x_IF1_MS_EXT_PROP1_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP1_NAME) + 4U)
#endif
#else
#error Custom Class $x Interface 1 Extended Properties OS Feature Descriptor Custom Property Section 1 Data Type is not allowed!
#endif
#else
#define USBD_CC$x_IF1_MS_EXT_PROP1_LEN      (0U)
#endif

#if    (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP2_EN != 0)
#if   ((USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP2_DATA_TYP == 1) || (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP2_DATA_TYP == 2) || (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP2_DATA_TYP == 6) )
#ifdef  USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP2_NAME_RAW
#define USBD_CC$x_IF1_MS_EXT_PROP2_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP2_NAME_RAW)) + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP2_DATA_STR_RAW)))
#else
#define USBD_CC$x_IF1_MS_EXT_PROP2_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP2_NAME) + sizeof(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP2_DATA_STR))
#endif
#elif ((USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP2_DATA_TYP == 4) || (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP2_DATA_TYP == 5) )
#ifdef  USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP2_NAME_RAW
#define USBD_CC$x_IF1_MS_EXT_PROP2_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP2_NAME_RAW)) + 4U)
#else
#define USBD_CC$x_IF1_MS_EXT_PROP2_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP2_NAME) + 4U)
#endif
#else
#error Custom Class $x Interface 1 Extended Properties OS Feature Descriptor Custom Property Section 2 Data Type is not allowed!
#endif
#else
#define USBD_CC$x_IF1_MS_EXT_PROP2_LEN      (0U)
#endif

#if    (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP3_EN != 0)
#if   ((USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP3_DATA_TYP == 1) || (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP3_DATA_TYP == 2) || (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP3_DATA_TYP == 6) )
#ifdef  USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP3_NAME_RAW
#define USBD_CC$x_IF1_MS_EXT_PROP3_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP3_NAME_RAW)) + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP3_DATA_STR_RAW)))
#else
#define USBD_CC$x_IF1_MS_EXT_PROP3_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP3_NAME) + sizeof(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP3_DATA_STR))
#endif
#elif ((USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP3_DATA_TYP == 4) || (USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP3_DATA_TYP == 5) )
#ifdef  USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP3_NAME_RAW
#define USBD_CC$x_IF1_MS_EXT_PROP3_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP3_NAME_RAW)) + 4U)
#else
#define USBD_CC$x_IF1_MS_EXT_PROP3_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF1_OS_EXT_PROP3_NAME) + 4U)
#endif
#else
#error Custom Class $x Interface 1 Extended Properties OS Feature Descriptor Custom Property Section 3 Data Type is not allowed!
#endif
#else
#define USBD_CC$x_IF1_MS_EXT_PROP3_LEN      (0U)
#endif

#define USBD_CC$x_IF1_MS_EXT_PROP_TOT_LEN   (USBD_CC$x_IF1_MS_EXT_PROP0_LEN + \
                                            USBD_CC$x_IF1_MS_EXT_PROP1_LEN + \
                                            USBD_CC$x_IF1_MS_EXT_PROP2_LEN + \
                                            USBD_CC$x_IF1_MS_EXT_PROP3_LEN )


// USB Device Custom Class $x Interface 2 Microsoft Extended Properties OS Feature Descriptor calculations
#define USBD_CC$x_IF2_MS_EXT_PROP_CNT      ((USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP0_EN != 0) + \
                                           (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP1_EN != 0) + \
                                           (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP2_EN != 0) + \
                                           (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP3_EN != 0) )

#if    (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP0_EN != 0)
#if   ((USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP0_DATA_TYP == 1) || (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP0_DATA_TYP == 2) || (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP0_DATA_TYP == 6) )
#ifdef  USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP0_NAME_RAW
#define USBD_CC$x_IF2_MS_EXT_PROP0_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP0_NAME_RAW)) + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP0_DATA_STR_RAW)))
#else
#define USBD_CC$x_IF2_MS_EXT_PROP0_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP0_NAME) + sizeof(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP0_DATA_STR))
#endif
#elif ((USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP0_DATA_TYP == 4) || (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP0_DATA_TYP == 5) )
#ifdef  USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP0_NAME_RAW
#define USBD_CC$x_IF2_MS_EXT_PROP0_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP0_NAME_RAW)) + 4U)
#else
#define USBD_CC$x_IF2_MS_EXT_PROP0_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP0_NAME) + 4U)
#endif
#else
#error Custom Class $x Interface 2 Extended Properties OS Feature Descriptor Custom Property Section 0 Data Type is not allowed!
#endif
#else
#define USBD_CC$x_IF2_MS_EXT_PROP0_LEN      (0U)
#endif

#if    (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP1_EN != 0)
#if   ((USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP1_DATA_TYP == 1) || (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP1_DATA_TYP == 2) || (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP1_DATA_TYP == 6) )
#ifdef  USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP1_NAME_RAW
#define USBD_CC$x_IF2_MS_EXT_PROP1_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP1_NAME_RAW)) + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP1_DATA_STR_RAW)))
#else
#define USBD_CC$x_IF2_MS_EXT_PROP1_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP1_NAME) + sizeof(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP1_DATA_STR))
#endif
#elif ((USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP1_DATA_TYP == 4) || (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP1_DATA_TYP == 5) )
#ifdef  USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP1_NAME_RAW
#define USBD_CC$x_IF2_MS_EXT_PROP1_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP1_NAME_RAW)) + 4U)
#else
#define USBD_CC$x_IF2_MS_EXT_PROP1_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP1_NAME) + 4U)
#endif
#else
#error Custom Class $x Interface 2 Extended Properties OS Feature Descriptor Custom Property Section 1 Data Type is not allowed!
#endif
#else
#define USBD_CC$x_IF2_MS_EXT_PROP1_LEN      (0U)
#endif

#if    (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP2_EN != 0)
#if   ((USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP2_DATA_TYP == 1) || (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP2_DATA_TYP == 2) || (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP2_DATA_TYP == 6) )
#ifdef  USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP2_NAME_RAW
#define USBD_CC$x_IF2_MS_EXT_PROP2_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP2_NAME_RAW)) + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP2_DATA_STR_RAW)))
#else
#define USBD_CC$x_IF2_MS_EXT_PROP2_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP2_NAME) + sizeof(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP2_DATA_STR))
#endif
#elif ((USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP2_DATA_TYP == 4) || (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP2_DATA_TYP == 5) )
#ifdef  USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP2_NAME_RAW
#define USBD_CC$x_IF2_MS_EXT_PROP2_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP2_NAME_RAW)) + 4U)
#else
#define USBD_CC$x_IF2_MS_EXT_PROP2_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP2_NAME) + 4U)
#endif
#else
#error Custom Class $x Interface 2 Extended Properties OS Feature Descriptor Custom Property Section 2 Data Type is not allowed!
#endif
#else
#define USBD_CC$x_IF2_MS_EXT_PROP2_LEN      (0U)
#endif

#if    (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP3_EN != 0)
#if   ((USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP3_DATA_TYP == 1) || (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP3_DATA_TYP == 2) || (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP3_DATA_TYP == 6) )
#ifdef  USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP3_NAME_RAW
#define USBD_CC$x_IF2_MS_EXT_PROP3_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP3_NAME_RAW)) + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP3_DATA_STR_RAW)))
#else
#define USBD_CC$x_IF2_MS_EXT_PROP3_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP3_NAME) + sizeof(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP3_DATA_STR))
#endif
#elif ((USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP3_DATA_TYP == 4) || (USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP3_DATA_TYP == 5) )
#ifdef  USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP3_NAME_RAW
#define USBD_CC$x_IF2_MS_EXT_PROP3_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP3_NAME_RAW)) + 4U)
#else
#define USBD_CC$x_IF2_MS_EXT_PROP3_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF2_OS_EXT_PROP3_NAME) + 4U)
#endif
#else
#error Custom Class $x Interface 2 Extended Properties OS Feature Descriptor Custom Property Section 3 Data Type is not allowed!
#endif
#else
#define USBD_CC$x_IF2_MS_EXT_PROP3_LEN      (0U)
#endif

#define USBD_CC$x_IF2_MS_EXT_PROP_TOT_LEN   (USBD_CC$x_IF2_MS_EXT_PROP0_LEN + \
                                            USBD_CC$x_IF2_MS_EXT_PROP1_LEN + \
                                            USBD_CC$x_IF2_MS_EXT_PROP2_LEN + \
                                            USBD_CC$x_IF2_MS_EXT_PROP3_LEN )


// USB Device Custom Class $x Interface 3 Microsoft Extended Properties OS Feature Descriptor calculations
#define USBD_CC$x_IF3_MS_EXT_PROP_CNT      ((USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP0_EN != 0) + \
                                           (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP1_EN != 0) + \
                                           (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP2_EN != 0) + \
                                           (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP3_EN != 0) )

#if    (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP0_EN != 0)
#if   ((USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP0_DATA_TYP == 1) || (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP0_DATA_TYP == 2) || (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP0_DATA_TYP == 6) )
#ifdef  USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP0_NAME_RAW
#define USBD_CC$x_IF3_MS_EXT_PROP0_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP0_NAME_RAW)) + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP0_DATA_STR_RAW)))
#else
#define USBD_CC$x_IF3_MS_EXT_PROP0_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP0_NAME) + sizeof(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP0_DATA_STR))
#endif
#elif ((USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP0_DATA_TYP == 4) || (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP0_DATA_TYP == 5) )
#ifdef  USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP0_NAME_RAW
#define USBD_CC$x_IF3_MS_EXT_PROP0_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP0_NAME_RAW)) + 4U)
#else
#define USBD_CC$x_IF3_MS_EXT_PROP0_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP0_NAME) + 4U)
#endif
#else
#error Custom Class $x Interface 3 Extended Properties OS Feature Descriptor Custom Property Section 0 Data Type is not allowed!
#endif
#else
#define USBD_CC$x_IF3_MS_EXT_PROP0_LEN      (0U)
#endif

#if    (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP1_EN != 0)
#if   ((USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP1_DATA_TYP == 1) || (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP1_DATA_TYP == 2) || (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP1_DATA_TYP == 6) )
#ifdef  USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP1_NAME_RAW
#define USBD_CC$x_IF3_MS_EXT_PROP1_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP1_NAME_RAW)) + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP1_DATA_STR_RAW)))
#else
#define USBD_CC$x_IF3_MS_EXT_PROP1_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP1_NAME) + sizeof(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP1_DATA_STR))
#endif
#elif ((USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP1_DATA_TYP == 4) || (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP1_DATA_TYP == 5) )
#ifdef  USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP1_NAME_RAW
#define USBD_CC$x_IF3_MS_EXT_PROP1_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP1_NAME_RAW)) + 4U)
#else
#define USBD_CC$x_IF3_MS_EXT_PROP1_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP1_NAME) + 4U)
#endif
#else
#error Custom Class $x Interface 3 Extended Properties OS Feature Descriptor Custom Property Section 1 Data Type is not allowed!
#endif
#else
#define USBD_CC$x_IF3_MS_EXT_PROP1_LEN      (0U)
#endif

#if    (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP2_EN != 0)
#if   ((USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP2_DATA_TYP == 1) || (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP2_DATA_TYP == 2) || (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP2_DATA_TYP == 6) )
#ifdef  USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP2_NAME_RAW
#define USBD_CC$x_IF3_MS_EXT_PROP2_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP2_NAME_RAW)) + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP2_DATA_STR_RAW)))
#else
#define USBD_CC$x_IF3_MS_EXT_PROP2_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP2_NAME) + sizeof(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP2_DATA_STR))
#endif
#elif ((USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP2_DATA_TYP == 4) || (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP2_DATA_TYP == 5) )
#ifdef  USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP2_NAME_RAW
#define USBD_CC$x_IF3_MS_EXT_PROP2_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP2_NAME_RAW)) + 4U)
#else
#define USBD_CC$x_IF3_MS_EXT_PROP2_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP2_NAME) + 4U)
#endif
#else
#error Custom Class $x Interface 3 Extended Properties OS Feature Descriptor Custom Property Section 2 Data Type is not allowed!
#endif
#else
#define USBD_CC$x_IF3_MS_EXT_PROP2_LEN      (0U)
#endif

#if    (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP3_EN != 0)
#if   ((USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP3_DATA_TYP == 1) || (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP3_DATA_TYP == 2) || (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP3_DATA_TYP == 6) )
#ifdef  USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP3_NAME_RAW
#define USBD_CC$x_IF3_MS_EXT_PROP3_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP3_NAME_RAW)) + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP3_DATA_STR_RAW)))
#else
#define USBD_CC$x_IF3_MS_EXT_PROP3_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP3_NAME) + sizeof(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP3_DATA_STR))
#endif
#elif ((USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP3_DATA_TYP == 4) || (USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP3_DATA_TYP == 5) )
#ifdef  USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP3_NAME_RAW
#define USBD_CC$x_IF3_MS_EXT_PROP3_LEN      (14U + sizeof(USB_STR_RAW_TO_UTF16(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP3_NAME_RAW)) + 4U)
#else
#define USBD_CC$x_IF3_MS_EXT_PROP3_LEN      (14U + sizeof(USBD_CUSTOM_CLASS$x_IF3_OS_EXT_PROP3_NAME) + 4U)
#endif
#else
#error Custom Class $x Interface 3 Extended Properties OS Feature Descriptor Custom Property Section 3 Data Type is not allowed!
#endif
#else
#define USBD_CC$x_IF3_MS_EXT_PROP3_LEN      (0U)
#endif

#define USBD_CC$x_IF3_MS_EXT_PROP_TOT_LEN   (USBD_CC$x_IF3_MS_EXT_PROP0_LEN + \
                                            USBD_CC$x_IF3_MS_EXT_PROP1_LEN + \
                                            USBD_CC$x_IF3_MS_EXT_PROP2_LEN + \
                                            USBD_CC$x_IF3_MS_EXT_PROP3_LEN )
