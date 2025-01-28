/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation
 * Copyright (c) 2018-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_HID_0.h
 * Purpose: USB Device Human Interface Device class (HID) User header
 *----------------------------------------------------------------------------*/

#ifndef USBD_USER_HID_0_H_
#define USBD_USER_HID_0_H_

#include <stdbool.h>

typedef struct {
  // Variables for test control
  bool testing_active;
  bool echo_active;

  // Variables for test results
  bool cb_initialize;
  bool cb_uninitialize;
  bool cb_get_report;
  bool cb_set_report;
} MW_CV_USBD_HID_t;

// Global structure used for testing control and results
extern MW_CV_USBD_HID_t mw_cv_usbd_hid0;

#endif // USBD_USER_HID_0_H_
