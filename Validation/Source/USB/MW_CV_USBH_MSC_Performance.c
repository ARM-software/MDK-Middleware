/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Host
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USBH_MSC_Performance.c
 * Purpose: MDK Middleware - Component Validation - USB Host -
 *          Mass Storage Class - Performance tests module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"
#if     ((MW_CV_USBH != 0U) && (MW_CV_USBH_MSC_PERFORMANCE != 0U))

#include "MW_CV_USBH_MSC_Performance.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "RTE_Components.h"

#include "MW_CV_TestReport.h"
#include "MW_CV_Timer.h"
#include "MW_CV_USB.h"

#include "rl_usb.h"
#include "rl_fs.h"
#include "USBH_MSC.h"


// Local variables used for testing
static uint8_t test_data_buf[4096] __ALIGNED(4);


/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\defgroup mw_cv_usbh_msc_test_funcs MDK Middleware - Component Validation - USB Host - Mass Storage Class - Performance
\brief USB Host MSC performance validation test functions
\details
The MDK Middleware Component Validation for USB Host Mass Storage Class performance checks
USB Host Mass Mass Storage Class performance.
@{
*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Test: MW_CV_USBH_MSC_Performance
\details
The MW_CV_USBH_MSC_Performance test function tests the \b performance of the USB Host Mass Storage Class.
USB Host writes a file with specific pattern, then reads it back, verifies content and deletes the file.
It is done cyclically until no new USB Flash Disk is connected or old one is not disconnected until timeout.
*/
void MW_CV_USBH_MSC_Performance(void) {
  FILE     *f;                          // Pointer to file stream object
  double    wr_speed, rd_speed;
  uint32_t  i, j, timeout_cnt, cnt;
  uint32_t  block_count;
  uint32_t  block_size;
  uint16_t  vid, pid;
  uint8_t   con, con_last, instance;
  char      ch;
  bool      verify;

  wr_speed = 0.0;
  rd_speed = 0.0;
  cnt      = 0U;
  con      = 0U;
  con_last = 0U;
  instance = 0U;
  verify   = true;

#if defined(RTE_USB_Host_0)
  if (USBH_Initialize(0U) != usbOK) {
    ASSERT_TRUE(false, "USBH_Initialize(0) != usbOK");
    return;
  }
#endif
#if defined(RTE_USB_Host_1)
  if (USBH_Initialize(1U) != usbOK) {
    ASSERT_TRUE(false, "USBH_Initialize(1) != usbOK");
    return;
  }
#endif

  for (;;) {
detect:
    // Wait for device to be connected
    for (timeout_cnt = (MW_CV_USB_TIMEOUT_IN_SEC * 10U); timeout_cnt > 0U; timeout_cnt--) {
      if (USBH_MSC_DriveGetMediaStatus("U0:") == USBH_MSC_OK) {
        con |=  1U;
      } else {
        con &= ~1U;
        con_last &= ~1U;
      }
      if (USBH_MSC_DriveGetMediaStatus("U1:") == USBH_MSC_OK) {
        con |=  2U;
      } else {
        con &= ~2U;
        con_last &= ~2U;
      }
      if ((con & (con ^ con_last)) != 0U) {
        break;
      } else if ((MW_CV_USB_TIMEOUT_IN_SEC * 10U) == (timeout_cnt + 50U)) {
        if (cnt == 0U) {
        } else {
        }
      }
      osDelay(100U);
    }
    if (timeout_cnt == 0U) {
      if (cnt == 0U) {
        DETAIL_INFO("No USB Flash Disk available for test, test skipped");
        goto exit;
      } else {
        goto done;
      }
    }

    if ((con & (con ^ con_last)) & 1U) {        // Test drive U0:
      con_last |= 1U;
      if (USBH_MSC_DriveMount("U0:") != USBH_MSC_OK) {
        DETAIL_INFO("USBH_MSC_DriveMount(\"U0:\") != USBH_MSC_OK, possibly unformatted USB Flash Drive");
        goto detect;
      }
      fchdrive("U0:");
      instance  = 0U;
    } else if ((con & (con ^ con_last)) & 2U) { // Test drive U1:
      con_last |= 2U;
      if (USBH_MSC_DriveMount("U1:") != USBH_MSC_OK) {
        DETAIL_INFO("USBH_MSC_DriveMount(\"U1:\") != USBH_MSC_OK, possibly unformatted USB Flash Drive");
        goto detect;
      }
      fchdrive("U1:");
      instance  = 1U;
    }

    vid = USBH_Device_GetVID(instance);
    pid = USBH_Device_GetPID(instance);
    USBH_MSC_ReadCapacity(instance, &block_count, &block_size);

    // Mount drive

    // Write test data to file
    f = fopen("MW_CV_USB_Host.txt", "w");       // Open/create file for writing
    if (f == NULL) {
      ASSERT_TRUE(false, "fopen(\"MW_CV_USB_Host.txt\", \"w\") == NULL");
      goto exit;
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
      goto exit;
    }
    wr_speed = (((double)MW_CV_USB_TEST_FILE_SIZE_IN_MB)*1000000)/((double)(MW_CV_TimerGetTime()));

    // Read file content
    f = fopen("MW_CV_USB_Host.txt", "r");       // Open file for reading
    if (f == NULL) {
      ASSERT_TRUE(false, "fopen(\"MW_CV_USB_Host.txt\", \"r\") == NULL");
      goto exit;
    }
    MW_CV_TimerReset();
    for (i = (MW_CV_USB_TEST_FILE_SIZE_IN_MB*1024U*1024U)/sizeof(test_data_buf); i > 0 ; i--) {
      MW_CV_TimerStart();
      fread(test_data_buf, sizeof(test_data_buf), 1, f);
      MW_CV_TimerStop();
    }
    if (fclose(f) != 0) {
      ASSERT_TRUE(false, "fclose(f) != 0");
      goto exit;
    }
    rd_speed = (((double)MW_CV_USB_TEST_FILE_SIZE_IN_MB)*1000000)/((double)(MW_CV_TimerGetTime()));

    // Verify file content
    f = fopen("MW_CV_USB_Host.txt", "r");       // Open file for reading
    if (f == NULL) {
      ASSERT_TRUE(false, "fopen(\"MW_CV_USB_Host.txt\", \"r\") == NULL");
      goto exit;
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
        goto exit;
      }
    }
    if (fclose(f) != 0) {
      ASSERT_TRUE(false, "fclose(f) != 0");
      goto exit;
    }
    if (fdelete("MW_CV_USB_Host.txt", NULL) != fsOK) {  // Delete test file
      ASSERT_TRUE(false, "fdelete(\"MW_CV_USB_Host.txt\") != fsOK");
    }
    cnt++;

    DETAIL_INFO("VID: 0x%04X, PID: 0x%04X, Disk capacity: %.2f MB, Write speed: %.2f MB/s, Read speed: %.2f MB/s",
                 vid, pid, ((((uint32_t)(((uint64_t)block_count)*block_size))/1024)/1024.0), wr_speed, rd_speed);
  }

done:
  ASSERT_TRUE(true, "");                        // Test passed

exit:
#if defined(RTE_USB_Host_0)
  USBH_Uninitialize(0U);
#endif
#if defined(RTE_USB_Host_1)
  USBH_Uninitialize(1U);
#endif
}

/**
@}
*/

#endif
