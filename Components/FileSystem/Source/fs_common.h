/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_common.h
 * Purpose: Common file system function definitions
 *----------------------------------------------------------------------------*/

#ifndef FS_COMMON_H__
#define FS_COMMON_H__

#include "fs_core.h"

/* Functions */
extern void         fs_init_cfg (void);
extern fsStatus     fs_set_cdrive (int32_t id);
extern int32_t      fs_drive_id (const char *path, const char **fn);
extern int32_t      fs_strpos (const char *sp, const char ch);
extern uint32_t     fs_strncasecmp (const char *s1, const char *s2, uint32_t n);
extern uint32_t     fs_strmatch (const char *s1, const char *s2, uint32_t len1, uint32_t len2);

extern uint16_t     get_u16 (const uint8_t *p16);
extern void         set_u16 (uint8_t *p16, uint16_t val);
extern uint32_t     get_u32 (const uint8_t *p32);
extern void         set_u32 (uint8_t *p32, uint32_t val);

#endif /* FS_COMMON_H__ */
