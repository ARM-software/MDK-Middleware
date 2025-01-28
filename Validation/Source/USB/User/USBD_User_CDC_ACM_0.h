/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation
 * Copyright (c) 2018-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_CDC_ACM_0.h
 * Purpose: USB Device Communication Device Class (CDC)
 *          Abstract Control Model (ACM) User header
 *----------------------------------------------------------------------------*/

#ifndef USBD_USER_CDC_ACM_0_H_
#define USBD_USER_CDC_ACM_0_H_

#include <stdbool.h>

typedef struct {
  // Variables for test control
  bool testing_active;
  bool echo_active;

  // Variables for test results
  bool cb_initialize;
  bool cb_uninitialize;
  bool cb_reset;
  bool cb_set_line_coding;
  bool cb_get_line_coding;
  bool cb_set_control_line_state;
  bool cb_data_received;
  bool cb_data_sent;
} MW_CV_USBD_CDC_ACM_t;

// Global structure used for testing control and results
extern MW_CV_USBD_CDC_ACM_t mw_cv_usbd_cdc0_acm;

#endif // USBD_USER_CDC_ACM_0_H_
