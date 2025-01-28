/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation
 * Copyright (c) 2018-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_Device_0.h
 * Purpose: USB Device User header
 *----------------------------------------------------------------------------*/

#ifndef USBD_USER_DEVICE_0_H_
#define USBD_USER_DEVICE_0_H_

#include <stdbool.h>

typedef struct {
  // Variables for test control
  bool    testing_active;

  // Variables for test results
  bool    cb_initialize;
  bool    cb_uninitialize;
  bool    cb_vbus_changed;
  bool    cb_reset;
  bool    cb_hs_activated;
  bool    cb_suspended;
  bool    cb_resumed;
  bool    cb_conf_changed;
  bool    cb_enable_remote_wakeup;
  bool    cb_disable_remote_wakeup;
  bool    cb_ep0_setup_set_received;
  bool    cb_ep0_setup_get_received;
  bool    cb_ep0_setup_pocessed;
  bool    cb_ep0_out_data_received;
  bool    cb_ep0_in_data_sent;
} MW_CV_USBD_CORE_t;

// Global structure used for testing control and results
extern MW_CV_USBD_CORE_t mw_cv_usbd_core;

#endif // USBD_USER_DEVICE_0_H_
