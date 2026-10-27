/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - File System - FAT
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_FS_FAT_H_
#define MW_CV_FS_FAT_H_

#define MW_CV_FS_FAT_TESTS(X)                                                                                          \
  X(MW_CV_FS_FAT_ffind_0)                                                                                             \
  X(MW_CV_FS_FAT_ffind_1)                                                                                             \
  X(MW_CV_FS_FAT_ffind_2)                                                                                             \
  X(MW_CV_FS_FAT_ffind_3)                                                                                             \
  X(MW_CV_FS_FAT_fattrib_0)                                                                                           \
  X(MW_CV_FS_FAT_fattrib_1)                                                                                           \
  X(MW_CV_FS_FAT_fattrib_2)                                                                                           \
  X(MW_CV_FS_FAT_fpwd_0)                                                                                              \
  X(MW_CV_FS_FAT_fpwd_1)                                                                                              \
  X(MW_CV_FS_FAT_fpwd_2)                                                                                              \
  X(MW_CV_FS_FAT_fchdir_0)                                                                                            \
  X(MW_CV_FS_FAT_fchdir_1)                                                                                            \
  X(MW_CV_FS_FAT_fchdir_2)                                                                                            \
  X(MW_CV_FS_FAT_fchdir_3)                                                                                            \
  X(MW_CV_FS_FAT_fchdir_4)                                                                                            \
  X(MW_CV_FS_FAT_fchdir_5)                                                                                            \
  X(MW_CV_FS_FAT_fmkdir_0)                                                                                            \
  X(MW_CV_FS_FAT_fmkdir_1)                                                                                            \
  X(MW_CV_FS_FAT_fmkdir_2)                                                                                            \
  X(MW_CV_FS_FAT_fmkdir_3)                                                                                            \
  X(MW_CV_FS_FAT_fmkdir_4)                                                                                            \
  X(MW_CV_FS_FAT_frmdir_0)                                                                                            \
  X(MW_CV_FS_FAT_frmdir_1)                                                                                            \
  X(MW_CV_FS_FAT_frmdir_2)                                                                                            \
  X(MW_CV_FS_FAT_frmdir_3)                                                                                            \
  X(MW_CV_FS_FAT_frmdir_4)                                                                                            \
  X(MW_CV_FS_FAT_fvol_0)                                                                                              \
  X(MW_CV_FS_FAT_fvol_1)                                                                                              \
  X(MW_CV_FS_FAT_fvol_2)                                                                                              \
  X(MW_CV_FS_FAT_ftime_set_0)                                                                                         \
  X(MW_CV_FS_FAT_ftime_set_1)                                                                                         \
  X(MW_CV_FS_FAT_ftime_set_2)                                                                                         \
  X(MW_CV_FS_FAT_ftime_set_3)                                                                                         \
  X(MW_CV_FS_FAT_ftime_get_0)                                                                                         \
  X(MW_CV_FS_FAT_ftime_get_1)                                                                                         \
  X(MW_CV_FS_FAT_ftime_get_2)                                                                                         \
  X(MW_CV_FS_FAT_ftime_get_3)

#define MW_CV_FS_DECLARE_TEST(name) extern void name (void);
MW_CV_FS_FAT_TESTS(MW_CV_FS_DECLARE_TEST)
#undef MW_CV_FS_DECLARE_TEST

#endif /* MW_CV_FS_FAT_H_ */
