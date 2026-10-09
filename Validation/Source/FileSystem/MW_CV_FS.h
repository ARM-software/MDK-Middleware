/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - File System
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_FS_H_
#define MW_CV_FS_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rl_fs.h"

#define MW_CV_FS_DRIVE FS_DRIVE ":"
#define MW_CV_FS_INVALID_DRIVE "X0:"

extern uint32_t MW_CV_FS_GetVersion (void);
extern fsStatus MW_CV_FS_EnsureReady (void);
extern fsStatus MW_CV_FS_Reopen (void);
extern fsStatus MW_CV_FS_Format (const char *options);
extern fsType   MW_CV_FS_GetType (void);
extern bool     MW_CV_FS_IsFAT (void);
extern bool     MW_CV_FS_StrEqualIgnoreCase (const char *str1, const char *str2);
extern void     MW_CV_FS_MakePath (char *buf, size_t size, const char *name);
extern void     MW_CV_FS_FillPattern (uint8_t *buf, size_t size, uint32_t seed);
extern bool     MW_CV_FS_CreateFile (const char *path, const void *data, size_t size);
extern bool     MW_CV_FS_VerifyFile (const char *path, const void *data, size_t size);
extern void     MW_CV_FS_DeleteFile (const char *path);

#endif /* MW_CV_FS_H_ */
