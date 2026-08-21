/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - File System - Core API
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_FS_Core.c
 * Purpose: MDK Middleware - Component Validation - File System - Core API -
 *          tests module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"

#if (MW_CV_FS != 0U)

#include <stdio.h>
#include <string.h>

#include "MW_CV_TestReport.h"
#include "MW_CV_FS.h"
#include "MW_CV_FS_Core.h"

#define CHECK(expr) ASSERT_TRUE((expr), "%s", #expr)

static fsTime cv_time = {6U, 7U, 8U, 9U, 10U, 2025U};

/*
\brief Provide deterministic time information to the FileSystem component.
\param[out] time Destination for the current validation timestamp.
\return fsOK when time is valid, or fsError when time is NULL.
*/
fsStatus fs_get_time (fsTime *time) {
  /* Reject a missing destination */
  if (time == NULL) {
    return fsError;
  }

  /* Return the injected validation timestamp */
  *time = cv_time;
  return fsOK;
}


/*
\brief Ensure that the configured drive is ready for a Core API test.
\return true when drive preparation succeeds; otherwise false.
*/
static bool ready (void) {
  fsStatus status = MW_CV_FS_EnsureReady();

  /* Verify drive preparation */
  CHECK(status == fsOK);

  /* Report whether the test may continue */
  return (status == fsOK);
}


/*
\brief Remove the mounted and initialized state from the configured drive.
*/
static void make_uninitialized (void) {
  /* Remove both mounted and initialized drive state */
  (void)funmount (MW_CV_FS_DRIVE);
  (void)funinit (MW_CV_FS_DRIVE);
}


/*
\brief Return the configured drive to the shared ready state and report failure.
*/
static void restore (void) {
  /* Reopen the configured drive for the following test */
  CHECK(MW_CV_FS_Reopen() == fsOK);
}


/*
\brief Create a file with bounded deterministic content.
\details Limits the requested payload to the local work buffer before creating the file.
\param[in] name File name to create or replace.
\param[in] seed Seed for the deterministic payload.
\param[in] size Requested payload size in bytes.
*/
static void make_file (const char *name, uint32_t seed, size_t size) {
  uint8_t data[32];

  if (size > sizeof (data)) {
    size = sizeof (data);
  }

  /* Generate and persist a bounded deterministic payload for the caller */
  MW_CV_FS_FillPattern (data, size, seed);
  MW_CV_FS_DeleteFile (name);
  CHECK(MW_CV_FS_CreateFile (name, data, size));
}


/**
\defgroup mw_cv_fs_core_test_funcs Core API
\brief File System Core API validation test functions
\details
These tests validate lifecycle, common file, utility, and time APIs.
@{
*/

/**
\brief Test the fversion function.
\details
\par Method
Read and validate the encoded library version.
\par Expected
- The filesystem reports a nonzero encoded version.
*/
void MW_CV_FS_Core_fversion_0 (void) {
  /* Read and validate the encoded component version */
  CHECK(fversion() != 0U);
}


/**
\brief Test the fversion function.
\details
\par Method
Repeat the call and verify a stable value.
\par Expected
- Repeated requests return the same result.
- The encoded major version is nonzero.
*/
void MW_CV_FS_Core_fversion_1 (void) {
  uint32_t a, b;

  /* Read the component version twice */
  a = fversion();
  b = fversion();

  /* Verify stability and a nonzero major version */
  CHECK(a == b);
  CHECK(((a >> 24) & 0xFFU) != 0U);
}


/**
\brief Test the finit function.
\details
\par Method
Initialize one explicitly configured drive.
\par Expected
- The drive initialization succeeds.
*/
void MW_CV_FS_Core_finit_0 (void) {
  /* Remove existing drive state */
  make_uninitialized();

  /* Initialize the explicitly configured drive */
  CHECK(finit (MW_CV_FS_DRIVE) == fsOK);

  /* Restore the shared ready state */
  restore();
}


/**
\brief Test the finit function.
\details
\par Method
Initialize using the documented current-drive specification.
\par Expected
- The drive initialization succeeds.
*/
void MW_CV_FS_Core_finit_1 (void) {
  /* Remove existing drive state */
  make_uninitialized();

  /* Initialize through the current-drive specification */
  CHECK(finit ("") == fsOK);

  /* Restore the shared ready state */
  restore();
}


/**
\brief Test the finit function.
\details
\par Method
Exercise repeated initialization.
\par Expected
- The drive initialization succeeds.
*/
void MW_CV_FS_Core_finit_2 (void) {
  /* Remove existing drive state */
  make_uninitialized();

  /* Initialize the same drive twice */
  CHECK(finit (MW_CV_FS_DRIVE) == fsOK);
  CHECK(finit (MW_CV_FS_DRIVE) == fsOK);

  /* Restore the shared ready state */
  restore();
}


/**
\brief Test the finit function.
\details
\par Method
Reject malformed and nonexistent drives with distinct statuses.
\par Expected
- The drive initialization reports an invalid parameter.
- The drive initialization reports an invalid drive.
*/
void MW_CV_FS_Core_finit_3 (void) {
  /* Verify null and invalid drive handling */
  CHECK(finit (NULL) == fsInvalidParameter);
  CHECK(finit (MW_CV_FS_INVALID_DRIVE) == fsInvalidDrive);
}


/**
\brief Test the funinit function.
\details
\par Method
Uninitialize an initialized, unmounted drive.
\par Expected
- The drive unmount succeeds.
- The drive uninitialization succeeds.
*/
void MW_CV_FS_Core_funinit_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Unmount and uninitialize the configured drive */
  CHECK(funmount (MW_CV_FS_DRIVE) == fsOK);
  CHECK(funinit (MW_CV_FS_DRIVE) == fsOK);

  /* Restore the shared ready state */
  restore();
}


/**
\brief Test the funinit function.
\details
\par Method
Uninitialize through the current-drive specification.
\par Expected
- The drive unmount succeeds.
- The drive uninitialization succeeds.
*/
void MW_CV_FS_Core_funinit_1 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Unmount and uninitialize through the current-drive specification */
  CHECK(funmount ("") == fsOK);
  CHECK(funinit ("") == fsOK);

  /* Restore the shared ready state */
  restore();
}


/**
\brief Test the funinit function.
\details
\par Method
Uninitialize an already uninitialized drive.
\par Expected
- The drive uninitialization succeeds.
*/
void MW_CV_FS_Core_funinit_2 (void) {
  /* Remove existing drive state */
  make_uninitialized();

  /* Repeat uninitialization and restore the drive */
  CHECK(funinit (MW_CV_FS_DRIVE) == fsOK);
  restore();
}


/**
\brief Test the funinit function.
\details
\par Method
Exercise uninitialization while the volume is mounted and restore a usable state.
\par Expected
- Uninitializing the mounted drive either succeeds or reports a general filesystem error, as documented.
*/
void MW_CV_FS_Core_funinit_3 (void) {
  fsStatus status;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Attempt uninitialization while the volume is mounted */
  status = funinit (MW_CV_FS_DRIVE);
  CHECK((status == fsOK) || (status == fsError));

  /* Restore the shared ready state */
  restore();
}


/**
\brief Test the funinit function.
\details
\par Method
Reject malformed and nonexistent drives with distinct statuses.
\par Expected
- The drive uninitialization reports an invalid parameter.
- The drive uninitialization reports an invalid drive.
*/
void MW_CV_FS_Core_funinit_4 (void) {
  /* Verify null and invalid drive handling */
  CHECK(funinit (NULL) == fsInvalidParameter);
  CHECK(funinit (MW_CV_FS_INVALID_DRIVE) == fsInvalidDrive);
}


/**
\brief Test the fmount function.
\details
\par Method
Mount an initialized formatted drive.
\par Expected
- The drive unmount succeeds.
- The drive mount succeeds.
*/
void MW_CV_FS_Core_fmount_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Unmount and remount the configured drive */
  CHECK(funmount (MW_CV_FS_DRIVE) == fsOK);
  CHECK(fmount (MW_CV_FS_DRIVE) == fsOK);
}


/**
\brief Test the fmount function.
\details
\par Method
Mount through the current-drive specification.
\par Expected
- The drive unmount succeeds.
- The drive mount succeeds.
*/
void MW_CV_FS_Core_fmount_1 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Unmount and remount through the current-drive specification */
  CHECK(funmount ("") == fsOK);
  CHECK(fmount ("") == fsOK);
}


/**
\brief Test the fmount function.
\details
\par Method
Exercise repeated mounting.
\par Expected
- The drive mount succeeds.
*/
void MW_CV_FS_Core_fmount_2 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Repeat the mount request */
  CHECK(fmount (MW_CV_FS_DRIVE) == fsOK);
}


/**
\brief Test the fmount function.
\details
\par Method
Reject mounting an uninitialized drive.
\par Expected
- The drive mount reports an uninitialized drive.
*/
void MW_CV_FS_Core_fmount_3 (void) {
  /* Remove initialization state */
  make_uninitialized();

  /* Verify that an uninitialized drive cannot be mounted */
  CHECK(fmount (MW_CV_FS_DRIVE) == fsUninitializedDrive);

  /* Restore the shared ready state */
  restore();
}


/**
\brief Test the fmount function.
\details
\par Method
Reject malformed and nonexistent drives with distinct statuses.
\par Expected
- The drive mount reports an invalid parameter.
- The drive mount reports an invalid drive.
*/
void MW_CV_FS_Core_fmount_4 (void) {
  /* Verify null and invalid drive handling */
  CHECK(fmount (NULL) == fsInvalidParameter);
  CHECK(fmount (MW_CV_FS_INVALID_DRIVE) == fsInvalidDrive);
}


/**
\brief Test the funmount function.
\details
\par Method
Unmount a mounted drive.
\par Expected
- The drive unmount succeeds.
*/
void MW_CV_FS_Core_funmount_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Unmount and restore the configured drive */
  CHECK(funmount (MW_CV_FS_DRIVE) == fsOK);
  restore();
}


/**
\brief Test the funmount function.
\details
\par Method
Unmount through the current-drive specification.
\par Expected
- The drive unmount succeeds.
*/
void MW_CV_FS_Core_funmount_1 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Unmount through the current-drive specification and restore the drive */
  CHECK(funmount ("") == fsOK);
  restore();
}


/**
\brief Test the funmount function.
\details
\par Method
Exercise repeated unmounting.
\par Expected
- The drive unmount succeeds.
*/
void MW_CV_FS_Core_funmount_2 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Unmount the same drive twice */
  CHECK(funmount (MW_CV_FS_DRIVE) == fsOK);
  CHECK(funmount (MW_CV_FS_DRIVE) == fsOK);
  restore();
}


/**
\brief Test the funmount function.
\details
\par Method
Exercise documented handling of an active stream and restore a usable state.
\par Expected
- The file stream opens successfully.
- Unmounting a drive with an active stream either succeeds or reports access denied, as documented.
*/
void MW_CV_FS_Core_funmount_3 (void) {
  FILE    *file;
  fsStatus status;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Open a stream that owns the mounted volume */
  file = fopen ("UMOUNT3.TST", "w");
  CHECK(file != NULL);

  /* Attempt the state transition while a stream still owns the volume */
  status = funmount (MW_CV_FS_DRIVE);
  CHECK((status == fsOK) || (status == fsAccessDenied));

  /* Close the stream if the volume remained mounted */
  if ((file != NULL) && (status != fsOK)) {
    (void)fclose (file);
  }
  /* Restore the drive and remove the test file */
  restore();
  MW_CV_FS_DeleteFile ("UMOUNT3.TST");
}


/**
\brief Test the funmount function.
\details
\par Method
Reject malformed and nonexistent drives with distinct statuses.
\par Expected
- The drive unmount reports an invalid parameter.
- The drive unmount reports an invalid drive.
*/
void MW_CV_FS_Core_funmount_4 (void) {
  /* Verify null and invalid drive handling */
  CHECK(funmount (NULL) == fsInvalidParameter);
  CHECK(funmount (MW_CV_FS_INVALID_DRIVE) == fsInvalidDrive);
}


/**
\brief Test the fdelete function.
\details
\par Method
Delete one existing file.
\par Expected
- The file deletion succeeds.
*/
void MW_CV_FS_Core_fdelete_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create and delete one file */
  make_file ("DELETE0.TST", 0U, 8U);
  CHECK(fdelete ("DELETE0.TST", NULL) == fsOK);
}


/**
\brief Test the fdelete function.
\details
\par Method
Reject deletion while the file has an active handle, then close and delete it.
\par Expected
- The file stream opens successfully.
- The file deletion reports access denied.
- The valid file stream closes successfully.
- The file deletion succeeds.
*/
void MW_CV_FS_Core_fdelete_1 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create and open the file for reading */
  make_file ("DELETE1.TST", 1U, 4U);
  file = fopen ("DELETE1.TST", "r");
  CHECK(file != NULL);

  /* Reject deletion while the stream is active */
  if (file != NULL) {
    CHECK(fdelete ("DELETE1.TST", NULL) == fsAccessDenied);
    CHECK(fclose (file) == 0);
  }
  /* Delete the file after closing the stream */
  CHECK(fdelete ("DELETE1.TST", NULL) == fsOK);
}


/**
\brief Test the fdelete function.
\details
\par Method
Delete a destination, rename another file into the released name, and verify its data.
\par Expected
- The file deletion succeeds.
- The rename operation succeeds.
- The file content matches the expected data.
*/
void MW_CV_FS_Core_fdelete_2 (void) {
  uint8_t data[8];

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  MW_CV_FS_FillPattern (data, sizeof (data), 2U);

  /* Release the destination name before moving the prepared source into it */
  MW_CV_FS_DeleteFile ("DEL_DST.TST");
  make_file ("DEL_SRC.TST", 2U, sizeof (data));
  make_file ("DEL_DST.TST", 3U, sizeof (data));
  CHECK(fdelete ("DEL_DST.TST", NULL) == fsOK);
  CHECK(frename ("DEL_SRC.TST", "DEL_DST.TST") == fsOK);
  CHECK(MW_CV_FS_VerifyFile ("DEL_DST.TST", data, sizeof (data)));
  MW_CV_FS_DeleteFile ("DEL_DST.TST");
}


/**
\brief Test the fdelete function.
\details
\par Method
Reject a missing file.
\par Expected
- The file deletion reports that the file or directory was not found.
*/
void MW_CV_FS_Core_fdelete_3 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Ensure the target is absent and verify deletion failure */
  MW_CV_FS_DeleteFile ("MISSING.TST");
  CHECK(fdelete ("MISSING.TST", NULL) == fsFileNotFound);
}


/**
\brief Test the ffind function.
\details
\par Method
Find one exact existing file.
\par Expected
- The file search succeeds.
- The reported file size is 3 bytes.
*/
void MW_CV_FS_Core_ffind_0 (void) {
  fsFileInfo info = {0};

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create and find an exact file name */
  make_file ("FIND0.TST", 0U, 3U);

  /* Verify the returned file size */
  CHECK(ffind ("FIND0.TST", &info) == fsOK);
  CHECK(info.size == 3U);
  /* Remove the test file */
  MW_CV_FS_DeleteFile ("FIND0.TST");
}


/**
\brief Test the ffind function.
\details
\par Method
Report no match and the documented end status.
\par Expected
- The file search reports that the file or directory was not found.
*/
void MW_CV_FS_Core_ffind_1 (void) {
  fsFileInfo info = {0};

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Ensure the target is absent and verify the search result */
  MW_CV_FS_DeleteFile ("NOFIND.TST");
  CHECK(ffind ("NOFIND.TST", &info) == fsFileNotFound);
}


/**
\brief Test the ffind function.
\details
\par Method
Enumerate prefix and suffix wildcard matches.
\par Expected
- Exactly 2 matching entries are found.
*/
void MW_CV_FS_Core_ffind_2 (void) {
  fsFileInfo info  = {0};
  uint32_t   count = 0U;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create two files matching one wildcard pattern */
  make_file ("FND_A.TST", 0U, 1U);
  make_file ("FND_B.TST", 0U, 1U);

  /* Count all matching directory entries */
  while (ffind ("FND_*.TST", &info) == fsOK) {
    count++;
  }
  CHECK(count == 2U);
  /* Remove both test files */
  MW_CV_FS_DeleteFile ("FND_A.TST");
  MW_CV_FS_DeleteFile ("FND_B.TST");
}


/**
\brief Test the ffind function.
\details
\par Method
Verify <tt>fileID</tt> continuation and reset semantics.
\par Expected
- The file search succeeds.
- The file search reports an error.
*/
void MW_CV_FS_Core_ffind_3 (void) {
  fsFileInfo info = {0};

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  make_file ("FIND3.TST", 0U, 1U);

  /* Exhaust one search, then reset its state and repeat the lookup */
  info.fileID = 0U;
  CHECK(ffind ("FIND3.TST", &info) == fsOK);
  CHECK(ffind ("FIND3.TST", &info) == fsError);

  info.fileID = 0U;
  CHECK(ffind ("FIND3.TST", &info) == fsOK);
  MW_CV_FS_DeleteFile ("FIND3.TST");
}


/**
\brief Test the ffind function.
\details
\par Method
Verify metadata for empty and one-byte files.
\par Expected
- The file search succeeds.
- The reported file size is 0 bytes.
- The file search succeeds.
- The reported file size is 1 byte.
*/
void MW_CV_FS_Core_ffind_4 (void) {
  fsFileInfo info = {0};

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  make_file ("EMPTY.TST", 0U, 0U);
  make_file ("ONE.TST", 0U, 1U);

  /* Compare metadata for empty and nonempty files */
  CHECK(ffind ("EMPTY.TST", &info) == fsOK);
  CHECK(info.size == 0U);
  info.fileID = 0U;
  CHECK(ffind ("ONE.TST", &info) == fsOK);
  CHECK(info.size == 1U);
  MW_CV_FS_DeleteFile ("EMPTY.TST");
  MW_CV_FS_DeleteFile ("ONE.TST");
}


/**
\brief Test the ffind function.
\details
\par Method
Reject calls on unmounted and uninitialized drives.
\par Expected
- The drive unmount succeeds.
- The file search reports access denied.
- The drive uninitialization succeeds.
- The file search reports access denied.
*/
void MW_CV_FS_Core_ffind_5 (void) {
  fsFileInfo info = {0};

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Verify search failures across mounted, initialized, and restored states */
  CHECK(funmount (MW_CV_FS_DRIVE) == fsOK);
  CHECK(ffind ("*.TST", &info) == fsAccessDenied);
  CHECK(funinit (MW_CV_FS_DRIVE) == fsOK);
  info.fileID = 0U;
  CHECK(ffind ("*.TST", &info) == fsAccessDenied);
  restore();
}


/**
\brief Test the ffind function.
\details
\par Method
Search for a longer exact name when only its proper prefix exists.
\par Expected
- The file search reports that the file or directory was not found.
*/
void MW_CV_FS_Core_ffind_6 (void) {
  fsFileInfo info = {0};

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create only the proper-prefix name */
  MW_CV_FS_DeleteFile ("FF6LONG");
  make_file ("FF6", 0U, 1U);

  /* Verify that an exact longer-name search does not match */
  CHECK(ffind ("FF6LONG", &info) == fsFileNotFound);

  /* Remove the prefix file */
  MW_CV_FS_DeleteFile ("FF6");
}


/**
\brief Test the ffind function.
\details
\par Method
Search for a longer exact name after creating its proper prefix first.
\par Expected
- The file search succeeds.
- The returned name equals <tt>FF7LONG</tt> without regard to letter case.
- The reported file size is 7 bytes.
*/
void MW_CV_FS_Core_ffind_7 (void) {
  fsFileInfo info = {0};

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create both prefix-related names */
  MW_CV_FS_DeleteFile ("FF7LONG");
  make_file ("FF7", 0U, 1U);
  make_file ("FF7LONG", 1U, 7U);

  /* Find and verify the exact longer name */
  CHECK(ffind ("FF7LONG", &info) == fsOK);
  CHECK(MW_CV_FS_StrEqualIgnoreCase (info.name, "FF7LONG"));
  CHECK(info.size == 7U);
  /* Remove both test files */
  MW_CV_FS_DeleteFile ("FF7");
  MW_CV_FS_DeleteFile ("FF7LONG");
}


/**
\brief Test the ffind function.
\details
\par Method
Search for a shorter exact name when only a longer name with the same prefix exists.
\par Expected
- The file search reports that the file or directory was not found.
*/
void MW_CV_FS_Core_ffind_8 (void) {
  fsFileInfo info = {0};

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create only the longer prefix-related name */
  MW_CV_FS_DeleteFile ("FF8");
  make_file ("FF8LONG", 0U, 1U);

  /* Verify that an exact shorter-name search does not match */
  CHECK(ffind ("FF8", &info) == fsFileNotFound);

  /* Remove the longer file */
  MW_CV_FS_DeleteFile ("FF8LONG");
}


/**
\brief Test the ffind function.
\details
\par Method
Create a lowercase 8.3 file name and search for it using an equal-length mixed-case pattern.
\par Expected
- The file search succeeds.
- The returned name equals <tt>ff9case.tst</tt> without regard to letter case.
*/
void MW_CV_FS_Core_ffind_9 (void) {
  fsFileInfo info = {0};

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a lowercase short name */
  make_file ("ff9case.tst", 0U, 1U);

  /* Find it through a mixed-case spelling */
  CHECK(ffind ("Ff9CaSe.TsT", &info) == fsOK);
  CHECK(MW_CV_FS_StrEqualIgnoreCase (info.name, "ff9case.tst"));

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("ff9case.tst");
}


/**
\brief Test the frename function.
\details
\par Method
Rename one file in the drive root and preserve its data.
\par Expected
- The rename operation succeeds.
- The file content matches the expected data.
*/
void MW_CV_FS_Core_frename_0 (void) {
  uint8_t data[8];

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Prepare the source payload and both names */
  MW_CV_FS_FillPattern (data, sizeof (data), 0U);
  make_file ("REN0A.TST", 0U, sizeof (data));
  MW_CV_FS_DeleteFile ("REN0B.TST");
  /* Rename the source and verify its payload */
  CHECK(frename ("REN0A.TST", "REN0B.TST") == fsOK);
  CHECK(MW_CV_FS_VerifyFile ("REN0B.TST", data, sizeof (data)));
  /* Remove the renamed file */
  MW_CV_FS_DeleteFile ("REN0B.TST");
}


/**
\brief Test the frename function.
\details
\par Method
Verify the old path is inaccessible and the released name can be reused.
\par Expected
- The rename operation succeeds.
- The file search reports that the file or directory was not found.
*/
void MW_CV_FS_Core_frename_1 (void) {
  fsFileInfo info = {0};

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Prepare and rename the source file */
  make_file ("REN1A.TST", 1U, 1U);
  MW_CV_FS_DeleteFile ("REN1B.TST");
  CHECK(frename ("REN1A.TST", "REN1B.TST") == fsOK);
  /* Verify release and immediate reuse of the old name */
  CHECK(ffind ("REN1A.TST", &info) == fsFileNotFound);
  make_file ("REN1A.TST", 2U, 1U);
  /* Remove both names */
  MW_CV_FS_DeleteFile ("REN1A.TST");
  MW_CV_FS_DeleteFile ("REN1B.TST");
}


/**
\brief Test the frename function.
\details
\par Method
Reject a missing source.
\par Expected
- The rename operation reports that the file or directory was not found.
*/
void MW_CV_FS_Core_frename_2 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Ensure the source is absent and verify rename failure */
  MW_CV_FS_DeleteFile ("REN2A.TST");
  CHECK(frename ("REN2A.TST", "REN2B.TST") == fsFileNotFound);
}


/**
\brief Test the frename function.
\details
\par Method
Reject a conflicting destination without data loss.
\par Expected
- The rename operation reports that the name already exists.
*/
void MW_CV_FS_Core_frename_3 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create distinct source and destination files */
  make_file ("REN3A.TST", 1U, 1U);
  make_file ("REN3B.TST", 2U, 1U);
  /* Verify rejection of the occupied destination */
  CHECK(frename ("REN3A.TST", "REN3B.TST") == fsAlreadyExists);

  /* Remove both files */
  MW_CV_FS_DeleteFile ("REN3A.TST");
  MW_CV_FS_DeleteFile ("REN3B.TST");
}


/**
\brief Test the fchdrive function.
\details
\par Method
Call <tt>fchdrive</tt> with a configured drive.
Call <tt>fchdrive</tt> with an empty drive string.
\par Expected
- The current-drive selection succeeds.
- The current-drive selection succeeds.
*/
void MW_CV_FS_Core_fchdrive_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Select the configured drive explicitly */
  CHECK(fchdrive (MW_CV_FS_DRIVE) == fsOK);

  /* Select the documented current drive */
  CHECK(fchdrive ("") == fsOK);
}


/**
\brief Test the fchdrive function.
\details
\par Method
Reject a nonexistent drive.
Call <tt>fchdrive</tt> with a NULL drive.
\par Expected
- The current-drive selection reports an invalid drive.
- The current-drive selection reports an invalid parameter.
*/
void MW_CV_FS_Core_fchdrive_1 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Verify rejection of an unknown drive */
  CHECK(fchdrive (MW_CV_FS_INVALID_DRIVE) == fsInvalidDrive);

  /* Verify rejection of a null drive specification */
  CHECK(fchdrive (NULL) == fsInvalidParameter);
}


/**
\brief Test the ffree function.
\details
\par Method
Query free space on the configured drive.
\par Expected
- The free-space query returns a valid nonnegative result.
*/
void MW_CV_FS_Core_ffree_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Query and validate the available capacity */
  CHECK(ffree (MW_CV_FS_DRIVE) >= 0);
}


/**
\brief Test the ffree function.
\details
\par Method
Verify free space changes after file allocation and deletion.
\par Expected
- The test file is created with the expected data.
- All reported free-space measurements are valid nonnegative values.
- Creating the file does not increase the reported free space.
- Deleting the file restores the consumed free space.
*/
void MW_CV_FS_Core_ffree_1 (void) {
  int64_t before, used, after;
  uint8_t data[513];

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  MW_CV_FS_DeleteFile ("FREE1.TST");
  before = ffree (MW_CV_FS_DRIVE);
  MW_CV_FS_FillPattern (data, sizeof (data), 1U);

  /* Allocate and release data before comparing free-space transitions */
  CHECK(MW_CV_FS_CreateFile ("FREE1.TST", data, sizeof (data)));
  used = ffree (MW_CV_FS_DRIVE);
  MW_CV_FS_DeleteFile ("FREE1.TST");
  after = ffree (MW_CV_FS_DRIVE);

  /* Compare all three observations only after the volume is restored */
  CHECK((before >= 0) && (used >= 0) && (after >= 0));
  CHECK(used <= before);
  CHECK(after >= used);
}


/**
\brief Test the ffree function.
\details
\par Method
Reject an invalid drive.
\par Expected
- The free-space query reports an invalid drive.
*/
void MW_CV_FS_Core_ffree_2 (void) {
  /* Verify rejection of an invalid drive */
  CHECK(ffree (MW_CV_FS_INVALID_DRIVE) == -(int64_t)fsInvalidDrive);
}


/**
\brief Test the fformat function.
\details
\par Method
Format dedicated validation media with default options.
\par Expected
- The drive is formatted successfully.
- Formatting leaves a recognized filesystem on the drive.
*/
void MW_CV_FS_Core_fformat_0 (void) {
  /* Format with default backend options */
  CHECK(MW_CV_FS_Format (NULL) == fsOK);

  /* Verify that a filesystem was created */
  CHECK(MW_CV_FS_GetType() != fsTypeNone);
}


/**
\brief Test the fformat function.
\details
\par Method
Format using backend-specific options and verify with <tt>finfo</tt>.
\par Expected
- The drive is formatted successfully with the requested options.
- Formatting leaves a recognized filesystem on the drive.
*/
void MW_CV_FS_Core_fformat_1 (void) {
  const char *options = MW_CV_FS_IsFAT() ? "/L CVTEST" : "/IGNORED";

  /* Format with backend-specific options */
  CHECK(MW_CV_FS_Format (options) == fsOK);

  /* Verify that a filesystem was created */
  CHECK(MW_CV_FS_GetType() != fsTypeNone);
}


/**
\brief Test the fformat function.
\details
\par Method
Reject invalid drive parameters.
\par Expected
- The format request reports an invalid parameter.
- The format request reports an invalid drive.
*/
void MW_CV_FS_Core_fformat_2 (void) {
  /* Verify null and invalid drive handling */
  CHECK(fformat (NULL, NULL) == fsInvalidParameter);
  CHECK(fformat (MW_CV_FS_INVALID_DRIVE, NULL) == fsInvalidDrive);
}


/**
\brief Test the finfo function.
\details
\par Method
Read information for the configured drive.
\par Expected
- The drive-information query succeeds.
*/
void MW_CV_FS_Core_finfo_0 (void) {
  fsDriveInfo info;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Read information for the configured drive */
  CHECK(finfo (MW_CV_FS_DRIVE, &info) == fsOK);
}


/**
\brief Test the finfo function.
\details
\par Method
Verify type and capacity fields after format and mount.
\par Expected
- The drive-information query succeeds.
- The drive reports nonzero capacity.
- The reported filesystem type is one of the supported FAT variants.
- The reported filesystem type is EFS.
*/
void MW_CV_FS_Core_finfo_1 (void) {
  fsDriveInfo info;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Read and validate capacity information */
  CHECK(finfo (MW_CV_FS_DRIVE, &info) == fsOK);
  CHECK(info.capacity != 0U);

  /* Verify the backend-specific filesystem type */
  if (MW_CV_FS_IsFAT()) {
    CHECK((info.fs_type == fsTypeFAT12) || (info.fs_type == fsTypeFAT16) || (info.fs_type == fsTypeFAT32));
  } else {
    CHECK(info.fs_type == fsTypeEFS);
  }
}


/**
\brief Test the finfo function.
\details
\par Method
Reject an invalid drive.
\par Expected
- The drive-information query reports an invalid parameter.
- The drive-information query reports an invalid parameter.
- The drive-information query reports an invalid drive.
*/
void MW_CV_FS_Core_finfo_2 (void) {
  fsDriveInfo info;

  /* Verify null drive and destination handling */
  CHECK(finfo (NULL, &info) == fsInvalidParameter);
  CHECK(finfo (MW_CV_FS_DRIVE, NULL) == fsInvalidParameter);

  /* Verify rejection of an invalid drive */
  CHECK(finfo (MW_CV_FS_INVALID_DRIVE, &info) == fsInvalidDrive);
}


/**
\brief Test the fs_get_time function.
\details
\par Method
Return one fixed valid time.
\par Expected
- The filesystem-time query succeeds.
- The returned data matches the expected data exactly.
*/
void MW_CV_FS_Core_fs_get_time_0 (void) {
  fsTime time;

  /* Read the injected timestamp */
  CHECK(fs_get_time (&time) == fsOK);

  /* Compare every returned field */
  CHECK(memcmp (&time, &cv_time, sizeof (time)) == 0);
}


/**
\brief Test the fs_get_time function.
\details
\par Method
Change the injected time and verify the complete result.
\par Expected
- The filesystem-time query succeeds.
- The returned data matches the expected data exactly.
- The filesystem-time query reports an error.
*/
void MW_CV_FS_Core_fs_get_time_1 (void) {
  fsTime saved = cv_time;
  fsTime time;

  /* Save and modify the injected timestamp */
  cv_time.sec = 10U;
  cv_time.day = 11U;

  /* Read and verify the modified timestamp */
  CHECK(fs_get_time (&time) == fsOK);
  CHECK(memcmp (&time, &cv_time, sizeof (time)) == 0);

  /* Verify null destination handling */
  CHECK(fs_get_time (NULL) == fsError);

  /* Restore the original timestamp */
  cv_time = saved;
}


/**
\brief Test the fs_get_time function.
\details
\par Method
Verify timestamp use through a FAT file operation.
\par Expected
- The timestamp query succeeds.
- The stored creation year matches the supplied filesystem time.
- The filesystem-time query succeeds.
*/
void MW_CV_FS_Core_fs_get_time_2 (void) {
  fsTime create, access, write;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  if (MW_CV_FS_IsFAT()) {
    /* Create a FAT file and read its stored timestamps */
    make_file ("TIME2.TST", 2U, 1U);
    CHECK(ftime_get ("TIME2.TST", &create, &access, &write) == fsOK);

    /* Verify use of the injected creation year */
    CHECK(create.year == cv_time.year);

    /* Remove the FAT test file */
    MW_CV_FS_DeleteFile ("TIME2.TST");
  } else {
    /* Verify the callback directly when timestamps are unsupported */
    CHECK(fs_get_time (&create) == fsOK);
  }
}


/**
@}
*/

#endif /* MW_CV_FS */
