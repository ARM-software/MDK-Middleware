/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - File System - C Library
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_FS_Clib.h
 * Purpose: MDK Middleware - Component Validation - File System - C Library -
 *          tests header
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_FS_CLIB_H_
#define MW_CV_FS_CLIB_H_

#define MW_CV_FS_CLIB_COMMON_TESTS(X)                                                                                  \
  X(MW_CV_FS_Clib_fopen_0)                                                                                            \
  X(MW_CV_FS_Clib_fopen_1)                                                                                            \
  X(MW_CV_FS_Clib_fopen_2)                                                                                            \
  X(MW_CV_FS_Clib_fopen_3)                                                                                            \
  X(MW_CV_FS_Clib_fopen_4)                                                                                            \
  X(MW_CV_FS_Clib_fopen_5)                                                                                            \
  X(MW_CV_FS_Clib_fopen_6)                                                                                            \
  X(MW_CV_FS_Clib_fopen_7)                                                                                            \
  X(MW_CV_FS_Clib_fopen_8)                                                                                            \
  X(MW_CV_FS_Clib_fopen_9)                                                                                            \
  X(MW_CV_FS_Clib_fclose_0)                                                                                           \
  X(MW_CV_FS_Clib_fclose_1)                                                                                           \
  X(MW_CV_FS_Clib_fclose_2)                                                                                           \
  X(MW_CV_FS_Clib_fwrite_0)                                                                                           \
  X(MW_CV_FS_Clib_fwrite_1)                                                                                           \
  X(MW_CV_FS_Clib_fwrite_2)                                                                                           \
  X(MW_CV_FS_Clib_fwrite_3)                                                                                           \
  X(MW_CV_FS_Clib_fwrite_4)                                                                                           \
  X(MW_CV_FS_Clib_fread_0)                                                                                            \
  X(MW_CV_FS_Clib_fread_1)                                                                                            \
  X(MW_CV_FS_Clib_fread_2)                                                                                            \
  X(MW_CV_FS_Clib_fread_3)                                                                                            \
  X(MW_CV_FS_Clib_fread_4)                                                                                            \
  X(MW_CV_FS_Clib_fgetc_0)                                                                                            \
  X(MW_CV_FS_Clib_fgetc_1)                                                                                            \
  X(MW_CV_FS_Clib_fgetc_2)                                                                                            \
  X(MW_CV_FS_Clib_fseek_0)                                                                                            \
  X(MW_CV_FS_Clib_fseek_1)                                                                                            \
  X(MW_CV_FS_Clib_fseek_2)                                                                                            \
  X(MW_CV_FS_Clib_fseek_3)                                                                                            \
  X(MW_CV_FS_Clib_fseek_4)                                                                                            \
  X(MW_CV_FS_Clib_fseek_5)

#define MW_CV_FS_CLIB_FAT_TESTS(X)                                                                                     \
  X(MW_CV_FS_FAT_Clib_fopen_0)                                                                                        \
  X(MW_CV_FS_FAT_Clib_fopen_1)                                                                                        \
  X(MW_CV_FS_FAT_Clib_fopen_2)                                                                                        \
  X(MW_CV_FS_FAT_Clib_fopen_3)                                                                                        \
  X(MW_CV_FS_FAT_Clib_fopen_4)                                                                                        \
  X(MW_CV_FS_FAT_Clib_fopen_5)                                                                                        \
  X(MW_CV_FS_FAT_Clib_fopen_6)                                                                                        \
  X(MW_CV_FS_FAT_Clib_fopen_7)                                                                                        \
  X(MW_CV_FS_FAT_Clib_fseek_0)                                                                                        \
  X(MW_CV_FS_FAT_Clib_fseek_1)

#define MW_CV_FS_CLIB_EFS_TESTS(X)                                                                                     \
  X(MW_CV_FS_EFS_Clib_fopen_0)                                                                                        \
  X(MW_CV_FS_EFS_Clib_fopen_1)                                                                                        \
  X(MW_CV_FS_EFS_Clib_fopen_2)                                                                                        \
  X(MW_CV_FS_EFS_Clib_fopen_3)                                                                                        \
  X(MW_CV_FS_EFS_Clib_fopen_4)                                                                                        \
  X(MW_CV_FS_EFS_Clib_fseek_0)

#define MW_CV_FS_DECLARE_TEST(name) extern void name (void);
MW_CV_FS_CLIB_COMMON_TESTS(MW_CV_FS_DECLARE_TEST)
MW_CV_FS_CLIB_FAT_TESTS(MW_CV_FS_DECLARE_TEST)
MW_CV_FS_CLIB_EFS_TESTS(MW_CV_FS_DECLARE_TEST)
#undef MW_CV_FS_DECLARE_TEST

#endif /* MW_CV_FS_CLIB_H_ */
