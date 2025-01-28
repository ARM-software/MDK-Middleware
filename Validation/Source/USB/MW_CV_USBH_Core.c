/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Host
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USBH_Core.c
 * Purpose: MDK Middleware - Component Validation - USB Host - Core -
 *          tests module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"
#if     ((MW_CV_USBH != 0U) && (MW_CV_USBH_CORE != 0U))

#include "MW_CV_USBH_Core.h"

#include <stdbool.h>
#include <string.h>

#include "RTE_Components.h"

#include "MW_CV_TestReport.h"
#include "MW_CV_USB.h"

#include "rl_usb.h"
#include "rl_fs.h"
#include "USBH_MSC.h"


// Local variables used for testing
static const uint8_t CTRL[4] = { 0U, 1U, 2U, 255U };
static const uint8_t DEV [3] = { 0U, 1U, 255U };

static uint8_t     test_data_buf[32] __ALIGNED(4);
static bool        cb_notify        = false;
static uint8_t     cb_notify_ctrl   = 255U;
static uint8_t     cb_notify_port   = 255U;
static uint8_t     cb_notify_device = 255U;
static USBH_NOTIFY cb_notify_notify = USBH_NOTIFY_DISCONNECT;


// Callback functions
void USBH_Notify(uint8_t ctrl, uint8_t port, uint8_t device, USBH_NOTIFY notify) {
  cb_notify        = true;
  cb_notify_ctrl   = ctrl;
  cb_notify_port   = port;
  cb_notify_device = device;
  cb_notify_notify = notify;
}


/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\defgroup mw_cv_usbh_core_test_funcs MDK Middleware - Component Validation - USB Host - Core
\brief USB Host Core validation test functions
\details
The MDK Middleware Component Validation for USB Host Core checks USB Host Core API functions and callbacks.
@{
*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_GetVersion
\details
The MW_CV_USBH_GetVersion test function verifies the \b USBH_GetVersion it expects that
version of USB Host component is at least 6.0.0.
*/
void MW_CV_USBH_GetVersion(void) {
  uint32_t ver, major, minor, patch;

  ver   =  USBH_GetVersion();
  major =  ver / 10000000U;
  minor = (ver % 10000000U) / 10000U;
  patch = (ver %    10000U);

  DETAIL_INFO("Reported component version %d.%d.%d", major, minor, patch);

  ASSERT_TRUE(ver >= 60000000U, "Expected version at least 6.0.0");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_Initialize
\details
The MW_CV_USBH_Initialize test function verifies the \b USBH_Initialize function with the following parameter values:
 - ctrl: 0, 1, 2, 255
*/
void MW_CV_USBH_Initialize(void) {
  uint32_t timeout_cnt, expected_result;
  uint8_t  ctrl, i;

  for (i = 0U; i < sizeof(CTRL); i++) {
    ctrl = CTRL[i];
    expected_result = usbControllerError;
#if defined(RTE_USB_Host_0)
    if (ctrl == 0U) {
      expected_result = usbOK;
    }
#endif
#if defined(RTE_USB_Host_1)
    if (ctrl == 1U) {
      expected_result = usbOK;
    }
#endif
    ASSERT_TRUE(USBH_Initialize(ctrl) == expected_result, "USBH_Initialize(%d) != %s",
                ctrl, MW_CW_USB_STATUS_TXT(expected_result));
    ASSERT_TRUE(USBH_Initialize(ctrl) == expected_result, "USBH_Initialize(%d) != %s (2nd call)",
                ctrl, MW_CW_USB_STATUS_TXT(expected_result));
  }

  // Initialize the USB Device which will be used for testing of the USB Host
  USBD_Initialize(0U);
  USBD_Connect   (0U);

  // Wait for device to be detected as most of following tests depend on it
  for (timeout_cnt = (MW_CV_USB_TIMEOUT_IN_SEC * 10U); timeout_cnt > 0U; timeout_cnt--) {
    if (USBH_MSC_GetStatus(0U) == usbOK) {
      break;
    }
    osDelay(100U);
  }
  if (timeout_cnt == 0U) {
    ASSERT_TRUE(false, "USB0 not connected to USB1 or SD Card not inserted, necessary for further tests");
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_Port_Suspend
\details
The MW_CV_USBH_Port_Suspend function verifies the \b USBH_Port_Suspend function with the following parameter values:
 - ctrl: 0, 1, 2, 255
 - port: 0
*/
void MW_CV_USBH_Port_Suspend(void) {
  uint32_t expected_result;
  uint8_t  ctrl, i;

  for (i = 0U; i < sizeof(CTRL); i++) {
    ctrl = CTRL[i];
    expected_result = usbControllerError;
#if defined(RTE_USB_Host_0)
    if (ctrl == 0U) {
      expected_result = usbOK;
    }
#endif
#if defined(RTE_USB_Host_1)
    if (ctrl == 1U) {
      expected_result = usbOK;
    }
#endif
    if (ctrl == 0U) {
      if (USBH_Port_Suspend(ctrl, 0U) == expected_result) {
        ASSERT_TRUE(true, "");
      } else {
        DETAIL_INFO("USBH_Port_Suspend(%d, %d) != %s, possibly Host Driver does not support Suspend",            ctrl, 0U, MW_CW_USB_STATUS_TXT(expected_result));
      }
      if (USBH_Port_Suspend(ctrl, 0U) == expected_result) {
        ASSERT_TRUE(true, "");
      } else {
        DETAIL_INFO("USBH_Port_Suspend(%d, %d) != %s (2nd call), possibly Host Driver does not support Suspend", ctrl, 0U, MW_CW_USB_STATUS_TXT(expected_result));
      }
    } else {
      ASSERT_TRUE(USBH_Port_Suspend(ctrl, 0U) == expected_result,
                 "USBH_Port_Suspend(%d, %d) != %s",            ctrl, 0U, MW_CW_USB_STATUS_TXT(expected_result));
      ASSERT_TRUE(USBH_Port_Suspend(ctrl, 0U) == expected_result,
                 "USBH_Port_Suspend(%d, %d) != %s (2nd call)", ctrl, 0U, MW_CW_USB_STATUS_TXT(expected_result));
    }
  }

  osDelay(1000U);                       // 1 second USB bus suspended state (no SOFs)
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_Port_Resume
\details
The MW_CV_USBH_Port_Resume function verifies the \b USBH_Port_Resume function with the following parameter values:
 - ctrl: 0, 1, 2, 255
 - port: 0
*/
void MW_CV_USBH_Port_Resume(void) {
  uint32_t expected_result;
  uint8_t  ctrl, i;

  for (i = 0U; i < sizeof(CTRL); i++) {
    ctrl = CTRL[i];
    expected_result = usbControllerError;
#if defined(RTE_USB_Host_0)
    if (ctrl == 0U) {
      expected_result = usbOK;
    }
#endif
#if defined(RTE_USB_Host_1)
    if (ctrl == 1U) {
      expected_result = usbOK;
    }
#endif
    if (ctrl == 0U) {
      if (USBH_Port_Resume(ctrl, 0U) == expected_result) {
        ASSERT_TRUE(true, "");
      } else {
        DETAIL_INFO("USBH_Port_Resume(%d, %d) != %s, possibly Host Driver does not support Resume",            ctrl, 0U, MW_CW_USB_STATUS_TXT(expected_result));
      }
      if (USBH_Port_Resume(ctrl, 0U) == expected_result) {
        ASSERT_TRUE(true, "");
      } else {
        DETAIL_INFO("USBH_Port_Resume(%d, %d) != %s (2nd call), possibly Host Driver does not support Resume", ctrl, 0U, MW_CW_USB_STATUS_TXT(expected_result));
      }
    } else {
      ASSERT_TRUE(USBH_Port_Resume(ctrl, 0U) == expected_result,
                 "USBH_Port_Resume(%d, %d) != %s",            ctrl, 0U, MW_CW_USB_STATUS_TXT(expected_result));
      ASSERT_TRUE(USBH_Port_Resume(ctrl, 0U) == expected_result,
                 "USBH_Port_Resume(%d, %d) != %s (2nd call)", ctrl, 0U, MW_CW_USB_STATUS_TXT(expected_result));
    }
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_Device_GetController
\details
The MW_CV_USBH_Device_GetController function verifies the \b USBH_Device_GetController function with the following parameter
values:
 - device: 0, 1, 255
*/
void MW_CV_USBH_Device_GetController(void) {
  uint8_t expected_result, dev, i;

  if (USBH_MSC_GetStatus(0U) != usbOK) {
    ASSERT_TRUE(false, "No Mass Storage device available or SD Card not inserted");
    return;
  }

  for (i = 0U; i < sizeof(DEV); i++) {
    dev = DEV[i];
    expected_result = 255U;
    if (dev == 0U) {
      expected_result = 0U;
    }
    ASSERT_TRUE(USBH_Device_GetController(dev) == expected_result,
               "USBH_Device_GetController(%d) != %d",            dev, expected_result);
    ASSERT_TRUE(USBH_Device_GetController(dev) == expected_result,
               "USBH_Device_GetController(%d) != %d (2nd call)", dev, expected_result);
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_Device_GetPort
\details
The MW_CV_USBH_Device_GetPort function verifies the \b USBH_Device_GetPort function with the following parameter values:
 - device: 0, 1, 255
*/
void MW_CV_USBH_Device_GetPort(void) {
  uint8_t expected_result, dev, i;

  if (USBH_MSC_GetStatus(0U) != usbOK) {
    ASSERT_TRUE(false, "No Mass Storage device available or SD Card not inserted");
    return;
  }

  for (i = 0U; i < sizeof(DEV); i++) {
    dev = DEV[i];
    expected_result = 255U;
    if (dev == 0U) {
      expected_result = 0U;
    }
    ASSERT_TRUE(USBH_Device_GetPort(dev) == expected_result,
               "USBH_Device_GetPort(%d) != %d",            dev, expected_result);
    ASSERT_TRUE(USBH_Device_GetPort(dev) == expected_result,
               "USBH_Device_GetPort(%d) != %d (2nd call)", dev, expected_result);
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_Device_GetStatus
\details
The MW_CV_USBH_Device_GetStatus function verifies the \b USBH_Device_GetStatus function with the following parameter values:
 - device: 0, 1, 255
*/
void MW_CV_USBH_Device_GetStatus(void) {
  uint32_t expected_result;
  uint8_t  dev, i;

  if (USBH_MSC_GetStatus(0U) != usbOK) {
    ASSERT_TRUE(false, "No Mass Storage device available or SD Card not inserted");
    return;
  }

  for (i = 0U; i < sizeof(DEV); i++) {
    dev = DEV[i];
    expected_result = usbDeviceError;
    if (dev == 0U) {
      expected_result = usbOK;
    }
    ASSERT_TRUE(USBH_Device_GetStatus(dev) == expected_result,
               "USBH_Device_GetStatus(%d) != %s",            dev, MW_CW_USB_STATUS_TXT(expected_result));
    ASSERT_TRUE(USBH_Device_GetStatus(dev) == expected_result,
               "USBH_Device_GetStatus(%d) != %s (2nd call)", dev, MW_CW_USB_STATUS_TXT(expected_result));
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_Device_GetSpeed
\details
The MW_CV_USBH_Device_GetSpeed function verifies the \b USBH_Device_GetSpeed function with the following parameter values:
 - device: 0, 1, 255
*/
void MW_CV_USBH_Device_GetSpeed(void) {
  uint8_t expected_result, dev, i;

  if (USBH_MSC_GetStatus(0U) != usbOK) {
    ASSERT_TRUE(false, "No Mass Storage device available or SD Card not inserted");
    return;
  }

  for (i = 0U; i < sizeof(DEV); i++) {
    dev = DEV[i];
    expected_result = USB_SPEED_LOW;
    if (dev == 0U) {
      expected_result = USB_SPEED_FULL;
    }
    ASSERT_TRUE(USBH_Device_GetSpeed(dev) == expected_result,
               "USBH_Device_GetSpeed(%d) != %s",            dev, MW_CW_USB_SPEED_TXT(expected_result));
    ASSERT_TRUE(USBH_Device_GetSpeed(dev) == expected_result,
               "USBH_Device_GetSpeed(%d) != %s (2nd call)", dev, MW_CW_USB_SPEED_TXT(expected_result));
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_Device_GetAddress
\details
The MW_CV_USBH_Device_GetAddress function verifies the \b USBH_Device_GetAddress function with the following parameter values:
 - device: 0, 1, 255
*/
void MW_CV_USBH_Device_GetAddress(void) {
  uint8_t dev, i;

  if (USBH_MSC_GetStatus(0U) != usbOK) {
    ASSERT_TRUE(false, "No Mass Storage device available or SD Card not inserted");
    return;
  }

  for (i = 0U; i < sizeof(DEV); i++) {
    dev = DEV[i];
    if (dev == 0U) {
      ASSERT_TRUE(USBH_Device_GetAddress(dev) <= 127U,
                 "USBH_Device_GetAddress(%d) > 127 %s",         dev);
      ASSERT_TRUE(USBH_Device_GetAddress(dev) <= 127U,
                 "USBH_Device_GetAddress(%d) > 127 (2nd call)", dev);
    } else {
      ASSERT_TRUE(USBH_Device_GetAddress(dev) == 255U,
                 "USBH_Device_GetAddress(%d) != 255",            dev);
      ASSERT_TRUE(USBH_Device_GetAddress(dev) == 255U,
                 "USBH_Device_GetAddress(%d) != 255 (2nd call)", dev);
    }
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_Device_GetVID
\details
The MW_CV_USBH_Device_GetVID function verifies the \b USBH_Device_GetVID function with the following parameter values:
 - device: 0, 1, 255
*/
void MW_CV_USBH_Device_GetVID(void) {
  uint16_t expected_result;
  uint8_t  dev, i;

  if (USBH_MSC_GetStatus(0U) != usbOK) {
    ASSERT_TRUE(false, "No Mass Storage device available or SD Card not inserted");
    return;
  }

  for (i = 0U; i < sizeof(DEV); i++) {
    dev = DEV[i];
    expected_result = 0U;
    if (dev == 0U) {
      expected_result = 0xC251U;
    }
    ASSERT_TRUE(USBH_Device_GetVID(dev) == expected_result,
               "USBH_Device_GetVID(%d) != %0X04",            dev, expected_result);
    ASSERT_TRUE(USBH_Device_GetVID(dev) == expected_result,
               "USBH_Device_GetVID(%d) != %0X04 (2nd call)", dev, expected_result);
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_Device_GetPID
\details
The MW_CV_USBH_Device_GetPID function verifies the \b USBH_Device_GetPID function with the following parameter values:
 - device: 0, 1, 255
*/
void MW_CV_USBH_Device_GetPID(void) {
  uint16_t expected_result;
  uint8_t  dev, i;

  if (USBH_MSC_GetStatus(0U) != usbOK) {
    ASSERT_TRUE(false, "No Mass Storage device available or SD Card not inserted");
    return;
  }

  for (i = 0U; i < sizeof(DEV); i++) {
    dev = DEV[i];
    expected_result = 0U;
    if (dev == 0U) {
      expected_result = 0x5903U;
    }
    ASSERT_TRUE(USBH_Device_GetPID(dev) == expected_result,
               "USBH_Device_GetPID(%d) != %0X04",            dev, expected_result);
    ASSERT_TRUE(USBH_Device_GetPID(dev) == expected_result,
               "USBH_Device_GetPID(%d) != %0X04 (2nd call)", dev, expected_result);
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_Device_GetStringDescriptor
\details
The MW_CV_USBH_Device_GetStringDescriptor function verifies the \b USBH_Device_GetStringDescriptor function by reading
serial number string of attached device.
*/
void MW_CV_USBH_Device_GetStringDescriptor(void) {
  uint8_t dev, i;

  if (USBH_MSC_GetStatus(0U) != usbOK) {
    ASSERT_TRUE(false, "No Mass Storage device available or SD Card not inserted");
    return;
  }

  for (i = 0U; i < sizeof(DEV); i++) {
    dev = DEV[i];
    if (dev == 0U) {
      ASSERT_TRUE(USBH_Device_GetStringDescriptor(0, 3U, 0x0409U, test_data_buf, 26U) == usbOK,
                 "USBH_Device_GetStringDescriptor(0, 3, 0x0409, buf, 30) != usbOK");
      if (memcmp((void *)test_data_buf, (void *)"\x1A\0030\0000\0000\0001\000A\0000\0000\0000\0000\0000\0000\0000\000", 26) != 0) {
        ASSERT_TRUE(false, "USBH_Device_GetStringDescriptor(0, 3, 0x0409, buf, 26) received serial number string is not correct");
      }
      ASSERT_TRUE(USBH_Device_GetStringDescriptor(0, 3U, 0x0409U, test_data_buf, 26U) == usbOK,
                 "USBH_Device_GetStringDescriptor(0, 3, 0x0409, buf, 30) != usbOK (2nd call)");
      if (memcmp((void *)test_data_buf, (void *)"\x1A\0030\0000\0000\0001\000A\0000\0000\0000\0000\0000\0000\0000\000", 26) != 0) {
        ASSERT_TRUE(false, "USBH_Device_GetStringDescriptor(0, 3, 0x0409, buf, 26) received serial number string is not correct (2nd call)");
      }
    } else {
      ASSERT_TRUE(USBH_Device_GetStringDescriptor(dev, 3U, 0x0409U, test_data_buf, 26U) == usbDeviceError,
                 "USBH_Device_GetStringDescriptor(%d, 3, 0x0409, buf, 30) != usbDeviceError", dev);
      ASSERT_TRUE(USBH_Device_GetStringDescriptor(dev, 3U, 0x0409U, test_data_buf, 26U) == usbDeviceError,
                 "USBH_Device_GetStringDescriptor(%d, 3, 0x0409, buf, 30) != usbDeviceError (2nd call)", dev);
    }
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_Notify
\details
The MW_CV_USBH_Notify function verifies the \b USBH_Notify function by checking that notification callback was called
when USB Mass Storage device was detected.
*/
void MW_CV_USBH_Notify(void) {

  ASSERT_TRUE(cb_notify, "USBH_Notify(...) callback was not called");
  if (cb_notify) {
    ASSERT_TRUE(cb_notify_ctrl   == 0U,                "USBH_Notify(ctrl, ...) called with ctrl != 0U");
    ASSERT_TRUE(cb_notify_port   == 0U,                "USBH_Notify(..., port, ...) called with port != 0U");
    ASSERT_TRUE(cb_notify_device == 0U,                "USBH_Notify(..., device,...) called with device != 0U");
    ASSERT_TRUE(cb_notify_notify == USBH_NOTIFY_READY, "USBH_Notify(..., notify) called with notify != USBH_NOTIFY_READY");
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_Uninitialize
\details
The MW_CV_USBH_Uninitialize test function verifies the \b USBH_Uninitialize function with the following parameter values:
 - ctrl: 0, 1, 2, 255
*/
void MW_CV_USBH_Uninitialize(void) {
  uint32_t expected_result;
  uint8_t  ctrl, i;

  for (i = 0U; i < sizeof(CTRL); i++) {
    ctrl = CTRL[i];
    expected_result = usbControllerError;
#if defined(RTE_USB_Host_0)
    if (ctrl == 0U) {
      expected_result = usbOK;
    }
#endif
#if defined(RTE_USB_Host_1)
    if (ctrl == 1U) {
      expected_result = usbOK;
    }
#endif
    ASSERT_TRUE(USBH_Uninitialize(ctrl) == expected_result,
               "USBH_Uninitialize(%d) != %s",            ctrl, MW_CW_USB_STATUS_TXT(expected_result));
    ASSERT_TRUE(USBH_Uninitialize(ctrl) == expected_result,
               "USBH_Uninitialize(%d) != %s (2nd call)", ctrl, MW_CW_USB_STATUS_TXT(expected_result));
  }

  // Uninitialize the USB Device which was used for testing of the USB Host
  USBD_Disconnect  (0U);
  USBD_Uninitialize(0U);
}

/**
@}
*/

#endif
