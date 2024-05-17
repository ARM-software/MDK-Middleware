/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_config_desc_1.c
 * Purpose: USB Device 1 Descriptor Creation
 *----------------------------------------------------------------------------*/

/* USB Device 1 Endpoint 0 Descriptor */
extern const uint8_t usbd1_ep0_descriptor[];
__WEAK const uint8_t usbd1_ep0_descriptor[] __ALIGNED(4) = { USBD_EP0(1) };

/* USB Device 1 Standard Descriptor */
extern const uint8_t usbd1_device_descriptor[];
__WEAK const uint8_t usbd1_device_descriptor[] __ALIGNED(4) = {
  USB_DEVICE_DESC_SIZE,                 /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,           /* bDescriptorType */
  WBVAL(USBD1_BDC_USB),                 /* bcdUSB */
#if (USBD1_USE_IAD)
  USB_DEVICE_CLASS_MISCELLANEOUS,       /* bDeviceClass    (use from Interface Association Descriptors) */
  0x02,                                 /* bDeviceSubClass (use from Interface Association Descriptors) */
  0x01,                                 /* bDeviceProtocol (use from Interface Association Descriptors) */
#elif (((USBD1_CDC_CNT == 1U) && (USBD1_CDC_ACM_CNT == 1U)) || \
       ((USBD1_CDC_CNT == 1U) && (USBD1_CDC_NCM_CNT == 1U)))
  CDC_COMMUNICATION_INTERFACE_CLASS,    /* bDeviceClass */
  0x00,                                 /* bDeviceSubClass */
  0x00,                                 /* bDeviceProtocol */
#else
  0x00,                                 /* bDeviceClass    (use from Interface Descriptors) */
  0x00,                                 /* bDeviceSubClass (use from Interface Descriptors) */
  0x00,                                 /* bDeviceProtocol (use from Interface Descriptors) */
#endif
  USBD1_MAX_PACKET0,                    /* bMaxPacketSize0 */
  WBVAL(USBD1_DEV_DESC_IDVENDOR),       /* idVendor */
  WBVAL(USBD1_DEV_DESC_IDPRODUCT),      /* idProduct */
  WBVAL(USBD1_DEV_DESC_BCDDEVICE),      /* bcdDevice */
  0x01,                                 /* iManufacturer */
  0x02,                                 /* iProduct */
  0x03*USBD1_STR_DESC_SER_EN,           /* iSerialNumber */
  0x01                                  /* bNumConfigurations: one possible configuration */
};

/* USB Device 1 Configuration Descriptor (for Full Speed) */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor) */
extern const uint8_t usbd1_config_descriptor_fs[];
__WEAK const uint8_t usbd1_config_descriptor_fs[] __ALIGNED(4) = {
  /* Configuration 1 */
  USB_CONFIGURATION_DESC_SIZE,          /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE,    /* bDescriptorType */
  WBVAL(USBD1_WTOTALLENGTH),            /* wTotalLength */
  USBD1_IF_CNT,                         /* bNumInterfaces */
  0x01,                                 /* bConfigurationValue: 0x01 is used to select this configuration */
  0x00,                                 /* iConfiguration: no string to describe this configuration */
  USBD1_CFG_DESC_BMATTRIBUTES,          /* bmAttributes */
  USBD1_CFG_DESC_BMAXPOWER,             /* bMaxPower, device power consumption */

#if (USBD1_CUSTOM_CLASS0)
#if (USBD_CUSTOM_CLASS0_IAD_EN)
  USBD_CUSTOM_CLASSn_IAD                          (0)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (0,0)
#if (USBD_CUSTOM_CLASS0_IF0_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,0,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,1,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,2,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,3,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,4,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,5,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,6,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,7,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (0,1)
#if (USBD_CUSTOM_CLASS0_IF1_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,0,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,1,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,2,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,3,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,4,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,5,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,6,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,7,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (0,2)
#if (USBD_CUSTOM_CLASS0_IF2_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,0,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,1,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,2,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,3,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,4,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,5,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,6,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,7,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (0,3)
#if (USBD_CUSTOM_CLASS0_IF3_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,0,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,1,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,2,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,3,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,4,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,5,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,6,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,7,FS)
#endif
#endif
#endif

#if (USBD1_CUSTOM_CLASS1)
#if (USBD_CUSTOM_CLASS1_IAD_EN)
  USBD_CUSTOM_CLASSn_IAD                          (1)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (1,0)
#if (USBD_CUSTOM_CLASS1_IF0_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,0,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,1,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,2,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,3,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,4,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,5,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,6,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,7,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (1,1)
#if (USBD_CUSTOM_CLASS1_IF1_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,0,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,1,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,2,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,3,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,4,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,5,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,6,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,7,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (1,2)
#if (USBD_CUSTOM_CLASS1_IF2_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,0,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,1,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,2,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,3,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,4,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,5,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,6,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,7,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (1,3)
#if (USBD_CUSTOM_CLASS1_IF3_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,0,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,1,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,2,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,3,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,4,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,5,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,6,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,7,FS)
#endif
#endif
#endif

#if (USBD1_CUSTOM_CLASS2)
#if (USBD_CUSTOM_CLASS2_IAD_EN)
  USBD_CUSTOM_CLASSn_IAD                          (2)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (2,0)
#if (USBD_CUSTOM_CLASS2_IF0_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,0,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,1,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,2,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,3,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,4,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,5,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,6,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,7,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (2,1)
#if (USBD_CUSTOM_CLASS2_IF1_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,0,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,1,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,2,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,3,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,4,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,5,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,6,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,7,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (2,2)
#if (USBD_CUSTOM_CLASS2_IF2_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,0,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,1,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,2,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,3,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,4,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,5,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,6,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,7,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (2,3)
#if (USBD_CUSTOM_CLASS2_IF3_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,0,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,1,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,2,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,3,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,4,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,5,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,6,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,7,FS)
#endif
#endif
#endif


#if (USBD1_CUSTOM_CLASS3)
#if (USBD_CUSTOM_CLASS3_IAD_EN)
  USBD_CUSTOM_CLASSn_IAD                          (3)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (3,0)
#if (USBD_CUSTOM_CLASS3_IF0_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,0,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,1,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,2,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,3,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,4,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,5,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,6,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,7,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (3,1)
#if (USBD_CUSTOM_CLASS3_IF1_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,0,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,1,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,2,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,3,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,4,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,5,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,6,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,7,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (3,2)
#if (USBD_CUSTOM_CLASS3_IF2_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,0,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,1,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,2,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,3,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,4,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,5,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,6,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,7,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (3,3)
#if (USBD_CUSTOM_CLASS3_IF3_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,0,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,1,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,2,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,3,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,4,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,5,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,6,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,7,FS)
#endif
#endif
#endif

#if (USBD1_ADC0)
#if (USBD1_USE_IAD)
  USBD_ADCn_IAD                                   (0,USBD_ADC0_IF_CTRL_NUM,USBD_ADC0_IF_CNT)
#endif
  USBD_ADCn_IF_DESC_AC                            (0)
#if   (USBD_ADC0_IF_SPKR && USBD_ADC0_IF_MIC)
  USBD_ADCn_IF_DESC_AC_HEADER                     (0,USBD_ADC0_OUT_CH_NUM,USBD_ADC0_IN_CH_NUM)
#elif (USBD_ADC0_IF_SPKR)
  USBD_ADCn_IF_DESC_AC_HEADER_SPEAKER             (0,USBD_ADC0_OUT_CH_NUM)
#elif (USBD_ADC0_IF_MIC)
  USBD_ADCn_IF_DESC_AC_HEADER_MICROPHONE          (0,USBD_ADC0_IN_CH_NUM)
#endif
#if   (USBD_ADC0_IF_SPKR)
#if   (USBD_ADC0_OUT_CH_NUM == 1)
  USBD_ADCn_IF_DESC_AC_IT_FU_OT_MONO_SPEAKER      (0,1)
#elif (USBD_ADC0_OUT_CH_NUM == 2)
  USBD_ADCn_IF_DESC_AC_IT_FU_OT_STEREO_SPEAKERS   (0,1)
#endif
#endif
#if   (USBD_ADC0_IF_MIC)
#if   (USBD_ADC0_IN_CH_NUM == 1)
  USBD_ADCn_IF_DESC_AC_IT_FU_OT_MONO_MICROPHONE   (0,1+USBD_ADC0_IF_SPKR*3)
#elif (USBD_ADC0_IN_CH_NUM == 2)
  USBD_ADCn_IF_DESC_AC_IT_FU_OT_STEREO_MICROPHONES(0,1+USBD_ADC0_IF_SPKR*3)
#endif
#endif
#if   (USBD_ADC0_IF_SPKR)
  USBD_ADCn_IF_DESC_AS_SPEAKER                    (0)
  USBD_ADCn_IF_DESC_AS_IF_FT_SPEAKER              (0,1,USBD_ADC0_OUT_CH_NUM)
  USBD_ADCn_EP_DESC_SPEAKER                       (0)
#endif
#if   (USBD_ADC0_IF_MIC)
  USBD_ADCn_IF_DESC_AS_MICROPHONE                 (0)
  USBD_ADCn_IF_DESC_AS_IF_FT_MICROPHONE           (0,3+USBD_ADC0_IF_SPKR*3,USBD_ADC0_IN_CH_NUM)
  USBD_ADCn_EP_DESC_MICROPHONE                    (0)
#endif
#endif
#if (USBD1_ADC1)
#if (USBD1_USE_IAD)
  USBD_ADCn_IAD                                   (1,USBD_ADC1_IF_CTRL_NUM,USBD_ADC1_IF_CNT)
#endif
  USBD_ADCn_IF_DESC_AC                            (1)
#if   (USBD_ADC1_IF_SPKR && USBD_ADC1_IF_MIC)
  USBD_ADCn_IF_DESC_AC_HEADER                     (1,USBD_ADC1_OUT_CH_NUM,USBD_ADC1_IN_CH_NUM)
#elif (USBD_ADC1_IF_SPKR)
  USBD_ADCn_IF_DESC_AC_HEADER_SPEAKER             (1,USBD_ADC1_OUT_CH_NUM)
#elif (USBD_ADC1_IF_MIC)
  USBD_ADCn_IF_DESC_AC_HEADER_MICROPHONE          (1,USBD_ADC1_IN_CH_NUM)
#endif
#if   (USBD_ADC1_IF_SPKR)
#if   (USBD_ADC1_OUT_CH_NUM == 1)
  USBD_ADCn_IF_DESC_AC_IT_FU_OT_MONO_SPEAKER      (1,1)
#elif (USBD_ADC1_OUT_CH_NUM == 2)
  USBD_ADCn_IF_DESC_AC_IT_FU_OT_STEREO_SPEAKERS   (1,1)
#endif
#endif
#if   (USBD_ADC1_IF_MIC)
#if   (USBD_ADC1_IN_CH_NUM == 1)
  USBD_ADCn_IF_DESC_AC_IT_FU_OT_MONO_MICROPHONE   (1,1+USBD_ADC1_IF_SPKR*3)
#elif (USBD_ADC1_IN_CH_NUM == 2)
  USBD_ADCn_IF_DESC_AC_IT_FU_OT_STEREO_MICROPHONES(1,1+USBD_ADC1_IF_SPKR*3)
#endif
#endif
#if   (USBD_ADC1_IF_SPKR)
  USBD_ADCn_IF_DESC_AS_SPEAKER                    (1)
  USBD_ADCn_IF_DESC_AS_IF_FT_SPEAKER              (1,1,USBD_ADC1_OUT_CH_NUM)
  USBD_ADCn_EP_DESC_SPEAKER                       (1)
#endif
#if   (USBD_ADC1_IF_MIC)
  USBD_ADCn_IF_DESC_AS_MICROPHONE                 (1)
  USBD_ADCn_IF_DESC_AS_IF_FT_MICROPHONE           (1,3+USBD_ADC1_IF_SPKR*3,USBD_ADC1_IN_CH_NUM)
  USBD_ADCn_EP_DESC_MICROPHONE                    (1)
#endif
#endif
#if (USBD1_ADC2)
#if (USBD1_USE_IAD)
  USBD_ADCn_IAD                                   (2,USBD_ADC2_IF_CTRL_NUM,USBD_ADC2_IF_CNT)
#endif
  USBD_ADCn_IF_DESC_AC                            (2)
#if   (USBD_ADC2_IF_SPKR && USBD_ADC2_IF_MIC)
  USBD_ADCn_IF_DESC_AC_HEADER                     (2,USBD_ADC2_OUT_CH_NUM,USBD_ADC2_IN_CH_NUM)
#elif (USBD_ADC2_IF_SPKR)
  USBD_ADCn_IF_DESC_AC_HEADER_SPEAKER             (2,USBD_ADC2_OUT_CH_NUM)
#elif (USBD_ADC2_IF_MIC)
  USBD_ADCn_IF_DESC_AC_HEADER_MICROPHONE          (2,USBD_ADC2_IN_CH_NUM)
#endif
#if   (USBD_ADC2_IF_SPKR)
#if   (USBD_ADC2_OUT_CH_NUM == 1)
  USBD_ADCn_IF_DESC_AC_IT_FU_OT_MONO_SPEAKER      (2,1)
#elif (USBD_ADC2_OUT_CH_NUM == 2)
  USBD_ADCn_IF_DESC_AC_IT_FU_OT_STEREO_SPEAKERS   (2,1)
#endif
#endif
#if   (USBD_ADC2_IF_MIC)
#if   (USBD_ADC2_IN_CH_NUM == 1)
  USBD_ADCn_IF_DESC_AC_IT_FU_OT_MONO_MICROPHONE   (2,1+USBD_ADC2_IF_SPKR*3)
#elif (USBD_ADC2_IN_CH_NUM == 2)
  USBD_ADCn_IF_DESC_AC_IT_FU_OT_STEREO_MICROPHONES(2,1+USBD_ADC2_IF_SPKR*3)
#endif
#endif
#if   (USBD_ADC2_IF_SPKR)
  USBD_ADCn_IF_DESC_AS_SPEAKER                    (2)
  USBD_ADCn_IF_DESC_AS_IF_FT_SPEAKER              (2,1,USBD_ADC2_OUT_CH_NUM)
  USBD_ADCn_EP_DESC_SPEAKER                       (2)
#endif
#if   (USBD_ADC2_IF_MIC)
  USBD_ADCn_IF_DESC_AS_MICROPHONE                 (2)
  USBD_ADCn_IF_DESC_AS_IF_FT_MICROPHONE           (2,3+USBD_ADC2_IF_SPKR*3,USBD_ADC2_IN_CH_NUM)
  USBD_ADCn_EP_DESC_MICROPHONE                    (2)
#endif
#endif
#if (USBD1_ADC3)
#if (USBD1_USE_IAD)
  USBD_ADCn_IAD                                   (3,USBD_ADC3_IF_CTRL_NUM,USBD_ADC3_IF_CNT)
#endif
  USBD_ADCn_IF_DESC_AC                            (3)
#if   (USBD_ADC3_IF_SPKR && USBD_ADC3_IF_MIC)
  USBD_ADCn_IF_DESC_AC_HEADER                     (3,USBD_ADC3_OUT_CH_NUM,USBD_ADC3_IN_CH_NUM)
#elif (USBD_ADC3_IF_SPKR)
  USBD_ADCn_IF_DESC_AC_HEADER_SPEAKER             (3,USBD_ADC3_OUT_CH_NUM)
#elif (USBD_ADC3_IF_MIC)
  USBD_ADCn_IF_DESC_AC_HEADER_MICROPHONE          (3,USBD_ADC3_IN_CH_NUM)
#endif
#if   (USBD_ADC3_IF_SPKR)
#if   (USBD_ADC3_OUT_CH_NUM == 1)
  USBD_ADCn_IF_DESC_AC_IT_FU_OT_MONO_SPEAKER      (3,1)
#elif (USBD_ADC3_OUT_CH_NUM == 2)
  USBD_ADCn_IF_DESC_AC_IT_FU_OT_STEREO_SPEAKERS   (3,1)
#endif
#endif
#if   (USBD_ADC3_IF_MIC)
#if   (USBD_ADC3_IN_CH_NUM == 1)
  USBD_ADCn_IF_DESC_AC_IT_FU_OT_MONO_MICROPHONE   (3,1+USBD_ADC3_IF_SPKR*3)
#elif (USBD_ADC3_IN_CH_NUM == 2)
  USBD_ADCn_IF_DESC_AC_IT_FU_OT_STEREO_MICROPHONES(3,1+USBD_ADC3_IF_SPKR*3)
#endif
#endif
#if   (USBD_ADC3_IF_SPKR)
  USBD_ADCn_IF_DESC_AS_SPEAKER                    (3)
  USBD_ADCn_IF_DESC_AS_IF_FT_SPEAKER              (3,1,USBD_ADC3_OUT_CH_NUM)
  USBD_ADCn_EP_DESC_SPEAKER                       (3)
#endif
#if   (USBD_ADC3_IF_MIC)
  USBD_ADCn_IF_DESC_AS_MICROPHONE                 (3)
  USBD_ADCn_IF_DESC_AS_IF_FT_MICROPHONE           (3,3+USBD_ADC3_IF_SPKR*3,USBD_ADC3_IN_CH_NUM)
  USBD_ADCn_EP_DESC_MICROPHONE                    (3)
#endif
#endif

#if   (USBD1_CDC0)
#if   (USBD1_CDC0_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC0_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (0,USBD_CDC0_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (0,USBD_CDC0_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (0)
  USBD_CDCn_ACM_EP_IF0                            (0)
  USBD_CDCn_ACM_DESC_IF1                          (0)
  USBD_CDCn_ACM_EP_IF1                            (0)
#elif (USBD1_CDC0_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (0,USBD_CDC0_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (0)
  USBD_CDCn_NCM_EP_IF0                            (0)
  USBD_CDCn_NCM_DESC_IF1                          (0)
  USBD_CDCn_NCM_EP_IF1                            (0)
#endif
#endif
#if   (USBD1_CDC1)
#if   (USBD1_CDC1_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC1_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (1,USBD_CDC1_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (1,USBD_CDC1_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (1)
  USBD_CDCn_ACM_EP_IF0                            (1)
  USBD_CDCn_ACM_DESC_IF1                          (1)
  USBD_CDCn_ACM_EP_IF1                            (1)
#elif (USBD1_CDC1_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (1,USBD_CDC1_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (1)
  USBD_CDCn_NCM_EP_IF0                            (1)
  USBD_CDCn_NCM_DESC_IF1                          (1)
  USBD_CDCn_NCM_EP_IF1                            (1)
#endif
#endif
#if   (USBD1_CDC2)
#if   (USBD1_CDC2_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC2_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (2,USBD_CDC2_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (2,USBD_CDC2_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (2)
  USBD_CDCn_ACM_EP_IF0                            (2)
  USBD_CDCn_ACM_DESC_IF1                          (2)
  USBD_CDCn_ACM_EP_IF1                            (2)
#elif (USBD1_CDC2_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (2,USBD_CDC2_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (2)
  USBD_CDCn_NCM_EP_IF0                            (2)
  USBD_CDCn_NCM_DESC_IF1                          (2)
  USBD_CDCn_NCM_EP_IF1                            (2)
#endif
#endif
#if   (USBD1_CDC3)
#if   (USBD1_CDC3_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC3_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (3,USBD_CDC3_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (3,USBD_CDC3_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (3)
  USBD_CDCn_ACM_EP_IF0                            (3)
  USBD_CDCn_ACM_DESC_IF1                          (3)
  USBD_CDCn_ACM_EP_IF1                            (3)
#elif (USBD1_CDC3_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (3,USBD_CDC3_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (3)
  USBD_CDCn_NCM_EP_IF0                            (3)
  USBD_CDCn_NCM_DESC_IF1                          (3)
  USBD_CDCn_NCM_EP_IF1                            (3)
#endif
#endif
#if   (USBD1_CDC4)
#if   (USBD1_CDC4_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC4_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (4,USBD_CDC4_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (4,USBD_CDC4_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (4)
  USBD_CDCn_ACM_EP_IF0                            (4)
  USBD_CDCn_ACM_DESC_IF1                          (4)
  USBD_CDCn_ACM_EP_IF1                            (4)
#elif (USBD1_CDC4_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (4,USBD_CDC4_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (4)
  USBD_CDCn_NCM_EP_IF0                            (4)
  USBD_CDCn_NCM_DESC_IF1                          (4)
  USBD_CDCn_NCM_EP_IF1                            (4)
#endif
#endif
#if   (USBD1_CDC5)
#if   (USBD1_CDC5_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC5_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (5,USBD_CDC5_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (5,USBD_CDC5_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (5)
  USBD_CDCn_ACM_EP_IF0                            (5)
  USBD_CDCn_ACM_DESC_IF1                          (5)
  USBD_CDCn_ACM_EP_IF1                            (5)
#elif (USBD1_CDC5_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (5,USBD_CDC5_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (5)
  USBD_CDCn_NCM_EP_IF0                            (5)
  USBD_CDCn_NCM_DESC_IF1                          (5)
  USBD_CDCn_NCM_EP_IF1                            (5)
#endif
#endif
#if   (USBD1_CDC6)
#if   (USBD1_CDC6_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC6_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (6,USBD_CDC6_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (6,USBD_CDC6_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (6)
  USBD_CDCn_ACM_EP_IF0                            (6)
  USBD_CDCn_ACM_DESC_IF1                          (6)
  USBD_CDCn_ACM_EP_IF1                            (6)
#elif (USBD1_CDC6_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (6,USBD_CDC6_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (6)
  USBD_CDCn_NCM_EP_IF0                            (6)
  USBD_CDCn_NCM_DESC_IF1                          (6)
  USBD_CDCn_NCM_EP_IF1                            (6)
#endif
#endif
#if   (USBD1_CDC7)
#if   (USBD1_CDC7_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC7_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (7,USBD_CDC7_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (7,USBD_CDC7_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (7)
  USBD_CDCn_ACM_EP_IF0                            (7)
  USBD_CDCn_ACM_DESC_IF1                          (7)
  USBD_CDCn_ACM_EP_IF1                            (7)
#elif (USBD1_CDC7_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (7,USBD_CDC7_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (7)
  USBD_CDCn_NCM_EP_IF0                            (7)
  USBD_CDCn_NCM_DESC_IF1                          (7)
  USBD_CDCn_NCM_EP_IF1                            (7)
#endif
#endif

#if (USBD1_HID0)
  USBD_HIDn_DESC                                  (0)
#if (USBD_HID0_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT                              (0)
#else
  USBD_HIDn_EP                                    (0)
#endif
#endif
#if (USBD1_HID1)
  USBD_HIDn_DESC                                  (1)
#if (USBD_HID1_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT                              (1)
#else
  USBD_HIDn_EP                                    (1)
#endif
#endif
#if (USBD1_HID2)
  USBD_HIDn_DESC                                  (2)
#if (USBD_HID2_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT                              (2)
#else
  USBD_HIDn_EP                                    (2)
#endif
#endif
#if (USBD1_HID3)
  USBD_HIDn_DESC                                  (3)
#if (USBD_HID3_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT                              (3)
#else
  USBD_HIDn_EP                                    (3)
#endif
#endif
#if (USBD1_HID4)
  USBD_HIDn_DESC                                  (4)
#if (USBD_HID4_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT                              (4)
#else
  USBD_HIDn_EP                                    (4)
#endif
#endif
#if (USBD1_HID5)
  USBD_HIDn_DESC                                  (5)
#if (USBD_HID5_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT                              (5)
#else
  USBD_HIDn_EP                                    (5)
#endif
#endif
#if (USBD1_HID6)
  USBD_HIDn_DESC                                  (6)
#if (USBD_HID6_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT                              (6)
#else
  USBD_HIDn_EP                                    (6)
#endif
#endif
#if (USBD1_HID7)
  USBD_HIDn_DESC                                  (7)
#if (USBD_HID7_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT                              (7)
#else
  USBD_HIDn_EP                                    (7)
#endif
#endif

#if (USBD1_MSC0)
  USBD_MSCn_DESC                                  (0)
  USBD_MSCn_EP                                    (0)
#endif
#if (USBD1_MSC1)
  USBD_MSCn_DESC                                  (1)
  USBD_MSCn_EP                                    (1)
#endif
#if (USBD1_MSC2)
  USBD_MSCn_DESC                                  (2)
  USBD_MSCn_EP                                    (2)
#endif
#if (USBD1_MSC3)
  USBD_MSCn_DESC                                  (3)
  USBD_MSCn_EP                                    (3)
#endif

/* Terminator */
  0                                     /* bLength */
};

#if (!USBD1_HS)                         /* If High-speed not enabled, declare dummy descriptors for High-speed */
extern const uint8_t usbd1_device_qualifier_fs[];
__WEAK const uint8_t usbd1_device_qualifier_fs[]              = { 0 };
extern const uint8_t usbd1_device_qualifier_hs[];
__WEAK const uint8_t usbd1_device_qualifier_hs[]              = { 0 };
extern const uint8_t usbd1_config_descriptor_hs[];
__WEAK const uint8_t usbd1_config_descriptor_hs[]             = { 0 };
extern const uint8_t usbd1_other_speed_config_descriptor_fs[];
__WEAK const uint8_t usbd1_other_speed_config_descriptor_fs[] = { 0 };
extern const uint8_t usbd1_other_speed_config_descriptor_hs[];
__WEAK const uint8_t usbd1_other_speed_config_descriptor_hs[] = { 0 };

#else

#if (USBD1_ADC0 || USBD1_ADC1 || USBD1_ADC2 || USBD1_ADC3)
#error High-speed is not supported for Audio Device!
#endif

/* USB Device 1 Qualifier Descriptor (in Full Speed for High Speed) */
extern const uint8_t usbd1_device_qualifier_fs[];
__WEAK const uint8_t usbd1_device_qualifier_fs[] __ALIGNED(4) = {
  USB_DEVICE_QUALI_SIZE,                /* bLength */
  USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(0x0200), /* 2.00 */             /* bcdUSB */
  0x00,                                 /* bDeviceClass */
  0x00,                                 /* bDeviceSubClass */
  0x00,                                 /* bDeviceProtocol */
  USBD1_MAX_PACKET0,                    /* bMaxPacketSize0 */
  0x01,                                 /* bNumConfigurations */
  0x00                                  /* bReserved */
};

/* USB Device 1 Qualifier Descriptor (in High Speed for Full Speed) */
extern const uint8_t usbd1_device_qualifier_hs[];
__WEAK const uint8_t usbd1_device_qualifier_hs[] __ALIGNED(4) = {
  USB_DEVICE_QUALI_SIZE,                /* bLength */
  USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(0x0200), /* 2.00 */             /* bcdUSB */
  0x00,                                 /* bDeviceClass */
  0x00,                                 /* bDeviceSubClass */
  0x00,                                 /* bDeviceProtocol */
  USBD1_MAX_PACKET0,                    /* bMaxPacketSize0 */
  0x01,                                 /* bNumConfigurations */
  0x00                                  /* bReserved */
};

/* USB Device 1 Configuration Descriptor (for High Speed) */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor) */
extern const uint8_t usbd1_config_descriptor_hs[];
__WEAK const uint8_t usbd1_config_descriptor_hs[] __ALIGNED(4) = {
  /* Configuration 1 */
  USB_CONFIGURATION_DESC_SIZE,          /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE,    /* bDescriptorType */
  WBVAL(USBD1_WTOTALLENGTH),            /* wTotalLength */
  USBD1_IF_CNT,                         /* bNumInterfaces */
  0x01,                                 /* bConfigurationValue: 0x01 is used to select this configuration */
  0x00,                                 /* iConfiguration: no string to describe this configuration */
  USBD1_CFG_DESC_BMATTRIBUTES,          /* bmAttributes */
  USBD1_CFG_DESC_BMAXPOWER,             /* bMaxPower, device power consumption */

#if (USBD1_CUSTOM_CLASS0)
#if (USBD_CUSTOM_CLASS0_IAD_EN)
  USBD_CUSTOM_CLASSn_IAD                          (0)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (0,0)
#if (USBD_CUSTOM_CLASS0_IF0_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,0,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,1,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,2,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,3,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,4,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,5,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,6,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,7,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (0,1)
#if (USBD_CUSTOM_CLASS0_IF1_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,0,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,1,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,2,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,3,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,4,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,5,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,6,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,7,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (0,2)
#if (USBD_CUSTOM_CLASS0_IF2_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,0,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,1,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,2,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,3,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,4,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,5,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,6,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,7,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (0,3)
#if (USBD_CUSTOM_CLASS0_IF3_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,0,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,1,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,2,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,3,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,4,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,5,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,6,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,7,HS)
#endif
#endif
#endif

#if (USBD1_CUSTOM_CLASS1)
#if (USBD_CUSTOM_CLASS1_IAD_EN)
  USBD_CUSTOM_CLASSn_IAD                          (1)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (1,0)
#if (USBD_CUSTOM_CLASS1_IF0_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,0,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,1,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,2,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,3,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,4,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,5,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,6,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,7,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (1,1)
#if (USBD_CUSTOM_CLASS1_IF1_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,0,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,1,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,2,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,3,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,4,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,5,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,6,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,7,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (1,2)
#if (USBD_CUSTOM_CLASS1_IF2_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,0,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,1,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,2,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,3,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,4,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,5,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,6,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,7,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (1,3)
#if (USBD_CUSTOM_CLASS1_IF3_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,0,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,1,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,2,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,3,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,4,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,5,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,6,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,7,HS)
#endif
#endif
#endif

#if (USBD1_CUSTOM_CLASS2)
#if (USBD_CUSTOM_CLASS2_IAD_EN)
  USBD_CUSTOM_CLASSn_IAD                          (2)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (2,0)
#if (USBD_CUSTOM_CLASS2_IF0_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,0,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,1,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,2,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,3,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,4,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,5,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,6,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,7,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (2,1)
#if (USBD_CUSTOM_CLASS2_IF1_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,0,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,1,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,2,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,3,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,4,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,5,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,6,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,7,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (2,2)
#if (USBD_CUSTOM_CLASS2_IF2_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,0,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,1,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,2,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,3,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,4,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,5,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,6,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,7,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (2,3)
#if (USBD_CUSTOM_CLASS2_IF3_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,0,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,1,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,2,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,3,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,4,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,5,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,6,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,7,HS)
#endif
#endif
#endif


#if (USBD1_CUSTOM_CLASS3)
#if (USBD_CUSTOM_CLASS3_IAD_EN)
  USBD_CUSTOM_CLASSn_IAD                          (3)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (3,0)
#if (USBD_CUSTOM_CLASS3_IF0_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,0,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,1,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,2,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,3,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,4,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,5,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,6,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,7,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (3,1)
#if (USBD_CUSTOM_CLASS3_IF1_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,0,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,1,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,2,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,3,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,4,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,5,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,6,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,7,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (3,2)
#if (USBD_CUSTOM_CLASS3_IF2_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,0,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,1,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,2,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,3,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,4,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,5,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,6,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,7,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (3,3)
#if (USBD_CUSTOM_CLASS3_IF3_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,0,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,1,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,2,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,3,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,4,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,5,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP6_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,6,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP7_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,7,HS)
#endif
#endif
#endif

#if   (USBD1_CDC0)
#if   (USBD1_CDC0_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC0_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (0,USBD_CDC0_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (0,USBD_CDC0_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (0)
  USBD_CDCn_ACM_EP_IF0_HS                         (0)
  USBD_CDCn_ACM_DESC_IF1                          (0)
  USBD_CDCn_ACM_EP_IF1_HS                         (0)
#elif (USBD1_CDC0_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (0,USBD_CDC0_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (0)
  USBD_CDCn_NCM_EP_IF0_HS                         (0)
  USBD_CDCn_NCM_DESC_IF1                          (0)
  USBD_CDCn_NCM_EP_IF1_HS                         (0)
#endif
#endif
#if   (USBD1_CDC1)
#if   (USBD1_CDC1_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC1_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (1,USBD_CDC1_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (1,USBD_CDC1_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (1)
  USBD_CDCn_ACM_EP_IF0_HS                         (1)
  USBD_CDCn_ACM_DESC_IF1                          (1)
  USBD_CDCn_ACM_EP_IF1_HS                         (1)
#elif (USBD1_CDC1_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (1,USBD_CDC1_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (1)
  USBD_CDCn_NCM_EP_IF0_HS                         (1)
  USBD_CDCn_NCM_DESC_IF1                          (1)
  USBD_CDCn_NCM_EP_IF1_HS                         (1)
#endif
#endif
#if   (USBD1_CDC2)
#if   (USBD1_CDC2_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC2_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (2,USBD_CDC2_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (2,USBD_CDC2_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (2)
  USBD_CDCn_ACM_EP_IF0_HS                         (2)
  USBD_CDCn_ACM_DESC_IF1                          (2)
  USBD_CDCn_ACM_EP_IF1_HS                         (2)
#elif (USBD1_CDC2_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (2,USBD_CDC2_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (2)
  USBD_CDCn_NCM_EP_IF0_HS                         (2)
  USBD_CDCn_NCM_DESC_IF1                          (2)
  USBD_CDCn_NCM_EP_IF1_HS                         (2)
#endif
#endif
#if   (USBD1_CDC3)
#if   (USBD1_CDC3_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC3_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (3,USBD_CDC3_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (3,USBD_CDC3_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (3)
  USBD_CDCn_ACM_EP_IF0_HS                         (3)
  USBD_CDCn_ACM_DESC_IF1                          (3)
  USBD_CDCn_ACM_EP_IF1_HS                         (3)
#elif (USBD1_CDC3_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (3,USBD_CDC3_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (3)
  USBD_CDCn_NCM_EP_IF0_HS                         (3)
  USBD_CDCn_NCM_DESC_IF1                          (3)
  USBD_CDCn_NCM_EP_IF1_HS                         (3)
#endif
#endif
#if   (USBD1_CDC4)
#if   (USBD1_CDC4_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC4_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (4,USBD_CDC4_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (4,USBD_CDC4_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (4)
  USBD_CDCn_ACM_EP_IF0_HS                         (4)
  USBD_CDCn_ACM_DESC_IF1                          (4)
  USBD_CDCn_ACM_EP_IF1_HS                         (4)
#elif (USBD1_CDC4_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (4,USBD_CDC4_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (4)
  USBD_CDCn_NCM_EP_IF0_HS                         (4)
  USBD_CDCn_NCM_DESC_IF1                          (4)
  USBD_CDCn_NCM_EP_IF1_HS                         (4)
#endif
#endif
#if   (USBD1_CDC5)
#if   (USBD1_CDC5_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC5_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (5,USBD_CDC5_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (5,USBD_CDC5_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (5)
  USBD_CDCn_ACM_EP_IF0_HS                         (5)
  USBD_CDCn_ACM_DESC_IF1                          (5)
  USBD_CDCn_ACM_EP_IF1_HS                         (5)
#elif (USBD1_CDC5_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (5,USBD_CDC5_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (5)
  USBD_CDCn_NCM_EP_IF0_HS                         (5)
  USBD_CDCn_NCM_DESC_IF1                          (5)
  USBD_CDCn_NCM_EP_IF1_HS                         (5)
#endif
#endif
#if   (USBD1_CDC6)
#if   (USBD1_CDC6_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC6_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (6,USBD_CDC6_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (6,USBD_CDC6_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (6)
  USBD_CDCn_ACM_EP_IF0_HS                         (6)
  USBD_CDCn_ACM_DESC_IF1                          (6)
  USBD_CDCn_ACM_EP_IF1_HS                         (6)
#elif (USBD1_CDC6_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (6,USBD_CDC6_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (6)
  USBD_CDCn_NCM_EP_IF0_HS                         (6)
  USBD_CDCn_NCM_DESC_IF1                          (6)
  USBD_CDCn_NCM_EP_IF1_HS                         (6)
#endif
#endif
#if   (USBD1_CDC7)
#if   (USBD1_CDC7_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC7_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (7,USBD_CDC7_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (7,USBD_CDC7_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (7)
  USBD_CDCn_ACM_EP_IF0_HS                         (7)
  USBD_CDCn_ACM_DESC_IF1                          (7)
  USBD_CDCn_ACM_EP_IF1_HS                         (7)
#elif (USBD1_CDC7_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (7,USBD_CDC7_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (7)
  USBD_CDCn_NCM_EP_IF0_HS                         (7)
  USBD_CDCn_NCM_DESC_IF1                          (7)
  USBD_CDCn_NCM_EP_IF1_HS                         (7)
#endif
#endif

#if (USBD1_HID0)
  USBD_HIDn_DESC                                  (0)
#if (USBD_HID0_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT_HS                           (0)
#else
  USBD_HIDn_EP_HS                                 (0)
#endif
#endif
#if (USBD1_HID1)
  USBD_HIDn_DESC                                  (1)
#if (USBD_HID1_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT_HS                           (1)
#else
  USBD_HIDn_EP_HS                                 (1)
#endif
#endif
#if (USBD1_HID2)
  USBD_HIDn_DESC                                  (2)
#if (USBD_HID2_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT_HS                           (2)
#else
  USBD_HIDn_EP_HS                                 (2)
#endif
#endif
#if (USBD1_HID3)
  USBD_HIDn_DESC                                  (3)
#if (USBD_HID3_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT_HS                           (3)
#else
  USBD_HIDn_EP_HS                                 (3)
#endif
#endif
#if (USBD1_HID4)
  USBD_HIDn_DESC                                  (4)
#if (USBD_HID4_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT_HS                           (4)
#else
  USBD_HIDn_EP_HS                                 (4)
#endif
#endif
#if (USBD1_HID5)
  USBD_HIDn_DESC                                  (5)
#if (USBD_HID5_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT_HS                           (5)
#else
  USBD_HIDn_EP_HS                                 (5)
#endif
#endif
#if (USBD1_HID6)
  USBD_HIDn_DESC                                  (6)
#if (USBD_HID6_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT_HS                           (6)
#else
  USBD_HIDn_EP_HS                                 (6)
#endif
#endif
#if (USBD1_HID7)
  USBD_HIDn_DESC                                  (7)
#if (USBD_HID7_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT_HS                           (7)
#else
  USBD_HIDn_EP_HS                                 (7)
#endif
#endif

#if (USBD1_MSC0)
  USBD_MSCn_DESC                                  (0)
  USBD_MSCn_EP_HS                                 (0)
#endif
#if (USBD1_MSC1)
  USBD_MSCn_DESC                                  (1)
  USBD_MSCn_EP_HS                                 (1)
#endif
#if (USBD1_MSC2)
  USBD_MSCn_DESC                                  (2)
  USBD_MSCn_EP_HS                                 (2)
#endif
#if (USBD1_MSC3)
  USBD_MSCn_DESC                                  (3)
  USBD_MSCn_EP_HS                                 (3)
#endif

/* Terminator */
  0                                     /* bLength */
};

/* USB Device 1 Other Speed Configuration Descriptor (in Full Speed for High Speed) */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor) */
extern const uint8_t usbd1_other_speed_config_descriptor_fs[];
__WEAK const uint8_t usbd1_other_speed_config_descriptor_fs[] __ALIGNED(4) = {
  /* Configuration 1 */
  USB_CONFIGURATION_DESC_SIZE,          /* bLength */
  USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(USBD1_WTOTALLENGTH),            /* wTotalLength */
  USBD1_IF_CNT,                         /* bNumInterfaces */
  0x01,                                 /* bConfigurationValue: 0x01 is used to select this configuration */
  0x00,                                 /* iConfiguration: no string to describe this configuration */
  USBD1_CFG_DESC_BMATTRIBUTES,          /* bmAttributes */
  USBD1_CFG_DESC_BMAXPOWER,             /* bMaxPower, device power consumption */

#if (USBD1_CUSTOM_CLASS0)
#if (USBD_CUSTOM_CLASS0_IAD_EN)
  USBD_CUSTOM_CLASSn_IAD                          (0)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (0,0)
#if (USBD_CUSTOM_CLASS0_IF0_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,0,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,1,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,2,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,3,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,4,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,5,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (0,1)
#if (USBD_CUSTOM_CLASS0_IF1_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,0,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,1,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,2,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,3,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,4,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,5,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (0,2)
#if (USBD_CUSTOM_CLASS0_IF2_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,0,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,1,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,2,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,3,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,4,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,5,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (0,3)
#if (USBD_CUSTOM_CLASS0_IF3_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,0,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,1,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,2,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,3,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,4,HS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,5,HS)
#endif
#endif
#endif

#if (USBD1_CUSTOM_CLASS1)
#if (USBD_CUSTOM_CLASS1_IAD_EN)
  USBD_CUSTOM_CLASSn_IAD                          (1)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (1,0)
#if (USBD_CUSTOM_CLASS1_IF0_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,0,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,1,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,2,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,3,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,4,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,5,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (1,1)
#if (USBD_CUSTOM_CLASS1_IF1_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,0,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,1,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,2,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,3,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,4,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,5,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (1,2)
#if (USBD_CUSTOM_CLASS1_IF2_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,0,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,1,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,2,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,3,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,4,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,5,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (1,3)
#if (USBD_CUSTOM_CLASS1_IF3_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,0,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,1,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,2,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,3,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,4,HS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,5,HS)
#endif
#endif
#endif

#if (USBD1_CUSTOM_CLASS2)
#if (USBD_CUSTOM_CLASS2_IAD_EN)
  USBD_CUSTOM_CLASSn_IAD                          (2)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (2,0)
#if (USBD_CUSTOM_CLASS2_IF0_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,0,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,1,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,2,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,3,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,4,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,5,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (2,1)
#if (USBD_CUSTOM_CLASS2_IF1_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,0,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,1,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,2,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,3,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,4,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,5,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (2,2)
#if (USBD_CUSTOM_CLASS2_IF2_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,0,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,1,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,2,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,3,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,4,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,5,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (2,3)
#if (USBD_CUSTOM_CLASS2_IF3_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,0,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,1,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,2,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,3,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,4,HS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,5,HS)
#endif
#endif
#endif


#if (USBD1_CUSTOM_CLASS3)
#if (USBD_CUSTOM_CLASS3_IAD_EN)
  USBD_CUSTOM_CLASSn_IAD                          (3)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (3,0)
#if (USBD_CUSTOM_CLASS3_IF0_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,0,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,1,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,2,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,3,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,4,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,5,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (3,1)
#if (USBD_CUSTOM_CLASS3_IF1_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,0,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,1,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,2,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,3,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,4,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,5,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (3,2)
#if (USBD_CUSTOM_CLASS3_IF2_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,0,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,1,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,2,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,3,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,4,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,5,HS)
#endif
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (3,3)
#if (USBD_CUSTOM_CLASS3_IF3_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,0,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,1,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,2,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,3,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,4,HS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,5,HS)
#endif
#endif
#endif

#if   (USBD1_CDC0)
#if   (USBD1_CDC0_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC0_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (0,USBD_CDC0_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (0,USBD_CDC0_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (0)
  USBD_CDCn_ACM_EP_IF0_HS                         (0)
  USBD_CDCn_ACM_DESC_IF1                          (0)
  USBD_CDCn_ACM_EP_IF1_HS                         (0)
#elif (USBD1_CDC0_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (0,USBD_CDC0_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (0)
  USBD_CDCn_NCM_EP_IF0_HS                         (0)
  USBD_CDCn_NCM_DESC_IF1                          (0)
  USBD_CDCn_NCM_EP_IF1_HS                         (0)
#endif
#endif
#if   (USBD1_CDC1)
#if   (USBD1_CDC1_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC1_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (1,USBD_CDC1_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (1,USBD_CDC1_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (1)
  USBD_CDCn_ACM_EP_IF0_HS                         (1)
  USBD_CDCn_ACM_DESC_IF1                          (1)
  USBD_CDCn_ACM_EP_IF1_HS                         (1)
#elif (USBD1_CDC1_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (1,USBD_CDC1_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (1)
  USBD_CDCn_NCM_EP_IF0_HS                         (1)
  USBD_CDCn_NCM_DESC_IF1                          (1)
  USBD_CDCn_NCM_EP_IF1_HS                         (1)
#endif
#endif
#if   (USBD1_CDC2)
#if   (USBD1_CDC2_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC2_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (2,USBD_CDC2_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (2,USBD_CDC2_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (2)
  USBD_CDCn_ACM_EP_IF0_HS                         (2)
  USBD_CDCn_ACM_DESC_IF1                          (2)
  USBD_CDCn_ACM_EP_IF1_HS                         (2)
#elif (USBD1_CDC2_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (2,USBD_CDC2_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (2)
  USBD_CDCn_NCM_EP_IF0_HS                         (2)
  USBD_CDCn_NCM_DESC_IF1                          (2)
  USBD_CDCn_NCM_EP_IF1_HS                         (2)
#endif
#endif
#if   (USBD1_CDC3)
#if   (USBD1_CDC3_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC3_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (3,USBD_CDC3_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (3,USBD_CDC3_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (3)
  USBD_CDCn_ACM_EP_IF0_HS                         (3)
  USBD_CDCn_ACM_DESC_IF1                          (3)
  USBD_CDCn_ACM_EP_IF1_HS                         (3)
#elif (USBD1_CDC3_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (3,USBD_CDC3_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (3)
  USBD_CDCn_NCM_EP_IF0_HS                         (3)
  USBD_CDCn_NCM_DESC_IF1                          (3)
  USBD_CDCn_NCM_EP_IF1_HS                         (3)
#endif
#endif
#if   (USBD1_CDC4)
#if   (USBD1_CDC4_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC4_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (4,USBD_CDC4_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (4,USBD_CDC4_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (4)
  USBD_CDCn_ACM_EP_IF0_HS                         (4)
  USBD_CDCn_ACM_DESC_IF1                          (4)
  USBD_CDCn_ACM_EP_IF1_HS                         (4)
#elif (USBD1_CDC4_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (4,USBD_CDC4_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (4)
  USBD_CDCn_NCM_EP_IF0_HS                         (4)
  USBD_CDCn_NCM_DESC_IF1                          (4)
  USBD_CDCn_NCM_EP_IF1_HS                         (4)
#endif
#endif
#if   (USBD1_CDC5)
#if   (USBD1_CDC5_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC5_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (5,USBD_CDC5_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (5,USBD_CDC5_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (5)
  USBD_CDCn_ACM_EP_IF0_HS                         (5)
  USBD_CDCn_ACM_DESC_IF1                          (5)
  USBD_CDCn_ACM_EP_IF1_HS                         (5)
#elif (USBD1_CDC5_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (5,USBD_CDC5_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (5)
  USBD_CDCn_NCM_EP_IF0_HS                         (5)
  USBD_CDCn_NCM_DESC_IF1                          (5)
  USBD_CDCn_NCM_EP_IF1_HS                         (5)
#endif
#endif
#if   (USBD1_CDC6)
#if   (USBD1_CDC6_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC6_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (6,USBD_CDC6_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (6,USBD_CDC6_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (6)
  USBD_CDCn_ACM_EP_IF0_HS                         (6)
  USBD_CDCn_ACM_DESC_IF1                          (6)
  USBD_CDCn_ACM_EP_IF1_HS                         (6)
#elif (USBD1_CDC6_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (6,USBD_CDC6_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (6)
  USBD_CDCn_NCM_EP_IF0_HS                         (6)
  USBD_CDCn_NCM_DESC_IF1                          (6)
  USBD_CDCn_NCM_EP_IF1_HS                         (6)
#endif
#endif
#if   (USBD1_CDC7)
#if   (USBD1_CDC7_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC7_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (7,USBD_CDC7_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (7,USBD_CDC7_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (7)
  USBD_CDCn_ACM_EP_IF0_HS                         (7)
  USBD_CDCn_ACM_DESC_IF1                          (7)
  USBD_CDCn_ACM_EP_IF1_HS                         (7)
#elif (USBD1_CDC7_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (7,USBD_CDC7_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (7)
  USBD_CDCn_NCM_EP_IF0_HS                         (7)
  USBD_CDCn_NCM_DESC_IF1                          (7)
  USBD_CDCn_NCM_EP_IF1_HS                         (7)
#endif
#endif

#if (USBD1_HID0)
  USBD_HIDn_DESC                                  (0)
#if (USBD_HID0_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT_HS                           (0)
#else
  USBD_HIDn_EP_HS                                 (0)
#endif
#endif
#if (USBD1_HID1)
  USBD_HIDn_DESC                                  (1)
#if (USBD_HID1_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT_HS                           (1)
#else
  USBD_HIDn_EP_HS                                 (1)
#endif
#endif
#if (USBD1_HID2)
  USBD_HIDn_DESC                                  (2)
#if (USBD_HID2_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT_HS                           (2)
#else
  USBD_HIDn_EP_HS                                 (2)
#endif
#endif
#if (USBD1_HID3)
  USBD_HIDn_DESC                                  (3)
#if (USBD_HID3_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT_HS                           (3)
#else
  USBD_HIDn_EP_HS                                 (3)
#endif
#endif
#if (USBD1_HID4)
  USBD_HIDn_DESC                                  (4)
#if (USBD_HID4_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT_HS                           (4)
#else
  USBD_HIDn_EP_HS                                 (4)
#endif
#endif
#if (USBD1_HID5)
  USBD_HIDn_DESC                                  (5)
#if (USBD_HID5_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT_HS                           (5)
#else
  USBD_HIDn_EP_HS                                 (5)
#endif
#endif
#if (USBD1_HID6)
  USBD_HIDn_DESC                                  (6)
#if (USBD_HID6_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT_HS                           (6)
#else
  USBD_HIDn_EP_HS                                 (6)
#endif
#endif
#if (USBD1_HID7)
  USBD_HIDn_DESC                                  (7)
#if (USBD_HID7_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT_HS                           (7)
#else
  USBD_HIDn_EP_HS                                 (7)
#endif
#endif

#if (USBD1_MSC0)
  USBD_MSCn_DESC                                  (0)
  USBD_MSCn_EP_HS                                 (0)
#endif
#if (USBD1_MSC1)
  USBD_MSCn_DESC                                  (1)
  USBD_MSCn_EP_HS                                 (1)
#endif
#if (USBD1_MSC2)
  USBD_MSCn_DESC                                  (2)
  USBD_MSCn_EP_HS                                 (2)
#endif
#if (USBD1_MSC3)
  USBD_MSCn_DESC                                  (3)
  USBD_MSCn_EP_HS                                 (3)
#endif

/* Terminator */
  0                                     /* bLength */
};

/* USB Device 1 Other Speed Configuration Descriptor (in High Speed for Full Speed) */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor) */
extern const uint8_t usbd1_other_speed_config_descriptor_hs[];
__WEAK const uint8_t usbd1_other_speed_config_descriptor_hs[] __ALIGNED(4) = {
  /* Configuration 1 */
  USB_CONFIGURATION_DESC_SIZE,          /* bLength */
  USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(USBD1_WTOTALLENGTH),            /* wTotalLength */
  USBD1_IF_CNT,                         /* bNumInterfaces */
  0x01,                                 /* bConfigurationValue: 0x01 is used to select this configuration */
  0x00,                                 /* iConfiguration: no string to describe this configuration */
  USBD1_CFG_DESC_BMATTRIBUTES,          /* bmAttributes */
  USBD1_CFG_DESC_BMAXPOWER,             /* bMaxPower, device power consumption */

#if (USBD1_CUSTOM_CLASS0)
#if (USBD_CUSTOM_CLASS0_IAD_EN)
  USBD_CUSTOM_CLASSn_IAD                          (0)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (0,0)
#if (USBD_CUSTOM_CLASS0_IF0_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,0,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,1,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,2,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,3,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,4,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF0_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,0,5,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (0,1)
#if (USBD_CUSTOM_CLASS0_IF1_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,0,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,1,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,2,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,3,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,4,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF1_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,1,5,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (0,2)
#if (USBD_CUSTOM_CLASS0_IF2_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,0,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,1,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,2,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,3,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,4,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF2_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,2,5,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (0,3)
#if (USBD_CUSTOM_CLASS0_IF3_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,0,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,1,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,2,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,3,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,4,FS)
#endif
#if (USBD_CUSTOM_CLASS0_IF3_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (0,3,5,FS)
#endif
#endif
#endif

#if (USBD1_CUSTOM_CLASS1)
#if (USBD_CUSTOM_CLASS1_IAD_EN)
  USBD_CUSTOM_CLASSn_IAD                          (1)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (1,0)
#if (USBD_CUSTOM_CLASS1_IF0_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,0,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,1,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,2,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,3,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,4,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF0_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,0,5,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (1,1)
#if (USBD_CUSTOM_CLASS1_IF1_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,0,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,1,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,2,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,3,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,4,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF1_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,1,5,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (1,2)
#if (USBD_CUSTOM_CLASS1_IF2_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,0,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,1,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,2,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,3,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,4,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF2_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,2,5,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (1,3)
#if (USBD_CUSTOM_CLASS1_IF3_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,0,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,1,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,2,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,3,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,4,FS)
#endif
#if (USBD_CUSTOM_CLASS1_IF3_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (1,3,5,FS)
#endif
#endif
#endif

#if (USBD1_CUSTOM_CLASS2)
#if (USBD_CUSTOM_CLASS2_IAD_EN)
  USBD_CUSTOM_CLASSn_IAD                          (2)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (2,0)
#if (USBD_CUSTOM_CLASS2_IF0_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,0,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,1,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,2,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,3,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,4,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF0_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,0,5,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (2,1)
#if (USBD_CUSTOM_CLASS2_IF1_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,0,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,1,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,2,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,3,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,4,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF1_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,1,5,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (2,2)
#if (USBD_CUSTOM_CLASS2_IF2_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,0,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,1,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,2,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,3,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,4,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF2_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,2,5,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (2,3)
#if (USBD_CUSTOM_CLASS2_IF3_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,0,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,1,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,2,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,3,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,4,FS)
#endif
#if (USBD_CUSTOM_CLASS2_IF3_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (2,3,5,FS)
#endif
#endif
#endif


#if (USBD1_CUSTOM_CLASS3)
#if (USBD_CUSTOM_CLASS3_IAD_EN)
  USBD_CUSTOM_CLASSn_IAD                          (3)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (3,0)
#if (USBD_CUSTOM_CLASS3_IF0_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,0,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,1,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,2,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,3,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,4,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF0_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,0,5,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (3,1)
#if (USBD_CUSTOM_CLASS3_IF1_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,0,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,1,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,2,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,3,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,4,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF1_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,1,5,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (3,2)
#if (USBD_CUSTOM_CLASS3_IF2_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,0,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,1,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,2,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,3,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,4,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF2_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,2,5,FS)
#endif
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EN)
  USBD_CUSTOM_CLASSn_IFx_DESC                     (3,3)
#if (USBD_CUSTOM_CLASS3_IF3_EP0_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,0,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP1_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,1,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP2_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,2,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP3_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,3,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP4_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,4,FS)
#endif
#if (USBD_CUSTOM_CLASS3_IF3_EP5_EN)
  USBD_CUSTOM_CLASSn_IFx_EP                       (3,3,5,FS)
#endif
#endif
#endif

#if   (USBD1_CDC0)
#if   (USBD1_CDC0_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC0_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (0,USBD_CDC0_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (0,USBD_CDC0_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (0)
  USBD_CDCn_ACM_EP_IF0                            (0)
  USBD_CDCn_ACM_DESC_IF1                          (0)
  USBD_CDCn_ACM_EP_IF1                            (0)
#elif (USBD1_CDC0_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (0,USBD_CDC0_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (0)
  USBD_CDCn_NCM_EP_IF0                            (0)
  USBD_CDCn_NCM_DESC_IF1                          (0)
  USBD_CDCn_NCM_EP_IF1                            (0)
#endif
#endif
#if   (USBD1_CDC1)
#if   (USBD1_CDC1_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC1_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (1,USBD_CDC1_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (1,USBD_CDC1_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (1)
  USBD_CDCn_ACM_EP_IF0                            (1)
  USBD_CDCn_ACM_DESC_IF1                          (1)
  USBD_CDCn_ACM_EP_IF1                            (1)
#elif (USBD1_CDC1_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (1,USBD_CDC1_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (1)
  USBD_CDCn_NCM_EP_IF0                            (1)
  USBD_CDCn_NCM_DESC_IF1                          (1)
  USBD_CDCn_NCM_EP_IF1                            (1)
#endif
#endif
#if   (USBD1_CDC2)
#if   (USBD1_CDC2_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC2_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (2,USBD_CDC2_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (2,USBD_CDC2_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (2)
  USBD_CDCn_ACM_EP_IF0                            (2)
  USBD_CDCn_ACM_DESC_IF1                          (2)
  USBD_CDCn_ACM_EP_IF1                            (2)
#elif (USBD1_CDC2_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (2,USBD_CDC2_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (2)
  USBD_CDCn_NCM_EP_IF0                            (2)
  USBD_CDCn_NCM_DESC_IF1                          (2)
  USBD_CDCn_NCM_EP_IF1                            (2)
#endif
#endif
#if   (USBD1_CDC3)
#if   (USBD1_CDC3_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC3_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (3,USBD_CDC3_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (3,USBD_CDC3_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (3)
  USBD_CDCn_ACM_EP_IF0                            (3)
  USBD_CDCn_ACM_DESC_IF1                          (3)
  USBD_CDCn_ACM_EP_IF1                            (3)
#elif (USBD1_CDC3_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (3,USBD_CDC3_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (3)
  USBD_CDCn_NCM_EP_IF0                            (3)
  USBD_CDCn_NCM_DESC_IF1                          (3)
  USBD_CDCn_NCM_EP_IF1                            (3)
#endif
#endif
#if   (USBD1_CDC4)
#if   (USBD1_CDC4_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC4_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (4,USBD_CDC4_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (4,USBD_CDC4_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (4)
  USBD_CDCn_ACM_EP_IF0                            (4)
  USBD_CDCn_ACM_DESC_IF1                          (4)
  USBD_CDCn_ACM_EP_IF1                            (4)
#elif (USBD1_CDC4_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (4,USBD_CDC4_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (4)
  USBD_CDCn_NCM_EP_IF0                            (4)
  USBD_CDCn_NCM_DESC_IF1                          (4)
  USBD_CDCn_NCM_EP_IF1                            (4)
#endif
#endif
#if   (USBD1_CDC5)
#if   (USBD1_CDC5_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC5_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (5,USBD_CDC5_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (5,USBD_CDC5_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (5)
  USBD_CDCn_ACM_EP_IF0                            (5)
  USBD_CDCn_ACM_DESC_IF1                          (5)
  USBD_CDCn_ACM_EP_IF1                            (5)
#elif (USBD1_CDC5_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (5,USBD_CDC5_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (5)
  USBD_CDCn_NCM_EP_IF0                            (5)
  USBD_CDCn_NCM_DESC_IF1                          (5)
  USBD_CDCn_NCM_EP_IF1                            (5)
#endif
#endif
#if   (USBD1_CDC6)
#if   (USBD1_CDC6_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC6_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (6,USBD_CDC6_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (6,USBD_CDC6_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (6)
  USBD_CDCn_ACM_EP_IF0                            (6)
  USBD_CDCn_ACM_DESC_IF1                          (6)
  USBD_CDCn_ACM_EP_IF1                            (6)
#elif (USBD1_CDC6_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (6,USBD_CDC6_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (6)
  USBD_CDCn_NCM_EP_IF0                            (6)
  USBD_CDCn_NCM_DESC_IF1                          (6)
  USBD_CDCn_NCM_EP_IF1                            (6)
#endif
#endif
#if   (USBD1_CDC7)
#if   (USBD1_CDC7_ACM)
#if   (USBD1_USE_IAD)
#if   (USBD1_CDC7_RNDIS)
  USBD_CDCn_ACM_RNDIS_IAD                         (7,USBD_CDC7_IF0_NUM,2)
#else
  USBD_CDCn_ACM_IAD                               (7,USBD_CDC7_IF0_NUM,2)
#endif
#endif
  USBD_CDCn_ACM_DESC_IF0                          (7)
  USBD_CDCn_ACM_EP_IF0                            (7)
  USBD_CDCn_ACM_DESC_IF1                          (7)
  USBD_CDCn_ACM_EP_IF1                            (7)
#elif (USBD1_CDC7_NCM)
#if   (USBD1_USE_IAD)
  USBD_CDCn_NCM_IAD                               (7,USBD_CDC7_IF0_NUM,2)
#endif
  USBD_CDCn_NCM_DESC_IF0                          (7)
  USBD_CDCn_NCM_EP_IF0                            (7)
  USBD_CDCn_NCM_DESC_IF1                          (7)
  USBD_CDCn_NCM_EP_IF1                            (7)
#endif
#endif

#if (USBD1_HID0)
  USBD_HIDn_DESC                                  (0)
#if (USBD_HID0_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT                              (0)
#else
  USBD_HIDn_EP                                    (0)
#endif
#endif
#if (USBD1_HID1)
  USBD_HIDn_DESC                                  (1)
#if (USBD_HID1_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT                              (1)
#else
  USBD_HIDn_EP                                    (1)
#endif
#endif
#if (USBD1_HID2)
  USBD_HIDn_DESC                                  (2)
#if (USBD_HID2_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT                              (2)
#else
  USBD_HIDn_EP                                    (2)
#endif
#endif
#if (USBD1_HID3)
  USBD_HIDn_DESC                                  (3)
#if (USBD_HID3_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT                              (3)
#else
  USBD_HIDn_EP                                    (3)
#endif
#endif
#if (USBD1_HID4)
  USBD_HIDn_DESC                                  (4)
#if (USBD_HID4_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT                              (4)
#else
  USBD_HIDn_EP                                    (4)
#endif
#endif
#if (USBD1_HID5)
  USBD_HIDn_DESC                                  (5)
#if (USBD_HID5_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT                              (5)
#else
  USBD_HIDn_EP                                    (5)
#endif
#endif
#if (USBD1_HID6)
  USBD_HIDn_DESC                                  (6)
#if (USBD_HID6_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT                              (6)
#else
  USBD_HIDn_EP                                    (6)
#endif
#endif
#if (USBD1_HID7)
  USBD_HIDn_DESC                                  (7)
#if (USBD_HID7_EP_INT_OUT != 0)
  USBD_HIDn_EP_INOUT                              (7)
#else
  USBD_HIDn_EP                                    (7)
#endif
#endif

#if (USBD1_MSC0)
  USBD_MSCn_DESC                                  (0)
  USBD_MSCn_EP                                    (0)
#endif
#if (USBD1_MSC1)
  USBD_MSCn_DESC                                  (1)
  USBD_MSCn_EP                                    (1)
#endif
#if (USBD1_MSC2)
  USBD_MSCn_DESC                                  (2)
  USBD_MSCn_EP                                    (2)
#endif
#if (USBD1_MSC3)
  USBD_MSCn_DESC                                  (3)
  USBD_MSCn_EP                                    (3)
#endif

/* Terminator */
  0                                     /* bLength */
};

#endif

typedef const __PACKED_STRUCT {
  uint8_t  len;
  uint8_t  type;
  uint16_t langid;
#ifdef USBD1_STR_DESC_MAN_RAW
  USBD_STR_DEF_u(0,  USBD1_STR_DESC_MAN_RAW);
#else
  USBD_STR_DEF  (0,  USBD1_STR_DESC_MAN);
#endif
#ifdef USBD1_STR_DESC_PROD_RAW
  USBD_STR_DEF_u(1,  USBD1_STR_DESC_PROD_RAW);
#else
  USBD_STR_DEF  (1,  USBD1_STR_DESC_PROD);
#endif
#if (USBD1_STR_DESC_SER_EN)
#ifdef USBD1_STR_DESC_SER_RAW
  USBD_STR_DEF_u(2,  USBD1_STR_DESC_SER_RAW);
#else
  USBD_STR_DEF  (2,  USBD1_STR_DESC_SER);
#endif
#endif

#if (USBD1_CUSTOM_CLASS0)
#if ((USBD_CUSTOM_CLASS0_IF0_EN != 0) && (USBD_CUSTOM_CLASS0_IF0_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS0_IF0_STR_RAW
  USBD_STR_DEF_u(3,  USBD_CUSTOM_CLASS0_IF0_STR_RAW);
#else
  USBD_STR_DEF  (3,  USBD_CUSTOM_CLASS0_IF0_STR);
#endif
#endif
#if ((USBD_CUSTOM_CLASS0_IF1_EN != 0) && (USBD_CUSTOM_CLASS0_IF1_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS0_IF1_STR_RAW
  USBD_STR_DEF_u(4,  USBD_CUSTOM_CLASS0_IF1_STR_RAW);
#else
  USBD_STR_DEF  (4,  USBD_CUSTOM_CLASS0_IF1_STR);
#endif
#endif
#if ((USBD_CUSTOM_CLASS0_IF2_EN != 0) && (USBD_CUSTOM_CLASS0_IF2_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS0_IF2_STR_RAW
  USBD_STR_DEF_u(5,  USBD_CUSTOM_CLASS0_IF2_STR_RAW);
#else
  USBD_STR_DEF  (5,  USBD_CUSTOM_CLASS0_IF2_STR);
#endif
#endif
#if ((USBD_CUSTOM_CLASS0_IF3_EN != 0) && (USBD_CUSTOM_CLASS0_IF3_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS0_IF3_STR_RAW
  USBD_STR_DEF_u(6,  USBD_CUSTOM_CLASS0_IF3_STR_RAW);
#else
  USBD_STR_DEF  (6,  USBD_CUSTOM_CLASS0_IF3_STR);
#endif
#endif
#endif
#if (USBD1_CUSTOM_CLASS1)
#if ((USBD_CUSTOM_CLASS1_IF0_EN != 0) && (USBD_CUSTOM_CLASS1_IF0_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS1_IF0_STR_RAW
  USBD_STR_DEF_u(7,  USBD_CUSTOM_CLASS1_IF0_STR_RAW);
#else
  USBD_STR_DEF  (7,  USBD_CUSTOM_CLASS1_IF0_STR);
#endif
#endif
#if ((USBD_CUSTOM_CLASS1_IF1_EN != 0) && (USBD_CUSTOM_CLASS1_IF1_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS1_IF1_STR_RAW
  USBD_STR_DEF_u(8,  USBD_CUSTOM_CLASS1_IF1_STR_RAW);
#else
  USBD_STR_DEF  (8,  USBD_CUSTOM_CLASS1_IF1_STR);
#endif
#endif
#if ((USBD_CUSTOM_CLASS1_IF2_EN != 0) && (USBD_CUSTOM_CLASS1_IF2_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS1_IF2_STR_RAW
  USBD_STR_DEF_u(9,  USBD_CUSTOM_CLASS1_IF2_STR_RAW);
#else
  USBD_STR_DEF  (9,  USBD_CUSTOM_CLASS1_IF2_STR);
#endif
#endif
#if ((USBD_CUSTOM_CLASS1_IF3_EN != 0) && (USBD_CUSTOM_CLASS1_IF3_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS1_IF3_STR_RAW
  USBD_STR_DEF_u(10, USBD_CUSTOM_CLASS1_IF3_STR_RAW);
#else
  USBD_STR_DEF  (10, USBD_CUSTOM_CLASS1_IF3_STR);
#endif
#endif
#endif
#if (USBD1_CUSTOM_CLASS2)
#if ((USBD_CUSTOM_CLASS2_IF0_EN != 0) && (USBD_CUSTOM_CLASS2_IF0_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS2_IF0_STR_RAW
  USBD_STR_DEF_u(11, USBD_CUSTOM_CLASS2_IF0_STR_RAW);
#else
  USBD_STR_DEF  (11, USBD_CUSTOM_CLASS2_IF0_STR);
#endif
#endif
#if ((USBD_CUSTOM_CLASS2_IF1_EN != 0) && (USBD_CUSTOM_CLASS2_IF1_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS2_IF1_STR_RAW
  USBD_STR_DEF_u(12, USBD_CUSTOM_CLASS2_IF1_STR_RAW);
#else
  USBD_STR_DEF  (12, USBD_CUSTOM_CLASS2_IF1_STR);
#endif
#endif
#if ((USBD_CUSTOM_CLASS2_IF2_EN != 0) && (USBD_CUSTOM_CLASS2_IF2_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS2_IF2_STR_RAW
  USBD_STR_DEF_u(13, USBD_CUSTOM_CLASS2_IF2_STR_RAW);
#else
  USBD_STR_DEF  (13, USBD_CUSTOM_CLASS2_IF2_STR);
#endif
#endif
#if ((USBD_CUSTOM_CLASS2_IF3_EN != 0) && (USBD_CUSTOM_CLASS2_IF3_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS2_IF3_STR_RAW
  USBD_STR_DEF_u(14, USBD_CUSTOM_CLASS2_IF3_STR_RAW);
#else
  USBD_STR_DEF  (14, USBD_CUSTOM_CLASS2_IF3_STR);
#endif
#endif
#endif
#if (USBD1_CUSTOM_CLASS3)
#if ((USBD_CUSTOM_CLASS3_IF0_EN != 0) && (USBD_CUSTOM_CLASS3_IF0_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS3_IF0_STR_RAW
  USBD_STR_DEF_u(15, USBD_CUSTOM_CLASS3_IF0_STR_RAW);
#else
  USBD_STR_DEF  (15, USBD_CUSTOM_CLASS3_IF0_STR);
#endif
#endif
#if ((USBD_CUSTOM_CLASS3_IF1_EN != 0) && (USBD_CUSTOM_CLASS3_IF1_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS3_IF1_STR_RAW
  USBD_STR_DEF_u(16, USBD_CUSTOM_CLASS3_IF1_STR_RAW);
#else
  USBD_STR_DEF  (16, USBD_CUSTOM_CLASS3_IF1_STR);
#endif
#endif
#if ((USBD_CUSTOM_CLASS3_IF2_EN != 0) && (USBD_CUSTOM_CLASS3_IF2_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS3_IF2_STR_RAW
  USBD_STR_DEF_u(17, USBD_CUSTOM_CLASS3_IF2_STR_RAW);
#else
  USBD_STR_DEF  (17, USBD_CUSTOM_CLASS3_IF2_STR);
#endif
#endif
#if ((USBD_CUSTOM_CLASS3_IF3_EN != 0) && (USBD_CUSTOM_CLASS3_IF3_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS3_IF3_STR_RAW
  USBD_STR_DEF_u(18, USBD_CUSTOM_CLASS3_IF3_STR_RAW);
#else
  USBD_STR_DEF  (18, USBD_CUSTOM_CLASS3_IF3_STR);
#endif
#endif
#endif

#if (USBD1_ADC0)
#ifdef USBD_ADC0_CIF_STR_DESC_RAW
  USBD_STR_DEF_u(19, USBD_ADC0_CIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (19, USBD_ADC0_CIF_STR_DESC);
#endif
#if (USBD_ADC0_IF_SPKR)
#ifdef USBD_ADC0_OUT_SIF_STR0_DESC_RAW
  USBD_STR_DEF_u(20, USBD_ADC0_OUT_SIF_STR0_DESC_RAW);
#else
  USBD_STR_DEF  (20, USBD_ADC0_OUT_SIF_STR0_DESC);
#endif
#ifdef USBD_ADC0_OUT_SIF_STR1_DESC_RAW
  USBD_STR_DEF_u(21, USBD_ADC0_OUT_SIF_STR1_DESC_RAW);
#else
  USBD_STR_DEF  (21, USBD_ADC0_OUT_SIF_STR1_DESC);
#endif
#endif
#if (USBD_ADC0_IF_MIC)
#ifdef USBD_ADC0_IN_SIF_STR0_DESC_RAW
  USBD_STR_DEF_u(22, USBD_ADC0_IN_SIF_STR0_DESC_RAW);
#else
  USBD_STR_DEF  (22, USBD_ADC0_IN_SIF_STR0_DESC);
#endif
#ifdef USBD_ADC0_IN_SIF_STR1_DESC_RAW
  USBD_STR_DEF_u(23, USBD_ADC0_IN_SIF_STR1_DESC_RAW);
#else
  USBD_STR_DEF  (23, USBD_ADC0_IN_SIF_STR1_DESC);
#endif
#endif
#endif
#if (USBD1_ADC1)
#ifdef USBD_ADC1_CIF_STR_DESC_RAW
  USBD_STR_DEF_u(24, USBD_ADC1_CIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (24, USBD_ADC1_CIF_STR_DESC);
#endif
#if (USBD_ADC1_IF_SPKR)
#ifdef USBD_ADC1_OUT_SIF_STR0_DESC_RAW
  USBD_STR_DEF_u(25, USBD_ADC1_OUT_SIF_STR0_DESC_RAW);
#else
  USBD_STR_DEF  (25, USBD_ADC1_OUT_SIF_STR0_DESC);
#endif
#ifdef USBD_ADC1_OUT_SIF_STR1_DESC_RAW
  USBD_STR_DEF_u(26, USBD_ADC1_OUT_SIF_STR1_DESC_RAW);
#else
  USBD_STR_DEF  (26, USBD_ADC1_OUT_SIF_STR1_DESC);
#endif
#endif
#if (USBD_ADC1_IF_MIC)
#ifdef USBD_ADC1_IN_SIF_STR0_DESC_RAW
  USBD_STR_DEF_u(27, USBD_ADC1_IN_SIF_STR0_DESC_RAW);
#else
  USBD_STR_DEF  (27, USBD_ADC1_IN_SIF_STR0_DESC);
#endif
#ifdef USBD_ADC1_IN_SIF_STR1_DESC_RAW
  USBD_STR_DEF_u(28, USBD_ADC1_IN_SIF_STR1_DESC_RAW);
#else
  USBD_STR_DEF  (28, USBD_ADC1_IN_SIF_STR1_DESC);
#endif
#endif
#endif
#if (USBD1_ADC2)
#ifdef USBD_ADC2_CIF_STR_DESC_RAW
  USBD_STR_DEF_u(29, USBD_ADC2_CIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (29, USBD_ADC2_CIF_STR_DESC);
#endif
#if (USBD_ADC2_IF_SPKR)
#ifdef USBD_ADC2_OUT_SIF_STR0_DESC_RAW
  USBD_STR_DEF_u(30, USBD_ADC2_OUT_SIF_STR0_DESC_RAW);
#else
  USBD_STR_DEF  (30, USBD_ADC2_OUT_SIF_STR0_DESC);
#endif
#ifdef USBD_ADC2_OUT_SIF_STR1_DESC_RAW
  USBD_STR_DEF_u(31, USBD_ADC2_OUT_SIF_STR1_DESC_RAW);
#else
  USBD_STR_DEF  (31, USBD_ADC2_OUT_SIF_STR1_DESC);
#endif
#endif
#if (USBD_ADC2_IF_MIC)
#ifdef USBD_ADC2_IN_SIF_STR0_DESC_RAW
  USBD_STR_DEF_u(32, USBD_ADC2_IN_SIF_STR0_DESC_RAW);
#else
  USBD_STR_DEF  (32, USBD_ADC2_IN_SIF_STR0_DESC);
#endif
#ifdef USBD_ADC2_IN_SIF_STR1_DESC_RAW
  USBD_STR_DEF_u(33, USBD_ADC2_IN_SIF_STR1_DESC_RAW);
#else
  USBD_STR_DEF  (33, USBD_ADC2_IN_SIF_STR1_DESC);
#endif
#endif
#endif
#if (USBD1_ADC3)
#ifdef USBD_ADC3_CIF_STR_DESC_RAW
  USBD_STR_DEF_u(34, USBD_ADC3_CIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (34, USBD_ADC3_CIF_STR_DESC);
#endif
#if (USBD_ADC3_IF_SPKR)
#ifdef USBD_ADC3_OUT_SIF_STR0_DESC_RAW
  USBD_STR_DEF_u(35, USBD_ADC3_OUT_SIF_STR0_DESC_RAW);
#else
  USBD_STR_DEF  (35, USBD_ADC3_OUT_SIF_STR0_DESC);
#endif
#ifdef USBD_ADC3_OUT_SIF_STR1_DESC_RAW
  USBD_STR_DEF_u(36, USBD_ADC3_OUT_SIF_STR1_DESC_RAW);
#else
  USBD_STR_DEF  (36, USBD_ADC3_OUT_SIF_STR1_DESC);
#endif
#endif
#if (USBD_ADC3_IF_MIC)
#ifdef USBD_ADC3_IN_SIF_STR0_DESC_RAW
  USBD_STR_DEF_u(37, USBD_ADC3_IN_SIF_STR0_DESC_RAW);
#else
  USBD_STR_DEF  (37, USBD_ADC3_IN_SIF_STR0_DESC);
#endif
#ifdef USBD_ADC3_IN_SIF_STR1_DESC_RAW
  USBD_STR_DEF_u(38, USBD_ADC3_IN_SIF_STR1_DESC_RAW);
#else
  USBD_STR_DEF  (38, USBD_ADC3_IN_SIF_STR1_DESC);
#endif
#endif
#endif

#if (USBD1_CDC0)
#ifdef USBD_CDC0_CIF_STR_DESC_RAW
  USBD_STR_DEF_u(39, USBD_CDC0_CIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (39, USBD_CDC0_CIF_STR_DESC);
#endif
#ifdef USBD_CDC0_DIF_STR_DESC_RAW
  USBD_STR_DEF_u(40, USBD_CDC0_DIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (40, USBD_CDC0_DIF_STR_DESC);
#endif
#if (USBD1_CDC0_NCM)
#ifdef USBD_CDC0_NCM_MAC_ADDRESS_RAW
  USBD_STR_DEF_u(41, USBD_CDC0_NCM_MAC_ADDRESS_RAW);
#else
  USBD_STR_DEF  (41, USBD_CDC0_NCM_MAC_ADDRESS);
#endif
#endif
#endif
#if (USBD1_CDC1)
#ifdef USBD_CDC1_CIF_STR_DESC_RAW
  USBD_STR_DEF_u(42, USBD_CDC1_CIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (42, USBD_CDC1_CIF_STR_DESC);
#endif
#ifdef USBD_CDC1_DIF_STR_DESC_RAW
  USBD_STR_DEF_u(43, USBD_CDC1_DIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (43, USBD_CDC1_DIF_STR_DESC);
#endif
#if (USBD1_CDC1_NCM)
#ifdef USBD_CDC1_NCM_MAC_ADDRESS_RAW
  USBD_STR_DEF_u(44, USBD_CDC1_NCM_MAC_ADDRESS_RAW);
#else
  USBD_STR_DEF  (44, USBD_CDC1_NCM_MAC_ADDRESS);
#endif
#endif
#endif
#if (USBD1_CDC2)
#ifdef USBD_CDC2_CIF_STR_DESC_RAW
  USBD_STR_DEF_u(45, USBD_CDC2_CIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (45, USBD_CDC2_CIF_STR_DESC);
#endif
#ifdef USBD_CDC2_DIF_STR_DESC_RAW
  USBD_STR_DEF_u(46, USBD_CDC2_DIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (46, USBD_CDC2_DIF_STR_DESC);
#endif
#if (USBD1_CDC2_NCM)
#ifdef USBD_CDC2_NCM_MAC_ADDRESS_RAW
  USBD_STR_DEF_u(47, USBD_CDC2_NCM_MAC_ADDRESS_RAW);
#else
  USBD_STR_DEF  (47, USBD_CDC2_NCM_MAC_ADDRESS);
#endif
#endif
#endif
#if (USBD1_CDC3)
#ifdef USBD_CDC3_CIF_STR_DESC_RAW
  USBD_STR_DEF_u(48, USBD_CDC3_CIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (48, USBD_CDC3_CIF_STR_DESC);
#endif
#ifdef USBD_CDC3_DIF_STR_DESC_RAW
  USBD_STR_DEF_u(49, USBD_CDC3_DIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (49, USBD_CDC3_DIF_STR_DESC);
#endif
#if (USBD1_CDC3_NCM)
#ifdef USBD_CDC3_NCM_MAC_ADDRESS_RAW
  USBD_STR_DEF_u(50, USBD_CDC3_NCM_MAC_ADDRESS_RAW);
#else
  USBD_STR_DEF  (50, USBD_CDC3_NCM_MAC_ADDRESS);
#endif
#endif
#endif
#if (USBD1_CDC4)
#ifdef USBD_CDC4_CIF_STR_DESC_RAW
  USBD_STR_DEF_u(51, USBD_CDC4_CIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (51, USBD_CDC4_CIF_STR_DESC);
#endif
#ifdef USBD_CDC4_DIF_STR_DESC_RAW
  USBD_STR_DEF_u(52, USBD_CDC4_DIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (52, USBD_CDC4_DIF_STR_DESC);
#endif
#if (USBD1_CDC4_NCM)
#ifdef USBD_CDC4_NCM_MAC_ADDRESS_RAW
  USBD_STR_DEF_u(53, USBD_CDC4_NCM_MAC_ADDRESS_RAW);
#else
  USBD_STR_DEF  (53, USBD_CDC4_NCM_MAC_ADDRESS);
#endif
#endif
#endif
#if (USBD1_CDC5)
#ifdef USBD_CDC5_CIF_STR_DESC_RAW
  USBD_STR_DEF_u(54, USBD_CDC5_CIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (54, USBD_CDC5_CIF_STR_DESC);
#endif
#ifdef USBD_CDC5_DIF_STR_DESC_RAW
  USBD_STR_DEF_u(55, USBD_CDC5_DIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (55, USBD_CDC5_DIF_STR_DESC);
#endif
#if (USBD1_CDC5_NCM)
#ifdef USBD_CDC5_NCM_MAC_ADDRESS_RAW
  USBD_STR_DEF_u(56, USBD_CDC5_NCM_MAC_ADDRESS_RAW);
#else
  USBD_STR_DEF  (56, USBD_CDC5_NCM_MAC_ADDRESS);
#endif
#endif
#endif
#if (USBD1_CDC6)
#ifdef USBD_CDC6_CIF_STR_DESC_RAW
  USBD_STR_DEF_u(57, USBD_CDC6_CIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (57, USBD_CDC6_CIF_STR_DESC);
#endif
#ifdef USBD_CDC6_DIF_STR_DESC_RAW
  USBD_STR_DEF_u(58, USBD_CDC6_DIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (58, USBD_CDC6_DIF_STR_DESC);
#endif
#if (USBD1_CDC6_NCM)
#ifdef USBD_CDC6_NCM_MAC_ADDRESS_RAW
  USBD_STR_DEF_u(59, USBD_CDC6_NCM_MAC_ADDRESS_RAW);
#else
  USBD_STR_DEF  (59, USBD_CDC6_NCM_MAC_ADDRESS);
#endif
#endif
#endif
#if (USBD1_CDC7)
#ifdef USBD_CDC7_CIF_STR_DESC_RAW
  USBD_STR_DEF_u(60, USBD_CDC7_CIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (60, USBD_CDC7_CIF_STR_DESC);
#endif
#ifdef USBD_CDC7_DIF_STR_DESC_RAW
  USBD_STR_DEF_u(61, USBD_CDC7_DIF_STR_DESC_RAW);
#else
  USBD_STR_DEF  (61, USBD_CDC7_DIF_STR_DESC);
#endif
#if (USBD1_CDC7_NCM)
#ifdef USBD_CDC7_NCM_MAC_ADDRESS_RAW
  USBD_STR_DEF_u(62, USBD_CDC7_NCM_MAC_ADDRESS_RAW);
#else
  USBD_STR_DEF  (62, USBD_CDC7_NCM_MAC_ADDRESS);
#endif
#endif
#endif

#if (USBD1_HID0)
#ifdef USBD_HID0_STR_DESC_RAW
  USBD_STR_DEF_u(63, USBD_HID0_STR_DESC_RAW);
#else
  USBD_STR_DEF  (63, USBD_HID0_STR_DESC);
#endif
#endif
#if (USBD1_HID1)
#ifdef USBD_HID1_STR_DESC_RAW
  USBD_STR_DEF_u(64, USBD_HID1_STR_DESC_RAW);
#else
  USBD_STR_DEF  (64, USBD_HID1_STR_DESC);
#endif
#endif
#if (USBD1_HID2)
#ifdef USBD_HID2_STR_DESC_RAW
  USBD_STR_DEF_u(65, USBD_HID2_STR_DESC_RAW);
#else
  USBD_STR_DEF  (65, USBD_HID2_STR_DESC);
#endif
#endif
#if (USBD1_HID3)
#ifdef USBD_HID3_STR_DESC_RAW
  USBD_STR_DEF_u(66, USBD_HID3_STR_DESC_RAW);
#else
  USBD_STR_DEF  (66, USBD_HID3_STR_DESC);
#endif
#endif
#if (USBD1_HID4)
#ifdef USBD_HID4_STR_DESC_RAW
  USBD_STR_DEF_u(67, USBD_HID4_STR_DESC_RAW);
#else
  USBD_STR_DEF  (67, USBD_HID4_STR_DESC);
#endif
#endif
#if (USBD1_HID5)
#ifdef USBD_HID5_STR_DESC_RAW
  USBD_STR_DEF_u(68, USBD_HID5_STR_DESC_RAW);
#else
  USBD_STR_DEF  (68, USBD_HID5_STR_DESC);
#endif
#endif
#if (USBD1_HID6)
#ifdef USBD_HID6_STR_DESC_RAW
  USBD_STR_DEF_u(69, USBD_HID6_STR_DESC_RAW);
#else
  USBD_STR_DEF  (69, USBD_HID6_STR_DESC);
#endif
#endif
#if (USBD1_HID7)
#ifdef USBD_HID7_STR_DESC_RAW
  USBD_STR_DEF_u(70, USBD_HID7_STR_DESC_RAW);
#else
  USBD_STR_DEF  (70, USBD_HID7_STR_DESC);
#endif
#endif

#if (USBD1_MSC0)
#ifdef USBD_MSC0_STR_DESC_RAW
  USBD_STR_DEF_u(71, USBD_MSC0_STR_DESC_RAW);
#else
  USBD_STR_DEF  (71, USBD_MSC0_STR_DESC);
#endif
#endif
#if (USBD1_MSC1)
#ifdef USBD_MSC1_STR_DESC_RAW
  USBD_STR_DEF_u(72, USBD_MSC1_STR_DESC_RAW);
#else
  USBD_STR_DEF  (72, USBD_MSC1_STR_DESC);
#endif
#endif
#if (USBD1_MSC2)
#ifdef USBD_MSC2_STR_DESC_RAW
  USBD_STR_DEF_u(73, USBD_MSC2_STR_DESC_RAW);
#else
  USBD_STR_DEF  (73, USBD_MSC2_STR_DESC);
#endif
#endif
#if (USBD1_MSC3)
#ifdef USBD_MSC3_STR_DESC_RAW
  USBD_STR_DEF_u(74, USBD_MSC3_STR_DESC_RAW);
#else
  USBD_STR_DEF  (74, USBD_MSC3_STR_DESC);
#endif
#endif
  uint8_t bLength0_termination;

} usbd1_string_descriptor_t;

extern usbd1_string_descriptor_t usbd1_string_descriptor;
__WEAK usbd1_string_descriptor_t usbd1_string_descriptor __ALIGNED(4) = {
  4, USB_STRING_DESCRIPTOR_TYPE, USBD1_STR_DESC_LANGID,
#ifdef USBD1_STR_DESC_MAN_RAW
  USBD_STR_VAL_u(USBD1_STR_DESC_MAN_RAW),
#else
  USBD_STR_VAL  (USBD1_STR_DESC_MAN),
#endif
#ifdef USBD1_STR_DESC_PROD_RAW
  USBD_STR_VAL_u(USBD1_STR_DESC_PROD_RAW),
#else
  USBD_STR_VAL  (USBD1_STR_DESC_PROD),
#endif
#if (USBD1_STR_DESC_SER_EN)
#ifdef USBD1_STR_DESC_SER_RAW
  USBD_STR_VAL_u(USBD1_STR_DESC_SER_RAW),
#else
  USBD_STR_VAL  (USBD1_STR_DESC_SER),
#endif
#endif

#if (USBD1_CUSTOM_CLASS0)
#if ((USBD_CUSTOM_CLASS0_IF0_EN != 0) && (USBD_CUSTOM_CLASS0_IF0_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS0_IF0_STR_RAW
  USBD_STR_VAL_u(USBD_CUSTOM_CLASS0_IF0_STR_RAW),
#else
  USBD_STR_VAL  (USBD_CUSTOM_CLASS0_IF0_STR),
#endif
#endif
#if ((USBD_CUSTOM_CLASS0_IF1_EN != 0) && (USBD_CUSTOM_CLASS0_IF1_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS0_IF1_STR_RAW
  USBD_STR_VAL_u(USBD_CUSTOM_CLASS0_IF1_STR_RAW),
#else
  USBD_STR_VAL  (USBD_CUSTOM_CLASS0_IF1_STR),
#endif
#endif
#if ((USBD_CUSTOM_CLASS0_IF2_EN != 0) && (USBD_CUSTOM_CLASS0_IF2_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS0_IF2_STR_RAW
  USBD_STR_VAL_u(USBD_CUSTOM_CLASS0_IF2_STR_RAW),
#else
  USBD_STR_VAL  (USBD_CUSTOM_CLASS0_IF2_STR),
#endif
#endif
#if ((USBD_CUSTOM_CLASS0_IF3_EN != 0) && (USBD_CUSTOM_CLASS0_IF3_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS0_IF3_STR_RAW
  USBD_STR_VAL_u(USBD_CUSTOM_CLASS0_IF3_STR_RAW),
#else
  USBD_STR_VAL  (USBD_CUSTOM_CLASS0_IF3_STR),
#endif
#endif
#endif
#if (USBD1_CUSTOM_CLASS1)
#if ((USBD_CUSTOM_CLASS1_IF0_EN != 0) && (USBD_CUSTOM_CLASS1_IF0_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS1_IF0_STR_RAW
  USBD_STR_VAL_u(USBD_CUSTOM_CLASS1_IF0_STR_RAW),
#else
  USBD_STR_VAL  (USBD_CUSTOM_CLASS1_IF0_STR),
#endif
#endif
#if ((USBD_CUSTOM_CLASS1_IF1_EN != 0) && (USBD_CUSTOM_CLASS1_IF1_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS1_IF1_STR_RAW
  USBD_STR_VAL_u(USBD_CUSTOM_CLASS1_IF1_STR_RAW),
#else
  USBD_STR_VAL  (USBD_CUSTOM_CLASS1_IF1_STR),
#endif
#endif
#if ((USBD_CUSTOM_CLASS1_IF2_EN != 0) && (USBD_CUSTOM_CLASS1_IF2_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS1_IF2_STR_RAW
  USBD_STR_VAL_u(USBD_CUSTOM_CLASS1_IF2_STR_RAW),
#else
  USBD_STR_VAL  (USBD_CUSTOM_CLASS1_IF2_STR),
#endif
#endif
#if ((USBD_CUSTOM_CLASS1_IF3_EN != 0) && (USBD_CUSTOM_CLASS1_IF3_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS1_IF3_STR_RAW
  USBD_STR_VAL_u(USBD_CUSTOM_CLASS1_IF3_STR_RAW),
#else
  USBD_STR_VAL  (USBD_CUSTOM_CLASS1_IF3_STR),
#endif
#endif
#endif
#if (USBD1_CUSTOM_CLASS2)
#if ((USBD_CUSTOM_CLASS2_IF0_EN != 0) && (USBD_CUSTOM_CLASS2_IF0_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS2_IF0_STR_RAW
  USBD_STR_VAL_u(USBD_CUSTOM_CLASS2_IF0_STR_RAW),
#else
  USBD_STR_VAL  (USBD_CUSTOM_CLASS2_IF0_STR),
#endif
#endif
#if ((USBD_CUSTOM_CLASS2_IF1_EN != 0) && (USBD_CUSTOM_CLASS2_IF1_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS2_IF1_STR_RAW
  USBD_STR_VAL_u(USBD_CUSTOM_CLASS2_IF1_STR_RAW),
#else
  USBD_STR_VAL  (USBD_CUSTOM_CLASS2_IF1_STR),
#endif
#endif
#if ((USBD_CUSTOM_CLASS2_IF2_EN != 0) && (USBD_CUSTOM_CLASS2_IF2_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS2_IF2_STR_RAW
  USBD_STR_VAL_u(USBD_CUSTOM_CLASS2_IF2_STR_RAW),
#else
  USBD_STR_VAL  (USBD_CUSTOM_CLASS2_IF2_STR),
#endif
#endif
#if ((USBD_CUSTOM_CLASS2_IF3_EN != 0) && (USBD_CUSTOM_CLASS2_IF3_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS2_IF3_STR_RAW
  USBD_STR_VAL_u(USBD_CUSTOM_CLASS2_IF3_STR_RAW),
#else
  USBD_STR_VAL  (USBD_CUSTOM_CLASS2_IF3_STR),
#endif
#endif
#endif
#if (USBD1_CUSTOM_CLASS3)
#if ((USBD_CUSTOM_CLASS3_IF0_EN != 0) && (USBD_CUSTOM_CLASS3_IF0_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS3_IF0_STR_RAW
  USBD_STR_VAL_u(USBD_CUSTOM_CLASS3_IF0_STR_RAW),
#else
  USBD_STR_VAL  (USBD_CUSTOM_CLASS3_IF0_STR),
#endif
#endif
#if ((USBD_CUSTOM_CLASS3_IF1_EN != 0) && (USBD_CUSTOM_CLASS3_IF1_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS3_IF1_STR_RAW
  USBD_STR_VAL_u(USBD_CUSTOM_CLASS3_IF1_STR_RAW),
#else
  USBD_STR_VAL  (USBD_CUSTOM_CLASS3_IF1_STR),
#endif
#endif
#if ((USBD_CUSTOM_CLASS3_IF2_EN != 0) && (USBD_CUSTOM_CLASS3_IF2_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS3_IF2_STR_RAW
  USBD_STR_VAL_u(USBD_CUSTOM_CLASS3_IF2_STR_RAW),
#else
  USBD_STR_VAL  (USBD_CUSTOM_CLASS3_IF2_STR),
#endif
#endif
#if ((USBD_CUSTOM_CLASS3_IF3_EN != 0) && (USBD_CUSTOM_CLASS3_IF3_STR_EN != 0))
#ifdef USBD_CUSTOM_CLASS3_IF3_STR_RAW
  USBD_STR_VAL_u(USBD_CUSTOM_CLASS3_IF3_STR_RAW),
#else
  USBD_STR_VAL  (USBD_CUSTOM_CLASS3_IF3_STR),
#endif
#endif
#endif

#if (USBD1_ADC0)
#ifdef USBD_ADC0_CIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC0_CIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC0_CIF_STR_DESC),
#endif
#if (USBD_ADC0_IF_SPKR)
#ifdef USBD_ADC0_OUT_SIF_STR0_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC0_OUT_SIF_STR0_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC0_OUT_SIF_STR0_DESC),
#endif
#ifdef USBD_ADC0_OUT_SIF_STR1_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC0_OUT_SIF_STR1_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC0_OUT_SIF_STR1_DESC),
#endif
#endif
#if (USBD_ADC0_IF_MIC)
#ifdef USBD_ADC0_IN_SIF_STR0_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC0_IN_SIF_STR0_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC0_IN_SIF_STR0_DESC),
#endif
#ifdef USBD_ADC0_IN_SIF_STR1_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC0_IN_SIF_STR1_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC0_IN_SIF_STR1_DESC),
#endif
#endif
#endif
#if (USBD1_ADC1)
#ifdef USBD_ADC1_CIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC1_CIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC1_CIF_STR_DESC),
#endif
#if (USBD_ADC1_IF_SPKR)
#ifdef USBD_ADC1_OUT_SIF_STR0_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC1_OUT_SIF_STR0_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC1_OUT_SIF_STR0_DESC),
#endif
#ifdef USBD_ADC1_OUT_SIF_STR1_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC1_OUT_SIF_STR1_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC1_OUT_SIF_STR1_DESC),
#endif
#endif
#if (USBD_ADC1_IF_MIC)
#ifdef USBD_ADC1_IN_SIF_STR0_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC1_IN_SIF_STR0_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC1_IN_SIF_STR0_DESC),
#endif
#ifdef USBD_ADC1_IN_SIF_STR1_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC1_IN_SIF_STR1_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC1_IN_SIF_STR1_DESC),
#endif
#endif
#endif
#if (USBD1_ADC2)
#ifdef USBD_ADC2_CIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC2_CIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC2_CIF_STR_DESC),
#endif
#if (USBD_ADC2_IF_SPKR)
#ifdef USBD_ADC2_OUT_SIF_STR0_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC2_OUT_SIF_STR0_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC2_OUT_SIF_STR0_DESC),
#endif
#ifdef USBD_ADC2_OUT_SIF_STR1_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC2_OUT_SIF_STR1_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC2_OUT_SIF_STR1_DESC),
#endif
#endif
#if (USBD_ADC2_IF_MIC)
#ifdef USBD_ADC2_IN_SIF_STR0_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC2_IN_SIF_STR0_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC2_IN_SIF_STR0_DESC),
#endif
#ifdef USBD_ADC2_IN_SIF_STR1_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC2_IN_SIF_STR1_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC2_IN_SIF_STR1_DESC),
#endif
#endif
#endif
#if (USBD1_ADC3)
#ifdef USBD_ADC3_CIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC3_CIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC3_CIF_STR_DESC),
#endif
#if (USBD_ADC3_IF_SPKR)
#ifdef USBD_ADC3_OUT_SIF_STR0_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC3_OUT_SIF_STR0_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC3_OUT_SIF_STR0_DESC),
#endif
#ifdef USBD_ADC3_OUT_SIF_STR1_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC3_OUT_SIF_STR1_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC3_OUT_SIF_STR1_DESC),
#endif
#endif
#if (USBD_ADC3_IF_MIC)
#ifdef USBD_ADC3_IN_SIF_STR0_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC3_IN_SIF_STR0_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC3_IN_SIF_STR0_DESC),
#endif
#ifdef USBD_ADC3_IN_SIF_STR1_DESC_RAW
  USBD_STR_VAL_u(USBD_ADC3_IN_SIF_STR1_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_ADC3_IN_SIF_STR1_DESC),
#endif
#endif
#endif

#if (USBD1_CDC0)
#ifdef USBD_CDC0_CIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_CDC0_CIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_CDC0_CIF_STR_DESC),
#endif
#ifdef USBD_CDC0_DIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_CDC0_DIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_CDC0_DIF_STR_DESC),
#endif
#if (USBD1_CDC0_NCM)
#ifdef USBD_CDC0_NCM_MAC_ADDRESS_RAW
  USBD_STR_VAL_u(USBD_CDC0_NCM_MAC_ADDRESS_RAW),
#else
  USBD_STR_VAL  (USBD_CDC0_NCM_MAC_ADDRESS),
#endif
#endif
#endif
#if (USBD1_CDC1)
#ifdef USBD_CDC1_CIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_CDC1_CIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_CDC1_CIF_STR_DESC),
#endif
#ifdef USBD_CDC1_DIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_CDC1_DIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_CDC1_DIF_STR_DESC),
#endif
#if (USBD1_CDC1_NCM)
#ifdef USBD_CDC1_NCM_MAC_ADDRESS_RAW
  USBD_STR_VAL_u(USBD_CDC1_NCM_MAC_ADDRESS_RAW),
#else
  USBD_STR_VAL  (USBD_CDC1_NCM_MAC_ADDRESS),
#endif
#endif
#endif
#if (USBD1_CDC2)
#ifdef USBD_CDC2_CIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_CDC2_CIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_CDC2_CIF_STR_DESC),
#endif
#ifdef USBD_CDC2_DIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_CDC2_DIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_CDC2_DIF_STR_DESC),
#endif
#if (USBD1_CDC2_NCM)
#ifdef USBD_CDC2_NCM_MAC_ADDRESS_RAW
  USBD_STR_VAL_u(USBD_CDC2_NCM_MAC_ADDRESS_RAW),
#else
  USBD_STR_VAL  (USBD_CDC2_NCM_MAC_ADDRESS),
#endif
#endif
#endif
#if (USBD1_CDC3)
#ifdef USBD_CDC3_CIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_CDC3_CIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_CDC3_CIF_STR_DESC),
#endif
#ifdef USBD_CDC3_DIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_CDC3_DIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_CDC3_DIF_STR_DESC),
#endif
#if (USBD1_CDC3_NCM)
#ifdef USBD_CDC3_NCM_MAC_ADDRESS_RAW
  USBD_STR_VAL_u(USBD_CDC3_NCM_MAC_ADDRESS_RAW),
#else
  USBD_STR_VAL  (USBD_CDC3_NCM_MAC_ADDRESS),
#endif
#endif
#endif
#if (USBD1_CDC4)
#ifdef USBD_CDC4_CIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_CDC4_CIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_CDC4_CIF_STR_DESC),
#endif
#ifdef USBD_CDC4_DIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_CDC4_DIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_CDC4_DIF_STR_DESC),
#endif
#if (USBD1_CDC4_NCM)
#ifdef USBD_CDC4_NCM_MAC_ADDRESS_RAW
  USBD_STR_VAL_u(USBD_CDC4_NCM_MAC_ADDRESS_RAW),
#else
  USBD_STR_VAL  (USBD_CDC4_NCM_MAC_ADDRESS),
#endif
#endif
#endif
#if (USBD1_CDC5)
#ifdef USBD_CDC5_CIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_CDC5_CIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_CDC5_CIF_STR_DESC),
#endif
#ifdef USBD_CDC5_DIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_CDC5_DIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_CDC5_DIF_STR_DESC),
#endif
#if (USBD1_CDC5_NCM)
#ifdef USBD_CDC5_NCM_MAC_ADDRESS_RAW
  USBD_STR_VAL_u(USBD_CDC5_NCM_MAC_ADDRESS_RAW),
#else
  USBD_STR_VAL  (USBD_CDC5_NCM_MAC_ADDRESS),
#endif
#endif
#endif
#if (USBD1_CDC6)
#ifdef USBD_CDC6_CIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_CDC6_CIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_CDC6_CIF_STR_DESC),
#endif
#ifdef USBD_CDC6_DIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_CDC6_DIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_CDC6_DIF_STR_DESC),
#endif
#if (USBD1_CDC6_NCM)
#ifdef USBD_CDC6_NCM_MAC_ADDRESS_RAW
  USBD_STR_VAL_u(USBD_CDC6_NCM_MAC_ADDRESS_RAW),
#else
  USBD_STR_VAL  (USBD_CDC6_NCM_MAC_ADDRESS),
#endif
#endif
#endif
#if (USBD1_CDC7)
#ifdef USBD_CDC7_CIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_CDC7_CIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_CDC7_CIF_STR_DESC),
#endif
#ifdef USBD_CDC7_DIF_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_CDC7_DIF_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_CDC7_DIF_STR_DESC),
#endif
#if (USBD1_CDC7_NCM)
#ifdef USBD_CDC7_NCM_MAC_ADDRESS_RAW
  USBD_STR_VAL_u(USBD_CDC7_NCM_MAC_ADDRESS_RAW),
#else
  USBD_STR_VAL  (USBD_CDC7_NCM_MAC_ADDRESS),
#endif
#endif
#endif

#if (USBD1_HID0)
#ifdef USBD_HID0_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_HID0_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_HID0_STR_DESC),
#endif
#endif
#if (USBD1_HID1)
#ifdef USBD_HID1_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_HID1_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_HID1_STR_DESC),
#endif
#endif
#if (USBD1_HID2)
#ifdef USBD_HID2_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_HID2_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_HID2_STR_DESC),
#endif
#endif
#if (USBD1_HID3)
#ifdef USBD_HID3_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_HID3_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_HID3_STR_DESC),
#endif
#endif
#if (USBD1_HID4)
#ifdef USBD_HID4_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_HID4_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_HID4_STR_DESC),
#endif
#endif
#if (USBD1_HID5)
#ifdef USBD_HID5_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_HID5_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_HID5_STR_DESC),
#endif
#endif
#if (USBD1_HID6)
#ifdef USBD_HID6_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_HID6_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_HID6_STR_DESC),
#endif
#endif
#if (USBD1_HID7)
#ifdef USBD_HID7_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_HID7_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_HID7_STR_DESC),
#endif
#endif

#if (USBD1_MSC0)
#ifdef USBD_MSC0_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_MSC0_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_MSC0_STR_DESC),
#endif
#endif
#if (USBD1_MSC1)
#ifdef USBD_MSC1_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_MSC1_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_MSC1_STR_DESC),
#endif
#endif
#if (USBD1_MSC2)
#ifdef USBD_MSC2_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_MSC2_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_MSC2_STR_DESC),
#endif
#endif
#if (USBD1_MSC3)
#ifdef USBD_MSC3_STR_DESC_RAW
  USBD_STR_VAL_u(USBD_MSC3_STR_DESC_RAW),
#else
  USBD_STR_VAL  (USBD_MSC3_STR_DESC),
#endif
#endif
  0
};

#if ((USBD1_STR_DESC_SER_EN != 0) && (USBD1_STR_DESC_SER_MAX_LEN > 0))
typedef __PACKED_STRUCT {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint16_t bString[USBD1_STR_DESC_SER_MAX_LEN];
} usbd1_ser_num_string_descriptor_t;

static usbd1_ser_num_string_descriptor_t usbd1_ser_num_string_descriptor __ALIGNED(4);
#endif

#if (USBD1_OS_DESC_EN != 0)
// Microsoft OS String Descriptor
static const uint8_t usbd1_ms_os_string_descriptor[18] __ALIGNED(4) = {
  USBD_MS_OS_STR(0)
};

#if ((USBD1_CUSTOM_CLASS_MS_COMPAT_CNT != 0) || (USBD1_CDC_MS_COMPAT_CNT != 0))
// Microsoft Extended Compat ID OS Feature Descriptor
typedef const __PACKED_STRUCT {
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_HEADER_DEF
#if  (USBD1_CUSTOM_CLASS0 == 1)
#if ((USBD_CUSTOM_CLASS0_IF0_EN == 1) && (USBD_CUSTOM_CLASS0_IF0_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(0,0)
#endif
#if ((USBD_CUSTOM_CLASS0_IF1_EN == 1) && (USBD_CUSTOM_CLASS0_IF1_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(0,1)
#endif
#if ((USBD_CUSTOM_CLASS0_IF2_EN == 1) && (USBD_CUSTOM_CLASS0_IF2_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(0,2)
#endif
#if ((USBD_CUSTOM_CLASS0_IF3_EN == 1) && (USBD_CUSTOM_CLASS0_IF3_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(0,3)
#endif
#endif

#if  (USBD1_CUSTOM_CLASS1 == 1)
#if ((USBD_CUSTOM_CLASS1_IF0_EN == 1) && (USBD_CUSTOM_CLASS1_IF0_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(1,0)
#endif
#if ((USBD_CUSTOM_CLASS1_IF1_EN == 1) && (USBD_CUSTOM_CLASS1_IF1_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(1,1)
#endif
#if ((USBD_CUSTOM_CLASS1_IF2_EN == 1) && (USBD_CUSTOM_CLASS1_IF2_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(1,2)
#endif
#if ((USBD_CUSTOM_CLASS1_IF3_EN == 1) && (USBD_CUSTOM_CLASS1_IF3_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(1,3)
#endif
#endif

#if  (USBD1_CUSTOM_CLASS2 == 1)
#if ((USBD_CUSTOM_CLASS2_IF0_EN == 1) && (USBD_CUSTOM_CLASS2_IF0_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(2,0)
#endif
#if ((USBD_CUSTOM_CLASS2_IF1_EN == 1) && (USBD_CUSTOM_CLASS2_IF1_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(2,1)
#endif
#if ((USBD_CUSTOM_CLASS2_IF2_EN == 1) && (USBD_CUSTOM_CLASS2_IF2_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(2,2)
#endif
#if ((USBD_CUSTOM_CLASS2_IF3_EN == 1) && (USBD_CUSTOM_CLASS2_IF3_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(2,3)
#endif
#endif

#if  (USBD1_CUSTOM_CLASS3 == 1)
#if ((USBD_CUSTOM_CLASS3_IF0_EN == 1) && (USBD_CUSTOM_CLASS3_IF0_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(3,0)
#endif
#if ((USBD_CUSTOM_CLASS3_IF1_EN == 1) && (USBD_CUSTOM_CLASS3_IF1_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(3,1)
#endif
#if ((USBD_CUSTOM_CLASS3_IF2_EN == 1) && (USBD_CUSTOM_CLASS3_IF2_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(3,2)
#endif
#if ((USBD_CUSTOM_CLASS3_IF3_EN == 1) && (USBD_CUSTOM_CLASS3_IF3_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(3,3)
#endif
#endif

#if ((USBD1_CDC0 == 1) && (USBD1_CDC0_ACM == 1) && (USBD1_CDC0_RNDIS == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(CDC0,0)
#endif
#if ((USBD1_CDC1 == 1) && (USBD1_CDC1_ACM == 1) && (USBD1_CDC1_RNDIS == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(CDC1,1)
#endif
#if ((USBD1_CDC2 == 1) && (USBD1_CDC2_ACM == 1) && (USBD1_CDC2_RNDIS == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(CDC2,2)
#endif
#if ((USBD1_CDC3 == 1) && (USBD1_CDC3_ACM == 1) && (USBD1_CDC3_RNDIS == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(CDC3,3)
#endif
#if ((USBD1_CDC4 == 1) && (USBD1_CDC4_ACM == 1) && (USBD1_CDC4_RNDIS == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(CDC4,4)
#endif
#if ((USBD1_CDC5 == 1) && (USBD1_CDC5_ACM == 1) && (USBD1_CDC5_RNDIS == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(CDC5,5)
#endif
#if ((USBD1_CDC6 == 1) && (USBD1_CDC6_ACM == 1) && (USBD1_CDC6_RNDIS == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(CDC6,6)
#endif
#if ((USBD1_CDC7 == 1) && (USBD1_CDC7_ACM == 1) && (USBD1_CDC7_RNDIS == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_DEF(CDC7,7)
#endif
} usbd1_ms_os_ext_compat_id_descriptor_t;

static const usbd1_ms_os_ext_compat_id_descriptor_t usbd1_ms_os_ext_compat_id_descriptor __ALIGNED(4) = {
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_HEADER_VAL(0, (USBD1_CUSTOM_CLASS_MS_COMPAT_CNT + USBD1_CDC_MS_COMPAT_CNT))
#if  (USBD1_CUSTOM_CLASS0 == 1)
#if ((USBD_CUSTOM_CLASS0_IF0_EN == 1) && (USBD_CUSTOM_CLASS0_IF0_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_VAL(0,0)
#endif
#if ((USBD_CUSTOM_CLASS0_IF1_EN == 1) && (USBD_CUSTOM_CLASS0_IF1_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_VAL(0,1)
#endif
#if ((USBD_CUSTOM_CLASS0_IF2_EN == 1) && (USBD_CUSTOM_CLASS0_IF2_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_VAL(0,2)
#endif
#if ((USBD_CUSTOM_CLASS0_IF3_EN == 1) && (USBD_CUSTOM_CLASS0_IF3_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_VAL(0,3)
#endif
#endif

#if  (USBD1_CUSTOM_CLASS1 == 1)
#if ((USBD_CUSTOM_CLASS1_IF0_EN == 1) && (USBD_CUSTOM_CLASS1_IF0_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_VAL(1,0)
#endif
#if ((USBD_CUSTOM_CLASS1_IF1_EN == 1) && (USBD_CUSTOM_CLASS1_IF1_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_VAL(1,1)
#endif
#if ((USBD_CUSTOM_CLASS1_IF2_EN == 1) && (USBD_CUSTOM_CLASS1_IF2_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_VAL(1,2)
#endif
#if ((USBD_CUSTOM_CLASS1_IF3_EN == 1) && (USBD_CUSTOM_CLASS1_IF3_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_VAL(1,3)
#endif
#endif

#if  (USBD1_CUSTOM_CLASS2 == 1)
#if ((USBD_CUSTOM_CLASS2_IF0_EN == 1) && (USBD_CUSTOM_CLASS2_IF0_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_VAL(2,0)
#endif
#if ((USBD_CUSTOM_CLASS2_IF1_EN == 1) && (USBD_CUSTOM_CLASS2_IF1_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_VAL(2,1)
#endif
#if ((USBD_CUSTOM_CLASS2_IF2_EN == 1) && (USBD_CUSTOM_CLASS2_IF2_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_VAL(2,2)
#endif
#if ((USBD_CUSTOM_CLASS2_IF3_EN == 1) && (USBD_CUSTOM_CLASS2_IF3_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_VAL(2,3)
#endif
#endif

#if  (USBD1_CUSTOM_CLASS3 == 1)
#if ((USBD_CUSTOM_CLASS3_IF0_EN == 1) && (USBD_CUSTOM_CLASS3_IF0_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_VAL(3,0)
#endif
#if ((USBD_CUSTOM_CLASS3_IF1_EN == 1) && (USBD_CUSTOM_CLASS3_IF1_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_VAL(3,1)
#endif
#if ((USBD_CUSTOM_CLASS3_IF2_EN == 1) && (USBD_CUSTOM_CLASS3_IF2_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_VAL(3,2)
#endif
#if ((USBD_CUSTOM_CLASS3_IF3_EN == 1) && (USBD_CUSTOM_CLASS3_IF3_OS_EXT_COMPAT_ID_EN == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_FUNCTION_VAL(3,3)
#endif
#endif

#if ((USBD1_CDC0 == 1) && (USBD1_CDC0_ACM == 1) && (USBD1_CDC0_RNDIS == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_RNDIS_FUNCTION_VAL(0,0)
#endif
#if ((USBD1_CDC1 == 1) && (USBD1_CDC1_ACM == 1) && (USBD1_CDC1_RNDIS == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_RNDIS_FUNCTION_VAL(1,1)
#endif
#if ((USBD1_CDC2 == 1) && (USBD1_CDC2_ACM == 1) && (USBD1_CDC2_RNDIS == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_RNDIS_FUNCTION_VAL(2,2)
#endif
#if ((USBD1_CDC3 == 1) && (USBD1_CDC3_ACM == 1) && (USBD1_CDC3_RNDIS == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_RNDIS_FUNCTION_VAL(3,3)
#endif
#if ((USBD1_CDC4 == 1) && (USBD1_CDC4_ACM == 1) && (USBD1_CDC4_RNDIS == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_RNDIS_FUNCTION_VAL(4,4)
#endif
#if ((USBD1_CDC5 == 1) && (USBD1_CDC5_ACM == 1) && (USBD1_CDC5_RNDIS == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_RNDIS_FUNCTION_VAL(5,5)
#endif
#if ((USBD1_CDC6 == 1) && (USBD1_CDC6_ACM == 1) && (USBD1_CDC6_RNDIS == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_RNDIS_FUNCTION_VAL(6,6)
#endif
#if ((USBD1_CDC7 == 1) && (USBD1_CDC7_ACM == 1) && (USBD1_CDC7_RNDIS == 1))
  USBD_MS_OS_FEAT_EXT_COMPAT_ID_RNDIS_FUNCTION_VAL(7,7)
#endif
};
#endif // ((USBD1_CUSTOM_CLASS_MS_COMPAT_CNT != 0) || (USBD1_CDC_MS_COMPAT_CNT != 0))
#endif // (USBD1_OS_DESC_EN != 0)


// Pointers to all descriptors
extern 
usbd_desc_t usbd1_desc;
usbd_desc_t usbd1_desc = {
  usbd1_ep0_descriptor,
  usbd1_device_descriptor,
  usbd1_device_qualifier_fs,
  usbd1_device_qualifier_hs,
  usbd1_config_descriptor_fs,
  usbd1_config_descriptor_hs,
  usbd1_other_speed_config_descriptor_fs,
  usbd1_other_speed_config_descriptor_hs,
  (const uint8_t *)(&usbd1_string_descriptor),
#if ((USBD1_STR_DESC_SER_EN != 0) && (USBD1_STR_DESC_SER_MAX_LEN > 0))
  (uint8_t *)(&usbd1_ser_num_string_descriptor),
#else
  NULL,
#endif
#if (USBD1_OS_DESC_EN != 0)
  usbd1_ms_os_string_descriptor,
#else
  NULL,
#endif
#if ((USBD1_OS_DESC_EN != 0) && ((USBD1_CUSTOM_CLASS_MS_COMPAT_CNT != 0) || (USBD1_CDC_MS_COMPAT_CNT != 0)))
  (const uint8_t *)(&usbd1_ms_os_ext_compat_id_descriptor),
#else
  NULL,
#endif
};
