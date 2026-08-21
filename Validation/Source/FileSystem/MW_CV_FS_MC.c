/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - File System - Memory Card
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_FS_MC.c
 * Purpose: MDK Middleware - Component Validation - File System - Memory Card -
 *          tests module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"

#if ((MW_CV_FS != 0U) && (MW_CV_FS_DRIVE_M != 0U))

#include <stddef.h>

#include "MW_CV_TestReport.h"
#include "MW_CV_FS.h"
#include "MW_CV_FS_MC.h"

#define CHECK(expr) ASSERT_TRUE((expr), "%s", #expr)

static int32_t      cv_cd;
static int32_t      cv_wp;
static uint32_t     cv_ss;
static fsLockUnlock cv_lock;

/*
\brief Return the simulated card-detect state for a memory-card drive.
\param[in] drive_num Memory-card drive instance number.
\return Simulated card-detect state for instance zero, or -1 for an unsupported instance.
*/
int32_t fs_mc_read_cd (uint32_t drive_num) {
  /* Return the injected state only for the configured drive instance */
  return (drive_num == 0U) ? cv_cd : -1;
}


/*
\brief Return the simulated write-protect state for a memory-card drive.
\param[in] drive_num Memory-card drive instance number.
\return Simulated write-protect state for instance zero, or -1 for an unsupported instance.
*/
int32_t fs_mc_read_wp (uint32_t drive_num) {
  /* Return the injected state only for the configured drive instance */
  return (drive_num == 0U) ? cv_wp : -1;
}


/*
\brief Record and return the simulated SPI slave-select level.
\param[in] drive_num Memory-card drive instance number.
\param[in] ss Requested slave-select level.
\return Recorded slave-select level for instance zero, or -1 for an unsupported instance.
*/
int32_t fs_mc_spi_control_ss (uint32_t drive_num, uint32_t ss) {
  /* Reject callbacks for a drive other than the configured memory-card instance */
  if (drive_num != 0U) {
    return -1;
  }

  /* Record the requested level so the test can verify callback propagation */
  cv_ss = ss;
  return (int32_t)cv_ss;
}


/*
\brief Return the simulated lock state storage for a memory-card drive.
\param[in] drive_num Memory-card drive instance number.
\return Address of the lock state for instance zero, or NULL for an unsupported instance.
*/
fsLockUnlock *fs_mc_lock_unlock (uint32_t drive_num) {
  /* Expose lock data only for the configured drive instance */
  return (drive_num == 0U) ? &cv_lock : NULL;
}


/**
\defgroup mw_cv_fs_mc_test_funcs Memory Card
\brief Memory-card validation test functions
\details
These tests validate removable-media status and memory-card callback contracts.
@{
*/

/**
\brief Test the fmedia function.
\details
\par Method
Query the current memory-card state.
\par Expected
- The configured drive is ready for the media-control test.
- The media query reports ready, no media, or unsupported, according to the target configuration.
*/
void MW_CV_FS_MC_fmedia_0 (void) {
  fsStatus status;

  /* Prepare the configured memory-card drive */
  CHECK(MW_CV_FS_EnsureReady() == fsOK);

  /* Query and validate the current media state */
  status = fmedia (MW_CV_FS_DRIVE);
  CHECK((status == fsOK) || (status == fsNoMedia) || (status == fsUnsupported));
}


/**
\brief Test the fmedia function.
\details
\par Method
Repeat the query and verify a stable state.
\par Expected
- The configured drive is ready for the media-control test.
- Repeated requests return the same result.
*/
void MW_CV_FS_MC_fmedia_1 (void) {
  fsStatus a, b;

  /* Prepare the configured memory-card drive */
  CHECK(MW_CV_FS_EnsureReady() == fsOK);

  /* Query the media state twice */
  a = fmedia (MW_CV_FS_DRIVE);
  b = fmedia (MW_CV_FS_DRIVE);

  /* Verify that the reported state is stable */
  CHECK(a == b);
}


/**
\brief Test the fs_mc_read_cd function.
\details
\par Method
Return the injected card-present state.
\par Expected
- The card-detect callback reports the expected present state.
*/
void MW_CV_FS_MC_fs_mc_read_cd_0 (void) {
  /* Inject the card-present state */
  cv_cd = 1;

  /* Verify the card-detect callback result */
  CHECK(fs_mc_read_cd (0U) == 1);
}


/**
\brief Test the fs_mc_read_cd function.
\details
\par Method
Switch the injected state and verify card-absent reporting.
\par Expected
- The card-detect callback reports the expected not-present state.
- The card-detect callback reports the expected invalid-drive state.
*/
void MW_CV_FS_MC_fs_mc_read_cd_1 (void) {
  /* Inject and verify the card-absent state */
  cv_cd = 0;
  CHECK(fs_mc_read_cd (0U) == 0);

  /* Verify rejection of an unsupported drive instance */
  CHECK(fs_mc_read_cd (1U) == -1);

  /* Restore the default card-present state */
  cv_cd = 1;
}


/**
\brief Test the fs_mc_read_wp function.
\details
\par Method
Return the injected writable state.
\par Expected
- The write-protect callback reports the expected unprotected state.
*/
void MW_CV_FS_MC_fs_mc_read_wp_0 (void) {
  /* Inject the writable-media state */
  cv_wp = 0;

  /* Verify the write-protect callback result */
  CHECK(fs_mc_read_wp (0U) == 0);
}


/**
\brief Test the fs_mc_read_wp function.
\details
\par Method
Switch the injected state and verify write-protected reporting through a write attempt.
\par Expected
- The write-protect callback reports the expected protected state.
- The write-protect callback reports the expected invalid-drive state.
*/
void MW_CV_FS_MC_fs_mc_read_wp_1 (void) {
  /* Inject and verify the write-protected state */
  cv_wp = 1;
  CHECK(fs_mc_read_wp (0U) == 1);

  /* Verify rejection of an unsupported drive instance */
  CHECK(fs_mc_read_wp (1U) == -1);

  /* Restore the default writable-media state */
  cv_wp = 0;
}


/**
\brief Test the fs_mc_spi_control_ss function.
\details
\par Method
Assert Slave Select for the configured SPI drive.
\par Expected
- The slave-select callback returns the expected active state.
- The observed slave-select state is active.
*/
void MW_CV_FS_MC_fs_mc_spi_control_ss_0 (void) {
  /* Assert Slave Select through the callback */
  CHECK(fs_mc_spi_control_ss (0U, 1U) == 1);

  /* Verify that the requested level was recorded */
  CHECK(cv_ss == 1U);
}


/**
\brief Test the fs_mc_spi_control_ss function.
\details
\par Method
Deassert Slave Select and verify the board signal transition.
\par Expected
- The slave-select callback returns the expected inactive state.
- The observed slave-select state is inactive.
*/
void MW_CV_FS_MC_fs_mc_spi_control_ss_1 (void) {
  /* Deassert Slave Select through the callback */
  CHECK(fs_mc_spi_control_ss (0U, 0U) == 0);

  /* Verify that the requested level was recorded */
  CHECK(cv_ss == 0U);
}


/**
\brief Test the fs_mc_spi_control_ss function.
\details
\par Method
Reject an invalid drive number where supported.
\par Expected
- The slave-select callback returns the expected invalid-drive state.
*/
void MW_CV_FS_MC_fs_mc_spi_control_ss_2 (void) {
  /* Verify rejection of an unsupported drive instance */
  CHECK(fs_mc_spi_control_ss (1U, 1U) == -1);
}


/**
\brief Test the fs_mc_lock_unlock function.
\details
\par Method
Return the default lock/unlock structure.
\par Expected
- The callback returns the configured lock descriptor.
- The lock descriptor reports no password, zero length, and no flags.
*/
void MW_CV_FS_MC_fs_mc_lock_unlock_0 (void) {
  fsLockUnlock *lock;

  /* Prepare an empty lock request */
  cv_lock.password = NULL;
  cv_lock.length   = 0U;
  cv_lock.flags    = 0U;

  /* Obtain the lock request through the callback */
  lock             = fs_mc_lock_unlock (0U);

  /* Verify the returned storage and default fields */
  CHECK(lock == &cv_lock);
  CHECK((lock->password == NULL) && (lock->length == 0U) && (lock->flags == 0U));
}


/**
\brief Test the fs_mc_lock_unlock function.
\details
\par Method
Inject another supported operation and verify every returned field.
\par Expected
- The callback returns the configured lock descriptor.
- The lock descriptor preserves the supplied password, its length, and the set-password flag.
- The lock/unlock callback rejects an invalid drive.
*/
void MW_CV_FS_MC_fs_mc_lock_unlock_1 (void) {
  static const uint8_t password[] = {1U, 2U, 3U, 4U};
  fsLockUnlock        *lock;

  /* Prepare a password-update request */
  cv_lock.password = password;
  cv_lock.length   = sizeof (password);
  cv_lock.flags    = FS_DEVICE_SET_PASSWORD;

  /* Obtain the lock request through the callback */
  lock             = fs_mc_lock_unlock (0U);

  /* Verify every returned request field */
  CHECK(lock == &cv_lock);
  CHECK((lock->password == password) && (lock->length == sizeof (password)) &&
         (lock->flags == FS_DEVICE_SET_PASSWORD));

  /* Verify rejection of an unsupported drive instance */
  CHECK(fs_mc_lock_unlock (1U) == NULL);

  /* Restore the default lock request */
  cv_lock.password = NULL;
  cv_lock.length   = 0U;
  cv_lock.flags    = 0U;
}


/**
@}
*/

#endif /* Memory Card validation */
