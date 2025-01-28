/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Device
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USBD_CDC_ACM.c
 * Purpose: MDK Middleware - Component Validation - USB Device -
 *          Communication Device Class - Abstract Control Model -
 *          tests module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"
#if     ((MW_CV_USBD != 0U) && (MW_CV_USBD_CDC_ACM != 0U))

#include "MW_CV_USBD_CDC_ACM.h"

#include <stdbool.h>
#include <string.h>

#include "MW_CV_TestReport.h"
#include "MW_CV_USB.h"

#include "rl_usb.h"
#include "USBD_User_CDC_ACM_0.h"


// Local variables used for testing
static const char            test_data[]        = "USB Device CDC ACM Validation Test!";
static const CDC_LINE_CODING line_coding_115200 = { 115200U, 0U, 0U, 8U };
static       uint8_t         test_data_buf[4096] __ALIGNED(4);
static       bool            cb_usbh_notify;


// USB Host callback functions used for validation of some tests
void USBH_CDC_ACM_Notify (uint8_t instance, uint16_t status) {
  if ((instance == 0U) && (status == 0x7FU)) {
    cb_usbh_notify = true;
  }
}


/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\defgroup mw_cv_usbd_cdc_acm_test_funcs MDK Middleware - Component Validation - USB Device - Communication Device Class -
Abstract Control Model
\brief USB Device CDC ACM validation test functions
\details
The MDK Middleware Component Validation for USB Device Communication Device Class checks
USB Device Communication Device Class API as well as USB Device Communication Device Class functionality.
@{
*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_CDC0_ACM_Initialize
\details
The MW_CV_USBD_CDC0_ACM_Initialize test function verifies the \b USBD_CDC0_ACM_Initialize callback function.
*/
void MW_CV_USBD_CDC0_ACM_Initialize(void) {
  uint32_t timeout_cnt;

  // Initialize global structure used for testing control and results
  memset(&mw_cv_usbd_cdc0_acm, 0, sizeof(MW_CV_USBD_CDC_ACM_t));
  cb_usbh_notify = false;

  mw_cv_usbd_cdc0_acm.testing_active = true;    // Testing active
  mw_cv_usbd_cdc0_acm.echo_active    = true;    // Echo data active

  // Initialize the USB Host which will be used for testing of the USB Device
  USBH_Initialize(0U);
  osDelay(100U);

  // Initialize and connect the USB Device
  USBD_Initialize(0U);
  USBD_Connect   (0U);

  // Wait for device to be detected as most of following tests depend on it
  for (timeout_cnt = (MW_CV_USB_TIMEOUT_IN_SEC * 10U); timeout_cnt > 0U; timeout_cnt--) {
    if (USBH_CDC_ACM_GetStatus(0U) == usbOK) {
      break;
    }
    osDelay(100U);
  }
  if (timeout_cnt == 0U) {
    ASSERT_TRUE(false, "USB0 not connected to USB1, necessary for further tests");
    return;
  }

  ASSERT_TRUE(mw_cv_usbd_cdc0_acm.cb_initialize, "USBD_CDC0_ACM_Initialize() callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_CDC0_ACM_Reset
\details
The MW_CV_USBD_CDC0_ACM_Reset test function verifies the \b USBD_CDC0_ACM_Reset callback function.
*/
void MW_CV_USBD_CDC0_ACM_Reset (void) {
  ASSERT_TRUE(mw_cv_usbd_cdc0_acm.cb_reset, "USBD_CDC0_ACM_Reset() callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_CDC0_ACM_SetLineCoding
\details
The MW_CV_USBD_CDC0_ACM_SetLineCoding test function verifies the \b USBD_CDC0_ACM_SetLineCoding callback function.
*/
void MW_CV_USBD_CDC0_ACM_SetLineCoding (void) {

  USBH_CDC_ACM_SetLineCoding(0U, &line_coding_115200);
  osDelay(100U);

  ASSERT_TRUE(mw_cv_usbd_cdc0_acm.cb_set_line_coding, "USBD_CDC0_ACM_SetLineCoding(...) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_CDC0_ACM_GetLineCoding
\details
The MW_CV_USBD_CDC0_ACM_GetLineCoding test function verifies the \b USBD_CDC0_ACM_GetLineCoding callback function.
*/
void MW_CV_USBD_CDC0_ACM_GetLineCoding (void) {
  CDC_LINE_CODING line_coding;

  USBH_CDC_ACM_GetLineCoding(0U, &line_coding);
  osDelay(100U);

  ASSERT_TRUE(mw_cv_usbd_cdc0_acm.cb_set_line_coding, "USBD_CDC0_ACM_GetLineCoding(...) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_CDC0_ACM_SetControlLineState
\details
The MW_CV_USBD_CDC0_ACM_SetControlLineState test function verifies the \b USBD_CDC0_ACM_SetControlLineState callback function.
*/
void MW_CV_USBD_CDC0_ACM_SetControlLineState (void) {

  USBH_CDC_ACM_SetControlLineState(0U, 3U);
  osDelay(100U);

  ASSERT_TRUE(mw_cv_usbd_cdc0_acm.cb_set_control_line_state, "USBD_CDC0_ACM_SetControlLineState(...) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_CDC0_ACM_DataReceived
\details
The MW_CV_USBD_CDC0_ACM_DataReceived test function verifies the \b USBD_CDC0_ACM_DataReceived callback function.
*/
void MW_CV_USBD_CDC0_ACM_DataReceived (void) {

  USBH_CDC_ACM_Send(0U, (uint8_t *)"USB Device CDC ACM Validation Test!", 34U);
  osDelay(100U);

  ASSERT_TRUE(mw_cv_usbd_cdc0_acm.cb_data_received, "USBD_CDC0_ACM_DataReceived(...) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_CDC0_ACM_DataSent
\details
The MW_CV_USBD_CDC0_ACM_DataSent test function verifies the \b USBD_CDC0_ACM_DataSent callback function.
*/
void MW_CV_USBD_CDC0_ACM_DataSent (void) {

  USBH_CDC_ACM_Receive(0U, test_data_buf, 64U);
  osDelay(100U);

  ASSERT_TRUE(mw_cv_usbd_cdc0_acm.cb_data_sent, "USBD_CDC0_ACM_DataSent() callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_CDC_ACM_GetChar
\details
The MW_CV_USBD_CDC_ACM_GetChar test function verifies the \b USBD_CDC_ACM_GetChar function.
*/
void MW_CV_USBD_CDC_ACM_GetChar(void) {
  volatile uint8_t ch;

  mw_cv_usbd_cdc0_acm.echo_active = false;      // Echo data inactive

  USBH_CDC_ACM_Send(0U, (uint8_t *)"USB", 3U);
  osDelay(100U);

  ch = USBD_CDC_ACM_GetChar(0U);
  ASSERT_TRUE(ch == 'U', "USBD_CDC_ACM_GetChar(...) != \'U\'");
  ch = USBD_CDC_ACM_GetChar(0U);
  ASSERT_TRUE(ch == 'S', "USBD_CDC_ACM_GetChar(...) != \'S\'");
  ch = USBD_CDC_ACM_GetChar(0U);
  ASSERT_TRUE(ch == 'B', "USBD_CDC_ACM_GetChar(...) != \'B\'");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_CDC_ACM_PutChar
\details
The MW_CV_USBD_CDC_ACM_PutChar test function verifies the \b USBD_CDC_ACM_PutChar function.
*/
void MW_CV_USBD_CDC_ACM_PutChar(void) {

  mw_cv_usbd_cdc0_acm.echo_active = false;      // Echo data inactive

  ASSERT_TRUE(USBD_CDC_ACM_PutChar(0U, 'U') == 'U', "USBD_CDC_ACM_PutChar(0U, \'U\') != \'U\'");
  USBH_CDC_ACM_Receive(0U, test_data_buf, 1U);
  ASSERT_TRUE(test_data_buf[0] == 'U',       "USBD_CDC_ACM_PutChar(0U, \'U\'), received response != \'U\'");
  ASSERT_TRUE(USBD_CDC_ACM_PutChar(0U, 'S') == 'S', "USBD_CDC_ACM_PutChar(0U, \'S\') != \'S\'");
  USBH_CDC_ACM_Receive(0U, test_data_buf, 1U);
  ASSERT_TRUE(test_data_buf[0] == 'S',       "USBD_CDC_ACM_PutChar(0U, \'U\'), received response != \'S\'");
  ASSERT_TRUE(USBD_CDC_ACM_PutChar(0U, 'B') == 'B', "USBD_CDC_ACM_PutChar(0U, \'B\') != \'B\'");
  USBH_CDC_ACM_Receive(0U, test_data_buf, 1U);
  ASSERT_TRUE(test_data_buf[0] == 'B',       "USBD_CDC_ACM_PutChar(0U, \'U\'), received response != \'B\'");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_CDC_ACM_ReadData
\details
The MW_CV_USBD_CDC_ACM_ReadData test function verifies the \b USBD_CDC_ACM_ReadData function.
*/
void MW_CV_USBD_CDC_ACM_ReadData(void) {

  mw_cv_usbd_cdc0_acm.echo_active = false;      // Echo data inactive

  USBH_CDC_ACM_Send(0U, (uint8_t *)test_data, sizeof(test_data));
  osDelay(100U);

  ASSERT_TRUE(USBD_CDC_ACM_ReadData(0U, test_data_buf,   sizeof(test_data)) == sizeof(test_data),
             "USBD_CDC_ACM_ReadData(0, ...) != %d", sizeof(test_data));
  if (memcmp((void *)test_data_buf, (void *)test_data, sizeof(test_data)) != 0) {
    ASSERT_TRUE(false, "USBD_CDC_ACM_ReadData(0, ...) data is not as expected");
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_CDC_ACM_WriteData
\details
The MW_CV_USBD_CDC_ACM_WriteData test function verifies the \b USBD_CDC_ACM_WriteData function.
*/
void MW_CV_USBD_CDC_ACM_WriteData(void) {

  mw_cv_usbd_cdc0_acm.echo_active = false;      // Echo data inactive

  ASSERT_TRUE(USBD_CDC_ACM_WriteData(0U, (const uint8_t *)test_data,  sizeof(test_data)) == sizeof(test_data),
             "USBD_CDC_ACM_WriteData(0, ...) != %d", sizeof(test_data));
  osDelay(100U);

  USBH_CDC_ACM_Receive(0U, test_data_buf, 64U);

  if (memcmp((void *)test_data_buf, (void *)test_data, sizeof(test_data)) != 0) {
    ASSERT_TRUE(false, "USBD_CDC_ACM_WriteData(0, ...) data is not as expected");
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_CDC_ACM_DataAvailable
\details
The MW_CV_USBD_CDC_ACM_DataAvailable test function verifies the \b USBD_CDC_ACM_DataAvailable function.
*/
void MW_CV_USBD_CDC_ACM_DataAvailable(void) {

  mw_cv_usbd_cdc0_acm.echo_active = false;      // Echo data inactive

  USBH_CDC_ACM_Send(0U, (uint8_t *)test_data, sizeof(test_data));
  osDelay(100U);

  ASSERT_TRUE(USBD_CDC_ACM_DataAvailable(0U) == sizeof(test_data),
             "USBD_CDC_ACM_DataAvailable(0) != %d", sizeof(test_data));
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_CDC_ACM_Notify
\details
The MW_CV_USBD_CDC_ACM_Notify test function verifies the \b USBD_CDC_ACM_Notify function.
*/
void MW_CV_USBD_CDC_ACM_Notify(void) {

  ASSERT_TRUE(USBD_CDC_ACM_Notify(0U, 0x7FU) == usbOK, "USBD_CDC_ACM_Notify(0, 0x7F) != usbOK");
  osDelay(100U);

  ASSERT_TRUE(cb_usbh_notify, "USBD_CDC_ACM_Notify(0, 0x7F) has failed");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_CDC0_ACM_Uninitialize
\details
The MW_CV_USBD_CDC0_ACM_Uninitialize test function verifies the \b USBD_CDC0_ACM_Uninitialize callback function.
*/
void MW_CV_USBD_CDC0_ACM_Uninitialize(void) {

  // Uninitialize the USB Device
  USBD_Disconnect  (0U);
  USBD_Uninitialize(0U);
  osDelay(100U);

  // Uninitialize the USB Host which was used for testing of the USB Device
  USBH_Uninitialize(0U);
  osDelay(100U);

  ASSERT_TRUE(mw_cv_usbd_cdc0_acm.cb_uninitialize, "USBD_CDC0_ACM_Uninitialize() callback was not called");

  mw_cv_usbd_cdc0_acm.testing_active = false;   // Testing inactive
}

/**
@}
*/

#endif
