/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - File System - I/O Control
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_FS_IOC.c
 * Purpose: MDK Middleware - Component Validation - File System - I/O Control -
 *          tests module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"

#if ((MW_CV_FS != 0U) && (MW_CV_FS_IOC != 0U) &&                                                                       \
     ((MW_CV_FS_DRIVE_M != 0U) || (MW_CV_FS_DRIVE_N != 0U) || (MW_CV_FS_DRIVE_R != 0U)))

#include <stdint.h>
#include <string.h>

#include "MW_CV_TestReport.h"
#include "MW_CV_FS.h"
#include "MW_CV_FS_IOC.h"

#define CHECK(expr) ASSERT_TRUE((expr), "%s", #expr)
#define IOC_BUFFER_SIZE 4096U

/* Keep IOC buffers sector-aligned in the board's SD DMA memory region. */
static uint8_t ioc_a[IOC_BUFFER_SIZE] __attribute__((aligned(512)));
static uint8_t ioc_b[IOC_BUFFER_SIZE] __attribute__((aligned(512)));

/*
\brief Obtain and validate the media identifier of the configured drive.
\return Nonnegative media identifier on success, or the negative status returned by fs_ioc_get_id.
*/
static int32_t get_id (void) {
  int32_t id;

  /* Prepare the configured drive */
  CHECK(MW_CV_FS_EnsureReady() == fsOK);

  /* Obtain and validate its media identifier */
  id = fs_ioc_get_id (MW_CV_FS_DRIVE);
  CHECK(id >= 0);

  /* Return the identifier for dependent IOC operations */
  return id;
}


/*
\brief Read and validate essential media geometry.
\param[in] id Media identifier to query.
\param[out] info Destination for media geometry and capabilities.
\return true when media information is read successfully; otherwise false.
*/
static bool media_info (int32_t id, fsMediaInfo *info) {
  fsStatus status;

  /* Read the media geometry */
  status = fs_ioc_read_info (id, info);

  /* Validate the result and essential geometry fields */
  CHECK(status == fsOK);
  CHECK((info->block_cnt != 0U) && (info->read_blen != 0U));

  /* Report whether dependent media operations may continue */
  return (status == fsOK);
}


/**
\defgroup mw_cv_fs_ioc_test_funcs I/O Control
\brief File System I/O Control validation test functions
\details
These tests validate FAT media I/O-control operations and their safety checks.
@{
*/

/**
\brief Test the fs_ioc_get_id function.
\details
\par Method
Obtain the configured drive identifier.
\par Expected
- The configured drive has a valid nonnegative media identifier.
*/
void MW_CV_FS_IOC_fs_ioc_get_id_0 (void) {
  /* Obtain and validate the configured media identifier */
  CHECK(get_id() >= 0);
}


/**
\brief Test the fs_ioc_get_id function.
\details
\par Method
Reject an invalid drive specification.
\par Expected
- The media-identifier query reports an invalid parameter for a null drive.
- The media-identifier query reports an invalid drive.
*/
void MW_CV_FS_IOC_fs_ioc_get_id_1 (void) {
  /* Verify rejection of a null drive specification */
  CHECK(fs_ioc_get_id (NULL) == -(int32_t)fsInvalidParameter);

  /* Verify rejection of an unknown drive specification */
  CHECK(fs_ioc_get_id (MW_CV_FS_INVALID_DRIVE) == -(int32_t)fsInvalidDrive);
}


/**
\brief Test the fs_ioc_lock function.
\details
\par Method
Lock one valid drive.
\par Expected
- The media lock succeeds.
*/
void MW_CV_FS_IOC_fs_ioc_lock_0 (void) {
  int32_t id = get_id();

  /* Stop if the configured media has no valid identifier */
  if (id < 0) {
    return;
  }

  /* Lock and release the media */
  CHECK(fs_ioc_lock (id) == fsOK);
  CHECK(fs_ioc_unlock (id) == fsOK);
}


/**
\brief Test the fs_ioc_lock function.
\details
\par Method
Exercise repeated locking.
\par Expected
- The media lock succeeds.
- Repeating the lock either succeeds or reports access denied, as documented.
*/
void MW_CV_FS_IOC_fs_ioc_lock_1 (void) {
  int32_t  id = get_id();
  fsStatus status;

  /* Stop if the configured media has no valid identifier */
  if (id < 0) {
    return;
  }

  /* Acquire the initial media lock */
  CHECK(fs_ioc_lock (id) == fsOK);

  /* Repeat the lock request and validate the supported result */
  status = fs_ioc_lock (id);
  CHECK((status == fsOK) || (status == fsAccessDenied));

  /* Release the media lock */
  CHECK(fs_ioc_unlock (id) == fsOK);
}


/**
\brief Test the fs_ioc_lock function.
\details
\par Method
Reject an invalid identifier.
\par Expected
- The media lock reports an invalid drive.
*/
void MW_CV_FS_IOC_fs_ioc_lock_2 (void) {
  /* Verify rejection of an invalid media identifier */
  CHECK(fs_ioc_lock (-1) == fsInvalidDrive);
}


/**
\brief Test the fs_ioc_unlock function.
\details
\par Method
Unlock one locked drive.
\par Expected
- The drive locks successfully and <tt>fs_ioc_unlock</tt> returns <tt>fsOK</tt>.
*/
void MW_CV_FS_IOC_fs_ioc_unlock_0 (void) {
  int32_t id;

  /* Obtain the configured media identifier */
  id = get_id();

  /* Stop if the configured media has no valid identifier */
  if (id < 0) {
    return;
  }

  /* Lock and then unlock the media */
  CHECK(fs_ioc_lock (id) == fsOK);
  CHECK(fs_ioc_unlock (id) == fsOK);
}


/**
\brief Test the fs_ioc_unlock function.
\details
\par Method
Exercise unlocking an unlocked drive.
\par Expected
- Unlocking an already unlocked drive either succeeds or reports access denied, as documented.
*/
void MW_CV_FS_IOC_fs_ioc_unlock_1 (void) {
  int32_t  id;
  fsStatus status;

  /* Obtain the configured media identifier */
  id = get_id();

  /* Stop if the configured media has no valid identifier */
  if (id < 0) {
    return;
  }

  /* Establish and release the initial lock */
  CHECK(fs_ioc_lock (id) == fsOK);
  CHECK(fs_ioc_unlock (id) == fsOK);

  /* Repeat the unlock request and validate the supported result */
  status = fs_ioc_unlock (id);
  CHECK((status == fsOK) || (status == fsAccessDenied));
}


/**
\brief Test the fs_ioc_unlock function.
\details
\par Method
Reject an invalid identifier.
\par Expected
- The media unlock reports an invalid drive.
*/
void MW_CV_FS_IOC_fs_ioc_unlock_2 (void) {
  /* Verify rejection of an invalid media identifier */
  CHECK(fs_ioc_unlock (-1) == fsInvalidDrive);
}


/**
\brief Test the fs_ioc_get_cache function.
\details
\par Method
Retrieve cache address and size while locked.
\par Expected
- The cache query succeeds.
- A valid cache buffer is returned.
- The returned cache has nonzero capacity.
*/
void MW_CV_FS_IOC_fs_ioc_get_cache_0 (void) {
  int32_t     id;
  fsIOC_Cache cache;

  /* Obtain the configured media identifier */
  id = get_id();

  /* Stop if the configured media has no valid identifier */
  if (id < 0) {
    return;
  }

  /* Lock the media and retrieve its IOC cache */
  CHECK(fs_ioc_lock (id) == fsOK);
  CHECK(fs_ioc_get_cache (id, &cache) == fsOK);

  /* Verify the returned cache storage */
  CHECK(cache.buffer != NULL);
  CHECK(cache.size != 0U);

  /* Release the media lock */
  CHECK(fs_ioc_unlock (id) == fsOK);
}


/**
\brief Test the fs_ioc_get_cache function.
\details
\par Method
Validate alignment and usable size.
\par Expected
- The cache query succeeds.
- The cache buffer is aligned on a four-byte boundary.
- The returned cache can hold at least one 512-byte sector.
*/
void MW_CV_FS_IOC_fs_ioc_get_cache_1 (void) {
  int32_t     id;
  fsIOC_Cache cache;

  /* Obtain the configured media identifier */
  id = get_id();

  /* Stop if the configured media has no valid identifier */
  if (id < 0) {
    return;
  }

  /* Lock the media and retrieve its IOC cache */
  CHECK(fs_ioc_lock (id) == fsOK);
  CHECK(fs_ioc_get_cache (id, &cache) == fsOK);

  /* Verify cache alignment and minimum usable size */
  CHECK(((uintptr_t)cache.buffer & 3U) == 0U);
  CHECK(cache.size >= 512U);

  /* Release the media lock */
  CHECK(fs_ioc_unlock (id) == fsOK);
}


/**
\brief Test the fs_ioc_get_cache function.
\details
\par Method
Reject an invalid identifier.
\par Expected
- The cache query reports an invalid drive.
- The cache query reports an invalid parameter.
*/
void MW_CV_FS_IOC_fs_ioc_get_cache_2 (void) {
  fsIOC_Cache cache;

  /* Verify rejection of an invalid media identifier */
  CHECK(fs_ioc_get_cache (-1, &cache) == fsInvalidDrive);

  /* Verify rejection of a null cache descriptor */
  CHECK(fs_ioc_get_cache (0, NULL) == fsInvalidParameter);
}


/**
\brief Test the fs_ioc_read_sector function.
\details
\par Method
Read one valid sector.
\par Expected
- One read block fits in the validation buffer.
- The sector read succeeds.
*/
void MW_CV_FS_IOC_fs_ioc_read_sector_0 (void) {
  int32_t     id;
  fsMediaInfo info;

  /* Obtain the configured media identifier */
  id = get_id();

  /* Stop if the configured media has no valid identifier */
  if (id < 0) {
    return;
  }

  /* Lock the media before direct sector access */
  CHECK(fs_ioc_lock (id) == fsOK);

  /* Validate the reported sector size before issuing the media read */
  if (media_info (id, &info)) {
    CHECK(info.read_blen <= sizeof (ioc_a));

    if (info.read_blen <= sizeof (ioc_a)) {
      CHECK(fs_ioc_read_sector (id, 0U, ioc_a, 1U) == fsOK);
    }
  }

  /* Release the media lock */
  CHECK(fs_ioc_unlock (id) == fsOK);
}


/**
\brief Test the fs_ioc_read_sector function.
\details
\par Method
Read consecutive sectors.
\par Expected
- Two read blocks fit in the validation buffer.
- The sector read succeeds.
*/
void MW_CV_FS_IOC_fs_ioc_read_sector_1 (void) {
  int32_t     id;
  fsMediaInfo info;

  /* Obtain the configured media identifier */
  id = get_id();

  /* Stop if the configured media has no valid identifier */
  if (id < 0) {
    return;
  }

  /* Lock the media before direct sector access */
  CHECK(fs_ioc_lock (id) == fsOK);

  /* Validate the two-sector transfer size and read consecutive sectors */
  if (media_info (id, &info)) {
    CHECK((2U * info.read_blen) <= sizeof (ioc_a));

    if ((2U * info.read_blen) <= sizeof (ioc_a)) {
      CHECK(fs_ioc_read_sector (id, 0U, ioc_a, 2U) == fsOK);
    }
  }

  /* Release the media lock */
  CHECK(fs_ioc_unlock (id) == fsOK);
}


/**
\brief Test the fs_ioc_read_sector function.
\details
\par Method
Reject an out-of-range request.
\par Expected
- The sector read is rejected.
*/
void MW_CV_FS_IOC_fs_ioc_read_sector_2 (void) {
  int32_t     id;
  fsMediaInfo info;

  /* Obtain the configured media identifier */
  id = get_id();

  /* Stop if the configured media has no valid identifier */
  if (id < 0) {
    return;
  }

  /* Lock the media before direct sector access */
  CHECK(fs_ioc_lock (id) == fsOK);

  /* Attempt to read the first sector beyond the media */
  if (media_info (id, &info)) {
    CHECK(fs_ioc_read_sector (id, info.block_cnt, ioc_a, 1U) != fsOK);
  }

  /* Release the media lock */
  CHECK(fs_ioc_unlock (id) == fsOK);
}


/*
\brief Write and verify a sector pattern on disposable validation media.
\details Bounds the request against media geometry and the validation buffers before modifying sectors from zero.
\param[in] id Locked media identifier.
\param[in] count Number of consecutive sectors to validate.
*/
static void write_verify (int32_t id, uint32_t count) {
  fsMediaInfo info;
  fsStatus    status;
  uint32_t    bytes, sector, i;

  /* IOC write tests run after all tests that depend on a valid filesystem */
  if (media_info (id, &info)) {
    /* Calculate and validate the destructive transfer range */
    sector = 0U;
    bytes  = count * info.write_blen;
    CHECK((info.write_blen != 0U) && (info.read_blen == info.write_blen));
    CHECK(bytes <= sizeof (ioc_a));
    CHECK((sector < info.block_cnt) && (count <= (info.block_cnt - sector)));

    if ((info.write_blen != 0U) && (info.read_blen == info.write_blen) &&
        (bytes <= sizeof (ioc_a)) && (sector < info.block_cnt) && (count <= (info.block_cnt - sector))) {
      /* Write a deterministic pattern to the requested sectors */
      MW_CV_FS_FillPattern (ioc_a, bytes, count);
      status = fs_ioc_write_sector (id, sector, ioc_a, count);
      CHECK(status == fsOK);

      if (status == fsOK) {
        /* Read the modified sectors into an independent buffer */
        (void)memset (ioc_b, 0, bytes);
        status = fs_ioc_read_sector (id, sector, ioc_b, count);
        CHECK(status == fsOK);

        if (status == fsOK) {
          /* Compare every byte with the written pattern */
          for (i = 0U; i < bytes; i++) {
            CHECK(ioc_b[i] == ioc_a[i]);
          }
        }
      }
    }
  }
}


/**
\brief Test the fs_ioc_write_sector function.
\details
\par Method
Write and verify sector zero on disposable validation media.
\par Expected
- Sector zero is within the reported media bounds and fits the validation buffers.
- The test pattern is written and read back exactly.
*/
void MW_CV_FS_IOC_fs_ioc_write_sector_0 (void) {
  int32_t id = get_id();

  /* Stop if the configured media has no valid identifier */
  if (id < 0) {
    return;
  }

  /* Lock, overwrite, verify, and release one sector */
  CHECK(fs_ioc_lock (id) == fsOK);
  write_verify (id, 1U);
  CHECK(fs_ioc_unlock (id) == fsOK);
}


/**
\brief Test the fs_ioc_write_sector function.
\details
\par Method
Write and verify two consecutive sectors from sector zero on disposable validation media.
\par Expected
- Both sectors are within the reported media bounds and fit the validation buffers.
- The test pattern is written and read back exactly.
*/
void MW_CV_FS_IOC_fs_ioc_write_sector_1 (void) {
  int32_t id = get_id();

  /* Stop if the configured media has no valid identifier */
  if (id < 0) {
    return;
  }

  /* Lock, overwrite, verify, and release two sectors */
  CHECK(fs_ioc_lock (id) == fsOK);
  write_verify (id, 2U);
  CHECK(fs_ioc_unlock (id) == fsOK);
}


/**
\brief Test the fs_ioc_write_sector function.
\details
\par Method
Reject an out-of-range request.
\par Expected
- The sector write is rejected.
*/
void MW_CV_FS_IOC_fs_ioc_write_sector_2 (void) {
  int32_t     id;
  fsMediaInfo info;

  /* Obtain the configured media identifier */
  id = get_id();

  /* Stop if the configured media has no valid identifier */
  if (id < 0) {
    return;
  }

  /* Lock the media before direct sector access */
  CHECK(fs_ioc_lock (id) == fsOK);

  /* Attempt to write the first sector beyond the media */
  if (media_info (id, &info)) {
    CHECK(fs_ioc_write_sector (id, info.block_cnt, ioc_a, 1U) != fsOK);
  }

  /* Release the media lock */
  CHECK(fs_ioc_unlock (id) == fsOK);
}


/**
\brief Test the fs_ioc_read_info function.
\details
\par Method
Read media geometry.
\par Expected
- <tt>fs_ioc_read_info</tt> returns <tt>fsOK</tt> with nonzero block count and read-block length.
*/
void MW_CV_FS_IOC_fs_ioc_read_info_0 (void) {
  int32_t     id;
  fsMediaInfo info;

  /* Obtain the configured media identifier */
  id = get_id();

  /* Stop if the configured media has no valid identifier */
  if (id < 0) {
    return;
  }

  /* Lock the media and validate its geometry */
  CHECK(fs_ioc_lock (id) == fsOK);
  (void)media_info (id, &info);

  /* Release the media lock */
  CHECK(fs_ioc_unlock (id) == fsOK);
}


/**
\brief Test the fs_ioc_read_info function.
\details
\par Method
Compare geometry with <tt>finfo</tt>.
\par Expected
- The raw media reports nonzero capacity.
- The drive-information query succeeds.
- The mounted drive reports nonzero capacity.
*/
void MW_CV_FS_IOC_fs_ioc_read_info_1 (void) {
  int32_t     id;
  fsMediaInfo media;
  fsDriveInfo drive;

  /* Obtain the configured media identifier */
  id = get_id();

  /* Stop if the configured media has no valid identifier */
  if (id < 0) {
    return;
  }

  /* Read and validate the raw media capacity */
  CHECK(fs_ioc_lock (id) == fsOK);
  if (media_info (id, &media)) {
    CHECK((uint64_t)media.block_cnt * media.read_blen != 0U);
  }
  CHECK(fs_ioc_unlock (id) == fsOK);

  /* Compare against the mounted drive information */
  CHECK(finfo (MW_CV_FS_DRIVE, &drive) == fsOK);
  CHECK(drive.capacity != 0U);
}


/**
\brief Test the fs_ioc_read_info function.
\details
\par Method
Reject an invalid identifier.
\par Expected
- The media-information query reports an invalid drive.
*/
void MW_CV_FS_IOC_fs_ioc_read_info_2 (void) {
  fsMediaInfo info;

  /* Verify rejection of an invalid media identifier */
  CHECK(fs_ioc_read_info (-1, &info) == fsInvalidDrive);
}


/**
\brief Test the fs_ioc_device_ctrl function.
\details
\par Method
Issue the least invasive supported status command.
\par Expected
- The status command either succeeds or reports that it is unsupported.
*/
void MW_CV_FS_IOC_fs_ioc_device_ctrl_0 (void) {
  int32_t  id;
  uint32_t status = 0U;
  fsStatus result;

  /* Obtain the configured media identifier */
  id = get_id();

  /* Stop if the configured media has no valid identifier */
  if (id < 0) {
    return;
  }

  /* Lock the media and query its presence state */
  CHECK(fs_ioc_lock (id) == fsOK);
  result = fs_ioc_device_ctrl (id, fsDevCtrlCodeCheckMedia, &status);
  CHECK((result == fsOK) || (result == fsUnsupported));

  /* Release the media lock */
  CHECK(fs_ioc_unlock (id) == fsOK);
}


/**
\brief Test the fs_ioc_device_ctrl function.
\details
\par Method
Exercise each applicable read-only control code.
\par Expected
- Every applicable control code succeeds, reports that it is unsupported, or reports a media error.
*/
void MW_CV_FS_IOC_fs_ioc_device_ctrl_1 (void) {
  int32_t  id;
  uint32_t serial = 0U;
  fsStatus result;

  /* Obtain the configured media identifier */
  id = get_id();

  /* Stop if the configured media has no valid identifier */
  if (id < 0) {
    return;
  }

  /* Lock the media and query its serial number */
  CHECK(fs_ioc_lock (id) == fsOK);
  result = fs_ioc_device_ctrl (id, fsDevCtrlCodeSerial, &serial);
  CHECK((result == fsOK) || (result == fsUnsupported) || (result == fsError));

  /* Release the media lock */
  CHECK(fs_ioc_unlock (id) == fsOK);
}


/**
@}
*/

#endif /* IOC validation */
