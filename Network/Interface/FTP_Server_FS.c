/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2022 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    FTP_Server_FS.c
 * Purpose: FTP Server File System Interface
 * Rev.:    V7.2.0
 *----------------------------------------------------------------------------*/
//! [code_FTP_Server_FS]
#include <stdio.h>
#include <string.h>
#include "rl_net.h"

#ifdef RTE_FileSystem_Core
#include "rl_fs.h"

static fsFileInfo info;

#ifdef MW_FILESYSTEM_FDELETE_LEGACY
  #define FDELETE(fn)   fdelete(fn)
#else
  #define FDELETE(fn)   fdelete(fn,NULL)
#endif

#ifndef NET_FTP_SERVER_VBUF_SIZE
  #define NET_FTP_SERVER_VBUF_SIZE 0
#endif

// Open a file for reading or writing on FTP server.
__weak void *netFTPs_fopen (const char *fname, const char *mode) {
  FILE *f = fopen (fname, mode);
#if (NET_FTP_SERVER_VBUF_SIZE > 512)
  if (f != NULL) {
    setvbuf (f, NULL, _IOFBF, NET_FTP_SERVER_VBUF_SIZE);
  }
#endif
  return (f);
}

// Close a file previously open on FTP server.
__weak void netFTPs_fclose (void *file) {
  fclose (file);
}

// Read block of data from a file on FTP server.
__weak uint32_t netFTPs_fread (void *file, uint8_t *buf, uint32_t len) {
  return (fread (buf, 1, len, file));
}

// Write block of data to a file on FTP server.
__weak uint32_t netFTPs_fwrite (void *file, const uint8_t *buf, uint32_t len) {
  return (fwrite (buf, 1, len, file));
}

// Delete a file on FTP server.
__weak bool netFTPs_fdelete (const char *fname) {
  if (FDELETE (fname) == fsOK) {
    return (true);
  }
  return (false);
}

// Rename a file or directory on FTP server.
__weak bool netFTPs_frename (const char *fname, const char *newname) {
  if (frename (fname, newname) == fsOK) {
    return (true);
  }
  return (false);
}

// Make a new directory on FTP server.
__weak bool netFTPs_mkdir (const char *path) {
  if (fmkdir (path) == fsOK) {
    return (true);
  }
  return (false);
}

// Remove an empty directory on FTP server.
__weak bool netFTPs_rmdir (const char *path) {
  if (frmdir (path, NULL) == fsOK) {
    return (true);
  }
  return (false);
}

// Verify that the directory path exists on FTP server.
__weak bool netFTPs_chdir (const char *path) {
  if (fchdir (path) == fsOK) {
    return (true);
  }
  return (false);
}

// Search the file system directory for matching files.
__weak int32_t netFTPs_ffind (const char *mask, char *fname,
                              uint32_t *fsize, NET_FS_TIME *ftime, bool first) {
  if (first) {
    /* First call, initialize the info. */
    info.fileID = 0;
  }
  if (ffind (mask, &info) == fsOK) {
    if (fname != NULL) {
      strcpy (fname, info.name);
    }
    if (fsize != NULL) {
      *fsize = info.size;
    }
    if (ftime != NULL) {
      ftime->hr  = info.time.hr;
      ftime->min = info.time.min;
      ftime->sec = info.time.sec;
      ftime->day = info.time.day;
      ftime->mon = info.time.mon;
      ftime->year= info.time.year;
    }
    if (info.attrib & FS_FAT_ATTR_DIRECTORY) {
      return (NET_FS_ATTR_DIRECTORY);
    }
    return (NET_FS_ATTR_FILE);
  }
  return (0);
}
#endif
//! [code_FTP_Server_FS]
