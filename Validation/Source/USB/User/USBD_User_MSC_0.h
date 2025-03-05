/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation
 * Copyright (c) 2018-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_MSC_0.h
 * Purpose: USB Device Mass Storage Device class (MSC) User header
 *----------------------------------------------------------------------------*/

#ifndef USBD_USER_MSC_0_H_
#define USBD_USER_MSC_0_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  // Variables for test results
  bool     cb_initialize;
  bool     cb_uninitialize;
  bool     cb_get_cache_info;
  bool     cb_get_media_capacity;
  bool     cb_read;
  bool     cb_write;
  bool     cb_check_media;
  bool     cb_get_max_lun;
  bool     cb_lun_get_media_capacity;
  bool     cb_lun_read;
  bool     cb_lun_write;
  bool     cb_lun_check_media;

  // Variables for additional information
  uint32_t block_count;
  uint32_t block_size;
} MW_CV_USBD_MSC_t;

// Global structure used for testing control and results
extern MW_CV_USBD_MSC_t mw_cv_usbd_msc0;

#endif // USBD_USER_MSC_0_H_
