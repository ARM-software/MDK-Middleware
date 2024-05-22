/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_sys.h 
 * Purpose: System Module Definitions
 *----------------------------------------------------------------------------*/

#ifndef FS_SYS_H__
#define FS_SYS_H__

#include "fs_core.h"

/* Mutex lock macros. */
#define START_LOCK(x)   x return_value;                                 \
                        FS_MUTEX volume_mutex = NULL

#define VOLUME_LOCK(x)  volume_mutex = (x)->Mutex;                      \
                        if (volume_mutex)                               \
                          fs_mutex_acquire (volume_mutex)

#define RETURN(x)       return_value = (x);                             \
                        goto end_of_func

#define END_LOCK        end_of_func:                                    \
                        if (volume_mutex)                               \
                          fs_mutex_release (volume_mutex);              \
                        return (return_value)

/* File Handle Bit Masks */
#define SYS_HANDLE_FAT    (1 << 16)
#define SYS_HANDLE_EFS    (1 << 17)

/* Open mode flags */
#ifndef OPEN_R
#define OPEN_R    0
#endif
#ifndef OPEN_W
#define OPEN_W    4
#endif
#ifndef OPEN_A
#define OPEN_A    8
#endif
#ifndef OPEN_B
#define OPEN_B    1
#endif
#ifndef OPEN_PLUS
#define OPEN_PLUS 2
#endif

#endif /* FS_SYS_H__ */
