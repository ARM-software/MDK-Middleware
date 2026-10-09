/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - File System - EFS
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_FS_EFS_H_
#define MW_CV_FS_EFS_H_

#define MW_CV_FS_EFS_TESTS(X)                                                                                          \
  X(MW_CV_FS_EFS_fanalyse_0)                                                                                          \
  X(MW_CV_FS_EFS_fanalyse_1)                                                                                          \
  X(MW_CV_FS_EFS_fanalyse_2)                                                                                          \
  X(MW_CV_FS_EFS_fcheck_0)                                                                                            \
  X(MW_CV_FS_EFS_fcheck_1)                                                                                            \
  X(MW_CV_FS_EFS_fcheck_2)                                                                                            \
  X(MW_CV_FS_EFS_fdefrag_0)                                                                                           \
  X(MW_CV_FS_EFS_fdefrag_1)                                                                                           \
  X(MW_CV_FS_EFS_fdefrag_2)                                                                                           \
  X(MW_CV_FS_EFS_fdefrag_3)                                                                                           \
  X(MW_CV_FS_EFS_fdefrag_4)                                                                                           \
  X(MW_CV_FS_EFS_fdefrag_5)                                                                                           \
  X(MW_CV_FS_EFS_fattrib_0)                                                                                           \
  X(MW_CV_FS_EFS_fpwd_0)                                                                                              \
  X(MW_CV_FS_EFS_fchdir_0)                                                                                            \
  X(MW_CV_FS_EFS_fmkdir_0)                                                                                            \
  X(MW_CV_FS_EFS_frmdir_0)                                                                                            \
  X(MW_CV_FS_EFS_fvol_0)                                                                                              \
  X(MW_CV_FS_EFS_ftime_set_0)                                                                                         \
  X(MW_CV_FS_EFS_ftime_get_0)                                                                                         \
  X(MW_CV_FS_EFS_fs_ioc_get_id_0)

#define MW_CV_FS_DECLARE_TEST(name) extern void name (void);
MW_CV_FS_EFS_TESTS(MW_CV_FS_DECLARE_TEST)
#undef MW_CV_FS_DECLARE_TEST

#endif /* MW_CV_FS_EFS_H_ */
