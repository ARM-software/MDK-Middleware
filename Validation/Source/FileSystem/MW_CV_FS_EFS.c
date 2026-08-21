/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - File System - EFS
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_FS_EFS.c
 * Purpose: MDK Middleware - Component Validation - File System - EFS -
 *          tests module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"

#if ((MW_CV_FS != 0U) && (MW_CV_FS_DRIVE_F != 0U))

#include <stdio.h>
#include <string.h>

#include "MW_CV_TestReport.h"
#include "MW_CV_FS.h"
#include "MW_CV_FS_EFS.h"

#define CHECK(expr) ASSERT_TRUE((expr), "%s", #expr)

static uint8_t pattern[512];

/*
\brief Ensure that the configured EFS drive is ready for a test.
\return true when drive preparation succeeds; otherwise false.
*/
static bool ready (void) {
  fsStatus status = MW_CV_FS_EnsureReady();

  /* Verify drive preparation and the EFS backend */
  CHECK(status == fsOK);
  CHECK(MW_CV_FS_GetType() == fsTypeEFS);

  /* Prepare deterministic data for dependent tests */
  MW_CV_FS_FillPattern (pattern, sizeof (pattern), 0U);

  /* Report whether the test may continue */
  return ((status == fsOK) && (MW_CV_FS_GetType() == fsTypeEFS));
}


/*
\brief Remove a numbered group of EFS test files.
\param[in] count Number of files named EFnn.TST to remove.
*/
static void remove_files (uint32_t count) {
  char     name[16];
  uint32_t i;

  /* Remove every numbered file in the requested range */
  for (i = 0U; i < count; i++) {
    (void)snprintf (name, sizeof (name), "EF%02u.TST", (unsigned)i);
    MW_CV_FS_DeleteFile (name);
  }
}


/*
\brief Append deterministic chunks to a file in separate stream sessions.
\param[in] name File name to create and extend.
\param[in] chunks Number of pattern chunks to append.
\param[in] seed Seed used for the first chunk; each following chunk increments it.
\return true when every write and stream closure succeeds; otherwise false.
*/
static bool append_chunks (const char *name, uint32_t chunks, uint32_t seed) {
  FILE    *file;
  uint32_t i;
  bool     ok = true;

  /* Remove stale file state */
  MW_CV_FS_DeleteFile (name);

  /* Append deterministic chunks in separate sessions to create controlled fragmentation */
  for (i = 0U; i < chunks; i++) {
    MW_CV_FS_FillPattern (pattern, sizeof (pattern), seed + i);

    /* Open a separate append session for this chunk */
    file = fopen (name, "ab");

    if (file == NULL) {
      return false;
    }

    /* Append the deterministic chunk */
    if (fwrite (pattern, 1U, sizeof (pattern), file) != sizeof (pattern)) {
      ok = false;
    }

    /* Persist and close the current chunk */
    if (fclose (file) != 0) {
      ok = false;
    }

    /* Stop after the first failed write or close operation */
    if (!ok) {
      break;
    }
  }

  /* Report the combined result of all append sessions */
  return ok;
}


/*
\brief Verify the expected alternating deleted and retained EFS entries.
\param[in] count Number of EFnn.TST entries to inspect.
*/
static void check_survivors (uint32_t count) {
  char       name[16];
  fsFileInfo info;
  uint32_t   i;

  /* Verify that deleted entries remain absent and retained entries remain accessible */
  for (i = 0U; i < count; i++) {
    (void)snprintf (name, sizeof (name), "EF%02u.TST", (unsigned)i);
    (void)memset (&info, 0, sizeof (info));

    if ((i & 1U) == 0U) {
      CHECK(ffind (name, &info) == fsFileNotFound);
    } else {
      CHECK(ffind (name, &info) == fsOK);
      CHECK(info.size != 0U);
    }
  }
}


/**
\defgroup mw_cv_fs_efs_test_funcs EFS
\brief EFS validation test functions
\details
These tests validate EFS APIs and rejection of FAT-only operations.
@{
*/

/**
\brief Test the fanalyse function.
\details
\par Method
Analyse a valid EFS drive.
\par Expected
- The EFS analysis returns a valid nonnegative result.
*/
void MW_CV_FS_EFS_fanalyse_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Analyse the prepared EFS volume */
  CHECK(fanalyse (MW_CV_FS_DRIVE) >= 0);
}


/**
\brief Test the fanalyse function.
\details
\par Method
Verify the result after controlled file allocation/deletion.
\par Expected
- All requested data chunks are appended successfully.
- Both analysis measurements are valid nonnegative values.
*/
void MW_CV_FS_EFS_fanalyse_1 (void) {
  int32_t before, after;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Record analysis before allocating fragmented content */
  before = fanalyse (MW_CV_FS_DRIVE);

  /* Create repeated allocations and analyse the result */
  CHECK(append_chunks ("ANALYSE.TST", 4U, 1U));
  after = fanalyse (MW_CV_FS_DRIVE);

  /* Verify both analysis operations */
  CHECK((before >= 0) && (after >= 0));

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("ANALYSE.TST");
}


/**
\brief Test the fanalyse function.
\details
\par Method
Reject an invalid drive.
\par Expected
- The EFS analysis reports an invalid drive.
*/
void MW_CV_FS_EFS_fanalyse_2 (void) {
  /* Verify rejection of an invalid drive */
  CHECK(fanalyse (MW_CV_FS_INVALID_DRIVE) == -(int32_t)fsInvalidDrive);
}


/**
\brief Test the fcheck function.
\details
\par Method
Check a valid initialized EFS drive.
\par Expected
- The EFS consistency check succeeds.
*/
void MW_CV_FS_EFS_fcheck_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Check the prepared EFS volume */
  CHECK(fcheck (MW_CV_FS_DRIVE) == fsOK);
}


/**
\brief Test the fcheck function.
\details
\par Method
Check after format and file creation.
\par Expected
- All requested data chunks are appended successfully.
- The EFS consistency check succeeds.
*/
void MW_CV_FS_EFS_fcheck_1 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create repeated allocations and check the volume */
  CHECK(append_chunks ("CHECK1.TST", 2U, 1U));
  CHECK(fcheck (MW_CV_FS_DRIVE) == fsOK);

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("CHECK1.TST");
}


/**
\brief Test the fcheck function.
\details
\par Method
Reject an invalid drive.
\par Expected
- The EFS consistency check reports an invalid drive.
*/
void MW_CV_FS_EFS_fcheck_2 (void) {
  /* Verify rejection of an invalid drive */
  CHECK(fcheck (MW_CV_FS_INVALID_DRIVE) == fsInvalidDrive);
}


/**
\brief Test the fdefrag function.
\details
\par Method
Defragment an empty valid EFS drive.
\par Expected
- The EFS defragmentation succeeds.
*/
void MW_CV_FS_EFS_fdefrag_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Defragment the prepared EFS volume */
  CHECK(fdefrag (MW_CV_FS_DRIVE) == fsOK);
}


/**
\brief Test the fdefrag function.
\details
\par Method
Defragment one file built by repeated bounded appends.
\par Expected
- All requested data chunks are appended successfully.
- The EFS defragmentation succeeds.
*/
void MW_CV_FS_EFS_fdefrag_1 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create fragmented content and defragment the volume */
  CHECK(append_chunks ("DEFRAG1.TST", 8U, 1U));
  CHECK(fdefrag (MW_CV_FS_DRIVE) == fsOK);

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("DEFRAG1.TST");
}


/**
\brief Test the fdefrag function.
\details
\par Method
Defragment interleaved writes to several files.
\par Expected
- All requested data chunks are appended successfully.
- The EFS defragmentation succeeds.
*/
void MW_CV_FS_EFS_fdefrag_2 (void) {
  char     name[16];
  uint32_t i;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Remove stale numbered files */
  remove_files (5U);

  /* Create several independently fragmented files */
  for (i = 0U; i < 5U; i++) {
    (void)snprintf (name, sizeof (name), "EF%02u.TST", (unsigned)i);
    CHECK(append_chunks (name, 4U, i));
  }
  /* Defragment the populated volume */
  CHECK(fdefrag (MW_CV_FS_DRIVE) == fsOK);

  /* Remove the numbered files */
  remove_files (5U);
}


/**
\brief Test the fdefrag function.
\details
\par Method
Delete alternating files before defragmentation.
\par Expected
- All requested data chunks are appended successfully.
- The EFS defragmentation succeeds.
*/
void MW_CV_FS_EFS_fdefrag_3 (void) {
  char     name[16];
  uint32_t i;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create fragmented files, then remove alternating entries to expose free gaps */
  remove_files (8U);

  for (i = 0U; i < 8U; i++) {
    (void)snprintf (name, sizeof (name), "EF%02u.TST", (unsigned)i);
    CHECK(append_chunks (name, 2U, i));
  }

  for (i = 0U; i < 8U; i += 2U) {
    (void)snprintf (name, sizeof (name), "EF%02u.TST", (unsigned)i);
    MW_CV_FS_DeleteFile (name);
  }

  /* Defragment the remaining entries and verify their directory state */
  CHECK(fdefrag (MW_CV_FS_DRIVE) == fsOK);
  check_survivors (8U);
  remove_files (8U);
}


/**
\brief Test the fdefrag function.
\details
\par Method
Rename survivors into released names before defragmentation.
\par Expected
- All requested data chunks are appended successfully.
- All requested data chunks are appended successfully.
- The rename operation succeeds.
- The EFS defragmentation succeeds.
*/
void MW_CV_FS_EFS_fdefrag_4 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create two fragmented files */
  remove_files (4U);
  CHECK(append_chunks ("EF00.TST", 2U, 0U));
  CHECK(append_chunks ("EF01.TST", 2U, 1U));

  /* Release one name and rename the surviving file into it */
  MW_CV_FS_DeleteFile ("EF00.TST");
  CHECK(frename ("EF01.TST", "EF00.TST") == fsOK);

  /* Defragment and remove the renamed file */
  CHECK(fdefrag (MW_CV_FS_DRIVE) == fsOK);
  MW_CV_FS_DeleteFile ("EF00.TST");
}


/**
\brief Test the fdefrag function.
\details
\par Method
Reject an invalid drive.
\par Expected
- The EFS defragmentation reports an invalid drive.
*/
void MW_CV_FS_EFS_fdefrag_5 (void) {
  /* Verify rejection of an invalid drive */
  CHECK(fdefrag (MW_CV_FS_INVALID_DRIVE) == fsInvalidDrive);
}


/**
\brief Test the fattrib function.
\details
\par Method
Create an EFS file, request the FAT read-only attribute, then verify the original payload.
\par Expected
- The test file is created with the expected data.
- EFS rejects the request to set the FAT read-only attribute.
- The file content matches the expected data.
*/
void MW_CV_FS_EFS_fattrib_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a file and reject the FAT attribute request */
  CHECK(MW_CV_FS_CreateFile ("NOATTR.TST", pattern, 1U));
  CHECK(fattrib ("NOATTR.TST", "+R") != fsOK);

  /* Verify that the file content is unchanged */
  CHECK(MW_CV_FS_VerifyFile ("NOATTR.TST", pattern, 1U));

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("NOATTR.TST");
}


/**
\brief Test the fpwd function.
\details
\par Method
Call <tt>fpwd</tt> for the configured EFS drive with a valid destination buffer.
\par Expected
- The current-directory query is rejected.
*/
void MW_CV_FS_EFS_fpwd_0 (void) {
  char buf[16];

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Verify that EFS rejects the working-directory query */
  CHECK(fpwd (MW_CV_FS_DRIVE, buf, sizeof (buf)) != fsOK);
}


/**
\brief Test the fchdir function.
\details
\par Method
Call <tt>fchdir</tt> with a directory name on the configured EFS drive.
\par Expected
- The directory change is rejected.
*/
void MW_CV_FS_EFS_fchdir_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Verify that EFS rejects directory navigation */
  CHECK(fchdir ("DIR") != fsOK);
}


/**
\brief Test the fmkdir function.
\details
\par Method
Call <tt>fmkdir</tt> with a directory name on the configured EFS drive.
\par Expected
- The directory creation is rejected.
*/
void MW_CV_FS_EFS_fmkdir_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Verify that EFS rejects directory creation */
  CHECK(fmkdir ("DIR") != fsOK);
}


/**
\brief Test the frmdir function.
\details
\par Method
Call <tt>frmdir</tt> with a directory name on the configured EFS drive.
\par Expected
- The directory removal is rejected.
*/
void MW_CV_FS_EFS_frmdir_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Verify that EFS rejects directory removal */
  CHECK(frmdir ("DIR", NULL) != fsOK);
}


/**
\brief Test the fvol function.
\details
\par Method
Call <tt>fvol</tt> for the configured EFS drive with label and serial-number outputs.
\par Expected
- The volume-information request reports access denied.
*/
void MW_CV_FS_EFS_fvol_0 (void) {
  char     label[12];
  uint32_t serial;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Verify that EFS rejects FAT volume information */
  CHECK(fvol (MW_CV_FS_DRIVE, label, &serial) == fsAccessDenied);
}


/**
\brief Test the ftime_set function.
\details
\par Method
Create an EFS file and attempt to assign a FAT timestamp with <tt>ftime_set</tt>.
\par Expected
- The test file is created with the expected data.
- The timestamp update is rejected.
*/
void MW_CV_FS_EFS_ftime_set_0 (void) {
  fsTime time = {1U, 2U, 4U, 5U, 6U, 2024U};

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a file and reject the FAT timestamp update */
  CHECK(MW_CV_FS_CreateFile ("NOTIME.TST", pattern, 1U));
  CHECK(ftime_set ("NOTIME.TST", NULL, NULL, &time) != fsOK);

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("NOTIME.TST");
}


/**
\brief Test the ftime_get function.
\details
\par Method
Create an EFS file and attempt to retrieve a FAT timestamp with <tt>ftime_get</tt>.
\par Expected
- The test file is created with the expected data.
- The timestamp query is rejected.
*/
void MW_CV_FS_EFS_ftime_get_0 (void) {
  fsTime time;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a file and reject the FAT timestamp query */
  CHECK(MW_CV_FS_CreateFile ("NOTIME.TST", pattern, 1U));
  CHECK(ftime_get ("NOTIME.TST", NULL, NULL, &time) != fsOK);

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("NOTIME.TST");
}


/**
\brief Test the fs_ioc_get_id function.
\details
\par Method
Request an I/O-control identifier for the configured EFS drive.
\par Expected
- The media-identifier query reports access denied for EFS.
*/
void MW_CV_FS_EFS_fs_ioc_get_id_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Verify that EFS rejects direct IOC access */
  CHECK(fs_ioc_get_id (MW_CV_FS_DRIVE) == -(int32_t)fsAccessDenied);
}


/**
@}
*/

#endif /* EFS validation */
