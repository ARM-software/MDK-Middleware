/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_config_desc_hid_2.c
 * Purpose: USB Device HID 2 Specific Descriptor Creation
 *----------------------------------------------------------------------------*/

/* USB Device HID 2 Report Descriptor */
#if (USBD_HID2_USER_REPORT_DESCRIPTOR == 0)
  extern const uint8_t usbd_hid2_report_descriptor[];
  __WEAK const uint8_t usbd_hid2_report_descriptor[] __ALIGNED(4) = {
    HID_UsagePageVendor( 0x00                         ),
    HID_Usage          ( 0x01                         ),
    HID_Collection     ( HID_Application              ),
      HID_LogicalMin   ( 0                            ), /* value range: 0 - 0xFF */
      HID_LogicalMaxS  ( 0xFF                         ),
      HID_ReportSize   ( 8                            ), /* 8 bits */
  #if (USBD_HID2_IN_REPORT_MAX_SZ > 255)
      HID_ReportCountS ( USBD_HID2_IN_REPORT_MAX_SZ   ),
  #else
      HID_ReportCount  ( USBD_HID2_IN_REPORT_MAX_SZ   ),
  #endif
      HID_Usage        ( 0x01                         ),
      HID_Input        ( HID_Data | HID_Variable | HID_Absolute ),
  #if (USBD_HID2_OUT_REPORT_MAX_SZ > 255)
      HID_ReportCountS ( USBD_HID2_OUT_REPORT_MAX_SZ  ),
  #else
      HID_ReportCount  ( USBD_HID2_OUT_REPORT_MAX_SZ  ),
  #endif
      HID_Usage        ( 0x01                         ),
      HID_Output       ( HID_Data | HID_Variable | HID_Absolute ),
  #if (USBD_HID2_FEAT_REPORT_MAX_SZ > 255)
      HID_ReportCountS ( USBD_HID2_FEAT_REPORT_MAX_SZ ),
  #else
      HID_ReportCount  ( USBD_HID2_FEAT_REPORT_MAX_SZ ),
  #endif
      HID_Usage        ( 0x01                         ),
      HID_Feature      ( HID_Data | HID_Variable | HID_Absolute ),
    HID_EndCollection,
  };

  #define USBD_HID2_REPORT_DESC_SIZE    (sizeof(usbd_hid2_report_descriptor))
#else
  extern const uint8_t usbd_hid2_report_descriptor[];

  #define USBD_HID2_REPORT_DESC_SIZE    (USBD_HID2_USER_REPORT_DESCRIPTOR_SIZE)
#endif

#if     defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-qual"
#endif

extern 
usbd_hid_desc_t usbd_hid2_desc;
usbd_hid_desc_t usbd_hid2_desc = {      (uint8_t *)usbd_hid2_report_descriptor, USBD_HID2_REPORT_DESC_SIZE, USBD_HID2_DESC_OFS };

#if     defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang diagnostic pop
#endif
