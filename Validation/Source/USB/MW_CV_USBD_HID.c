/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Device
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USBD_HID.c
 * Purpose: MDK Middleware - Component Validation - USB Device -
 *          Human Interface Device - tests module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"
#if     ((MW_CV_USBD != 0U) && (MW_CV_USBD_HID != 0U))

#include "MW_CV_USBD_HID.h"

#include <stdbool.h>
#include <string.h>

#include "MW_CV_TestReport.h"
#include "MW_CV_USB.h"

#include "rl_usb.h"
#include "USBD_User_HID_0.h"


// Local variables used for testing
static uint8_t test_data_buf[4] __ALIGNED(4);


/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\defgroup mw_cv_usbd_hid_test_funcs MDK Middleware - Component Validation - USB Device - Human Interface Device class
\brief USB Device HID validation test functions
\details
The MDK Middleware Component Validation for USB Device Human Interface Device class checks
USB Device Human Interface Device API as well as USB Device Human Interface Device class functionality.
@{
*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_HID0_Initialize
\details
The MW_CV_USBD_HID0_Initialize test function verifies the \b USBD_HID0_Initialize callback function.
*/
void MW_CV_USBD_HID0_Initialize(void) {
  uint32_t timeout_cnt;

  // Initialize global structure used for testing control and results
  memset(&mw_cv_usbd_hid0, 0, sizeof(MW_CV_USBD_HID_t));

  mw_cv_usbd_hid0.testing_active = true;        // Testing active
  mw_cv_usbd_hid0.echo_active    = true;        // Echo data active

  // Initialize the USB Host which will be used for testing of the USB Device
  USBH_Initialize(0U);
  osDelay(100U);

  // Initialize and connect the USB Device
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

  ASSERT_TRUE(mw_cv_usbd_hid0.cb_initialize, "USBD_HID0_Initialize() callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_HID0_GetReport
\details
The MW_CV_USBD_HID0_GetReport test function verifies the \b USBD_HID0_GetReport callback function.
*/
void MW_CV_USBD_HID0_GetReport (void) {

  USBD_HID_GetReportTrigger (0U, 0U, (const uint8_t *)"G", 1U);
  osDelay(100U);

  USBH_HID_Read(0U, test_data_buf, 1U);

  ASSERT_TRUE(mw_cv_usbd_hid0.cb_get_report,  "USBD_HID0_GetReport(...) callback was not called");
  ASSERT_TRUE(test_data_buf[0] == 'G', "USBD_HID0_GetReport(...) returned wrong data");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_HID0_SetReport
\details
The MW_CV_USBD_HID0_SetReport test function verifies the \b USBD_HID0_SetReport callback function.
*/
void MW_CV_USBD_HID0_SetReport (void) {

  USBH_HID_Write(0U, (const uint8_t *)"S", 1U);
  osDelay(100U);

  ASSERT_TRUE(mw_cv_usbd_hid0.cb_get_report, "USBD_HID0_SetReport(...) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_HID_GetReportTrigger
\details
The MW_CV_USBD_HID_GetReportTrigger test function verifies the \b USBD_HID_GetReportTrigger function.
*/
void MW_CV_USBD_HID_GetReportTrigger(void) {

  ASSERT_TRUE(USBD_HID_GetReportTrigger(0U, 0U, (const uint8_t *)"T", 1U) == usbOK,
             "USBD_HID_GetReportTrigger(0, ...) != usbOK");
  osDelay(100U);

  USBH_HID_Read(0U, test_data_buf, 1U);

  if (test_data_buf[0] != 'T') {
    ASSERT_TRUE(false, "USBD_HID_GetReportTrigger(0, ...) sent data is not as expected");
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_HID0_Uninitialize
\details
The MW_CV_USBD_HID0_Uninitialize test function verifies the \b USBD_HID0_Uninitialize callback function.
*/
void MW_CV_USBD_HID0_Uninitialize(void) {

  // Uninitialize the USB Device
  USBD_Disconnect  (0U);
  USBD_Uninitialize(0U);
  osDelay(100U);

  // Uninitialize the USB Host which was used for testing of the USB Device
  USBH_Uninitialize(0U);
  osDelay(100U);

  ASSERT_TRUE(mw_cv_usbd_hid0.cb_uninitialize, "USBD_HID0_Uninitialize() callback was not called");

  mw_cv_usbd_hid0.testing_active = false;       // Testing inactive
}

/**
@}
*/

#endif
