/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Host
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USBH_HID.c
 * Purpose: MDK Middleware - Component Validation - USB Host -
 *          Human Interface Device - tests module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"
#if     ((MW_CV_USBH != 0U) && (MW_CV_USBH_HID != 0U))

#include "MW_CV_USBH_HID.h"

#include <stdbool.h>
#include <string.h>

#include "MW_CV_TestReport.h"
#include "MW_CV_USB.h"

#include "rl_usb.h"
#include "USBD_User_HID_0.h"


// Local variables used for testing
static const uint8_t INST[3]      = { 0U, 1U, 255U };
static       uint8_t test_data_buf[4] __ALIGNED(4);
static       bool    cb_hid0_init;
static       bool    cb_hid0_uninit;
static       bool    cb_hid0_parse_report_desc;
static       bool    cb_hid0_data_received;


// Callback functions
void USBH_HID_Initialize (uint8_t instance) {
  if (instance == 0U) {
    cb_hid0_init = true;
  }
}

void USBH_HID_Uninitialize (uint8_t instance) {
  if (instance == 0U) {
    cb_hid0_uninit = true;
  }
}

void USBH_HID_ParseReportDescriptor (uint8_t instance, const uint8_t *ptr_hid_report_desc, uint32_t len) {
  (void)ptr_hid_report_desc;

  if ((instance == 0U) && (len == 33U)) {
    cb_hid0_parse_report_desc = true;
  }
}

void USBH_HID_DataReceived (uint8_t instance, uint32_t len) {
  if ((instance == 0U) && (len == 1U)) {
    cb_hid0_data_received = true;
  }
}


/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\defgroup mw_cv_usbh_hid_test_funcs MDK Middleware - Component Validation - USB Host - Human Interface Device
\brief USB Host HID validation test functions
\details
The MDK Middleware Component Validation for USB Host Human Interface Device class checks
USB Host Human Interface Device API as well as USB Host Human Interface Device class functionality.
@{
*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_HID_Initialize
\details
The MW_CV_USBH_HID_Initialize test function verifies the \b USBH_HID_Initialize callback function.
*/
void MW_CV_USBH_HID_Initialize(void) {
  uint32_t timeout_cnt;

  cb_hid0_init              = false;
  cb_hid0_uninit            = false;
  cb_hid0_parse_report_desc = false;
  cb_hid0_data_received     = false;

  // Initialize the USB Host
  USBH_Initialize(0U);
  osDelay(100U);

  // Initialize the USB Device which will be used for testing of the USB Host
  USBD_Initialize(0U);
  USBD_Connect   (0U);

  // Wait for device to be detected as most of following tests depend on it
  for (timeout_cnt = (MW_CV_USB_TIMEOUT_IN_SEC * 10U); timeout_cnt > 0U; timeout_cnt--) {
    if (USBH_HID_GetStatus(0U) == usbOK) {
      break;
    }
    osDelay(100U);
  }
  if (timeout_cnt == 0U) {
    ASSERT_TRUE(false, "USB0 not connected to USB1, necessary for further tests");
    return;
  }

  ASSERT_TRUE(cb_hid0_init, "USBH_HID_Initialize(0) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_HID_GetDevice
\details
The MW_CV_USBH_HID_GetDevice function verifies the \b USBH_HID_GetDevice function with the following parameter values:
 - instance: 0, 1, 255
*/
void MW_CV_USBH_HID_GetDevice(void) {
  uint8_t expected_result, inst, i;

  for (i = 0U; i < sizeof(INST); i++) {
    inst = INST[i];
    expected_result = 255U;
    if (inst == 0U) {
      expected_result = 0U;
    }
    ASSERT_TRUE(USBH_HID_GetDevice(inst) == expected_result,
               "USBH_HID_GetDevice(%d) != %d",            inst, expected_result);
    ASSERT_TRUE(USBH_HID_GetDevice(inst) == expected_result,
               "USBH_HID_GetDevice(%d) != %d (2nd call)", inst, expected_result);
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_HID_GetStatus
\details
The MW_CV_USBH_HID_GetStatus function verifies the \b USBH_HID_GetStatus function with the following parameter values:
 - instance: 0, 1, 255
*/
void MW_CV_USBH_HID_GetStatus(void) {
  uint32_t expected_result;
  uint8_t  inst, i;

  for (i = 0U; i < sizeof(INST); i++) {
    inst = INST[i];
    expected_result = usbClassErrorHID;
    if (inst == 0U) {
      expected_result = usbOK;
    }
    ASSERT_TRUE(USBH_HID_GetStatus(inst) == expected_result,
               "USBH_HID_GetStatus(%d) != %s",            inst, MW_CW_USB_STATUS_TXT(expected_result));
    ASSERT_TRUE(USBH_HID_GetStatus(inst) == expected_result,
               "USBH_HID_GetStatus(%d) != %s (2nd call)", inst, MW_CW_USB_STATUS_TXT(expected_result));
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_HID_Write_Read
\details
The MW_CV_USBH_HID_Write_Read test function verifies the \b USBH_HID_Write and \b USBH_HID_Read of the
USB Host Human Interface Device class.
USB Host writes a test string, then reads it back and verifies contents of returned string.
*/
void MW_CV_USBH_HID_Write_Read(void) {

  if (USBH_HID_Write(0U, (const uint8_t *)"W", 1U) != 1) {
    ASSERT_TRUE(false, "USBH_HID_Write(0, ...) != 1");
    return;
  }
  osDelay(100U);

  if (USBH_HID_Read(0U, test_data_buf, 1U) != 1) {
    ASSERT_TRUE(false, "USBH_HID_Read(0, ...) != 1");
    return;
  }

  ASSERT_TRUE(test_data_buf[0] == 'W', "USBH_HID_Write/Read data is not valid");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_HID_GetKeyboardKey
\details
The MW_CV_USBH_HID_GetKeyboardKey function verifies the \b USBH_HID_GetKeyboardKey function.
Note: Not implemented as as it requires USB Device keyboard implementation for test.
*/
void MW_CV_USBH_HID_GetKeyboardKey(void) {
  DETAIL_INFO("This test case is not implemented");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_HID_GetMouseState
\details
The MW_CV_USBH_HID_GetMouseState function verifies the \b USBH_HID_GetMouseState function.
Note: Not implemented as as it requires USB Device mouse implementation for test.
*/
void MW_CV_USBH_HID_GetMouseState(void) {
  DETAIL_INFO("This test case is not implemented");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_HID_ParseReportDescriptor
\details
The MW_CV_USBH_HID_ParseReportDescriptor test function verifies the \b USBH_HID_ParseReportDescriptor callback function.
*/
void MW_CV_USBH_HID_ParseReportDescriptor (void) {
  ASSERT_TRUE(cb_hid0_parse_report_desc, "USBH_HID_ParseReportDescriptor(0, ...) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_HID_DataReceived
\details
The MW_CV_USBH_HID_DataReceived test function verifies the \b USBH_HID_DataReceived callback function.
*/
void MW_CV_USBH_HID_DataReceived (void) {

  USBH_HID_Read(0U, test_data_buf, 1U);
  osDelay(100U);

  ASSERT_TRUE(cb_hid0_data_received, "USBH_HID_DataReceived(0, ...) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_HID_Uninitialize
\details
The MW_CV_USBH_HID_Uninitialize test function verifies the \b USBH_HID_Uninitialize callback function.
*/
void MW_CV_USBH_HID_Uninitialize(void) {

  // Uninitialize the USB Device which was used for testing of the USB Host
  USBD_Disconnect  (0U);
  USBD_Uninitialize(0U);
  osDelay(100U);

  // Uninitialize the USB Host
  USBH_Uninitialize(0U);
  osDelay(100U);

  ASSERT_TRUE(cb_hid0_uninit, "USBH_HID_Uninitialize(0) callback was not called");
}

/**
@}
*/

#endif
