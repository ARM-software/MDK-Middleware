/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_sys.c
 * Purpose: System Retarget interface
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "fs_sys.h"
#include "fs_common.h"
#include "fs_fat.h"
#include "fs_efs.h"
#include "fs_evr.h"

/* Functions */
static int32_t fs_open  (const char *path, int32_t mode);
static int32_t fs_close (int32_t handle);
static int32_t fs_write (int32_t handle, const void *buf, uint32_t cnt);
static int32_t fs_read  (int32_t handle, void *buf, uint32_t cnt);
static int32_t fs_flush (int32_t handle);
static int64_t fs_seek  (int32_t handle, int64_t offset, int32_t whence);
static int64_t fs_size  (int32_t handle);

/**
  Retarget of the _sys_open(...) function.

  The openmode parameter is a bitmap whose bits mostly correspond directly to
  the ISO mode specification. Target-dependent extensions are possible, but
  freopen() must also be extended.

  \param[in] name     File name
  \param[in] openmode Mode specification bitmap

  \return    File handle or -1 if an error occurs.
*/
int __sys_open (const char *name, int openmode) {
  int32_t mode;
  int32_t flag;
  int32_t rval;

  EvrFsCore_sys_open (name, openmode);

  /* Set the open mode flags */
  if ((openmode & (OPEN_R | OPEN_W | OPEN_A)) == OPEN_W) {
    mode = FS_FOPEN_WR;
    flag = FS_FOPEN_CREATE | FS_FOPEN_TRUNCATE;
  }
  else if ((openmode & (OPEN_R | OPEN_W | OPEN_A)) == OPEN_A) {
    mode = FS_FOPEN_WR;
    flag = FS_FOPEN_APPEND;
  }
  else /* if ((openmode & (OPEN_R | OPEN_W | OPEN_A)) == OPEN_R) */ {
    mode = FS_FOPEN_RD;
    flag = 0;
  }

  if ((openmode & OPEN_PLUS) == OPEN_PLUS) {
    mode &= ~(FS_FOPEN_RD | FS_FOPEN_WR);
    mode  = FS_FOPEN_RDWR;
  }

  rval = fs_open (name, mode|flag);

  if (rval < 0) {
    rval = -1;
  }

  return (rval);
}

/**
  Retarget of the _sys_close(...) function.

  This function closes a file previously opened with _sys_open().

  \param[in] fh File handle

  \return    The return value is 0 if successful. A nonzero value indicates
             an error.
*/
int __sys_close (int fh) {
  int rval;

  EvrFsCore_sys_close (fh);

  rval = fs_close(fh);

  return (rval);
}

/**
  Retarget of the _sys_write(...) function.

  This function writes the contents of a buffer to a file previously opened
  with _sys_open().

  \param[in] fh   File handle
  \param[in] buf  Data buffer
  \param[in] len  Data length

  \return    The return value is either:
             - a positive number representing the number of characters not
               written (so any nonzero return value denotes a failure of
               some sort)
             - a negative number indicating an error.
*/
int __sys_write (int fh, const uint8_t *buf, uint32_t len) {
  int32_t rlen;
  int rval;

  EvrFsCore_sys_write (fh, (const unsigned char *)buf, len);

  rlen = fs_write (fh, buf, len);

  if (rlen >= 0) {
    /* Return number of bytes NOT written */
    rval = (int32_t)len - rlen;
  }
  else {
    /* Return -1 on error */
    rval = -1;
  }

  return (rval);
}

/**
  Retarget of the _sys_read(...) function.

  This function reads the contents of a file into a buffer.
  Reading up to and including the last byte of data does not turn on the EOF
  indicator. The EOF indicator is only reached when an attempt is made to read
  beyond the last byte of data. The target-independent code is capable of
  handling:
    - the EOF indicator being returned in the same read as the remaining bytes
      of data that precede the EOF
    - the EOF indicator being returned on its own after the remaining bytes of
      data have been returned in a previous read.

  \param[in] fh   File handle
  \param[in] buf  Data buffer
  \param[in] len  Data length

  \return     The return value is one of the following:
              - The number of bytes not read (that is, len - result number of
                bytes were read).
              - An error indication.
              - An EOF indicator. The EOF indication involves the setting of
                0x80000000 in the normal result.
*/
int __sys_read (int fh, uint8_t *buf, uint32_t len) {
  int32_t rlen;
  int rval;

  EvrFsCore_sys_read (fh, buf, len);

  rlen = fs_read (fh, buf, len);

  if (rlen >= 0) {
    /* Return number of bytes NOT read */
    rval = (int32_t)len - rlen;

    if (rlen == 0) {
      /* Nothing was read, must be EOF */
      rval |= 0x80000000;
    }
  }
  else {
    /* Return -1 on error */
    rval = -1;
  }

  return (rval);
}

/**
  Low level system file flush function.

  This function is never called by any ARM library function. Instead, it is
  implemented as a hook using _ARMABI int $Sub$$fflush( FILE *stream ) and
  is called after fflush returns.
*/
int __sys_ensure (int fh) {
  
  return fs_flush (fh);
}

/**
  Retarget of the _sys_flen(...) function.

  This function returns the current length of a file. (It is used by
  _sys_seek() to convert an offset relative to the end of a file into
  an offset relative to the beginning of the file.)

  \param[in] fh File handle

  \return    This function returns the current length of the file fh,
             or a negative error indicator.
*/
long __sys_flen (int fh) {
  int64_t rval;

  EvrFsCore_sys_flen (fh);

  rval = fs_size (fh);

  if ((rval >> 32) != 0) {
    /* Returned file offset does not fit into long */
    rval = -1;
  }

  return ((long)rval);

}

/**
  Retarget of the _sys_seek(...) function.

  This function puts the file pointer at offset pos from the beginning of the
  file. The current read or write position is set to the new location pos
  relative to the start of the current file fh.

  \param[in] fh  File handle
  \param[in] pos File pointer offset

  \return    The result is:
             - non-negative if no error occurs
             - negative if an error occurs
*/
int __sys_seek (int fh, uint32_t pos) {
  int64_t roff;
  int rval;

  EvrFsCore_sys_seek (fh, (long)pos);

  roff = fs_seek (fh, pos, FS_FSEEK_SET);

  if (roff >= 0) {
    /* File position indicator changed */
    rval = 0;
  }
  else {
    /* Failed to set the file position indicator */
    rval = -1;
  }

  return (rval);
}


/**
  Open a file.

  This function opens a file specified by the pathname.

  The mode parameter is a bitmap that specifies the file open mode.
  The following bits are exclusive:
  - FS_FOPEN_RD:   open file for reading only
  - FS_FOPEN_WR:   open file for writing only
  - FS_FOPEN_RDWR: open file for reading and writing

  In addition, any combination of the values below can be set:
  - FS_FOPEN_APPEND: if set, the file offset is set to the end of file prior to each write
  - FS_FOPEN_CREATE: if set, the file is created if it does not exist
  - FS_FOPEN_TRUNCATE: if set, the size of an existing file opened for writing is truncated to zero

  The file position offset shall be set to the beginning of the file unless append mode is specified.

  \param[in]     path    string specifying the pathname of the file to be opened
  \param[in]     mode    integer bitmap specifying the file open mode
  \return        a non-negative integer representing the file handle on success,
                 or negative \ref fsStatus return code on failure
*/
int32_t fs_fopen (const char *path, int32_t mode) {

  EvrFsCore_fs_fopen (path, mode);

  return fs_open (path, mode);
}

/**
  Close a file.

  This function closes the file associated with the file descriptor 'handle'.

  \param[in]     handle  file handle of an opened file
  \return        zero if the file was successfully closed,
                 or negative \ref fsStatus return code on failure
*/
int32_t fs_fclose (int32_t handle) {

  EvrFsCore_fs_fclose (handle);
  
  return fs_close(handle);
}

/**
  Write to a file.

  This function shall write cnt bytes from the buffer pointed to by buf to the
  file associated with the open file descriptor, fd.

  \param[in]     handle  file handle of an opened file
  \param[in]     buf     pointer to the buffer containing data to write
  \param[in]     cnt     number of bytes to write
  \return        number of bytes actually written,
                 or negative \ref fsStatus return code on failure
*/
int32_t fs_fwrite (int32_t handle, const void *buf, uint32_t cnt) {

  EvrFsCore_fs_fwrite (handle, buf, cnt);

  return fs_write (handle, buf, cnt);
}

/**
  Read from a file.

  This function reads cnt bytes from the file associated with the file
  descriptor fd, into the buffer pointed to by buf.
  The actual number of bytes read can be less than cnt.

  \param[in]     handle  file handle of an opened file
  \param[out]    buf     pointer to the buffer to store read data
  \param[in]     cnt     number of bytes to read
  \return        number of bytes actually read, 0 at the EOF,
                 or negative \ref fsStatus return code on failure
*/
int32_t fs_fread (int32_t handle, void *buf, uint32_t cnt) {
  
  EvrFsCore_fs_fread (handle, buf, cnt);

  return fs_read (handle, buf, cnt);
}

/**
  Flush file buffers.

  \param[in]     handle  file handle of an opened file
  \return        zero on success,
                 or negative \ref fsStatus return code on failure
*/
int32_t fs_fflush (int32_t handle) {

  EvrFsCore_fs_fflush (handle);

  return fs_flush (handle);
}

/**
  Move the file position pointer.

  This functions moves the file position pointer as specified with parameters
  offset and whence.
  Parameter whence can have the following possible values:
  - FS_FSEEK_SET: set the file position pointer to offset bytes from the start of the file
  - FS_FSEEK_CUR: set the file position pointer to offset bytes from the current location
  - FS_FSEEK_END: set the file position pointer to offset bytes from the end of the file

  \param[in]     handle  file handle of an opened file
  \param[in]     offset  the number of bytes to move
  \param[in]     whence  file position location
  \return        current file position from the beginning of the file,
                 or negative \ref fsStatus return code on failure
*/
int64_t fs_fseek (int32_t handle, int64_t offset, int32_t whence) {
  int64_t rval;

  EvrFsCore_fs_fseek (handle, offset, whence);

  rval = fs_seek (handle, offset, whence);

  return (rval);
}

/**
  Get file size.

  This function retrieves the size of an opened file.

  \param[in]     handle  file handle of an opened file
  \return        file size in bytes,
                 or negative \ref fsStatus return code on failure
*/
int64_t fs_fsize (int32_t handle) {
  int64_t rval;

  EvrFsCore_fs_fsize (handle);

  rval = fs_size (handle);

  return (rval);
}


/**
  Open a file, see fs_fopen for details.

  \param[in]     path    string specifying the pathname of the file to be opened
  \param[in]     mode    integer bitmap specifying the file open mode
  \return        a non-negative integer representing the file handle on success,
                 or negative \ref fsStatus return code on failure
*/
static int32_t fs_open (const char *path, int32_t mode) {
  fsStatus stat;
  FS_DEV  *dev;
  int32_t  id;
  int32_t  fh;

  START_LOCK (int32_t);

  fh = -1;
  id = fs_drive_id (path, &path);

  if (id < 0) {
    /* Nonexistent drive or invalid input */
    stat = (fsStatus)(-id);
  }
  else {
    /* Get drive attributes */
    dev = &fs_DevPool[id];

    if (dev->attr & FS_FAT) {
      /* Lock FAT volume */
      VOLUME_LOCK ((fsFAT_Volume *)dev->dcb);

      /* Get FAT file handle */
      fh = fat_handle_get ((fsFAT_Volume *)dev->dcb);

      /* Open a file on FAT drive */
      stat = fat_open (fh, path, mode);

      if (stat == fsOK) {
        /* Set "FAT handle" flag */
        fh |= SYS_HANDLE_FAT;
      }
    }
    else {
      /* Lock EFS volume */
      VOLUME_LOCK ((fsEFS_Volume *)dev->dcb);

      /* Get EFS file handle */
      fh = efs_handle_get ((fsEFS_Volume *)dev->dcb);

      /* Open a file on EFS drive */
      stat = efs_open (fh, path, mode);

      if (stat == fsOK) {
        /* Set "EFS handle" flag */
        fh |= SYS_HANDLE_EFS;
      }
      else {
        /* File open failed, invalidate file handle */
        fs_efs_fh[fh].flags = 0;
      }
    }
  }
  
  if (stat == fsOK) {
    EvrFsCore_sys_handle_assign (path, fh);
  }
  else {
    /* Set errno to last known status code */
    errno = (int)stat;

    /* Set negative fsStatus return code */
    fh = -(int32_t)stat;
  }

  /* Return file handle or negative fsStatus on error */
  RETURN (fh);
  END_LOCK;
}


/**
  Close a file, see fs_fclose for details.

  \param[in]     handle  file handle of an opened file
  \return        zero if the file was successfully closed,
                 or negative \ref fsStatus return code on failure
*/
static int32_t fs_close (int32_t handle) {
  fsStatus stat;
  int32_t rval;

  START_LOCK (int32_t);

  if (handle & SYS_HANDLE_FAT) {
    handle &= ~SYS_HANDLE_FAT;
    /* Lock FAT volume */
    VOLUME_LOCK (fs_fat_fh[handle].vol);

    /* Close a file opened on FAT drive */
    stat = fat_close (handle);
  }
  else if (handle & SYS_HANDLE_EFS) {
    handle &= ~SYS_HANDLE_EFS;
    /* Lock EFS volume */
    VOLUME_LOCK (fs_efs_fh[handle].vol);

    /* Close a file opened on EFS drive */
    stat = efs_close (handle);
  }
  else {
    /* Invalid file handle */
    stat = fsInvalidParameter;
  }

  if (stat == fsOK) {
    rval = 0;
  }
  else {
    /* Set errno to last known status code */
    errno = (int)stat;

    rval = -(int32_t)stat;
  }

  RETURN (rval);
  END_LOCK;
}

/**
  Write to a file, see fs_fwrite for details.

  \param[in]     handle  file handle of an opened file
  \param[in]     buf     pointer to the buffer containing data to write
  \param[in]     cnt     number of bytes to write
  \return        number of bytes actually written,
                 or negative \ref fsStatus return code on failure
*/
static int32_t fs_write (int32_t handle, const void *buf, uint32_t cnt) {
  int32_t rval;

  START_LOCK (int32_t);

  if (handle & SYS_HANDLE_FAT) {
    handle &= ~SYS_HANDLE_FAT;
    /* Lock FAT volume */
    VOLUME_LOCK (fs_fat_fh[handle].vol);

    /* Write data to FAT drive. */
    rval = fat_write (handle, buf, cnt);
  }
  else if (handle & SYS_HANDLE_EFS) {
    handle &= ~SYS_HANDLE_EFS;
    /* Lock EFS volume */
    VOLUME_LOCK (fs_efs_fh[handle].vol);

    /* Write data to EFS drive. */
    rval = efs_write (handle, buf, cnt);
  }
  else {
    /* Invalid file handle */
    rval = -(int32_t)fsInvalidParameter;
  }

  if (rval < 0) {
    /* Set errno to last known status code */
    errno = -rval;
  }

  RETURN (rval);
  END_LOCK;
}

/**
  Read from a file, see fs_fread for details.

  \param[in]     handle  file handle of an opened file
  \param[out]    buf     pointer to the buffer to store read data
  \param[in]     cnt     number of bytes to read
  \return        number of bytes actually read, 0 at the EOF,
                 or negative \ref fsStatus return code on failure
*/
static int32_t fs_read (int32_t handle, void *buf, uint32_t cnt) {
  int32_t rval;

  START_LOCK (int32_t);

  if (handle & SYS_HANDLE_FAT) {
    handle &= ~SYS_HANDLE_FAT;
    /* Lock FAT volume */
    VOLUME_LOCK (fs_fat_fh[handle].vol);

    /* Read data from FAT drive */
    rval = fat_read (handle, buf, cnt);
  }
  else if (handle & SYS_HANDLE_EFS) {
    handle &= ~SYS_HANDLE_EFS;
    /* Lock EFS volume */
    VOLUME_LOCK (fs_efs_fh[handle].vol);

    /* Read data from EFS drive */
    rval  = efs_read (handle, buf, cnt);
  }
  else {
    /* Invalid file handle */
    rval = 0;
  }

  if (rval < 0) {
    /* Set errno to last known status code */
    errno = -rval;
  }

  RETURN(rval);
  END_LOCK;
}

/**
  Flush file buffers, see fs_fflush for details.

  \param[in]     handle  file handle of an opened file
  \return        zero on success,
                 or negative \ref fsStatus return code on failure
*/
static int32_t fs_flush (int32_t handle) {
  fsStatus stat;
  int32_t rval;

  START_LOCK (int32_t);

  if (handle & SYS_HANDLE_FAT) {
    handle &= ~SYS_HANDLE_FAT;
    /* Lock FAT volume */
    VOLUME_LOCK (fs_fat_fh[handle].vol);

    /* Flush buffers on FAT drive. */
    stat = fat_flush (handle);
  }
  else if (handle & SYS_HANDLE_EFS) {
    handle &= ~SYS_HANDLE_EFS;
    /* Lock EFS volume */
    VOLUME_LOCK (fs_efs_fh[handle].vol);

    /* Write alloc info for EFS drive. */
    stat = efs_flush (handle);
  }
  else {
    /* Invalid file handles are IGNORED */
    stat = fsOK;
  }

  if (stat == fsOK) {
    rval = 0;
  }
  else {
    /* Set errno to last known status code */
    errno = (int)stat;

    rval = -(int32_t)stat;
  }

  RETURN(rval);
  END_LOCK;
}

/**
  Move the file position pointer, see fs_fseek for details.

  \param[in]     handle  file handle of an opened file
  \param[in]     offset  the number of bytes to move
  \param[in]     whence  file position location
  \return        current file position from the beginning of the file,
                 or negative \ref fsStatus return code on failure
*/
static int64_t fs_seek (int32_t handle, int64_t offset, int32_t whence) {
  int64_t rval;

  START_LOCK (int64_t);

  if (handle & SYS_HANDLE_FAT) {
    handle &= ~SYS_HANDLE_FAT;
    /* Lock FAT volume */
    VOLUME_LOCK (fs_fat_fh[handle].vol);

    /* Set fpos on FAT drive */
    rval = fat_seek (handle, offset, whence);
  }
  else if (handle & SYS_HANDLE_EFS) {
    handle &= ~SYS_HANDLE_EFS;
    /* Lock EFS volume */
    VOLUME_LOCK (fs_efs_fh[handle].vol);

    /* Set fpos on EFS drive */
    rval = efs_seek (handle, (int32_t)offset, whence);
  }
  else {
    /* Invalid file handle */
    rval = -(int64_t)fsInvalidParameter;
  }

  if (rval < 0) {
    /* Set errno to last known status code */
    errno = -(int)rval;
  }

  RETURN (rval);
  END_LOCK;
}

/**
  Get file size.

  This function retrieves the size of an opened file.

  \param[in]     handle  file handle of an opened file
  \return        file size in bytes,
                 or negative \ref fsStatus return code on failure
*/
static int64_t fs_size (int32_t handle) {
  int64_t rval;

  START_LOCK (int64_t);

  if (handle & SYS_HANDLE_FAT) {
    handle &= ~SYS_HANDLE_FAT;
    /* Lock FAT volume */
    VOLUME_LOCK (fs_fat_fh[handle].vol);

    /* FAT drive */
    rval = fat_flen(handle);
  }
  else if (handle & SYS_HANDLE_EFS) {
    handle &= ~SYS_HANDLE_EFS;
    /* Lock EFS volume */
    VOLUME_LOCK (fs_efs_fh[handle].vol);

    /* EFS drive */
    rval = efs_flen(handle);
  }
  else {
    /* Invalid file handle */
    rval = -(int32_t)fsInvalidParameter;
  }

  if (rval < 0) {
    /* Set errno to last known status code */
    errno = -(int32_t)rval;
  }

  RETURN (rval);
  END_LOCK;
}
