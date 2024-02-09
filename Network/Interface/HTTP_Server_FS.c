/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server_FS.c
 * Purpose: HTTP Server File System Interface
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "rl_net_lib.h"
#include "rl_fs.h"

#ifdef __clang__
  #pragma clang diagnostic ignored "-Wcast-align"
#endif

// Open a file for reading on HTTP server.
__WEAK void *netHTTPs_fopen (const char *fname) {
  return (fopen (fname, "r"));
}

// Close a file previously open on HTTP server.
__WEAK void netHTTPs_fclose (void *file) {
  fclose (file);
}

// Read block of data from a file on HTTP server.
__WEAK uint32_t netHTTPs_fread (void *file, uint8_t *buf, uint32_t len) {
  return (fread (buf, 1, len, file));
}

// Read a string from a file on HTTP server.
__WEAK char *netHTTPs_fgets (void *file, char *buf, uint32_t size) {
  return (fgets (buf, (int32_t)size, file));
}

// Retrieve size and last modification time of a file.
__WEAK void netHTTPs_fstat (const char *fname, uint32_t *fsize, uint32_t *ftime) {
  fsFileInfo *info;

  info = (fsFileInfo *)net_mem_alloc (sizeof (fsFileInfo));
  info->fileID = 0;
  if (ffind (fname, info) == fsOK) {
    // File found, return size
    *fsize = info->size;
    // Convert creation time to UTC format.
    *ftime = net_http_time (info->time.hr, info->time.min, info->time.sec,
                            info->time.day, info->time.mon, info->time.year);
  }
  net_mem_free ((NET_FRAME *)info);
}
