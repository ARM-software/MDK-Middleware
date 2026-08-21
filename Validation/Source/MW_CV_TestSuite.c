/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation
 * Copyright (c) 2018-2026 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_TestSuite.c
 * Purpose: MDK Middleware - Component Validation - Tests Suite module
 *----------------------------------------------------------------------------*/

#include "MW_CV_TestSuite.h"

#ifdef   CMSIS_target_header
#include CMSIS_target_header
#endif

#include "MW_CV_Config.h"

#if      (MW_CV_FS != 0)
#include "rl_fs.h"
#include "MW_CV_FS.h"
#include "MW_CV_FS_Core.h"
#include "MW_CV_FS_FAT.h"
#include "MW_CV_FS_EFS.h"
#include "MW_CV_FS_Clib.h"
#include "MW_CV_FS_MC.h"
#include "MW_CV_FS_IOC.h"
#include "MW_CV_FS_Performance.h"
#include "MW_CV_FS_Stress.h"
#endif

#if      (MW_CV_NET != 0)
#include "MW_CV_BSD.h"
#endif

#if      ((MW_CV_USBD != 0) || (MW_CV_USBH != 0))
#include "rl_usb.h"
#endif

#if      (MW_CV_USBD != 0)
#include "MW_CV_USBD_Core.h"
#include "MW_CV_USBD_CDC_ACM.h"
#include "MW_CV_USBD_HID.h"
#include "MW_CV_USBD_MSC.h"
#endif

#if      (MW_CV_USBH != 0)
#include "MW_CV_USBH_Core.h"
#include "MW_CV_USBH_CDC_ACM.h"
#include "MW_CV_USBH_HID.h"
#include "MW_CV_USBH_MSC.h"

#include "MW_CV_USBH_MSC_Performance.h"
#endif

// Check configuration requirements for Network validation

#if    ((MW_CV_NET == 1) && !defined(CMSIS_DRIVER_ETH))
#error Network component validation requires configured Ethernet peripheral on the Board!
#endif

// Check configuration requirements for USB validation

#if    (((MW_CV_USBH == 1) || (MW_CV_USBD == 1)) && (!defined(CMSIS_DRIVER_USBD) || !defined(CMSIS_DRIVER_USBH)))
#error USB Device/Host component validation requires 2 configured USB peripherals on the Board (one as USB Device and the other as USB Host)!
#endif

#if    (((MW_CV_USBD == 1) && (MW_CV_USBH == 1)) || \
       (((MW_CV_USBD == 1) && ((MW_CV_USBD_CORE + MW_CV_USBD_CDC_ACM + MW_CV_USBD_HID + MW_CV_USBD_MSC) > 1)) || \
        ((MW_CV_USBH == 1) && ((MW_CV_USBH_CORE + MW_CV_USBH_CDC_ACM + MW_CV_USBH_HID + MW_CV_USBH_MSC + MW_CV_USBH_MSC_PERFORMANCE) > 1))))
#error Only one USB Device or Host validation can be selected at a time!
#endif

/*-----------------------------------------------------------------------------
 *      Test suite initialization
 *----------------------------------------------------------------------------*/

static void TestSuiteInit (void) {
#if (MW_CV_FS != 0)
  (void)MW_CV_FS_Format("");
#endif
}

/*-----------------------------------------------------------------------------
 *      List of tests
 *----------------------------------------------------------------------------*/

static TEST_LIST_t test_list[] = {

  /************************* File System Validation ***************************/
#if (MW_CV_FS != 0)
  TEST_UNIT_DEF ("MDK Middleware: File System", MW_CV_FS_GetVersion),

  /* Common FileSystem core API tests */
  TEST_CASE_DEF ( MW_CV_FS_Core_fversion_0                          , "MW_CV_FS_Core_fversion_0"                         , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_fversion_1                          , "MW_CV_FS_Core_fversion_1"                         , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_finit_0                             , "MW_CV_FS_Core_finit_0"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_finit_1                             , "MW_CV_FS_Core_finit_1"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_finit_2                             , "MW_CV_FS_Core_finit_2"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_finit_3                             , "MW_CV_FS_Core_finit_3"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_funinit_0                           , "MW_CV_FS_Core_funinit_0"                          , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_funinit_1                           , "MW_CV_FS_Core_funinit_1"                          , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_funinit_2                           , "MW_CV_FS_Core_funinit_2"                          , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_funinit_3                           , "MW_CV_FS_Core_funinit_3"                          , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_funinit_4                           , "MW_CV_FS_Core_funinit_4"                          , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_fmount_0                            , "MW_CV_FS_Core_fmount_0"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_fmount_1                            , "MW_CV_FS_Core_fmount_1"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_fmount_2                            , "MW_CV_FS_Core_fmount_2"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_fmount_3                            , "MW_CV_FS_Core_fmount_3"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_fmount_4                            , "MW_CV_FS_Core_fmount_4"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_funmount_0                          , "MW_CV_FS_Core_funmount_0"                         , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_funmount_1                          , "MW_CV_FS_Core_funmount_1"                         , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_funmount_2                          , "MW_CV_FS_Core_funmount_2"                         , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_funmount_3                          , "MW_CV_FS_Core_funmount_3"                         , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_funmount_4                          , "MW_CV_FS_Core_funmount_4"                         , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_fdelete_0                           , "MW_CV_FS_Core_fdelete_0"                          , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_fdelete_1                           , "MW_CV_FS_Core_fdelete_1"                          , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_fdelete_2                           , "MW_CV_FS_Core_fdelete_2"                          , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_fdelete_3                           , "MW_CV_FS_Core_fdelete_3"                          , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_ffind_0                             , "MW_CV_FS_Core_ffind_0"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_ffind_1                             , "MW_CV_FS_Core_ffind_1"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_ffind_2                             , "MW_CV_FS_Core_ffind_2"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_ffind_3                             , "MW_CV_FS_Core_ffind_3"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_ffind_4                             , "MW_CV_FS_Core_ffind_4"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_ffind_5                             , "MW_CV_FS_Core_ffind_5"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_ffind_6                             , "MW_CV_FS_Core_ffind_6"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_ffind_7                             , "MW_CV_FS_Core_ffind_7"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_ffind_8                             , "MW_CV_FS_Core_ffind_8"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_ffind_9                             , "MW_CV_FS_Core_ffind_9"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_frename_0                           , "MW_CV_FS_Core_frename_0"                          , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_frename_1                           , "MW_CV_FS_Core_frename_1"                          , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_frename_2                           , "MW_CV_FS_Core_frename_2"                          , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_frename_3                           , "MW_CV_FS_Core_frename_3"                          , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_fchdrive_0                          , "MW_CV_FS_Core_fchdrive_0"                         , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_fchdrive_1                          , "MW_CV_FS_Core_fchdrive_1"                         , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_ffree_0                             , "MW_CV_FS_Core_ffree_0"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_ffree_1                             , "MW_CV_FS_Core_ffree_1"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_ffree_2                             , "MW_CV_FS_Core_ffree_2"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_fformat_0                           , "MW_CV_FS_Core_fformat_0"                          , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_fformat_1                           , "MW_CV_FS_Core_fformat_1"                          , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_fformat_2                           , "MW_CV_FS_Core_fformat_2"                          , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_finfo_0                             , "MW_CV_FS_Core_finfo_0"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_finfo_1                             , "MW_CV_FS_Core_finfo_1"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_finfo_2                             , "MW_CV_FS_Core_finfo_2"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_fs_get_time_0                       , "MW_CV_FS_Core_fs_get_time_0"                      , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_fs_get_time_1                       , "MW_CV_FS_Core_fs_get_time_1"                      , true),
  TEST_CASE_DEF ( MW_CV_FS_Core_fs_get_time_2                       , "MW_CV_FS_Core_fs_get_time_2"                      , true),

#if (MW_CV_FS_DRIVE_F != 0)
  /* EFS-specific API and unsupported-operation tests */
  TEST_CASE_DEF ( MW_CV_FS_EFS_fanalyse_0                                 , "MW_CV_FS_EFS_fanalyse_0"                                 , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_fanalyse_1                           , "MW_CV_FS_EFS_fanalyse_1"                          , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_fanalyse_2                           , "MW_CV_FS_EFS_fanalyse_2"                          , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_fcheck_0                             , "MW_CV_FS_EFS_fcheck_0"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_fcheck_1                             , "MW_CV_FS_EFS_fcheck_1"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_fcheck_2                             , "MW_CV_FS_EFS_fcheck_2"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_fdefrag_0                            , "MW_CV_FS_EFS_fdefrag_0"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_fdefrag_1                            , "MW_CV_FS_EFS_fdefrag_1"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_fdefrag_2                            , "MW_CV_FS_EFS_fdefrag_2"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_fdefrag_3                            , "MW_CV_FS_EFS_fdefrag_3"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_fdefrag_4                            , "MW_CV_FS_EFS_fdefrag_4"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_fdefrag_5                            , "MW_CV_FS_EFS_fdefrag_5"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_fattrib_0                            , "MW_CV_FS_EFS_fattrib_0"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_fpwd_0                               , "MW_CV_FS_EFS_fpwd_0"                              , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_fchdir_0                             , "MW_CV_FS_EFS_fchdir_0"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_fmkdir_0                             , "MW_CV_FS_EFS_fmkdir_0"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_frmdir_0                             , "MW_CV_FS_EFS_frmdir_0"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_fvol_0                               , "MW_CV_FS_EFS_fvol_0"                              , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_ftime_set_0                          , "MW_CV_FS_EFS_ftime_set_0"                         , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_ftime_get_0                          , "MW_CV_FS_EFS_ftime_get_0"                         , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_fs_ioc_get_id_0                      , "MW_CV_FS_EFS_fs_ioc_get_id_0"                     , true),
#else
  /* FAT-specific search, file, directory, volume, and timestamp tests */
  TEST_CASE_DEF ( MW_CV_FS_FAT_ffind_0                              , "MW_CV_FS_FAT_ffind_0"                             , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_ffind_1                              , "MW_CV_FS_FAT_ffind_1"                             , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_ffind_2                              , "MW_CV_FS_FAT_ffind_2"                             , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_ffind_3                              , "MW_CV_FS_FAT_ffind_3"                             , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fattrib_0                            , "MW_CV_FS_FAT_fattrib_0"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fattrib_1                            , "MW_CV_FS_FAT_fattrib_1"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fattrib_2                            , "MW_CV_FS_FAT_fattrib_2"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fpwd_0                               , "MW_CV_FS_FAT_fpwd_0"                              , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fpwd_1                               , "MW_CV_FS_FAT_fpwd_1"                              , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fpwd_2                               , "MW_CV_FS_FAT_fpwd_2"                              , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fchdir_0                             , "MW_CV_FS_FAT_fchdir_0"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fchdir_1                             , "MW_CV_FS_FAT_fchdir_1"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fchdir_2                             , "MW_CV_FS_FAT_fchdir_2"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fchdir_3                             , "MW_CV_FS_FAT_fchdir_3"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fchdir_4                             , "MW_CV_FS_FAT_fchdir_4"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fchdir_5                             , "MW_CV_FS_FAT_fchdir_5"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fmkdir_0                             , "MW_CV_FS_FAT_fmkdir_0"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fmkdir_1                             , "MW_CV_FS_FAT_fmkdir_1"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fmkdir_2                             , "MW_CV_FS_FAT_fmkdir_2"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fmkdir_3                             , "MW_CV_FS_FAT_fmkdir_3"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fmkdir_4                             , "MW_CV_FS_FAT_fmkdir_4"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_frmdir_0                             , "MW_CV_FS_FAT_frmdir_0"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_frmdir_1                             , "MW_CV_FS_FAT_frmdir_1"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_frmdir_2                             , "MW_CV_FS_FAT_frmdir_2"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_frmdir_3                             , "MW_CV_FS_FAT_frmdir_3"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_frmdir_4                             , "MW_CV_FS_FAT_frmdir_4"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fvol_0                               , "MW_CV_FS_FAT_fvol_0"                              , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fvol_1                               , "MW_CV_FS_FAT_fvol_1"                              , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_fvol_2                               , "MW_CV_FS_FAT_fvol_2"                              , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_ftime_set_0                          , "MW_CV_FS_FAT_ftime_set_0"                         , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_ftime_set_1                          , "MW_CV_FS_FAT_ftime_set_1"                         , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_ftime_set_2                          , "MW_CV_FS_FAT_ftime_set_2"                         , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_ftime_set_3                          , "MW_CV_FS_FAT_ftime_set_3"                         , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_ftime_get_0                          , "MW_CV_FS_FAT_ftime_get_0"                         , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_ftime_get_1                          , "MW_CV_FS_FAT_ftime_get_1"                         , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_ftime_get_2                          , "MW_CV_FS_FAT_ftime_get_2"                         , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_ftime_get_3                          , "MW_CV_FS_FAT_ftime_get_3"                         , true),
#endif

#if (MW_CV_FS_CLIB != 0)
  /* Common C library stream interface tests */
  TEST_CASE_DEF ( MW_CV_FS_Clib_fopen_0                             , "MW_CV_FS_Clib_fopen_0"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fopen_1                             , "MW_CV_FS_Clib_fopen_1"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fopen_2                             , "MW_CV_FS_Clib_fopen_2"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fopen_3                             , "MW_CV_FS_Clib_fopen_3"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fopen_4                             , "MW_CV_FS_Clib_fopen_4"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fopen_5                             , "MW_CV_FS_Clib_fopen_5"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fopen_6                             , "MW_CV_FS_Clib_fopen_6"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fopen_7                             , "MW_CV_FS_Clib_fopen_7"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fopen_8                             , "MW_CV_FS_Clib_fopen_8"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fopen_9                             , "MW_CV_FS_Clib_fopen_9"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fclose_0                            , "MW_CV_FS_Clib_fclose_0"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fclose_1                            , "MW_CV_FS_Clib_fclose_1"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fclose_2                            , "MW_CV_FS_Clib_fclose_2"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fwrite_0                            , "MW_CV_FS_Clib_fwrite_0"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fwrite_1                            , "MW_CV_FS_Clib_fwrite_1"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fwrite_2                            , "MW_CV_FS_Clib_fwrite_2"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fwrite_3                            , "MW_CV_FS_Clib_fwrite_3"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fwrite_4                            , "MW_CV_FS_Clib_fwrite_4"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fread_0                             , "MW_CV_FS_Clib_fread_0"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fread_1                             , "MW_CV_FS_Clib_fread_1"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fread_2                             , "MW_CV_FS_Clib_fread_2"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fread_3                             , "MW_CV_FS_Clib_fread_3"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fread_4                             , "MW_CV_FS_Clib_fread_4"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fgetc_0                             , "MW_CV_FS_Clib_fgetc_0"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fgetc_1                             , "MW_CV_FS_Clib_fgetc_1"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fgetc_2                             , "MW_CV_FS_Clib_fgetc_2"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fseek_0                             , "MW_CV_FS_Clib_fseek_0"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fseek_1                             , "MW_CV_FS_Clib_fseek_1"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fseek_2                             , "MW_CV_FS_Clib_fseek_2"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fseek_3                             , "MW_CV_FS_Clib_fseek_3"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fseek_4                             , "MW_CV_FS_Clib_fseek_4"                            , true),
  TEST_CASE_DEF ( MW_CV_FS_Clib_fseek_5                             , "MW_CV_FS_Clib_fseek_5"                            , true),
#if (MW_CV_FS_DRIVE_F != 0)
  /* EFS-specific C library limitations and filename tests */
  TEST_CASE_DEF ( MW_CV_FS_EFS_Clib_fopen_0                         , "MW_CV_FS_EFS_Clib_fopen_0"                        , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_Clib_fopen_1                         , "MW_CV_FS_EFS_Clib_fopen_1"                        , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_Clib_fopen_2                         , "MW_CV_FS_EFS_Clib_fopen_2"                        , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_Clib_fopen_3                         , "MW_CV_FS_EFS_Clib_fopen_3"                        , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_Clib_fopen_4                         , "MW_CV_FS_EFS_Clib_fopen_4"                        , true),
  TEST_CASE_DEF ( MW_CV_FS_EFS_Clib_fseek_0                         , "MW_CV_FS_EFS_Clib_fseek_0"                        , true),
#else
  /* FAT-specific update modes, paths, filenames, and seek tests */
  TEST_CASE_DEF ( MW_CV_FS_FAT_Clib_fopen_0                         , "MW_CV_FS_FAT_Clib_fopen_0"                        , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_Clib_fopen_1                         , "MW_CV_FS_FAT_Clib_fopen_1"                        , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_Clib_fopen_2                         , "MW_CV_FS_FAT_Clib_fopen_2"                        , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_Clib_fopen_3                         , "MW_CV_FS_FAT_Clib_fopen_3"                        , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_Clib_fopen_4                         , "MW_CV_FS_FAT_Clib_fopen_4"                        , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_Clib_fopen_5                         , "MW_CV_FS_FAT_Clib_fopen_5"                        , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_Clib_fopen_6                         , "MW_CV_FS_FAT_Clib_fopen_6"                        , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_Clib_fopen_7                         , "MW_CV_FS_FAT_Clib_fopen_7"                        , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_Clib_fseek_0                         , "MW_CV_FS_FAT_Clib_fseek_0"                        , true),
  TEST_CASE_DEF ( MW_CV_FS_FAT_Clib_fseek_1                         , "MW_CV_FS_FAT_Clib_fseek_1"                        , true),
#endif
#endif

#if (MW_CV_FS_DRIVE_M != 0)
  /* Removable-media behavior and Memory Card integration callbacks */
  TEST_CASE_DEF ( MW_CV_FS_MC_fmedia_0                              , "MW_CV_FS_MC_fmedia_0"                             , true),
  TEST_CASE_DEF ( MW_CV_FS_MC_fmedia_1                              , "MW_CV_FS_MC_fmedia_1"                             , true),
  TEST_CASE_DEF ( MW_CV_FS_MC_fs_mc_read_cd_0                       , "MW_CV_FS_MC_fs_mc_read_cd_0"                      , true),
  TEST_CASE_DEF ( MW_CV_FS_MC_fs_mc_read_cd_1                       , "MW_CV_FS_MC_fs_mc_read_cd_1"                      , true),
  TEST_CASE_DEF ( MW_CV_FS_MC_fs_mc_read_wp_0                       , "MW_CV_FS_MC_fs_mc_read_wp_0"                      , true),
  TEST_CASE_DEF ( MW_CV_FS_MC_fs_mc_read_wp_1                       , "MW_CV_FS_MC_fs_mc_read_wp_1"                      , true),
  TEST_CASE_DEF ( MW_CV_FS_MC_fs_mc_spi_control_ss_0                , "MW_CV_FS_MC_fs_mc_spi_control_ss_0"               , true),
  TEST_CASE_DEF ( MW_CV_FS_MC_fs_mc_spi_control_ss_1                , "MW_CV_FS_MC_fs_mc_spi_control_ss_1"               , true),
  TEST_CASE_DEF ( MW_CV_FS_MC_fs_mc_spi_control_ss_2                , "MW_CV_FS_MC_fs_mc_spi_control_ss_2"               , true),
  TEST_CASE_DEF ( MW_CV_FS_MC_fs_mc_lock_unlock_0                   , "MW_CV_FS_MC_fs_mc_lock_unlock_0"                  , true),
  TEST_CASE_DEF ( MW_CV_FS_MC_fs_mc_lock_unlock_1                   , "MW_CV_FS_MC_fs_mc_lock_unlock_1"                  , true),
#endif

#if (MW_CV_FS_STRESS != 0)
  /* Cross-API persistence, copy, concurrency, and cache stress tests */
  TEST_CASE_DEF ( MW_CV_FS_Stress_persistence_0                     , "MW_CV_FS_Stress_persistence_0"                    , true),
  TEST_CASE_DEF ( MW_CV_FS_Stress_persistence_1                     , "MW_CV_FS_Stress_persistence_1"                    , true),
  TEST_CASE_DEF ( MW_CV_FS_Stress_copy_0                            , "MW_CV_FS_Stress_copy_0"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_Stress_copy_1                            , "MW_CV_FS_Stress_copy_1"                           , true),
  TEST_CASE_DEF ( MW_CV_FS_Stress_concurrency_0                     , "MW_CV_FS_Stress_concurrency_0"                    , true),
  TEST_CASE_DEF ( MW_CV_FS_Stress_concurrency_1                     , "MW_CV_FS_Stress_concurrency_1"                    , true),
  TEST_CASE_DEF ( MW_CV_FS_Stress_concurrency_2                     , "MW_CV_FS_Stress_concurrency_2"                    , true),
#if (MW_CV_FS_DRIVE_F != 0)
  TEST_CASE_DEF ( MW_CV_FS_Stress_EFS_ffind_0                       , "MW_CV_FS_Stress_EFS_ffind_0"                      , true),
#endif
#if (MW_CV_FS_DRIVE_M != 0)
  TEST_CASE_DEF ( MW_CV_FS_Stress_FAT_ffind_0                       , "MW_CV_FS_Stress_FAT_ffind_0"                      , true),
  TEST_CASE_DEF ( MW_CV_FS_Stress_FAT_name_cache_0                  , "MW_CV_FS_Stress_FAT_name_cache_0"                 , true),
  TEST_CASE_DEF ( MW_CV_FS_Stress_FAT_name_cache_1                  , "MW_CV_FS_Stress_FAT_name_cache_1"                 , true),
  TEST_CASE_DEF ( MW_CV_FS_Stress_FAT_name_cache_2                  , "MW_CV_FS_Stress_FAT_name_cache_2"                 , true),
  TEST_CASE_DEF ( MW_CV_FS_Stress_FAT_name_cache_3                  , "MW_CV_FS_Stress_FAT_name_cache_3"                 , true),
  TEST_CASE_DEF ( MW_CV_FS_Stress_FAT_name_cache_4                  , "MW_CV_FS_Stress_FAT_name_cache_4"                 , true),
#endif
#endif

#if ((MW_CV_FS_IOC != 0) && (MW_CV_FS_DRIVE_F == 0))
  /* FAT media I/O Control tests run after the complete stress suite */
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_get_id_0                      , "MW_CV_FS_IOC_fs_ioc_get_id_0"                     , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_get_id_1                      , "MW_CV_FS_IOC_fs_ioc_get_id_1"                     , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_lock_0                        , "MW_CV_FS_IOC_fs_ioc_lock_0"                       , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_lock_1                        , "MW_CV_FS_IOC_fs_ioc_lock_1"                       , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_lock_2                        , "MW_CV_FS_IOC_fs_ioc_lock_2"                       , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_unlock_0                      , "MW_CV_FS_IOC_fs_ioc_unlock_0"                     , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_unlock_1                      , "MW_CV_FS_IOC_fs_ioc_unlock_1"                     , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_unlock_2                      , "MW_CV_FS_IOC_fs_ioc_unlock_2"                     , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_get_cache_0                   , "MW_CV_FS_IOC_fs_ioc_get_cache_0"                  , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_get_cache_1                   , "MW_CV_FS_IOC_fs_ioc_get_cache_1"                  , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_get_cache_2                   , "MW_CV_FS_IOC_fs_ioc_get_cache_2"                  , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_read_sector_0                 , "MW_CV_FS_IOC_fs_ioc_read_sector_0"                , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_read_sector_1                 , "MW_CV_FS_IOC_fs_ioc_read_sector_1"                , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_read_sector_2                 , "MW_CV_FS_IOC_fs_ioc_read_sector_2"                , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_read_info_0                   , "MW_CV_FS_IOC_fs_ioc_read_info_0"                  , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_read_info_1                   , "MW_CV_FS_IOC_fs_ioc_read_info_1"                  , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_read_info_2                   , "MW_CV_FS_IOC_fs_ioc_read_info_2"                  , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_device_ctrl_0                 , "MW_CV_FS_IOC_fs_ioc_device_ctrl_0"                , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_device_ctrl_1                 , "MW_CV_FS_IOC_fs_ioc_device_ctrl_1"                , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_write_sector_0                , "MW_CV_FS_IOC_fs_ioc_write_sector_0"               , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_write_sector_1                , "MW_CV_FS_IOC_fs_ioc_write_sector_1"               , true),
  TEST_CASE_DEF ( MW_CV_FS_IOC_fs_ioc_write_sector_2                , "MW_CV_FS_IOC_fs_ioc_write_sector_2"               , true),
#endif

#if (MW_CV_FS_PERFORMANCE != 0)
  /* Destructive performance measurements run after all FileSystem stress tests */
#if (MW_CV_FS_CLIB != 0)
  TEST_CASE_DEF ( MW_CV_FS_Performance_Clib                         , "MW_CV_FS_Performance_Clib"                        , true),
#endif
#if ((MW_CV_FS_IOC != 0) && (MW_CV_FS_DRIVE_F == 0))
  TEST_CASE_DEF ( MW_CV_FS_Performance_IOC                          , "MW_CV_FS_Performance_IOC"                         , true),
#endif
#endif
#endif

  /**************************** Network Validation ****************************/
#if (MW_CV_NET != 0)
  TEST_UNIT_DEF ("MDK Middleware: Network"  ,  BSD_GetVersion                  ),
  TEST_CASE_DEF ( BSD_Initialize            , "BSD_Initialize"           , true),
#if (MW_CV_NET_BSD_API != 0)
  /* BSD socket API tests */
  TEST_CASE_DEF ( BSD_SocketCreate          , "BSD_SocketCreate"         , true),
  TEST_CASE_DEF ( BSD_SocketBind            , "BSD_SocketBind"           , true),
  TEST_CASE_DEF ( BSD_SocketListen          , "BSD_SocketListen"         , true),
  TEST_CASE_DEF ( BSD_SocketAccept          , "BSD_SocketAccept"         , true),
  TEST_CASE_DEF ( BSD_SocketAccept_nbio     , "BSD_SocketAccept_nbio"    , true),
  TEST_CASE_DEF ( BSD_SocketConnect         , "BSD_SocketConnect"        , true),
  TEST_CASE_DEF ( BSD_SocketConnect_nbio    , "BSD_SocketConnect_nbio"   , true),
  TEST_CASE_DEF ( BSD_SocketRecv            , "BSD_SocketRecv"           , true),
  TEST_CASE_DEF ( BSD_SocketRecv_nbio       , "BSD_SocketRecv_nbio"      , true),
  TEST_CASE_DEF ( BSD_SocketRecvFrom        , "BSD_SocketRecvFrom"       , true),
  TEST_CASE_DEF ( BSD_SocketRecvFrom_nbio   , "BSD_SocketRecvFrom_nbio"  , true),
  TEST_CASE_DEF ( BSD_SocketRecvMsg         , "BSD_SocketRecvMsg"        , true),
  TEST_CASE_DEF ( BSD_SocketSend            , "BSD_SocketSend"           , true),
  TEST_CASE_DEF ( BSD_SocketSendTo          , "BSD_SocketSendTo"         , true),
  TEST_CASE_DEF ( BSD_SocketSendMsg         , "BSD_SocketSendMsg"        , true),
  TEST_CASE_DEF ( BSD_SocketGetSockName     , "BSD_SocketGetSockName"    , true),
  TEST_CASE_DEF ( BSD_SocketGetPeerName     , "BSD_SocketGetPeerName"    , true),
  TEST_CASE_DEF ( BSD_SocketGetOpt          , "BSD_SocketGetOpt"         , true),
  TEST_CASE_DEF ( BSD_SocketSetOpt          , "BSD_SocketSetOpt"         , true),
  TEST_CASE_DEF ( BSD_SocketClose           , "BSD_SocketClose"          , true),
  TEST_CASE_DEF ( BSD_SocketIoctl           , "BSD_SocketIoctl"          , true),
  TEST_CASE_DEF ( BSD_SocketSelect          , "BSD_SocketSelect"         , true),
  TEST_CASE_DEF ( BSD_GetHostByName         , "BSD_GetHostByName"        , true),
  TEST_CASE_DEF ( BSD_PingEcho              , "BSD_PingEcho"             , true),
#endif
#if (MW_CV_NET_BSD_OPERATION != 0)
  /* BSD socket operation tests */
  TEST_CASE_DEF ( BSD_Transfer_Fixed        , "BSD_Transfer_Fixed"       , true),
  TEST_CASE_DEF ( BSD_Transfer_Incremental  , "BSD_Transfer_Incremental" , true),
  TEST_CASE_DEF ( BSD_Send_Fragmented       , "BSD_Send_Fragmented"      , true),
  TEST_CASE_DEF ( BSD_Recv_Fragmented       , "BSD_Recv_Fragmented"      , true),
  TEST_CASE_DEF ( BSD_Test_Speed            , "BSD_Test_Speed"           , true),
  TEST_CASE_DEF ( BSD_Concurrent_Socket     , "BSD_Concurrent_Socket"    , true),
  TEST_CASE_DEF ( BSD_Downstream_Rate       , "BSD_Downstream_Rate"      , true),
  TEST_CASE_DEF ( BSD_Upstream_Rate         , "BSD_Upstream_Rate"        , true),
#endif
  TEST_CASE_DEF ( BSD_Uninitialize          , "BSD_Uninitialize"         , true),
#endif

  /**************************** USB Validation ********************************/
#if (MW_CV_USBD != 0)
#if (MW_CV_USBD_CORE != 0)
  TEST_UNIT_DEF ("MDK Middleware: USB Device - Core"                ,       USBD_GetVersion                                    ),
  TEST_CASE_DEF ( MW_CV_USBD_GetVersion                             , "API: USBD_GetVersion"                             , true),
  TEST_CASE_DEF ( MW_CV_USBD_Initialize                             , "API: USBD_Initialize"                             , true),
  TEST_CASE_DEF ( MW_CV_USBD_Connect                                , "API: USBD_Connect"                                , true),
  TEST_CASE_DEF ( MW_CV_USBD_Configured                             , "API: USBD_Configured"                             , true),
  TEST_CASE_DEF ( MW_CV_USBD_GetState                               , "API: USBD_GetState"                               , true),
  TEST_CASE_DEF ( MW_CV_USBD_SetSerialNumber                        , "API: USBD_SetSerialNumber"                        , true),
  TEST_CASE_DEF ( MW_CV_USBD_Disconnect                             , "API: USBD_Disconnect"                             , true),
  TEST_CASE_DEF ( MW_CV_USBD_Uninitialize                           , "API: USBD_Uninitialize"                           , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_Initialize                     , "CB : USBD_Device0_Initialize"                     , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_VbusChanged                    , "CB : USBD_Device0_VbusChanged"                    , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_Reset                          , "CB : USBD_Device0_Reset"                          , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_HighSpeedActivated             , "CB : USBD_Device0_HighSpeedActivated"             , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_Suspended                      , "CB : USBD_Device0_Suspended"                      , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_Resumed                        , "CB : USBD_Device0_Resumed"                        , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_ConfigurationChanged           , "CB : USBD_Device0_ConfigurationChanged"           , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_EnableRemoteWakeup             , "CB : USBD_Device0_EnableRemoteWakeup"             , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_DisableRemoteWakeup            , "CB : USBD_Device0_DisableRemoteWakeup"            , true),
//TEST_CASE_DEF ( MW_CV_USBD_Device0_Endpoint0_SetupPacketReceived  , "CB : USBD_Device0_Endpoint0_SetupPacketReceived"  , true),
//TEST_CASE_DEF ( MW_CV_USBD_Device0_Endpoint0_SetupPacketProcessed , "CB : USBD_Device0_Endpoint0_SetupPacketProcessed" , true),
//TEST_CASE_DEF ( MW_CV_USBD_Device0_Endpoint0_OutDataReceived      , "CB : USBD_Device0_Endpoint0_OutDataReceived"      , true),
//TEST_CASE_DEF ( MW_CV_USBD_Device0_Endpoint0_InDataSent           , "CB : USBD_Device0_Endpoint0_InDataSent"           , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_Uninitialize                   , "CB : USBD_Device0_Uninitialize"                   , true),
#endif
#if (MW_CV_USBD_CDC_ACM != 0)
  TEST_UNIT_DEF ("MDK Middleware: USB Device - CDC ACM"             ,       USBD_GetVersion                                    ),
  TEST_CASE_DEF ( MW_CV_USBD_CDC0_ACM_Initialize                    , "CB : USBD_CDC0_ACM_Initialize"                    , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC0_ACM_Reset                         , "CB : USBD_CDC0_ACM_Reset"                         , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC0_ACM_SetLineCoding                 , "CB : USBD_CDC0_ACM_SetLineCoding"                 , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC0_ACM_GetLineCoding                 , "CB : USBD_CDC0_ACM_GetLineCoding"                 , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC0_ACM_SetControlLineState           , "CB : USBD_CDC0_ACM_SetControlLineState"           , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC0_ACM_DataReceived                  , "CB : USBD_CDC0_ACM_DataReceived"                  , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC0_ACM_DataSent                      , "CB : USBD_CDC0_ACM_DataSent"                      , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC_ACM_GetChar                        , "API: USBD_CDC_ACM_GetChar"                        , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC_ACM_PutChar                        , "API: USBD_CDC_ACM_PutChar"                        , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC_ACM_WriteData                      , "API: USBD_CDC_ACM_WriteData"                      , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC_ACM_DataAvailable                  , "API: USBD_CDC_ACM_DataAvailable"                  , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC_ACM_Notify                         , "API: USBD_CDC_ACM_Notify"                         , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC0_ACM_Uninitialize                  , "CB : USBD_CDC0_ACM_Uninitialize"                  , true),
#endif
#if (MW_CV_USBD_HID != 0)
  TEST_UNIT_DEF ("MDK Middleware: USB Device - HID"                 ,       USBD_GetVersion                                    ),
  TEST_CASE_DEF ( MW_CV_USBD_HID0_Initialize                        , "CB : USBD_HID0_Initialize"                        , true),
  TEST_CASE_DEF ( MW_CV_USBD_HID0_GetReport                         , "CB : USBD_HID0_GetReport"                         , true),
  TEST_CASE_DEF ( MW_CV_USBD_HID0_SetReport                         , "CB : USBD_HID0_SetReport"                         , true),
  TEST_CASE_DEF ( MW_CV_USBD_HID_GetReportTrigger                   , "API: USBD_HID_GetReportTrigger"                   , true),
  TEST_CASE_DEF ( MW_CV_USBD_HID0_Uninitialize                      , "CB : USBD_HID0_Uninitialize"                      , true),
#endif
#if (MW_CV_USBD_MSC != 0)
  TEST_UNIT_DEF ("MDK Middleware: USB Device - MSC"                 ,       USBD_GetVersion                                    ),
  TEST_CASE_DEF ( MW_CV_USBD_MSC0_Initialize                        , "CB : USBD_MSC0_Initialize"                        , true),
  TEST_CASE_DEF ( MW_CV_USBD_MSC0_GetCacheInfo                      , "CB : USBD_MSC0_GetCacheInfo"                      , true),
  TEST_CASE_DEF ( MW_CV_USBD_MSC0_GetMediaCapacity                  , "CB : USBD_MSC0_GetMediaCapacity"                  , true),
  TEST_CASE_DEF ( MW_CV_USBD_MSC0_Write_Read                        , "CB : USBD_MSC0_Write/Read"                        , true),
  TEST_CASE_DEF ( MW_CV_USBD_MSC0_CheckMedia                        , "CB : USBD_MSC0_CheckMedia"                        , true),
//TEST_CASE_DEF ( MW_CV_USBD_MSC0_GetMaxLUN                         , "CB : USBD_MSC0_GetMaxLUN"                         , true),
//TEST_CASE_DEF ( MW_CV_USBD_MSC0_LUN_GetMediaCapacity              , "CB : USBD_MSC0_LUN_GetMediaCapacity"              , true),
//TEST_CASE_DEF ( MW_CV_USBD_MSC0_LUN_Read                          , "CB : USBD_MSC0_LUN_Read"                          , true),
//TEST_CASE_DEF ( MW_CV_USBD_MSC0_LUN_Write                         , "CB : USBD_MSC0_LUN_Write"                         , true),
//TEST_CASE_DEF ( MW_CV_USBD_MSC0_LUN_CheckMedia                    , "CB : USBD_MSC0_LUN_CheckMedia"                    , true),
  TEST_CASE_DEF ( MW_CV_USBD_MSC0_Uninitialize                      , "CB : USBD_MSC0_Uninitialize"                      , true),
#endif
#endif

#if (MW_CV_USBH      != 0)
#if (MW_CV_USBH_CORE != 0)
  TEST_UNIT_DEF ("MDK Middleware: USB Host - Core"                  ,       USBH_GetVersion                                    ),
  TEST_CASE_DEF ( MW_CV_USBH_GetVersion                             , "API: USBH_GetVersion"                             , true),
  TEST_CASE_DEF ( MW_CV_USBH_Initialize                             , "API: USBH_Initialize"                             , true),
  TEST_CASE_DEF ( MW_CV_USBH_Port_Suspend                           , "API: USBH_Port_Suspend"                           , true),
  TEST_CASE_DEF ( MW_CV_USBH_Port_Resume                            , "API: USBH_Port_Resume"                            , true),
  TEST_CASE_DEF ( MW_CV_USBH_Device_GetController                   , "API: USBH_Device_GetController"                   , true),
  TEST_CASE_DEF ( MW_CV_USBH_Device_GetPort                         , "API: USBH_Device_GetPort"                         , true),
  TEST_CASE_DEF ( MW_CV_USBH_Device_GetStatus                       , "API: USBH_Device_GetStatus"                       , true),
  TEST_CASE_DEF ( MW_CV_USBH_Device_GetSpeed                        , "API: USBH_Device_GetSpeed"                        , true),
  TEST_CASE_DEF ( MW_CV_USBH_Device_GetAddress                      , "API: USBH_Device_GetAddress"                      , true),
  TEST_CASE_DEF ( MW_CV_USBH_Device_GetVID                          , "API: USBH_Device_GetVID"                          , true),
  TEST_CASE_DEF ( MW_CV_USBH_Device_GetPID                          , "API: USBH_Device_GetPID"                          , true),
  TEST_CASE_DEF ( MW_CV_USBH_Device_GetStringDescriptor             , "API: USBH_Device_GetStringDescriptor"             , true),
  TEST_CASE_DEF ( MW_CV_USBH_Uninitialize                           , "API: USBH_Uninitialize"                           , true),
  TEST_CASE_DEF ( MW_CV_USBH_Notify                                 , "CB : USBH_Notify"                                 , true),
#endif
#if (MW_CV_USBH_CDC_ACM != 0)
  TEST_UNIT_DEF ("MDK Middleware: USB Host - CDC ACM"               ,       USBH_GetVersion                                    ),
  TEST_CASE_DEF ( MW_CV_USBH_CDC_ACM_Initialize                     , "CB : USBH_CDC_ACM_Initialize"                     , true),
  TEST_CASE_DEF ( MW_CV_USBH_CDC_ACM_GetDevice                      , "API: USBH_CDC_ACM_GetDevice"                      , true),
  TEST_CASE_DEF ( MW_CV_USBH_CDC_ACM_GetStatus                      , "API: USBH_CDC_ACM_GetStatus"                      , true),
  TEST_CASE_DEF ( MW_CV_USBH_CDC_ACM_Send_Receive                   , "API: USBH_CDC_ACM_Send/Receive"                   , true),
  TEST_CASE_DEF ( MW_CV_USBH_CDC_ACM_SetGetLineCoding               , "API: USBH_CDC_ACM_Set/GetLineCoding"              , true),
  TEST_CASE_DEF ( MW_CV_USBH_CDC_ACM_SetControlLineState            , "API: USBH_CDC_ACM_SetControlLineState"            , true),
  TEST_CASE_DEF ( MW_CV_USBH_CDC_ACM_Notify                         , "API: USBH_CDC_ACM_Notify"                         , true),
//TEST_CASE_DEF ( MW_CV_USBH_CDC_ACM_SendBreak                      , "API: USBH_CDC_ACM_SendBreak"                      , true),
  TEST_CASE_DEF ( MW_CV_USBH_CDC_ACM_Uninitialize                   , "CB : USBH_CDC_ACM_Uninitialize"                   , true),
#endif
#if (MW_CV_USBH_HID != 0)
  TEST_UNIT_DEF ("MDK Middleware: USB Host - HID"                   ,       USBH_GetVersion                                    ),
  TEST_CASE_DEF ( MW_CV_USBH_HID_Initialize                         , "CB : USBH_HID_Initialize"                         , true),
  TEST_CASE_DEF ( MW_CV_USBH_HID_GetDevice                          , "API: USBH_HID_GetDevice"                          , true),
  TEST_CASE_DEF ( MW_CV_USBH_HID_GetStatus                          , "API: USBH_HID_GetStatus"                          , true),
  TEST_CASE_DEF ( MW_CV_USBH_HID_Write_Read                         , "API: USBH_HID_Write/Read"                         , true),
//TEST_CASE_DEF ( MW_CV_USBH_HID_GetKeyboardKey                     , "API: USBH_HID_GetKeyboardKey"                     , true),
//TEST_CASE_DEF ( MW_CV_USBH_HID_GetMouseState                      , "API: USBH_HID_GetMouseState"                      , true),
  TEST_CASE_DEF ( MW_CV_USBH_HID_ParseReportDescriptor              , "CB : USBH_HID_ParseReportDescriptor"              , true),
  TEST_CASE_DEF ( MW_CV_USBH_HID_DataReceived                       , "CB : USBH_HID_DataReceived"                       , true),
  TEST_CASE_DEF ( MW_CV_USBH_HID_Uninitialize                       , "CB : USBH_HID_Uninitialize"                       , true),
#endif
#if (MW_CV_USBH_MSC != 0)
  TEST_UNIT_DEF ("MDK Middleware: USB Host - MSC"                   ,       USBH_GetVersion                                    ),
  TEST_CASE_DEF ( MW_CV_USBH_MSC_Initialize                         , "CB : USBH_MSC_Initialize"                         , true),
  TEST_CASE_DEF ( MW_CV_USBH_MSC_GetDevice                          , "API: USBH_MSC_GetDevice"                          , true),
  TEST_CASE_DEF ( MW_CV_USBH_MSC_GetStatus                          , "API: USBH_MSC_GetStatus"                          , true),
  TEST_CASE_DEF ( MW_CV_USBH_MSC_Write_Read                         , "API: USBH_MSC_Write/Read"                         , true),
  TEST_CASE_DEF ( MW_CV_USBH_MSC_ReadCapacity                       , "API: USBH_MSC_ReadCapacity"                       , true),
  TEST_CASE_DEF ( MW_CV_USBH_MSC_Uninitialize                       , "CB : USBH_MSC_Uninitialize"                       , true),
#endif
#if (MW_CV_USBH_MSC_PERFORMANCE != 0)
  TEST_UNIT_DEF ("MDK Middleware: USB Host - Performance"           ,       USBH_GetVersion                                    ),
  TEST_CASE_DEF ( MW_CV_USBH_MSC_Performance                        , "MSC Performance: Write/Read performance"          , true),
#endif
#endif

  // Dummy definition to provide at least one entry in case no tests are defined, not to be executed
  TEST_UNIT_DEF ("No tests selected!"                               ,       NULL                                               )
};

/*-----------------------------------------------------------------------------
 *      Test suite description
 *----------------------------------------------------------------------------*/

TEST_SUITE_t test_suite = {
  "MDK Middleware Component Test Suite v1.0.0",
  __DATE__,
  __TIME__,
  __FILE__,
  TestSuiteInit,
  test_list,
 (sizeof(test_list)/sizeof((test_list)[0])) - 1U,
};
