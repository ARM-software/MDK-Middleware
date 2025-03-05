/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Device
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USBD_MSC.c
 * Purpose: MDK Middleware - Component Validation - USB Device -
 *          Mass Storage Class - tests module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"
#if     ((MW_CV_USBD != 0U) && (MW_CV_USBD_MSC != 0U))

#include "MW_CV_USBD_MSC.h"

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
static uint8_t test_data_buf[4096] __ALIGNED(4);


/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\defgroup mw_cv_usbd_msc_test_funcs MDK Middleware - Component Validation - USB Device - Mass Storage Class
\brief USB Device MSC validation test functions
\details
The MDK Middleware Component Validation for USB Device Mass Storage Class checks USB Device Mass Storage API as well as
USB Device Mass Storage Class functionality.
@{
*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_MSC0_Initialize
\details
The MW_CV_USBD_MSC0_Initialize test function verifies the \b USBD_MSC0_Initialize callback function.
*/
void MW_CV_USBD_MSC0_Initialize(void) {
  uint32_t timeout_cnt;

  // Initialize global structure used for testing control and results
  memset(&mw_cv_usbd_msc0, 0, sizeof(MW_CV_USBD_MSC_t));

  // Initialize the USB Host which will be used for testing of the USB Device
  USBH_Initialize(0U);
  osDelay(100U);

  // Initialize and connect the USB Device
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

  ASSERT_TRUE(mw_cv_usbd_msc0.cb_initialize, "USBD_MSC0_Initialize() callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_MSC0_GetCacheInfo
\details
The MW_CV_USBD_MSC0_GetCacheInfo test function verifies the \b USBD_MSC0_GetCacheInfo callback function.
*/
void MW_CV_USBD_MSC0_GetCacheInfo(void) {
  ASSERT_TRUE(mw_cv_usbd_msc0.cb_get_cache_info, "USBD_MSC0_GetCacheInfo(...) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_MSC0_GetMediaCapacity
\details
The MW_CV_USBD_MSC0_GetMediaCapacity test function verifies the \b USBD_MSC0_GetMediaCapacity callback function.
*/
void MW_CV_USBD_MSC0_GetMediaCapacity(void) {

  if (mw_cv_usbd_msc0.cb_get_media_capacity) {
    DETAIL_INFO("Device capacity is %.2f MB", ((((uint32_t)(((uint64_t)mw_cv_usbd_msc0.block_count)*mw_cv_usbd_msc0.block_size))/1024)/1024.0));
  }
  ASSERT_TRUE(mw_cv_usbd_msc0.cb_get_media_capacity, "USBD_MSC0_GetMediaCapacity(...) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_MSC0_Write_Read
\details
The MW_CV_USBD_MSC0_Write_Read test function verifies the \b USBD_MSC0_Write and \b USBD_MSC0_Read of the USB Device Mass Storage Class.
USB Host writes a file with specific pattern, then reads it back, verifies contents and deletes the file.
*/
void MW_CV_USBD_MSC0_Write_Read(void) {
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
  f = fopen("U0:\\MW_CV_USB_Device.txt", "w");  // Open/create file for writing
  if (f == NULL) {
    ASSERT_TRUE(false, "fopen(\"U0:\\MW_CV_USB_Device.txt\", \"w\") == NULL");
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
  f = fopen("U0:\\MW_CV_USB_Device.txt", "r");  // Open file for reading
  if (f == NULL) {
    ASSERT_TRUE(false, "fopen(\"U0:\\MW_CV_USB_Device.txt\", \"r\") == NULL");
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
  f = fopen("U0:\\MW_CV_USB_Device.txt", "r");  // Open file for reading
  if (f == NULL) {
    ASSERT_TRUE(false, "fopen(\"U0:\\MW_CV_USB_Device.txt\", \"r\") == NULL");
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
  if (fdelete("U0:\\MW_CV_USB_Device.txt", NULL) != fsOK) {     // Delete test file
    ASSERT_TRUE(false, "fdelete(\"U0:\\MW_CV_USB_Device.txt\") != fsOK");
  }

  ASSERT_TRUE(mw_cv_usbd_msc0.cb_write, "USBD_MSC0_Write(...) callback was not called");
  ASSERT_TRUE(mw_cv_usbd_msc0.cb_read,  "USBD_MSC0_Read(...) callback was not called");
  DETAIL_INFO("Write speed: %.2f MB/s, Read speed: %.2f MB/s", wr_speed, rd_speed);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_MSC0_CheckMedia
\details
The MW_CV_USBD_MSC0_CheckMedia test function verifies the \b USBD_MSC0_CheckMedia callback function.
*/
void MW_CV_USBD_MSC0_CheckMedia (void) {
  ASSERT_TRUE(mw_cv_usbd_msc0.cb_check_media, "USBD_MSC0_CheckMedia(...) callback was not called");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_MSC0_GetMaxLUN
\details
The MW_CV_USBD_MSC0_GetMaxLUN test function verifies the \b USBD_MSC0_GetMaxLUN callback function.
Note: Not implemented as USB Host does not support devices with multiple LUNs.
*/
void MW_CV_USBD_MSC0_GetMaxLUN(void) {
  DETAIL_INFO("This test case is not implemented");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_MW_CV_USB_TEST_FILE_SIZE_IN_MB
\details
The MW_CV_MW_CV_USB_TEST_FILE_SIZE_IN_MB test function verifies the \b MW_CV_USB_TEST_FILE_SIZE_IN_MB callback function.
Note: Not implemented as USB Host does not support devices with multiple LUNs.
*/
void MW_CV_USBD_MSC0_LUN_GetMediaCapacity(void) {
  DETAIL_INFO("This test case is not implemented");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_MSC0_LUN_Read
\details
The MW_CV_USBD_MSC0_LUN_Read test function verifies the \b USBD_MSC0_LUN_Read callback function.
Note: Not implemented as USB Host does not support devices with multiple LUNs.
*/
void MW_CV_USBD_MSC0_LUN_Read(void) {
  DETAIL_INFO("This test case is not implemented");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_MSC0_LUN_Write
\details
The MW_CV_USBD_MSC0_LUN_Write test function verifies the \b USBD_MSC0_LUN_Write callback function.
Note: Not implemented as USB Host does not support devices with multiple LUNs.
*/
void MW_CV_USBD_MSC0_LUN_Write(void) {
  DETAIL_INFO("This test case is not implemented");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_MSC0_LUN_CheckMedia
\details
The MW_CV_USBD_MSC0_LUN_CheckMedia test function verifies the \b USBD_MSC0_LUN_CheckMedia callback function.
Note: Not implemented as USB Host does not support devices with multiple LUNs.
*/
void MW_CV_USBD_MSC0_LUN_CheckMedia(void) {
  DETAIL_INFO("This test case is not implemented");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBD_MSC0_Uninitialize
\details
The MW_CV_USBD_MSC0_Uninitialize test function verifies the \b USBD_MSC0_Uninitialize callback function.
*/
void MW_CV_USBD_MSC0_Uninitialize(void) {

  // Uninitialize the USB Device
  USBD_Disconnect  (0U);
  USBD_Uninitialize(0U);
  osDelay(100U);

  // Uninitialize the USB Host which was used for testing of the USB Device
  USBH_Uninitialize(0U);
  osDelay(100U);

  ASSERT_TRUE(mw_cv_usbd_msc0.cb_uninitialize, "USBD_MSC0_Uninitialize() callback was not called");
}

/**
@}
*/

#endif
