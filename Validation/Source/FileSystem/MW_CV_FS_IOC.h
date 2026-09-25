/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - File System - I/O Control
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_FS_IOC_H_
#define MW_CV_FS_IOC_H_

#define MW_CV_FS_IOC_SAFE_TESTS(X)                                                                                     \
  X(MW_CV_FS_IOC_fs_ioc_get_id_0)                                                                                     \
  X(MW_CV_FS_IOC_fs_ioc_get_id_1)                                                                                     \
  X(MW_CV_FS_IOC_fs_ioc_lock_0)                                                                                       \
  X(MW_CV_FS_IOC_fs_ioc_lock_1)                                                                                       \
  X(MW_CV_FS_IOC_fs_ioc_lock_2)                                                                                       \
  X(MW_CV_FS_IOC_fs_ioc_unlock_0)                                                                                     \
  X(MW_CV_FS_IOC_fs_ioc_unlock_1)                                                                                     \
  X(MW_CV_FS_IOC_fs_ioc_unlock_2)                                                                                     \
  X(MW_CV_FS_IOC_fs_ioc_get_cache_0)                                                                                  \
  X(MW_CV_FS_IOC_fs_ioc_get_cache_1)                                                                                  \
  X(MW_CV_FS_IOC_fs_ioc_get_cache_2)                                                                                  \
  X(MW_CV_FS_IOC_fs_ioc_read_sector_0)                                                                                \
  X(MW_CV_FS_IOC_fs_ioc_read_sector_1)                                                                                \
  X(MW_CV_FS_IOC_fs_ioc_read_sector_2)                                                                                \
  X(MW_CV_FS_IOC_fs_ioc_read_info_0)                                                                                  \
  X(MW_CV_FS_IOC_fs_ioc_read_info_1)                                                                                  \
  X(MW_CV_FS_IOC_fs_ioc_read_info_2)                                                                                  \
  X(MW_CV_FS_IOC_fs_ioc_device_ctrl_0)                                                                                \
  X(MW_CV_FS_IOC_fs_ioc_device_ctrl_1)

#define MW_CV_FS_IOC_DESTRUCTIVE_TESTS(X)                                                                              \
  X(MW_CV_FS_IOC_fs_ioc_write_sector_0)                                                                               \
  X(MW_CV_FS_IOC_fs_ioc_write_sector_1)                                                                               \
  X(MW_CV_FS_IOC_fs_ioc_write_sector_2)

#define MW_CV_FS_IOC_TESTS(X)                                                                                          \
  MW_CV_FS_IOC_SAFE_TESTS(X)                                                                                          \
  MW_CV_FS_IOC_DESTRUCTIVE_TESTS(X)

#define MW_CV_FS_DECLARE_TEST(name) extern void name (void);
MW_CV_FS_IOC_TESTS(MW_CV_FS_DECLARE_TEST)
#undef MW_CV_FS_DECLARE_TEST

#endif /* MW_CV_FS_IOC_H_ */
