/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - File System - Memory Card
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_FS_MC_H_
#define MW_CV_FS_MC_H_

#define MW_CV_FS_MC_TESTS(X)                                                                                           \
  X(MW_CV_FS_MC_fmedia_0)                                                                                             \
  X(MW_CV_FS_MC_fmedia_1)                                                                                             \
  X(MW_CV_FS_MC_fs_mc_read_cd_0)                                                                                      \
  X(MW_CV_FS_MC_fs_mc_read_cd_1)                                                                                      \
  X(MW_CV_FS_MC_fs_mc_read_wp_0)                                                                                      \
  X(MW_CV_FS_MC_fs_mc_read_wp_1)                                                                                      \
  X(MW_CV_FS_MC_fs_mc_spi_control_ss_0)                                                                               \
  X(MW_CV_FS_MC_fs_mc_spi_control_ss_1)                                                                               \
  X(MW_CV_FS_MC_fs_mc_spi_control_ss_2)                                                                               \
  X(MW_CV_FS_MC_fs_mc_lock_unlock_0)                                                                                  \
  X(MW_CV_FS_MC_fs_mc_lock_unlock_1)

#define MW_CV_FS_DECLARE_TEST(name) extern void name (void);
MW_CV_FS_MC_TESTS(MW_CV_FS_DECLARE_TEST)
#undef MW_CV_FS_DECLARE_TEST

#endif /* MW_CV_FS_MC_H_ */
