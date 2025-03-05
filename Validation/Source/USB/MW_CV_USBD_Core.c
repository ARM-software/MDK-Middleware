/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Device
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USBD_Core.c
 * Purpose: MDK Middleware - Component Validation - USB Device - Core -
 *          tests module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"
#if     ((MW_CV_USBD != 0U) && (MW_CV_USBD_CORE != 0U))

#include "MW_CV_USBD_Core.h"

#include <stdbool.h>
#include <string.h>

#include "RTE_Components.h"

#include "MW_CV_TestReport.h"
#include "MW_CV_USB.h"

#include "rl_usb.h"
#include "rl_fs.h"
#include "USBD_User_Device_0.h"
#include "USBH_MSC.h"


// Local variables used for testing
static const uint8_t DEV[4] = { 0U, 1U, 2U, 255U };
static       uint8_t test_data_buf[32] __ALIGNED(4);


/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\defgroup mw_cv_usbd_core_test_funcs MDK Middleware - Component Validation - USB Device - Core
\brief USB Device Core validation test functions
\details
The MDK Middleware Component Validation for USB Device Core checks USB Device Core API functions and callbacks.
@{
*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_GetVersion
\details
The MW_CV_USBD_GetVersion test function verifies the \b USBD_GetVersion it expects that
version of USB Device component is at least 6.0.0.
*/
void MW_CV_USBD_GetVersion(void) {
  uint32_t ver, major, minor, patch;

  ver   =  USBD_GetVersion();
  major =  ver / 10000000U;
  minor = (ver % 10000000U) / 10000U;
  patch = (ver %    10000U);

  DETAIL_INFO("Reported component version %d.%d.%d", major, minor, patch);

  ASSERT_TRUE(ver >= 60000000U, "Expected version at least 6.0.0");

  // Initialize global structure used for testing control and results
  memset(&mw_cv_usbd_core, 0, sizeof(MW_CV_USBD_CORE_t));
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_Initialize
\details
The MW_CV_USBD_Initialize test function verifies the \b USBD_Initialize function with the following parameter values:
 - device: 0, 1, 2, 255
*/
void MW_CV_USBD_Initialize(void) {
  uint32_t expected_result;
  uint8_t  dev, i;

  for (i = 0U; i < sizeof(DEV); i++) {
    dev = DEV[i];
    expected_result = usbDeviceError;
#if defined(RTE_USB_Device_0)
    if (dev == 0U) {
      expected_result = usbOK;
    }
#endif
#if defined(RTE_USB_Device_1)
    if (dev == 1U) {
      expected_result = usbOK;
    }
#endif
    ASSERT_TRUE(USBD_Initialize(dev) == expected_result,
               "USBD_Initialize(%d) != %s",            dev, MW_CW_USB_STATUS_TXT(expected_result));
    ASSERT_TRUE(USBD_Initialize(dev) == expected_result,
               "USBD_Initialize(%d) != %s (2nd call)", dev, MW_CW_USB_STATUS_TXT(expected_result));
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_Connect
\details
The MW_CV_USBD_Connect function verifies the \b USBD_Connect function with the following parameter values:
 - device: 0, 1, 2, 255
*/
void MW_CV_USBD_Connect(void) {
  uint32_t timeout_cnt, expected_result;
  uint8_t  dev, i;

  for (i = 0U; i < sizeof(DEV); i++) {
    dev = DEV[i];
    expected_result = usbDeviceError;
#if defined(RTE_USB_Device_0)
    if (dev == 0U) {
      expected_result = usbOK;
    }
#endif
#if defined(RTE_USB_Device_1)
    if (dev == 1U) {
      expected_result = usbOK;
    }
#endif
    ASSERT_TRUE(USBD_Connect(dev) == expected_result,
               "USBD_Connect(%d) != %s",            dev, MW_CW_USB_STATUS_TXT(expected_result));
    ASSERT_TRUE(USBD_Connect(dev) == expected_result,
               "USBD_Connect(%d) != %s (2nd call)", dev, MW_CW_USB_STATUS_TXT(expected_result));
  }

  // Initialize the USB Host which will be used for testing of the USB Device
  USBH_Initialize(0U);

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
\brief  Test: MW_CV_USBD_Configured
\details
The MW_CV_USBD_Configured function verifies the \b USBD_Configured function with the following parameter values:
 - device: 0, 1, 2, 255
*/
void MW_CV_USBD_Configured(void) {
  bool    expected_result;
  uint8_t dev, i;

  for (i = 0U; i < sizeof(DEV); i++) {
    dev = DEV[i];
    expected_result = false;
#if defined(RTE_USB_Device_0)
    if (dev == 0U) {
      expected_result = true;
    }
#endif
#if defined(RTE_USB_Device_1)
    if (dev == 1U) {
      expected_result = true;
    }
#endif
    ASSERT_TRUE(USBD_Configured(dev) == expected_result,
               "USBD_Configured(%d) != %s",            dev, MW_CW_USB_BOOL_TXT(expected_result));
    ASSERT_TRUE(USBD_Configured(dev) == expected_result,
               "USBD_Configured(%d) != %s (2nd call)", dev, MW_CW_USB_BOOL_TXT(expected_result));
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_GetState
\details
The MW_CV_USBD_GetState function verifies the \b USBD_GetState function with the following parameter values:
 - device: 0, 1, 2, 255
*/
void MW_CV_USBD_GetState(void) {
  uint8_t expected_result;
  uint8_t dev, i;

  for (i = 0U; i < sizeof(DEV); i++) {
    dev = DEV[i];
    expected_result = 0U;
#if defined(RTE_USB_Device_0)
    if (dev == 0U) {
      expected_result = 1U;
    }
#endif
#if defined(RTE_USB_Device_1)
    if (dev == 1U) {
      expected_result = 1U;
    }
#endif
    ASSERT_TRUE(USBD_GetState(dev).active == expected_result,
               "USBD_GetState(%d).active != %d",            dev, expected_result);
    ASSERT_TRUE(USBD_GetState(dev).active == expected_result,
               "USBD_GetState(%d).active != %d (2nd call)", dev, expected_result);
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_SetSerialNumber
\details
The MW_CV_USBD_SetSerialNumber function verifies the \b USBD_SetSerialNumber function with the following parameter values:
 - device: 0, 1, 2, 255
*/
void MW_CV_USBD_SetSerialNumber(void) {
  uint8_t dev, i;

  for (i = 0U; i < sizeof(DEV); i++) {
    dev = DEV[i];
    if (dev == 0U) {
      USBH_Device_GetStringDescriptor(0U, 3U, 0x0409U, test_data_buf, 26U);
      if (memcmp((void *)test_data_buf, (void *)"\x1A\0030\0000\0000\0001\000A\0000\0000\0000\0000\0000\0000\0000\000", 26) != 0) {
        ASSERT_TRUE(false, "Initial serial number string different than 0001A0000000");
      }
      ASSERT_TRUE(USBD_SetSerialNumber(0U, "0001A0000001") == usbOK,
                 "USBD_SetSerialNumber(0, \"0001A0000001\") != usbOK");
      USBH_Device_GetStringDescriptor(0U, 3U, 0x0409U, test_data_buf, 26U);
      if (memcmp((void *)test_data_buf, (void *)"\x1A\0030\0000\0000\0001\000A\0000\0000\0000\0000\0000\0000\0001\000", 26) != 0) {
        ASSERT_TRUE(false, "Serial number string different than set 0001A0000001");
      }
    } else {
      ASSERT_TRUE(USBD_SetSerialNumber(dev, "0001A0000001") != usbOK,
                 "USBD_SetSerialNumber(%d, \"0001A0000001\") == usbOK", dev);
    }
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_Disconnect
\details
The MW_CV_USBD_Disconnect function verifies the \b USBD_Disconnect function with the following parameter values:
 - device: 0, 1, 2, 255
*/
void MW_CV_USBD_Disconnect(void) {
  uint32_t expected_result;
  uint8_t  dev, i;

  for (i = 0U; i < sizeof(DEV); i++) {
    dev = DEV[i];
    expected_result = usbDeviceError;
#if defined(RTE_USB_Device_0)
    if (dev == 0U) {
      expected_result = usbOK;
    }
#endif
#if defined(RTE_USB_Device_1)
    if (dev == 1U) {
      expected_result = usbOK;
    }
#endif
    ASSERT_TRUE(USBD_Disconnect(dev) == expected_result,
               "USBD_Disconnect(%d) != %s",            dev, MW_CW_USB_STATUS_TXT(expected_result));
    ASSERT_TRUE(USBD_Disconnect(dev) == expected_result,
               "USBD_Disconnect(%d) != %s (2nd call)", dev, MW_CW_USB_STATUS_TXT(expected_result));
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_Uninitialize
\details
The MW_CV_USBD_Uninitialize test function verifies the \b USBD_Uninitialize function with the following parameter values:
 - device: 0, 1, 2, 255
*/
void MW_CV_USBD_Uninitialize(void) {
  uint32_t expected_result;
  uint8_t  dev, i;

  for (i = 0U; i < sizeof(DEV); i++) {
    dev = DEV[i];
    expected_result = usbDeviceError;
#if defined(RTE_USB_Device_0)
    if (dev == 0U) {
      expected_result = usbOK;
    }
#endif
#if defined(RTE_USB_Device_1)
    if (dev == 1U) {
      expected_result = usbOK;
    }
#endif
    ASSERT_TRUE(USBD_Uninitialize(dev) == expected_result,
               "USBD_Uninitialize(%d) != %s",            dev, MW_CW_USB_STATUS_TXT(expected_result));
    ASSERT_TRUE(USBD_Uninitialize(dev) == expected_result,
               "USBD_Uninitialize(%d) != %s (2nd call)", dev, MW_CW_USB_STATUS_TXT(expected_result));
  }
  osDelay(100U);

  // Uninitialize the USB Host which was used for testing of the USB Device
  USBH_Uninitialize(0U);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_Device0_Initialize
\details
The MW_CV_USBD_Device0_Initialize test function verifies the \b USBD_Device0_Initialize callback function.
*/
void MW_CV_USBD_Device0_Initialize(void) {
  uint32_t timeout_cnt;

  mw_cv_usbd_core.testing_active = true;        // Testing active

  // Initialize the USB Host which will be used for testing of the USB Device callbacks
  USBH_Initialize(0U);
  osDelay(100U);

  // Initialize and connect USB Device
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

  ASSERT_TRUE(mw_cv_usbd_core.cb_initialize, "USBD_Device0_Initialize(0) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_Device0_VbusChanged
\details
The MW_CV_USBD_Device0_VbusChanged test function verifies the \b USBD_Device0_VbusChanged callback function.
*/
void MW_CV_USBD_Device0_VbusChanged(void) {
  if (mw_cv_usbd_core.cb_vbus_changed) {
    ASSERT_TRUE(true, "");
  } else {
    DETAIL_INFO("USBD_Device0_VbusChanged(0) callback was not called, possibly driver does not support VBUS change detection");
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_Device0_Reset
\details
The MW_CV_USBD_Device0_Reset test function verifies the \b USBD_Device0_Reset callback function.
*/
void MW_CV_USBD_Device0_Reset(void) {
  ASSERT_TRUE(mw_cv_usbd_core.cb_reset, "USBD_Device0_Reset(0) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_Device0_HighSpeedActivated
\details
The MW_CV_USBD_Device0_HighSpeedActivated test function verifies the \b USBD_Device0_HighSpeedActivated callback function.
*/
void MW_CV_USBD_Device0_HighSpeedActivated  (void) {
  if (mw_cv_usbd_core.cb_hs_activated) {
    ASSERT_TRUE(true, "");
  } else {
    DETAIL_INFO("USBD_Device0_HighSpeedActivated(0) callback was not called, possibly only one port is high-speed capable");
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_Device0_Suspended
\details
The MW_CV_USBD_Device0_Suspended test function verifies the \b USBD_Device0_Suspended callback function.
*/
void MW_CV_USBD_Device0_Suspended(void) {

  USBH_Port_Suspend(0U, 0U);
  osDelay(1000U);

  ASSERT_TRUE(mw_cv_usbd_core.cb_suspended, "USBD_Device0_Suspended(0) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_Device0_Resumed
\details
The MW_CV_USBD_Device0_Resumed test function verifies the \b USBD_Device0_Resumed callback function.
*/
void MW_CV_USBD_Device0_Resumed(void) {

  USBH_Port_Resume(0U, 0U);
  osDelay(100U);

  if (mw_cv_usbd_core.cb_resumed) {
    ASSERT_TRUE(true, "");
  } else {
    DETAIL_INFO("USBD_Device0_Resumed(0) callback was not called, possibly driver does not support resume signal detection");
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_Device0_ConfigurationChanged
\details
The MW_CV_USBD_Device0_ConfigurationChanged test function verifies the \b USBD_Device0_ConfigurationChanged callback
function.
*/
void MW_CV_USBD_Device0_ConfigurationChanged(void) {
  ASSERT_TRUE(mw_cv_usbd_core.cb_conf_changed, "USBD_Device0_ConfigurationChanged(0) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_Device0_EnableRemoteWakeup
\details
The MW_CV_USBD_Device0_EnableRemoteWakeup test function verifies the \b USBD_Device0_EnableRemoteWakeup callback function.
*/
void MW_CV_USBD_Device0_EnableRemoteWakeup(void) {

  // Send Set Feature DEVICE REMOTE WAKEUP request
  USBH_DeviceRequest_SetFeature(0U, USB_REQUEST_TO_DEVICE, 0U, 1U);
  osDelay(100U);

  if (mw_cv_usbd_core.cb_enable_remote_wakeup) {
    ASSERT_TRUE(true, "");
  } else {
    DETAIL_INFO("USBD_Device0_EnableRemoteWakeup(0) callback was not called, possibly driver does not support remote wakeup");
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_Device0_DisableRemoteWakeup
\details
The MW_CV_USBD_Device0_DisableRemoteWakeup test function verifies the \b USBD_Device0_DisableRemoteWakeup callback function.
*/
void MW_CV_USBD_Device0_DisableRemoteWakeup(void) {

  // Send Clear Feature DEVICE REMOTE WAKEUP request
  USBH_DeviceRequest_ClearFeature (0U, USB_REQUEST_TO_DEVICE, 0U, 1U);
  osDelay(100U);

  if (mw_cv_usbd_core.cb_enable_remote_wakeup) {
    ASSERT_TRUE(true, "");
  } else {
    DETAIL_INFO("USBD_Device0_DisableRemoteWakeup(0) callback was not called, possibly driver does not support remote wakeup");
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_Device0_Endpoint0_SetupPacketReceived
\details
The MW_CV_USBD_Device0_Endpoint0_SetupPacketReceived test function verifies the
\b USBD_Device0_Endpoint0_SetupPacketReceived callback function.
*/
void MW_CV_USBD_Device0_Endpoint0_SetupPacketReceived(void) {

  USBH_DeviceRequest_SetDescriptor(0U, USB_REQUEST_TO_DEVICE, 3U, 3U, 0x0409U,
                                  (const uint8_t *)"\x1A\0030\0000\0000\0001\000A\0000\0000\0000\0000\0000\0000\0002\000", 26U);
  osDelay(100U);
  USBH_Device_GetStringDescriptor (0U, 3U, 0x0409U, test_data_buf, 26U);
  osDelay(100U);

  ASSERT_TRUE(mw_cv_usbd_core.cb_ep0_setup_set_received,
             "USBD_Device0_Endpoint0_SetupPacketReceived(...) callback was not called for SetDescriptor request");
  ASSERT_TRUE(mw_cv_usbd_core.cb_ep0_setup_get_received,
             "USBD_Device0_Endpoint0_SetupPacketReceived(...) callback was not called for GetDescriptor request");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_Device0_Endpoint0_SetupPacketProcessed
\details
The MW_CV_USBD_Device0_Endpoint0_SetupPacketProcessed test function verifies the
\b USBD_Device0_Endpoint0_SetupPacketProcessed callback function.
*/
void MW_CV_USBD_Device0_Endpoint0_SetupPacketProcessed(void) {
  ASSERT_TRUE(mw_cv_usbd_core.cb_ep0_setup_pocessed,
             "USBD_Device0_Endpoint0_SetupPacketProcessed(...) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_Device0_Endpoint0_OutDataReceived
\details
The MW_CV_USBD_Device0_Endpoint0_OutDataReceived test function verifies the \b USBD_Device0_Endpoint0_OutDataReceived
callback function.
*/
void MW_CV_USBD_Device0_Endpoint0_OutDataReceived(void) {
  ASSERT_TRUE(mw_cv_usbd_core.cb_ep0_out_data_received,
             "USBD_Device0_Endpoint0_OutDataReceived(...) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_Device0_Endpoint0_InDataSent
\details
The MW_CV_USBD_Device0_Endpoint0_InDataSent test function verifies the \b USBD_Device0_Endpoint0_InDataSent
callback function.
*/
void MW_CV_USBD_Device0_Endpoint0_InDataSent(void) {
  ASSERT_TRUE(mw_cv_usbd_core.cb_ep0_in_data_sent,
             "USBD_Device0_Endpoint0_InDataSent(...) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_Device0_Uninitialize
\details
The MW_CV_USBD_Device0_Uninitialize test function verifies the \b USBD_Device0_Uninitialize callback function.
*/
void MW_CV_USBD_Device0_Uninitialize(void) {

  // Uninitialize the USB Device
  USBD_Disconnect  (0U);
  USBD_Uninitialize(0U);
  osDelay(100U);

  // Uninitialize the USB Host which was used for testing of the USB Device
  USBH_Uninitialize(0U);
  osDelay(100U);

  ASSERT_TRUE(mw_cv_usbd_core.cb_uninitialize, "USBD_Device0_Uninitialize(0) callback was not called");

  mw_cv_usbd_core.testing_active = false;       // Testing inactive
}

/**
@}
*/

#endif
