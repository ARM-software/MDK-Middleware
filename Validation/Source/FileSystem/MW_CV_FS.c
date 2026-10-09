/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - File System
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_FS.c
 * Purpose: MDK Middleware - Component Validation - File System - common module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"

#if (MW_CV_FS != 0U)

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "MW_CV_FS.h"

static uint32_t fs_ready;

/*
\brief Convert the FileSystem library version to the validation-report format.
\details Converts the packed BCD FileSystem version to MMmmpppp decimal form.
\return FileSystem version encoded for the validation report.
*/
uint32_t MW_CV_FS_GetVersion (void) {
  uint32_t version;
  uint32_t major;
  uint32_t minor;
  uint32_t patch;

  /* Read and split the packed BCD version fields */
  version = fversion();
  major   = (((version >> 28) & 0x0FU) * 10U) + ((version >> 24) & 0x0FU);
  minor   = (((version >> 20) & 0x0FU) * 10U) + ((version >> 16) & 0x0FU);
  patch   = (((version >> 12) & 0x0FU) * 1000U) +
            (((version >>  8) & 0x0FU) *  100U) +
            (((version >>  4) & 0x0FU) *   10U) +
             ((version      ) & 0x0FU);

  /* Reassemble the fields in validation-report format */
  return ((major * 10000000U) + (minor * 10000U) + patch);
}


/*
\brief Prepare the configured drive for validation.
\details Initializes, mounts, and selects the drive. Media without a filesystem is formatted before selection.
\return fsOK on success, or the FileSystem status reported by the failed preparation operation.
*/
fsStatus MW_CV_FS_EnsureReady (void) {
  fsStatus status;

  /* Reuse the established volume state when a preceding test left it ready */
  if (fs_ready != 0U) {
    return fsOK;
  }

  /* Initialize and mount the configured drive, formatting uninitialized media */
  status = finit (MW_CV_FS_DRIVE);
  if (status != fsOK) {
    return status;
  }

  status = fmount (MW_CV_FS_DRIVE);
  if (status == fsNoFileSystem) {
    status = fformat (MW_CV_FS_DRIVE, NULL);
  }

  if (status != fsOK) {
    (void)funinit (MW_CV_FS_DRIVE);
    return status;
  }

  /* Select the mounted drive only after all preparation steps have succeeded */
  status = fchdrive (MW_CV_FS_DRIVE);
  if (status == fsOK) {
    fs_ready = 1U;
  }

  return status;
}


/*
\brief Reinitialize and reopen the configured drive.
\return fsOK on success, or the FileSystem status reported while preparing the drive.
*/
fsStatus MW_CV_FS_Reopen (void) {
  fsStatus status;

  /* Discard the current mounted and initialized state */
  (void)funmount (MW_CV_FS_DRIVE);
  (void)funinit (MW_CV_FS_DRIVE);
  fs_ready = 0U;

  /* Prepare the drive again */
  status = MW_CV_FS_EnsureReady();
  return status;
}


/*
\brief Format the configured drive and select it as the current drive.
\param[in] options Format options accepted by fformat, or NULL for default options.
\return fsOK on success, or the FileSystem status reported by preparation, formatting, or drive selection.
*/
fsStatus MW_CV_FS_Format (const char *options) {
  fsStatus status;

  /* Prepare the drive before formatting */
  status = MW_CV_FS_EnsureReady();
  if (status != fsOK) {
    return status;
  }

  /* Format and reselect the configured drive */
  status = fformat (MW_CV_FS_DRIVE, options);
  /* Record the resulting ready state */
  if (status == fsOK) {
    status = fchdrive (MW_CV_FS_DRIVE);
  }

  if (status == fsOK) {
    fs_ready = 1U;
  }

  return status;
}


/*
\brief Read the filesystem type of the configured drive.
\return Active filesystem type, or fsTypeNone when drive preparation or information retrieval fails.
*/
fsType MW_CV_FS_GetType (void) {
  fsDriveInfo info;

  /* Prepare the drive and read its filesystem type */
  if ((MW_CV_FS_EnsureReady() == fsOK) && (finfo (MW_CV_FS_DRIVE, &info) == fsOK)) {
    return info.fs_type;
  }
  /* Report the absence of usable drive information */
  return fsTypeNone;
}


/*
\brief Determine whether the configured drive uses a FAT filesystem.
\return true for FAT12, FAT16, or FAT32; otherwise false.
*/
bool MW_CV_FS_IsFAT (void) {
  fsType type = MW_CV_FS_GetType();

  /* Accept every supported FAT variant */
  return ((type == fsTypeFAT12) || (type == fsTypeFAT16) || (type == fsTypeFAT32));
}


/*
\brief Compare two strings without regard to ASCII letter case.
\param[in] str1 First zero-terminated string.
\param[in] str2 Second zero-terminated string.
\return true when both strings have equal length and differ only in letter case.
*/
bool MW_CV_FS_StrEqualIgnoreCase (const char *str1, const char *str2) {
  unsigned char ch1;
  unsigned char ch2;

  /* Reject missing strings */
  if ((str1 == NULL) || (str2 == NULL)) {
    return false;
  }

  /* Compare corresponding characters after ASCII case folding */
  do {
    ch1 = (unsigned char)*str1++;
    ch2 = (unsigned char)*str2++;

    if (tolower (ch1) != tolower (ch2)) {
      return false;
    }
  } while (ch1 != '\0');

  /* Both strings reached their terminator together */
  return true;
}


/*
\brief Build an absolute path on the configured drive.
\param[out] buf Destination for the drive-prefixed path.
\param[in] size Capacity of buf in bytes.
\param[in] name Relative file or directory name to append.
*/
void MW_CV_FS_MakePath (char *buf, size_t size, const char *name) {
  /* Build the path only when destination storage is available */
  if ((buf != NULL) && (size != 0U)) {
    (void)snprintf (buf, size, "%s%s", MW_CV_FS_DRIVE, name);
  }
}


/*
\brief Fill a buffer with deterministic validation data.
\param[out] buf Buffer to fill.
\param[in] size Number of bytes to generate.
\param[in] seed Value used to select the deterministic byte sequence.
*/
void MW_CV_FS_FillPattern (uint8_t *buf, size_t size, uint32_t seed) {
  size_t i;

  /* Generate every byte from its offset and the supplied seed */
  for (i = 0U; i < size; i++) {
    buf[i] = (uint8_t)(((i + seed) * 37U + 11U) & 0xFFU);
  }
}


/*
\brief Create a file containing the supplied payload.
\param[in] path File path to create or truncate.
\param[in] data Payload to write; it may be NULL when size is zero.
\param[in] size Payload size in bytes.
\return true when the complete payload is written and the stream closes successfully; otherwise false.
*/
bool MW_CV_FS_CreateFile (const char *path, const void *data, size_t size) {
  FILE *file;
  bool  ok;

  /* Create or truncate the target file */
  file = fopen (path, "wb");

  /* Stop when the stream cannot be acquired */
  if (file == NULL) {
    return false;
  }

  /* Write the complete payload, then include stream closure in the result */
  ok = ((size == 0U) || (fwrite (data, 1U, size, file) == size));

  /* Persist the payload and include closure in the result */
  if (fclose (file) != 0) {
    ok = false;
  }

  /* Report the combined write and close result */
  return ok;
}


/*
\brief Compare a complete file with an expected payload.
\details Reads bounded chunks, compares every requested byte, and rejects trailing file content.
\param[in] path File path to verify.
\param[in] data Expected payload; it may be NULL when size is zero.
\param[in] size Expected payload size in bytes.
\return true when content, length, and stream closure are valid; otherwise false.
*/
bool MW_CV_FS_VerifyFile (const char *path, const void *data, size_t size) {
  uint8_t        buf[64];
  const uint8_t *expected = data;
  FILE          *file;
  size_t         offset;
  size_t         count;
  bool           ok = true;

  /* Open the target file for binary verification */
  file = fopen (path, "rb");

  /* Stop when the stream cannot be acquired */
  if (file == NULL) {
    return false;
  }

  /* Compare bounded chunks so verification does not depend on payload size */
  offset = 0U;
  while (offset < size) {
    count = size - offset;
    if (count > sizeof (buf)) {
      count = sizeof (buf);
    }

    if ((fread (buf, 1U, count, file) != count) || (memcmp (buf, &expected[offset], count) != 0)) {
      ok = false;
      break;
    }
    offset += count;
  }

  /* Reject trailing content and propagate any stream-close failure */
  if ((ok) && (fgetc (file) != EOF)) {
    ok = false;
  }

  /* Include stream closure in the verification result */
  if (fclose (file) != 0) {
    ok = false;
  }

  /* Report the complete content and close result */
  return ok;
}


/*
\brief Remove a test file without reporting a missing-file cleanup error.
\param[in] path File path to remove.
*/
void MW_CV_FS_DeleteFile (const char *path) {
  /* Ignore missing-file cleanup errors */
  (void)fdelete (path, NULL);
}

#endif /* MW_CV_FS */
