/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Host
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USBH_CDC_ACM.c
 * Purpose: MDK Middleware - Component Validation - USB Host -
 *          Communication Device Class - Abstract Control Model -
 *          tests module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"
#if     ((MW_CV_USBH != 0U) && (MW_CV_USBH_CDC_ACM != 0U))

#include "MW_CV_USBH_CDC_ACM.h"

#include <stdbool.h>
#include <string.h>

#include "MW_CV_TestReport.h"
#include "MW_CV_USB.h"

#include "rl_usb.h"
#include "USBD_User_CDC_ACM_0.h"


// Local variables used for testing
static const uint8_t         INST[3]            = { 0U, 1U, 255U };
static const uint8_t         STATE[5]           = { 0U, 1U, 2U, 3U, 255U };
static const char            test_data[]        = "USB Host CDC ACM Validation Test!";
static const CDC_LINE_CODING line_coding_9600   = {   9600U, 1U, 1U, 8U };
static const CDC_LINE_CODING line_coding_115200 = { 115200U, 0U, 0U, 8U };
static       uint8_t         test_data_buf[64] __ALIGNED(4);
static       bool            cb_cdc0_acm_init;
static       bool            cb_cdc0_acm_uninit;
static       bool            cb_cdc0_acm_notify;


// Callback functions
void USBH_CDC_ACM_Initialize (uint8_t instance) {
  if (instance == 0U) {
    cb_cdc0_acm_init = true;
  }
}

void USBH_CDC_ACM_Uninitialize (uint8_t instance) {
  if (instance == 0U) {
    cb_cdc0_acm_uninit = true;
  }
}

void USBH_CDC_ACM_Notify (uint8_t instance, uint16_t status) {
  if ((instance == 0U) && (status == 3U)) {
    cb_cdc0_acm_notify = true;
  }
}


/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\defgroup mw_cv_usbh_cdc_acm_test_funcs MDK Middleware - Component Validation - USB Host - Communication Device Class -
Abstract Control Model
\brief USB Host CDC ACM validation test functions
\details
The MDK Middleware Component Validation for USB Host Communication Device Class checks
USB Host Communication Device Class API as well as USB Host Communication Device Class functionality.
@{
*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_CDC_ACM_Initialize
\details
The MW_CV_USBH_CDC_ACM_Initialize test function verifies the \b USBH_CDC_ACM_Initialize callback function.
*/
void MW_CV_USBH_CDC_ACM_Initialize(void) {
  uint32_t timeout_cnt;

  cb_cdc0_acm_init   = false;
  cb_cdc0_acm_uninit = false;
  cb_cdc0_acm_notify = false;

  mw_cv_usbd_cdc0_acm.testing_active = true;    // Testing active
  mw_cv_usbd_cdc0_acm.echo_active    = true;    // Echo data active

  // Initialize the USB Host
  USBH_Initialize(0U);
  osDelay(100U);

  // Initialize the USB Device which will be used for testing of the USB Host
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

  ASSERT_TRUE(cb_cdc0_acm_init, "USBH_CDC_ACM_Initialize(0) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_CDC_ACM_GetDevice
\details
The MW_CV_USBH_CDC_ACM_GetDevice function verifies the \b USBH_CDC_ACM_GetDevice function with the following parameter values:
 - instance: 0, 1, 255
*/
void MW_CV_USBH_CDC_ACM_GetDevice(void) {
  uint8_t expected_result, inst, i;

  for (i = 0U; i < sizeof(INST); i++) {
    inst = INST[i];
    expected_result = 255U;
    if (inst == 0U) {
      expected_result = 0U;
    }
    ASSERT_TRUE(USBH_CDC_ACM_GetDevice(inst) == expected_result,
               "USBH_CDC_ACM_GetDevice(%d) != %d",            inst, expected_result);
    ASSERT_TRUE(USBH_CDC_ACM_GetDevice(inst) == expected_result,
               "USBH_CDC_ACM_GetDevice(%d) != %d (2nd call)", inst, expected_result);
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_CDC_ACM_GetStatus
\details
The MW_CV_USBH_CDC_ACM_GetStatus function verifies the \b USBH_CDC_ACM_GetStatus function with the following parameter values:
 - instance: 0, 1, 255
*/
void MW_CV_USBH_CDC_ACM_GetStatus(void) {
  uint32_t expected_result;
  uint8_t  inst, i;

  for (i = 0U; i < sizeof(INST); i++) {
    inst = INST[i];
    expected_result = usbClassErrorCDC;
    if (inst == 0U) {
      expected_result = usbOK;
    }
    ASSERT_TRUE(USBH_CDC_ACM_GetStatus(inst) == expected_result,
               "USBH_CDC_ACM_GetStatus(%d) != %s",            inst, MW_CW_USB_STATUS_TXT(expected_result));
    ASSERT_TRUE(USBH_CDC_ACM_GetStatus(inst) == expected_result,
               "USBH_CDC_ACM_GetStatus(%d) != %s (2nd call)", inst, MW_CW_USB_STATUS_TXT(expected_result));
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_CDC_ACM_Send_Receive
\details
The MW_CV_USBH_CDC_ACM_Send_Receive test function verifies the \b USBH_CDC_ACM_Send, \b USBH_CDC_ACM_GetTxCount,
\b USBH_CDC_ACM_Receive and \b MW_CV_USBH_CDC_ACM_GetRxCount of the USB Host Communication Device Class -
Abstract Control Model.
USB Host writes a test string, then reads it back, verifies contents of returned string as well as transmit and
receive counts.
*/
void MW_CV_USBH_CDC_ACM_Send_Receive(void) {
  usbStatus usb_status;

  usb_status = USBH_CDC_ACM_Send(0U, (uint8_t *)test_data, sizeof(test_data));
  if (usb_status != usbOK) {
    ASSERT_TRUE(false, "USBH_CDC_ACM_Send(0, ...) != usbOK");
    return;
  }

  if (USBH_CDC_ACM_GetTxCount(0U) != sizeof(test_data)) {
    ASSERT_TRUE(false, "USBH_CDC_ACM_GetTxCount(0) != %d", sizeof(test_data));
  }

  usb_status = USBH_CDC_ACM_Receive(0U, test_data_buf, 64U);
  if (usb_status != usbOK) {
    ASSERT_TRUE(false, "USBH_CDC_ACM_Receive(0, ...) != usbOK");
    return;
  }

  if (USBH_CDC_ACM_GetRxCount(0U) != sizeof(test_data)) {
    ASSERT_TRUE(false, "USBH_CDC_ACM_GetRxCount(0) != %d", sizeof(test_data));
  }

  ASSERT_TRUE(true, "");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_CDC_ACM_SetGetLineCoding
\details
The MW_CV_USBH_CDC_ACM_SetGetLineCoding function verifies the \b USBH_CDC_ACM_SetLineCoding and \b USBH_CDC_ACM_GetLineCoding
function with the following parameter values:
 - instance: 0, 1, 255
*/
void MW_CV_USBH_CDC_ACM_SetGetLineCoding(void) {
  CDC_LINE_CODING line_coding;
  uint8_t         inst, i;

  for (i = 0U; i < sizeof(INST); i++) {
    inst = INST[i];
    if (inst == 0U) {
      ASSERT_TRUE(USBH_CDC_ACM_SetLineCoding(0U, &line_coding_9600) == usbOK,
                 "USBH_CDC_ACM_SetLineCoding(0, ...) != usbOK", inst);
      ASSERT_TRUE(USBH_CDC_ACM_SetLineCoding(0U, &line_coding_9600) == usbOK,
                 "USBH_CDC_ACM_SetLineCoding(0, ...) != usbOK (2nd call)");
      ASSERT_TRUE(USBH_CDC_ACM_GetLineCoding(0U, &line_coding) == usbOK,
                 "USBH_CDC_ACM_GetLineCoding(0, ...) != usbOK");
      if (memcmp((void *)&line_coding, (void *)&line_coding_9600, sizeof(CDC_LINE_CODING)) != 0) {
        ASSERT_TRUE(false, "Line coding returned by USBH_CDC_ACM_GetLineCoding(0, ...) is different then value set by USBH_CDC_ACM_SetLineCoding(0, ...)");
      }

      ASSERT_TRUE(USBH_CDC_ACM_SetLineCoding(0U, &line_coding_115200) == usbOK,
                 "USBH_CDC_ACM_SetLineCoding(0, ...) != usbOK", inst);
      ASSERT_TRUE(USBH_CDC_ACM_SetLineCoding(0U, &line_coding_115200) == usbOK,
                 "USBH_CDC_ACM_SetLineCoding(0, ...) != usbOK (2nd call)");
      ASSERT_TRUE(USBH_CDC_ACM_GetLineCoding(0U, &line_coding) == usbOK,
                 "USBH_CDC_ACM_GetLineCoding(0, ...) != usbOK");
      if (memcmp((void *)&line_coding, (void *)&line_coding_115200, sizeof(CDC_LINE_CODING)) != 0) {
        ASSERT_TRUE(false, "Line coding returned by USBH_CDC_ACM_GetLineCoding(0, ...) is different then value set by USBH_CDC_ACM_SetLineCoding(0, ...)");
      }
    } else {
      ASSERT_TRUE(USBH_CDC_ACM_SetLineCoding(inst, &line_coding_9600) == usbClassErrorCDC,
                 "USBH_CDC_ACM_SetLineCoding(%d, ...) != usbDeviceError", inst);
      ASSERT_TRUE(USBH_CDC_ACM_SetLineCoding(inst, &line_coding_9600) == usbClassErrorCDC,
                 "USBH_CDC_ACM_SetLineCoding(%d, ...) != usbDeviceError (2nd call)", inst);
    }
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_CDC_ACM_SetControlLineState
\details
The MW_CV_USBH_CDC_ACM_SetGetLineCoding function verifies the \b USBH_CDC_ACM_SetControlLineState function with the
following parameter values:
 - instance: 0, 1, 255
 - state: 0, 1, 2, 3, 255
*/
void MW_CV_USBH_CDC_ACM_SetControlLineState(void) {
  uint32_t expected_result;
  uint8_t  inst, i, state, j;

  for (i = 0U; i < sizeof(INST); i++) {
    inst = INST[i];
    expected_result = usbClassErrorCDC;
    if (inst == 0U) {
      expected_result = usbOK;
    }
    for (j = 0U; j < sizeof(STATE); j++) {
      state = STATE[j];
      ASSERT_TRUE(USBH_CDC_ACM_SetControlLineState(inst, state) == expected_result,
                 "USBH_CDC_ACM_SetControlLineState(%d, %d) != %s", inst, state, MW_CW_USB_STATUS_TXT(expected_result));
      osDelay(10U);                     // Allow time for USB Host to read status change on interrupt endpoint
      ASSERT_TRUE(USBH_CDC_ACM_SetControlLineState(inst, state) == expected_result,
                 "USBH_CDC_ACM_SetControlLineState(%d, %d) != %s (2nd call)", inst, state, MW_CW_USB_STATUS_TXT(expected_result));
      osDelay(10U);                     // Allow time for USB Host to read status change on interrupt endpoint
    }
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_CDC_ACM_Notify
\details
The MW_CV_USBH_CDC_ACM_Notify test function verifies the \b USBH_CDC_ACM_Notify callback function.
*/
void MW_CV_USBH_CDC_ACM_Notify(void) {
  ASSERT_TRUE(cb_cdc0_acm_notify, "USBH_CDC_ACM_Notify(0, ...) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_CDC_ACM_SendBreak
\details
The MW_CV_USBH_CDC_ACM_SendBreak function verifies the \b USBH_CDC_ACM_SendBreak function.
This test cannot be done by device self-test as USB Device does not support Send Break command through user API callback.
It support it through overriding "bool USBD_CDC_ACM_CoreSendBreak (uint8_t instance, uint16_t dur)" library function but
it is not included into standard user API.
*/
void MW_CV_USBH_CDC_ACM_SendBreak(void) {
  DETAIL_INFO("USBH_CDC_ACM_SendBreak cannot be tested with self-test as USB Device user API does not support Send Break command callback");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_CDC_ACM_Uninitialize
\details
The MW_CV_USBH_CDC_ACM_Uninitialize test function verifies the \b USBH_CDC_ACM_Uninitialize callback function.
*/
void MW_CV_USBH_CDC_ACM_Uninitialize(void) {

  // Uninitialize the USB Device which was used for testing of the USB Host
  USBD_Disconnect  (0U);
  USBD_Uninitialize(0U);
  osDelay(100U);

  // Uninitialize the USB Host
  USBH_Uninitialize(0U);
  osDelay(100U);

  ASSERT_TRUE(cb_cdc0_acm_uninit, "USBH_CDC_ACM_Uninitialize(0) callback was not called");
}

/**
@}
*/

#endif
