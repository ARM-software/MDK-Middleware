/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - File System - Stress
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_FS_Stress.c
 * Purpose: MDK Middleware - Component Validation - File System - Stress -
 *          tests module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"

#if ((MW_CV_FS != 0U) && (MW_CV_FS_STRESS != 0U))

#include <stdio.h>
#include <string.h>

#include "cmsis_os2.h"
#include "MW_CV_TestReport.h"
#include "MW_CV_FS.h"
#include "MW_CV_FS_Stress.h"

#if (MW_CV_FS_DRIVE_M != 0U)
#include "FS_Config_MC_0.h"
#endif

#define CHECK(expr) ASSERT_TRUE((expr), "%s", #expr)
#define START_FLAG 1U

#if (MW_CV_FS_DRIVE_F != 0U)
#define EFS_FFIND_FILE_COUNT          256U
#define EFS_FFIND_SHORT_FILE_COUNT    (EFS_FFIND_FILE_COUNT / 2U)
#endif

#if (MW_CV_FS_DRIVE_M != 0U)
#define FAT_NAME_CACHE_PRESSURE_COUNT (MC0_NAME_CACHE_SIZE + 8U)
#define FAT_FFIND_DIRECTORY_COUNT     25U
#define FAT_FFIND_FILE_COUNT          50U
#define FAT_FFIND_SHORT_FILE_COUNT    (FAT_FFIND_FILE_COUNT / 2U)
#endif

typedef struct {
  const char  *path;
  osThreadId_t owner;
  uint32_t     done;
  bool         write;
} WorkerContext;

static uint8_t data[256];

/*
\brief Prepare the configured drive and shared deterministic stress-test data.
\return true when drive preparation succeeds; otherwise false.
*/
static bool ready (void) {
  fsStatus status = MW_CV_FS_EnsureReady();

  /* Verify drive preparation */
  CHECK(status == fsOK);

  /* Fill the shared deterministic stress pattern */
  MW_CV_FS_FillPattern (data, sizeof (data), 0U);

  /* Report whether the test may continue */
  return (status == fsOK);
}


/*
\brief Copy a complete file using a bounded work buffer.
\param[in] dst Destination file path to create or replace.
\param[in] src Source file path to read.
\return true when all transfers and stream closures succeed; otherwise false.
*/
static bool copy_file (const char *dst, const char *src) {
  uint8_t buf[37];
  FILE   *in = NULL, *out = NULL;
  size_t  rd;
  bool    ok = false;

  in = fopen (src, "rb");
  if (in != NULL) {
    /* Open the destination only after the source stream is available */
    out = fopen (dst, "wb");
  }

  if ((in != NULL) && (out != NULL)) {
    ok = true;

    /* Transfer bounded chunks and stop immediately on a short write */
    do {
      rd = fread (buf, 1U, sizeof (buf), in);
      if (fwrite (buf, 1U, rd, out) != rd) {
        ok = false;
        break;
      }
    } while (rd == sizeof (buf));
  }

  /* Include both stream closures in the result of the copy operation */
  if ((out != NULL) && (fclose (out) != 0)) {
    ok = false;
  }

  if ((in != NULL) && (fclose (in) != 0)) {
    ok = false;
  }

  return ok;
}


#if (MW_CV_FS_DRIVE_F != 0U)

/*
\brief Generate one deterministic EFS stress-test file name.
\param[out] name Destination for the generated file name.
\param[in] size Size of the destination buffer in bytes.
\param[in] index File index in the complete EFS test population.
*/
static void efs_ffind_name (char *name, size_t size, uint32_t index) {
  if (index < EFS_FFIND_SHORT_FILE_COUNT) {
    /* Generate a compact 8.3-compatible name */
    (void)snprintf (name, size, "E%04u.TST", (unsigned)index);
  } else {
    /* Generate a longer name within the EFS 31-character limit */
    (void)snprintf (name, size, "EFS Long File %04u.tst",
                    (unsigned)(index - EFS_FFIND_SHORT_FILE_COUNT));
  }
}

#endif


#if (MW_CV_FS_DRIVE_M != 0U)

/*
\brief Generate one deterministic FAT search stress-test file name.
\param[out] name Destination for the generated file name.
\param[in] size Size of the destination buffer in bytes.
\param[in] index File index within one FAT test directory.
*/
static void fat_ffind_name (char *name, size_t size, uint32_t index) {
  if (index < FAT_FFIND_SHORT_FILE_COUNT) {
    /* Generate an 8.3 file name */
    (void)snprintf (name, size, "S%03u.TST", (unsigned)index);
  } else {
    /* Generate a FAT long file name */
    (void)snprintf (name, size, "Long File %03u.tst",
                    (unsigned)(index - FAT_FFIND_SHORT_FILE_COUNT));
  }
}

#endif


/*
\brief Perform one synchronized stream operation for a concurrency test.
\details Waits for the start flag, accesses the assigned path, closes the stream, and signals completion.
\param[in] argument Pointer to the WorkerContext supplied by the parent test thread.
*/
static void worker (void *argument) {
  WorkerContext *ctx = argument;
  FILE          *file;

  /* Wait until the parent has completed the shared worker setup */
  (void)osThreadFlagsWait (START_FLAG, osFlagsWaitAny, osWaitForever);

  /* Perform the selected stream operation while the peer can run concurrently */
  file = fopen (ctx->path, ctx->write ? "w" : "r");

  if (file != NULL) {
    if (ctx->write) {
      (void)fwrite (data, 1U, 8U, file);
    } else {
      (void)fgetc (file);
    }
    (void)osThreadYield();
    (void)fclose (file);
  }

  /* Notify the parent only after the stream has been released */
  (void)osThreadFlagsSet (ctx->owner, ctx->done);
}


/**
\defgroup mw_cv_fs_stress_test_funcs Stress Tests
\brief File System workflow and stress validation test functions
\details
These tests validate persistence, copying, concurrency, FAT directory searches, and name-cache workflows.
@{
*/

/**
\brief Test FileSystem persistence.
\details
\par Method
Create and verify one file across unmount/remount.
\par Expected
- The test file is created with the expected data.
- The drive reopens successfully.
- The file content matches the expected data.
*/
void MW_CV_FS_Stress_persistence_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create one deterministic file */
  MW_CV_FS_DeleteFile ("PERSIST0.TST");
  CHECK(MW_CV_FS_CreateFile ("PERSIST0.TST", data, sizeof (data)));

  /* Reopen the filesystem and verify persisted content */
  CHECK(MW_CV_FS_Reopen() == fsOK);
  CHECK(MW_CV_FS_VerifyFile ("PERSIST0.TST", data, sizeof (data)));

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("PERSIST0.TST");
}


/**
\brief Test FileSystem persistence.
\details
\par Method
Verify multiple files and, on FAT, a directory hierarchy across unmount, uninitialize, reinitialize, and remount.
\par Expected
- The test file is created with the expected data.
- The drive reopens successfully.
- The file content matches the expected data.
*/
void MW_CV_FS_Stress_persistence_1 (void) {
  char     name[24];
  uint32_t i;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create independent payloads before cycling the filesystem state */
  for (i = 0U; i < 8U; i++) {
    (void)snprintf (name, sizeof (name), "PS%02u.TST", (unsigned)i);
    MW_CV_FS_DeleteFile (name);
    CHECK(MW_CV_FS_CreateFile (name, data, sizeof (data)));
  }

  /* Reopen the volume, then verify and remove every persisted file */
  CHECK(MW_CV_FS_Reopen() == fsOK);

  for (i = 0U; i < 8U; i++) {
    (void)snprintf (name, sizeof (name), "PS%02u.TST", (unsigned)i);
    CHECK(MW_CV_FS_VerifyFile (name, data, sizeof (data)));
    MW_CV_FS_DeleteFile (name);
  }
}


/**
\brief Test FileSystem file-copy workflows.
\details
\par Method
Copy one deterministic file using bounded <tt>fread</tt>/<tt>fwrite</tt> chunks.
\par Expected
- The test file is created with the expected data.
- The source file is copied successfully.
- The file content matches the expected data.
*/
void MW_CV_FS_Stress_copy_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create and copy the deterministic source file */
  CHECK(MW_CV_FS_CreateFile ("COPY0A.TST", data, sizeof (data)));
  CHECK(copy_file ("COPY0B.TST", "COPY0A.TST"));

  /* Verify the copied payload */
  CHECK(MW_CV_FS_VerifyFile ("COPY0B.TST", data, sizeof (data)));

  /* Remove source and destination files */
  MW_CV_FS_DeleteFile ("COPY0A.TST");
  MW_CV_FS_DeleteFile ("COPY0B.TST");
}


/**
\brief Test FileSystem file-copy workflows.
\details
\par Method
Copy, delete, rename, and recreate a bounded batch while verifying every payload.
\par Expected
- The test file is created with the expected data.
- The source file is copied successfully.
- The file content matches the expected data.
- The rename operation succeeds.
- The file content matches the expected data.
*/
void MW_CV_FS_Stress_copy_1 (void) {
  char     src[16], dst[16];
  uint32_t i;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Copy, verify, rename, and reverify each deterministic payload */
  for (i = 0U; i < 8U; i++) {
    (void)snprintf (src, sizeof (src), "CS%02u.TST", (unsigned)i);
    (void)snprintf (dst, sizeof (dst), "CD%02u.TST", (unsigned)i);
    CHECK(MW_CV_FS_CreateFile (src, data, sizeof (data)));
    CHECK(copy_file (dst, src));
    CHECK(MW_CV_FS_VerifyFile (dst, data, sizeof (data)));
    MW_CV_FS_DeleteFile (src);
    CHECK(frename (dst, src) == fsOK);
    CHECK(MW_CV_FS_VerifyFile (src, data, sizeof (data)));
    MW_CV_FS_DeleteFile (src);
  }
}


/**
\brief Test concurrent FileSystem operations.
\details
\par Method
Interleave reads of distinct files from two threads.
\par Expected
- The test file is created with the expected data.
- The test file is created with the expected data.
- Both worker threads are created successfully.
- Both concurrent workers complete.
*/
void MW_CV_FS_Stress_concurrency_0 (void) {
  WorkerContext a, b;
  osThreadId_t  ta, tb;
  uint32_t      flags;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Prepare distinct inputs so the workers do not contend for one stream */
  CHECK(MW_CV_FS_CreateFile ("CON0A.TST", data, sizeof (data)));
  CHECK(MW_CV_FS_CreateFile ("CON0B.TST", data, sizeof (data)));
  a.path  = "CON0A.TST";
  a.owner = osThreadGetId();
  a.done  = 1U;
  a.write = false;
  b.path  = "CON0B.TST";
  b.owner = a.owner;
  b.done  = 2U;
  b.write = false;
  ta      = osThreadNew (worker, &a, NULL);
  tb      = osThreadNew (worker, &b, NULL);
  CHECK((ta != NULL) && (tb != NULL));

  if ((ta != NULL) && (tb != NULL)) {
    /* Start both workers together and wait for their independent completion flags */
    (void)osThreadFlagsSet (ta, START_FLAG);
    (void)osThreadFlagsSet (tb, START_FLAG);
    flags = osThreadFlagsWait (3U, osFlagsWaitAll, osKernelGetTickFreq() * 5U);
    CHECK((flags & 3U) == 3U);
    (void)osThreadTerminate (ta);
    (void)osThreadTerminate (tb);
  }

  /* Remove the worker inputs after the coordinated run */
  MW_CV_FS_DeleteFile ("CON0A.TST");
  MW_CV_FS_DeleteFile ("CON0B.TST");
}


/**
\brief Test concurrent FileSystem operations.
\details
\par Method
On FAT, interleave open/close with directory creation.
\par Expected
- The worker thread is created successfully.
- The directory creation succeeds.
- The concurrent worker completes.
*/
void MW_CV_FS_Stress_concurrency_1 (void) {
  WorkerContext ctx;
  osThreadId_t  thread;
  uint32_t      flags;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  ctx.path  = "CON1.TST";
  ctx.owner = osThreadGetId();
  ctx.done  = 1U;
  ctx.write = true;
  thread    = osThreadNew (worker, &ctx, NULL);
  CHECK(thread != NULL);

  if (thread != NULL) {
    /* Interleave the FAT directory update with the worker's stream access */
    (void)osThreadFlagsSet (thread, START_FLAG);

    if (MW_CV_FS_IsFAT()) {
      (void)frmdir ("CON1DIR", "/S");
      CHECK(fmkdir ("CON1DIR") == fsOK);
    }
    flags = osThreadFlagsWait (1U, osFlagsWaitAll, osKernelGetTickFreq() * 5U);
    CHECK((flags & 1U) != 0U);
    (void)osThreadTerminate (thread);
  }

  /* Restore common and FAT-specific filesystem state */
  MW_CV_FS_DeleteFile ("CON1.TST");

  if (MW_CV_FS_IsFAT()) {
    (void)frmdir ("CON1DIR", "/S");
  }
}


/**
\brief Test concurrent FileSystem operations.
\details
\par Method
Repeat bounded create/open/close operations from worker threads and verify all final entries.
\par Expected
- All 16 synchronized two-thread read iterations complete within the timeout and verify both file payloads.
*/
void MW_CV_FS_Stress_concurrency_2 (void) {
  uint32_t i;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Repeat the coordinated two-thread workflow */
  for (i = 0U; i < 16U; i++) {
    MW_CV_FS_Stress_concurrency_0();
  }
}


/**
\brief Test FAT name-cache behavior.
\details
\par Method
Rename an LFN to a mixed-case name, create neighboring SFN entries, and verify one unique entry per file.
\par Expected
- The test file is created with the expected data.
- The rename operation succeeds.
- The test file is created with the expected data.
- The test file is created with the expected data.
- Exactly 3 matching entries are found.
*/
void MW_CV_FS_Stress_FAT_name_cache_0 (void) {
  fsFileInfo info  = {0};
  uint32_t   count = 0U;

  if (!ready() || !MW_CV_FS_IsFAT()) {
    return;
  }

  /* Create and rename entries that exercise long- and short-name cache paths */
  MW_CV_FS_DeleteFile ("Long file name.txt");
  MW_CV_FS_DeleteFile ("MixCase.txt");
  CHECK(MW_CV_FS_CreateFile ("Long file name.txt", data, 8U));
  CHECK(frename ("Long file name.txt", "MixCase.txt") == fsOK);
  CHECK(MW_CV_FS_CreateFile ("SH1.PMM", data, 8U));
  CHECK(MW_CV_FS_CreateFile ("SH2.OBJ", data, 8U));

  /* Enumerate the directory and count only the test-owned entries */
  while (ffind ("*", &info) == fsOK) {
    if (MW_CV_FS_StrEqualIgnoreCase (info.name, "MixCase.txt") ||
        MW_CV_FS_StrEqualIgnoreCase (info.name, "SH1.PMM") ||
        MW_CV_FS_StrEqualIgnoreCase (info.name, "SH2.OBJ")) {
      count++;
    }
  }
  CHECK(count == 3U);
  MW_CV_FS_DeleteFile ("MixCase.txt");
  MW_CV_FS_DeleteFile ("SH1.PMM");
  MW_CV_FS_DeleteFile ("SH2.OBJ");
}


/**
\brief Test FAT name-cache behavior.
\details
\par Method
Build the same nested directory layout twice through find-or-create logic and verify no duplicate or inaccessible
entries.
\par Expected
- Each directory is either created or confirmed to exist already.
- Exactly 5 matching entries are found.
*/
void MW_CV_FS_Stress_FAT_name_cache_1 (void) {
  fsFileInfo  info = {0};
  uint32_t    pass, count = 0U;
  const char *dirs[] = {"CACHE\\SYSTEM", "CACHE\\TRACE", "CACHE\\CONFIG", "CACHE\\SCAN", "CACHE\\TUNE"};

  if (!ready() || !MW_CV_FS_IsFAT()) {
    return;
  }

  /* Repeat creation to exercise cache reuse and existing-entry handling */
  (void)frmdir ("CACHE", "/S");

  for (pass = 0U; pass < 2U; pass++) {
    uint32_t i;

    for (i = 0U; i < 5U; i++) {
      fsStatus s = fmkdir (dirs[i]);
      CHECK((s == fsOK) || (s == fsAlreadyExists));
    }
  }

  /* Verify that the repeated passes produced one accessible entry per path */
  while (ffind ("CACHE\\*", &info) == fsOK) {
    if (info.name[0] != '.') {
      count++;
    }
  }
  CHECK(count == 5U);
  (void)frmdir ("CACHE", "/S");
}


#if (MW_CV_FS_DRIVE_M != 0U)

/**
\brief Stress-test ffind across many FAT directories.
\details
\par Method
Create 25 directories containing 25 short 8.3 names and 25 long file names each, then find every file by its
exact path. Enumerate the directories and their files with wildcard patterns.
\par Expected
- All 25 directories are created.
- All 625 short-name and 625 long-name files are created.
- Every created file can be found individually.
- Every returned name matches the requested file name and reports a size of zero bytes.
- The <tt>FFD*</tt> wildcard enumerates all 25 test directories exactly once.
- A wildcard within each test directory enumerates all 50 contained files exactly once.
*/
void MW_CV_FS_Stress_FAT_ffind_0 (void) {
  fsFileInfo info;
  fsStatus   status;
  char       directory[8];
  char       name[24];
  char       path[32];
  uint32_t   directory_index;
  uint32_t   file_index;
  uint32_t   created  = 0U;
  uint32_t   verified = 0U;
  uint32_t   wildcard_directories = 0U;
  uint32_t   wildcard_files = 0U;
  bool       directory_found[FAT_FFIND_DIRECTORY_COUNT];
  bool       file_found[FAT_FFIND_FILE_COUNT];
  bool       complete = true;
  bool       result;

  /* Prepare the memory-card FAT drive */
  if (!ready() || !MW_CV_FS_IsFAT()) {
    return;
  }

  /* Restore the root before removing test-owned directory trees */
  CHECK(fchdir ("\\") == fsOK);
  for (directory_index = 0U; directory_index < FAT_FFIND_DIRECTORY_COUNT; directory_index++) {
    (void)snprintf (directory, sizeof (directory), "FFD%03u", (unsigned)directory_index);
    (void)fdelete (directory, "/S");
    (void)frmdir (directory, "/S");
  }

  /* Create every directory and populate it with 50 empty files */
  for (directory_index = 0U;
       (directory_index < FAT_FFIND_DIRECTORY_COUNT) && complete;
       directory_index++) {
    (void)snprintf (directory, sizeof (directory), "FFD%03u", (unsigned)directory_index);
    status = fmkdir (directory);
    CHECK(status == fsOK);

    if (status != fsOK) {
      complete = false;
      break;
    }

    for (file_index = 0U; file_index < FAT_FFIND_FILE_COUNT; file_index++) {
      fat_ffind_name (name, sizeof (name), file_index);
      (void)snprintf (path, sizeof (path), "%s\\%s", directory, name);

      result = MW_CV_FS_CreateFile (path, NULL, 0U);
      CHECK(result);
      if (!result) {
        complete = false;
        break;
      }
      created++;
    }
  }

  /* Verify every file through an independent exact-name search */
  for (directory_index = 0U;
       (directory_index < FAT_FFIND_DIRECTORY_COUNT) && complete;
       directory_index++) {
    (void)snprintf (directory, sizeof (directory), "FFD%03u", (unsigned)directory_index);

    for (file_index = 0U; file_index < FAT_FFIND_FILE_COUNT; file_index++) {
      fat_ffind_name (name, sizeof (name), file_index);
      (void)snprintf (path, sizeof (path), "%s\\%s", directory, name);

      /* Reset fileID so each call starts a new exact-name search */
      (void)memset (&info, 0, sizeof (info));
      status = ffind (path, &info);
      CHECK(status == fsOK);

      if (status != fsOK) {
        complete = false;
        break;
      }

      CHECK(MW_CV_FS_StrEqualIgnoreCase (info.name, name));
      CHECK(info.size == 0U);
      if (!MW_CV_FS_StrEqualIgnoreCase (info.name, name) || (info.size != 0U)) {
        complete = false;
        break;
      }
      verified++;
    }
  }

  /* Enumerate and verify all test-owned directories with a root wildcard */
  (void)memset (&info, 0, sizeof (info));
  (void)memset (directory_found, 0, sizeof (directory_found));
  while (complete && (ffind ("FFD*", &info) == fsOK)) {
    result = false;

    for (directory_index = 0U; directory_index < FAT_FFIND_DIRECTORY_COUNT; directory_index++) {
      (void)snprintf (directory, sizeof (directory), "FFD%03u", (unsigned)directory_index);
      if (MW_CV_FS_StrEqualIgnoreCase (info.name, directory)) {
        CHECK(!directory_found[directory_index]);
        CHECK((info.attrib & FS_FAT_ATTR_DIRECTORY) != 0U);
        if (!directory_found[directory_index] && ((info.attrib & FS_FAT_ATTR_DIRECTORY) != 0U)) {
          directory_found[directory_index] = true;
          wildcard_directories++;
        } else {
          complete = false;
        }
        result = true;
        break;
      }
    }
    CHECK(result);
    if (!result) {
      complete = false;
    }
  }

  /* Enumerate and verify all unique files within every test directory */
  for (directory_index = 0U;
       (directory_index < FAT_FFIND_DIRECTORY_COUNT) && complete;
       directory_index++) {
    (void)snprintf (directory, sizeof (directory), "FFD%03u", (unsigned)directory_index);
    (void)snprintf (path, sizeof (path), "%s\\*", directory);
    (void)memset (&info, 0, sizeof (info));
    (void)memset (file_found, 0, sizeof (file_found));

    while (ffind (path, &info) == fsOK) {
      /* Ignore the FAT current- and parent-directory entries */
      if (info.name[0] == '.') {
        continue;
      }

      result = false;

      for (file_index = 0U; file_index < FAT_FFIND_FILE_COUNT; file_index++) {
        fat_ffind_name (name, sizeof (name), file_index);
        if (MW_CV_FS_StrEqualIgnoreCase (info.name, name)) {
          CHECK(!file_found[file_index]);
          CHECK((info.attrib & FS_FAT_ATTR_DIRECTORY) == 0U);
          CHECK(info.size == 0U);
          if (!file_found[file_index] && ((info.attrib & FS_FAT_ATTR_DIRECTORY) == 0U) && (info.size == 0U)) {
            file_found[file_index] = true;
            wildcard_files++;
          } else {
            complete = false;
          }
          result = true;
          break;
        }
      }
      CHECK(result);
      if (!result || !complete) {
        complete = false;
        break;
      }
    }
  }

  /* Verify that exact and wildcard searches found the complete population */
  CHECK(created  == (FAT_FFIND_DIRECTORY_COUNT * FAT_FFIND_FILE_COUNT));
  CHECK(verified == (FAT_FFIND_DIRECTORY_COUNT * FAT_FFIND_FILE_COUNT));
  CHECK(wildcard_directories == FAT_FFIND_DIRECTORY_COUNT);
  CHECK(wildcard_files == (FAT_FFIND_DIRECTORY_COUNT * FAT_FFIND_FILE_COUNT));

  /* Remove all test-owned directory trees */
  for (directory_index = 0U; directory_index < FAT_FFIND_DIRECTORY_COUNT; directory_index++) {
    (void)snprintf (directory, sizeof (directory), "FFD%03u", (unsigned)directory_index);
    (void)frmdir (directory, "/S");
  }
}

#endif /* Drive M FAT directory search stress test */


#if (MW_CV_FS_DRIVE_F != 0U)

/**
\brief Stress-test ffind across the flat EFS namespace.
\details
\par Method
Create 128 compact names and 128 longer names without directories, then find every file by its exact name and
enumerate the complete test-owned population with a wildcard pattern.
\par Expected
- All 256 files are created in the EFS root.
- Every created file can be found individually.
- Every returned name matches the requested file name and reports a size of zero bytes.
- The all-names wildcard enumerates every test-owned name exactly once.
*/
void MW_CV_FS_Stress_EFS_ffind_0 (void) {
  fsFileInfo info;
  fsStatus   status;
  char       name[32];
  uint32_t   index;
  uint32_t   created  = 0U;
  uint32_t   verified = 0U;
  uint32_t   wildcard_verified = 0U;
  bool       wildcard_found[EFS_FFIND_FILE_COUNT];
  bool       complete = true;
  bool       result;

  /* Prepare the EFS drive */
  if (!ready() || (MW_CV_FS_GetType() != fsTypeEFS)) {
    return;
  }

  /* Remove every test-owned name left by an interrupted earlier run */
  for (index = 0U; index < EFS_FFIND_FILE_COUNT; index++) {
    efs_ffind_name (name, sizeof (name), index);
    MW_CV_FS_DeleteFile (name);
  }

  /* Create the complete flat EFS file population */
  for (index = 0U; index < EFS_FFIND_FILE_COUNT; index++) {
    efs_ffind_name (name, sizeof (name), index);
    result = MW_CV_FS_CreateFile (name, NULL, 0U);
    CHECK(result);

    if (!result) {
      complete = false;
      break;
    }
    created++;
  }

  /* Verify every file through an independent exact-name search */
  for (index = 0U; (index < EFS_FFIND_FILE_COUNT) && complete; index++) {
    efs_ffind_name (name, sizeof (name), index);

    /* Reset fileID so each call starts a new exact-name search */
    (void)memset (&info, 0, sizeof (info));
    status = ffind (name, &info);
    CHECK(status == fsOK);

    if (status != fsOK) {
      complete = false;
      break;
    }

    CHECK(MW_CV_FS_StrEqualIgnoreCase (info.name, name));
    CHECK(info.size == 0U);
    if (!MW_CV_FS_StrEqualIgnoreCase (info.name, name) || (info.size != 0U)) {
      complete = false;
      break;
    }
    verified++;
  }

  /* Enumerate and verify every unique test-owned name with a wildcard */
  (void)memset (&info, 0, sizeof (info));
  (void)memset (wildcard_found, 0, sizeof (wildcard_found));
  while (complete && (ffind ("*", &info) == fsOK)) {
    for (index = 0U; index < EFS_FFIND_FILE_COUNT; index++) {
      efs_ffind_name (name, sizeof (name), index);
      if (MW_CV_FS_StrEqualIgnoreCase (info.name, name)) {
        CHECK(!wildcard_found[index]);
        CHECK(info.size == 0U);
        if (!wildcard_found[index] && (info.size == 0U)) {
          wildcard_found[index] = true;
          wildcard_verified++;
        } else {
          complete = false;
        }
        break;
      }
    }
  }

  /* Verify that exact and wildcard searches found the complete population */
  CHECK(created  == EFS_FFIND_FILE_COUNT);
  CHECK(verified == EFS_FFIND_FILE_COUNT);
  CHECK(wildcard_verified == EFS_FFIND_FILE_COUNT);

  /* Remove the complete test-owned file population */
  for (index = 0U; index < EFS_FFIND_FILE_COUNT; index++) {
    efs_ffind_name (name, sizeof (name), index);
    MW_CV_FS_DeleteFile (name);
  }
}

#endif /* Drive F EFS search stress test */


#if (MW_CV_FS_DRIVE_M != 0U)

/**
\brief Test FAT name-cache behavior.
\details
\par Method
Create more long-name files than the configured validation cache can hold, access them in a nonsequential order,
then delete and recreate entries at different recency positions.
\par Expected
- All files remain accessible after cache replacement.
- Deleted names report <tt>fsFileNotFound</tt> before they are recreated.
- Recreated files contain the expected payload and the directory contains exactly one entry for each name.
*/
void MW_CV_FS_Stress_FAT_name_cache_2 (void) {
  static const uint8_t replace[] = {0U, 12U, 24U};
  fsFileInfo info = {0};
  char       path[40];
  uint32_t   count;
  uint32_t   i;
  uint32_t   index;

  if (!ready() || !MW_CV_FS_IsFAT()) {
    return;
  }

  /* Set root as working directory */
  fchdir("\\");

  /* Populate beyond the forty-entry cache used by the memory-card validation */
  (void)frmdir ("NCACHE2", "/S");
  CHECK(fmkdir ("NCACHE2") == fsOK);

  for (i = 0U; i < FAT_NAME_CACHE_PRESSURE_COUNT; i++) {
    (void)snprintf (path, sizeof (path), "NCACHE2\\Cache entry %02u.dat", (unsigned)i);
    CHECK(MW_CV_FS_CreateFile (path, data, 8U));
  }

  /* Mix cache hits and replacements instead of following directory order */
  for (i = 0U; i < FAT_NAME_CACHE_PRESSURE_COUNT; i++) {
    if ((i & 1U) == 0U) {
      index = i / 2U;
    }
    else {
      index = FAT_NAME_CACHE_PRESSURE_COUNT - 1U - (i / 2U);
    }
    (void)snprintf (path, sizeof (path), "NCACHE2\\Cache entry %02u.dat", (unsigned)index);
    CHECK(MW_CV_FS_VerifyFile (path, data, 8U));
  }

  /* Delete oldest, middle, and most-recent access positions, then reuse them */
  for (i = 0U; i < (sizeof (replace) / sizeof (replace[0])); i++) {
    (void)snprintf (path, sizeof (path), "NCACHE2\\Cache entry %02u.dat", (unsigned)replace[i]);
    CHECK(fdelete (path, NULL) == fsOK);
    CHECK(ffind (path, &info) == fsFileNotFound);
    CHECK(MW_CV_FS_CreateFile (path, data, 8U));
    CHECK(MW_CV_FS_VerifyFile (path, data, 8U));
  }

  count = 0U;
  while (ffind ("NCACHE2\\*", &info) == fsOK) {
    if (info.name[0] != '.') {
      count++;
    }
  }
  CHECK(count == FAT_NAME_CACHE_PRESSURE_COUNT);
  (void)frmdir ("NCACHE2", "/S");
}


/**
\brief Test FAT name-cache behavior.
\details
\par Method
Create a seven-level hierarchy, cache one file at every depth, descend using relative directory changes, and then
walk back to the root while opening the marker at every level.
\par Expected
- Every directory change succeeds across the supported cache depths.
- <tt>fpwd</tt> reports the deepest directory.
- Every depth-specific marker remains accessible while the cache depth is decremented.
*/
void MW_CV_FS_Stress_FAT_name_cache_3 (void) {
  char     marker[112];
  char     path[96];
  char     pwd[260];
  uint32_t depth;

  if (!ready() || !MW_CV_FS_IsFAT()) {
    return;
  }

  CHECK(fchdir ("\\") == fsOK);
  (void)frmdir ("NCACHE3", "/S");

  /* Prepare path: NCACHE3 */
  (void)snprintf (path, sizeof (path), "NCACHE3");

  for (depth = 0U; depth < 7U; depth++) {
    if (depth != 0U) {
      /* Extend path: NCACHE3\L1 .. NCACHE3\L6 */
      (void)snprintf (&path[strlen (path)], sizeof (path) - strlen (path), "\\L%u", (unsigned)depth);
    }
    /* Create directory using prepared path */
    CHECK(fmkdir (path) == fsOK);

    /* Create marker file: MARK00.TST .. MARK06.TST */
    (void)snprintf (marker, sizeof (marker), "%s\\MARK%02u.TST", path, (unsigned)depth);
    CHECK(MW_CV_FS_CreateFile (marker, data, 8U));
  }

  /* Establish and advance the cache's current-directory depth */
  CHECK(fchdir ("NCACHE3") == fsOK);
  for (depth = 1U; depth < 7U; depth++) {
    /* Change to the next level directory: L1 .. L6 */
    (void)snprintf (path, sizeof (path), "L%u", (unsigned)depth);
    CHECK(fchdir (path) == fsOK);
  }
  /* Verify current working directory */
  CHECK(fpwd (MW_CV_FS_DRIVE, pwd, sizeof (pwd)) == fsOK);
  CHECK(strstr (pwd, "L6") != NULL);

  /* Revisit one cached entry at each depth while walking back toward root */
  for (depth = 7U; depth > 0U; depth--) {
    (void)snprintf (marker, sizeof (marker), "MARK%02u.TST", (unsigned)(depth - 1U));
    CHECK(MW_CV_FS_VerifyFile (marker, data, 8U));
    CHECK(fchdir ("..") == fsOK);
  }

  CHECK(fchdir ("\\") == fsOK);
  (void)frmdir ("NCACHE3", "/S");
}


/**
\brief Test FAT name-cache behavior.
\details
\par Method
Populate the cache past capacity, reinitialize and remount the volume, rebuild the cache in reverse order, and then
rename half of the cached files while deleting the rest.
\par Expected
- All persisted files are found after reinitialization.
- Every rename and delete succeeds against the rebuilt cache.
- Renamed files retain their payload and deleted names remain absent.
*/
void MW_CV_FS_Stress_FAT_name_cache_4 (void) {
  fsFileInfo info = {0};
  char       name[24];
  char       newname[24];
  int32_t    index;

  if (!ready() || !MW_CV_FS_IsFAT()) {
    return;
  }

  (void)frmdir ("NCACHE4", "/S");
  CHECK(fmkdir ("NCACHE4") == fsOK);

  for (index = 0; index < (int32_t)FAT_NAME_CACHE_PRESSURE_COUNT; index++) {
    (void)snprintf (name, sizeof (name), "NCACHE4\\Original %02d.bin", (int)index);
    CHECK(MW_CV_FS_CreateFile (name, data, 8U));
  }

  /* Reinitialization discards all in-memory links without changing media */
  CHECK(MW_CV_FS_Reopen() == fsOK);

  for (index = (int32_t)FAT_NAME_CACHE_PRESSURE_COUNT - 1; index >= 0; index--) {
    (void)snprintf (name, sizeof (name), "NCACHE4\\Original %02d.bin", (int)index);
    CHECK(MW_CV_FS_VerifyFile (name, data, 8U));
  }

  for (index = 0; index < (int32_t)FAT_NAME_CACHE_PRESSURE_COUNT; index++) {
    (void)snprintf (name, sizeof (name), "NCACHE4\\Original %02d.bin", (int)index);

    if ((index & 1) == 0) {
      /* Prepare name: "Renamed XX.bin" */
      (void)snprintf (newname, sizeof (newname), "Renamed %02d.bin", (int)index);
      /* Pass only the replacement entry name to frename */
      CHECK(frename (name, newname) == fsOK);
      CHECK(ffind (name, &info) == fsFileNotFound);
      (void)snprintf (name, sizeof (name), "NCACHE4\\%s", newname);
      CHECK(MW_CV_FS_VerifyFile (name, data, 8U));
    }
    else {
      CHECK(fdelete (name, NULL) == fsOK);
      CHECK(ffind (name, &info) == fsFileNotFound);
    }
  }

  (void)frmdir ("NCACHE4", "/S");
}

#endif /* Drive M name-cache pressure tests */


/**
@}
*/

#endif /* Stress validation */
