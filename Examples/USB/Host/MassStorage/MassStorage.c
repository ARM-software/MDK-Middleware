/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MassStorage.c
 * Purpose: USB Host - Mass Storage example
 *----------------------------------------------------------------------------*/

#include "main.h"
#include <stdio.h>

#include "rl_fs.h"                      // Keil.MDK::File System:CORE
#include "rl_usb.h"                     // Keil.MDK::USB:CORE

#include "USBH_MSC.h"

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (4096U)
static uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
static const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

/*---------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main_thread (void *argument) {
  usbStatus usb_status;                 // USB status
  int32_t   msc_status;                 // MSC status
  FILE     *f;                          // Pointer to stream object
  uint8_t   con = 0U;                   // Connection status of MSC(s)

  (void)argument;

  printf("USB Host Mass Storage example\n");

  usb_status = USBH_Initialize (0U);    // Initialize USB Host 0

  if (usb_status != usbOK) {
    for (;;) {}                         // Handle USB Host 0 init failure
  }

  for (;;) {
    msc_status = USBH_MSC_DriveGetMediaStatus ("U0:");  // Get MSC device status
    if (msc_status == USBH_MSC_OK) {
      if (con == 0U) {                  // If stick was not connected previously
        con = 1U;                       // Stick got connected
        msc_status = USBH_MSC_DriveMount ("U0:");
        if (msc_status != USBH_MSC_OK) {
          continue;                     // Handle U0: mount failure
        }
        f = fopen ("Test.txt", "w");    // Open/create file for writing
        if (f == NULL) {
          continue;                     // Handle file opening/creation failure
        }
        fprintf (f, "USB Host Mass Storage!\n");
        fclose (f);                     // Close file
        msc_status = USBH_MSC_DriveUnmount ("U0:");
        if (msc_status != USBH_MSC_OK) {
          continue;                     // Handle U0: dismount failure
        }
      }
    } else {
      if (con == 1U) {                  // If stick was connected previously
        con = 0U;                       // Stick got disconnected
      }
    }
    osDelay(100U);
  }
}

/*-----------------------------------------------------------------------------
 *        Application main function
 *----------------------------------------------------------------------------*/
int app_main (void) {
  osKernelInitialize();
  osThreadNew(app_main_thread, NULL, &app_main_attr);
  osKernelStart();
  return 0;
}
