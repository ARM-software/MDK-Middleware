/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Host
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USBH_MSC.c
 * Purpose: MDK Middleware - Component Validation - USB Host -
 *          Mass Storage Class - tests module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"
#if     ((MW_CV_USBH != 0U) && (MW_CV_USBH_MSC != 0U))

#include "MW_CV_USBH_MSC.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "MW_CV_TestReport.h"
#include "MW_CV_Timer.h"
#include "MW_CV_USB.h"

#include "rl_usb.h"
#include "rl_fs.h"
#include "USBD_User_MSC_0.h"
#include "USBH_MSC.h"


// Local variables used for testing
static const uint8_t INST[3]      = { 0U, 1U, 255U };
static       uint8_t test_data_buf[4096] __ALIGNED(4);
static       bool    cb_msc0_init;
static       bool    cb_msc0_uninit;


// Callback functions
void USBH_MSC_Initialize (uint8_t instance) {
  if (instance == 0U) {
    cb_msc0_init = true;
  }
}

void USBH_MSC_Uninitialize (uint8_t instance) {
  if (instance == 0U) {
    cb_msc0_uninit = true;
  }
}


/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\defgroup mw_cv_usbh_msc_test_funcs MDK Middleware - Component Validation - USB Host - Mass Storage Class
\brief USB Host MSC validation test functions
\details
The MDK Middleware Component Validation for USB Host Mass Storage Class checks
USB Host Mass Storage API as well as USB Host Mass Storage Class functionality.
@{
*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_MSC_Initialize
\details
The MW_CV_USBH_MSC_Initialize test function verifies the \b USBH_MSC_Initialize callback function.
*/
void MW_CV_USBH_MSC_Initialize(void) {
  uint32_t timeout_cnt;

  cb_msc0_init   = false;
  cb_msc0_uninit = false;

  // Initialize the USB Host
  USBH_Initialize(0U);
  osDelay(100U);

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
    ASSERT_TRUE(false, "USB0 not connected to USB1, necessary for further tests");
    return;
  }

  ASSERT_TRUE(cb_msc0_init, "USBH_MSC_Initialize(0) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_MSC_GetDevice
\details
The MW_CV_USBH_MSC_GetDevice function verifies the \b USBH_MSC_GetDevice function with the following parameter values:
 - instance: 0, 1, 255
*/
void MW_CV_USBH_MSC_GetDevice(void) {
  uint8_t expected_result, inst, i;

  for (i = 0U; i < sizeof(INST); i++) {
    inst = INST[i];
    expected_result = 255U;
    if (inst == 0U) {
      expected_result = 0U;
    }
    ASSERT_TRUE(USBH_MSC_GetDevice(inst) == expected_result,
               "USBH_MSC_GetDevice(%d) != %d",            inst, expected_result);
    ASSERT_TRUE(USBH_MSC_GetDevice(inst) == expected_result,
               "USBH_MSC_GetDevice(%d) != %d (2nd call)", inst, expected_result);
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_MSC_GetStatus
\details
The MW_CV_USBH_MSC_GetStatus function verifies the \b USBH_MSC_GetStatus function with the following parameter values:
 - instance: 0, 1, 255
*/
void MW_CV_USBH_MSC_GetStatus(void) {
  uint32_t expected_result;
  uint8_t  inst, i;

  for (i = 0U; i < sizeof(INST); i++) {
    inst = INST[i];
    expected_result = usbClassErrorMSC;
    if (inst == 0U) {
      expected_result = usbOK;
    }
    ASSERT_TRUE(USBH_MSC_GetStatus(inst) == expected_result,
               "USBH_MSC_GetStatus(%d) != %s",            inst, MW_CW_USB_STATUS_TXT(expected_result));
    ASSERT_TRUE(USBH_MSC_GetStatus(inst) == expected_result,
               "USBH_MSC_GetStatus(%d) != %s (2nd call)", inst, MW_CW_USB_STATUS_TXT(expected_result));
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_MSC_Write_Read
\details
The MW_CV_USBH_MSC_Write_Read test function verifies the \b USBH_MSC_Write and \b USBH_MSC_Read of the USB Host Mass Storage Class.
USB Host writes a file with specific pattern, then reads it back, verifies contents and deletes the file.
*/
void MW_CV_USBH_MSC_Write_Read(void) {
  FILE     *f;                          // Pointer to file stream object
  double    wr_speed, rd_speed;
  uint32_t  i, j, timeout_cnt;
  char      ch;
  bool      verify;

  wr_speed = 0.0;
  rd_speed = 0.0;
  verify   = true;

  // Wait for device to be detected
  for (timeout_cnt = (MW_CV_USB_TIMEOUT_IN_SEC * 10U); timeout_cnt > 0U; timeout_cnt--) {
    if (USBH_MSC_DriveGetMediaStatus("U0:") == USBH_MSC_OK) {
      break;
    }
    osDelay(100U);
  }
  if (timeout_cnt == 0U) {
    ASSERT_TRUE(false, "No Mass Storage device available (or SD Card not inserted) for test");
    return;
  }

  // Mount drive
  if (USBH_MSC_DriveMount("U0:") != USBH_MSC_OK) {
    ASSERT_TRUE(false, "USBH_MSC_DriveMount(\"U0:\") != USBH_MSC_OK");
    return;
  }

  // Write test data to file
  f = fopen("U0:\\MW_CV_USB_Host.txt", "w");    // Open/create file for writing
  if (f == NULL) {
    ASSERT_TRUE(false, "fopen(\"U0:\\MW_CV_USB_Host.txt\", \"w\") == NULL");
    return;
  }
  MW_CV_TimerReset();
  ch = 'a';
  for (i = (MW_CV_USB_TEST_FILE_SIZE_IN_MB*1024U*1024U)/sizeof(test_data_buf); i > 0 ; i--) {
    memset(test_data_buf, ch++, sizeof(test_data_buf));
    if (ch >= 'z' + 1) {
      ch = 'a';
    }
    MW_CV_TimerStart();
    fwrite(test_data_buf, sizeof(test_data_buf), 1, f);
    MW_CV_TimerStop();
  }
  if (fclose(f) != 0) {
    ASSERT_TRUE(false, "fclose(f) != 0");
    return;
  }
  wr_speed = (((double)MW_CV_USB_TEST_FILE_SIZE_IN_MB)*1000000)/((double)(MW_CV_TimerGetTime()));

  // Read file content
  f = fopen("U0:\\MW_CV_USB_Host.txt", "r");    // Open file for reading
  if (f == NULL) {
    ASSERT_TRUE(false, "fopen(\"U0:\\MW_CV_USB_Host.txt\", \"r\") == NULL");
    return;
  }
  MW_CV_TimerReset();
  for (i = (MW_CV_USB_TEST_FILE_SIZE_IN_MB*1024U*1024U)/sizeof(test_data_buf); i > 0 ; i--) {
    MW_CV_TimerStart();
    fread(test_data_buf, sizeof(test_data_buf), 1, f);
    MW_CV_TimerStop();
  }
  if (fclose(f) != 0) {
    ASSERT_TRUE(false, "fclose(f) != 0");
    return;
  }
  rd_speed = (((double)MW_CV_USB_TEST_FILE_SIZE_IN_MB)*1000000)/((double)(MW_CV_TimerGetTime()));

  // Verify file content
  f = fopen("U0:\\MW_CV_USB_Host.txt", "r");    // Open file for reading
  if (f == NULL) {
    ASSERT_TRUE(false, "fopen(\"U0:\\MW_CV_USB_Host.txt\", \"r\") == NULL");
    return;
  }
  ch = 'a';
  for (i = (MW_CV_USB_TEST_FILE_SIZE_IN_MB*1024U*1024U)/sizeof(test_data_buf); i > 0 ; i--) {
    memset(test_data_buf, 0x55, sizeof(test_data_buf));
    if (ch >= 'z' + 1) {
      ch = 'a';
    }
    fread(test_data_buf, sizeof(test_data_buf), 1, f);
    for (j = 0U; j < sizeof(test_data_buf); j++) {
      if (test_data_buf[j] != ch) {
        verify = false;
        break;
      }
    }
    ch++;
    if (!verify) {
      ASSERT_TRUE(false, "File verification failed");
      return;
    }
  }
  if (fclose(f) != 0) {
    ASSERT_TRUE(false, "fclose(f) != 0");
    return;
  }
  if (fdelete("U0:\\MW_CV_USB_Host.txt", NULL) != fsOK) {       // Delete test file
    ASSERT_TRUE(false, "fdelete(\"U0:\\MW_CV_USB_Host.txt\") != fsOK");
  }

  ASSERT_TRUE(true, "");                        // Test passed
  DETAIL_INFO("Write speed: %.2f MB/s, Read speed: %.2f MB/s", wr_speed, rd_speed);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_MSC_ReadCapacity
\details
The MW_CV_USBH_MSC_ReadCapacity function verifies the \b USBH_MSC_ReadCapacity function with the following parameter values:
 - instance: 0, 1, 255
*/
void MW_CV_USBH_MSC_ReadCapacity(void) {
  usbStatus usb_status;
  uint32_t  expected_result;
  uint32_t  block_count;
  uint32_t  block_size;
  uint8_t   inst, i;


  for (i = 0U; i < sizeof(INST); i++) {
    inst = INST[i];
    expected_result = usbClassErrorMSC;
    if (inst == 0U) {
      expected_result = usbOK;
    }
    usb_status = USBH_MSC_ReadCapacity(inst, &block_count, &block_size);
    ASSERT_TRUE(usb_status == expected_result, "USBH_MSC_ReadCapacity(%d, ...) != %s",            inst, MW_CW_USB_STATUS_TXT(expected_result));
    if (usb_status == usbOK) {
      DETAIL_INFO("Device capacity is %.2f MB", ((((uint32_t)(((uint64_t)block_count)*block_size))/1024)/1024.0));
    }
    usb_status = USBH_MSC_ReadCapacity(inst, &block_count, &block_size);
    ASSERT_TRUE(usb_status == expected_result, "USBH_MSC_ReadCapacity(%d, ...) != %s (2nd call)", inst, MW_CW_USB_STATUS_TXT(expected_result));
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_MSC_Uninitialize
\details
The MW_CV_USBH_MSC_Uninitialize test function verifies the \b USBH_MSC_Uninitialize callback function.
*/
void MW_CV_USBH_MSC_Uninitialize(void) {

  // Uninitialize the USB Device which was used for testing of the USB Host
  USBD_Disconnect  (0U);
  USBD_Uninitialize(0U);
  osDelay(100U);

  // Uninitialize the USB Host
  USBH_Uninitialize(0U);
  osDelay(100U);

  ASSERT_TRUE(cb_msc0_uninit, "USBH_MSC_Uninitialize(0) callback was not called");
}

/**
@}
*/

#endif
