/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - File System - Stress
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_FS_STRESS_H_
#define MW_CV_FS_STRESS_H_

#define MW_CV_FS_STRESS_COMMON_TESTS(X)                                                                                \
  X(MW_CV_FS_Stress_persistence_0)                                                                                    \
  X(MW_CV_FS_Stress_persistence_1)                                                                                    \
  X(MW_CV_FS_Stress_copy_0)                                                                                           \
  X(MW_CV_FS_Stress_copy_1)                                                                                           \
  X(MW_CV_FS_Stress_concurrency_0)                                                                                    \
  X(MW_CV_FS_Stress_concurrency_1)                                                                                    \
  X(MW_CV_FS_Stress_concurrency_2)

#define MW_CV_FS_STRESS_EFS_TESTS(X)                                                                                   \
  X(MW_CV_FS_Stress_EFS_ffind_0)

#define MW_CV_FS_STRESS_FAT_TESTS(X)                                                                                   \
  X(MW_CV_FS_Stress_FAT_ffind_0)                                                                                      \
  X(MW_CV_FS_Stress_FAT_name_cache_0)                                                                                 \
  X(MW_CV_FS_Stress_FAT_name_cache_1)                                                                                 \
  X(MW_CV_FS_Stress_FAT_name_cache_2)                                                                                 \
  X(MW_CV_FS_Stress_FAT_name_cache_3)                                                                                 \
  X(MW_CV_FS_Stress_FAT_name_cache_4)

#define MW_CV_FS_DECLARE_TEST(name) extern void name (void);
MW_CV_FS_STRESS_COMMON_TESTS(MW_CV_FS_DECLARE_TEST)
MW_CV_FS_STRESS_EFS_TESTS(MW_CV_FS_DECLARE_TEST)
MW_CV_FS_STRESS_FAT_TESTS(MW_CV_FS_DECLARE_TEST)
#undef MW_CV_FS_DECLARE_TEST

#endif /* MW_CV_FS_STRESS_H_ */
