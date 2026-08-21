/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - File System - Core API
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_FS_CORE_H_
#define MW_CV_FS_CORE_H_

#define MW_CV_FS_CORE_TESTS(X)                                                                                         \
  X(MW_CV_FS_Core_fversion_0)                                                                                         \
  X(MW_CV_FS_Core_fversion_1)                                                                                         \
  X(MW_CV_FS_Core_finit_0)                                                                                            \
  X(MW_CV_FS_Core_finit_1)                                                                                            \
  X(MW_CV_FS_Core_finit_2)                                                                                            \
  X(MW_CV_FS_Core_finit_3)                                                                                            \
  X(MW_CV_FS_Core_funinit_0)                                                                                          \
  X(MW_CV_FS_Core_funinit_1)                                                                                          \
  X(MW_CV_FS_Core_funinit_2)                                                                                          \
  X(MW_CV_FS_Core_funinit_3)                                                                                          \
  X(MW_CV_FS_Core_funinit_4)                                                                                          \
  X(MW_CV_FS_Core_fmount_0)                                                                                           \
  X(MW_CV_FS_Core_fmount_1)                                                                                           \
  X(MW_CV_FS_Core_fmount_2)                                                                                           \
  X(MW_CV_FS_Core_fmount_3)                                                                                           \
  X(MW_CV_FS_Core_fmount_4)                                                                                           \
  X(MW_CV_FS_Core_funmount_0)                                                                                         \
  X(MW_CV_FS_Core_funmount_1)                                                                                         \
  X(MW_CV_FS_Core_funmount_2)                                                                                         \
  X(MW_CV_FS_Core_funmount_3)                                                                                         \
  X(MW_CV_FS_Core_funmount_4)                                                                                         \
  X(MW_CV_FS_Core_fdelete_0)                                                                                          \
  X(MW_CV_FS_Core_fdelete_1)                                                                                          \
  X(MW_CV_FS_Core_fdelete_2)                                                                                          \
  X(MW_CV_FS_Core_fdelete_3)                                                                                          \
  X(MW_CV_FS_Core_ffind_0)                                                                                            \
  X(MW_CV_FS_Core_ffind_1)                                                                                            \
  X(MW_CV_FS_Core_ffind_2)                                                                                            \
  X(MW_CV_FS_Core_ffind_3)                                                                                            \
  X(MW_CV_FS_Core_ffind_4)                                                                                            \
  X(MW_CV_FS_Core_ffind_5)                                                                                            \
  X(MW_CV_FS_Core_ffind_6)                                                                                            \
  X(MW_CV_FS_Core_ffind_7)                                                                                            \
  X(MW_CV_FS_Core_ffind_8)                                                                                            \
  X(MW_CV_FS_Core_ffind_9)                                                                                            \
  X(MW_CV_FS_Core_frename_0)                                                                                          \
  X(MW_CV_FS_Core_frename_1)                                                                                          \
  X(MW_CV_FS_Core_frename_2)                                                                                          \
  X(MW_CV_FS_Core_frename_3)                                                                                          \
  X(MW_CV_FS_Core_fchdrive_0)                                                                                         \
  X(MW_CV_FS_Core_fchdrive_1)                                                                                         \
  X(MW_CV_FS_Core_ffree_0)                                                                                            \
  X(MW_CV_FS_Core_ffree_1)                                                                                            \
  X(MW_CV_FS_Core_ffree_2)                                                                                            \
  X(MW_CV_FS_Core_fformat_0)                                                                                          \
  X(MW_CV_FS_Core_fformat_1)                                                                                          \
  X(MW_CV_FS_Core_fformat_2)                                                                                          \
  X(MW_CV_FS_Core_finfo_0)                                                                                            \
  X(MW_CV_FS_Core_finfo_1)                                                                                            \
  X(MW_CV_FS_Core_finfo_2)                                                                                            \
  X(MW_CV_FS_Core_fs_get_time_0)                                                                                      \
  X(MW_CV_FS_Core_fs_get_time_1)                                                                                      \
  X(MW_CV_FS_Core_fs_get_time_2)

#define MW_CV_FS_DECLARE_TEST(name) extern void name (void);
MW_CV_FS_CORE_TESTS(MW_CV_FS_DECLARE_TEST)
#undef MW_CV_FS_DECLARE_TEST

#endif /* MW_CV_FS_CORE_H_ */
