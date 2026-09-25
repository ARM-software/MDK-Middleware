/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - File System - C Library
 * Copyright (c) 2018-2026 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_FS_Clib.c
 * Purpose: MDK Middleware - Component Validation - File System - C Library -
 *          tests module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"

#if ((MW_CV_FS != 0U) && (MW_CV_FS_CLIB != 0U))

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "retarget_fs.h"
#include "FS_Config.h"
#include "MW_CV_TestReport.h"
#include "MW_CV_FS.h"
#include "MW_CV_FS_Clib.h"

#define CHECK(expr) ASSERT_TRUE((expr), "%s", #expr)

#if (MW_CV_FS_DRIVE_F != 0U)
#define CV_MAX_OPEN_FILES EFS_MAX_OPEN_FILES
#else
#define CV_MAX_OPEN_FILES FAT_MAX_OPEN_FILES
#endif

static uint8_t pattern[513];

/*
\brief Prepare the configured drive and the shared C library test pattern.
\return true when drive preparation succeeds; otherwise false.
*/
static bool ready (void) {
  fsStatus status;

  /* Prepare the configured drive */
  status = MW_CV_FS_EnsureReady();
  CHECK(status == fsOK);

  /* Fill the shared deterministic test pattern */
  MW_CV_FS_FillPattern (pattern, sizeof (pattern), 0U);

  /* Report whether the test may continue */
  return (status == fsOK);
}


/*
\brief Replace a file with a prefix of the shared deterministic pattern.
\param[in] name File name to create or replace.
\param[in] size Number of pattern bytes to write.
*/
static void create (const char *name, size_t size) {
  /* Remove stale state and create the requested pattern prefix */
  MW_CV_FS_DeleteFile (name);
  CHECK(MW_CV_FS_CreateFile (name, pattern, size));
}


/*
\brief Close an acquired stream and report any close failure.
\param[in] file Stream to close, or NULL when no stream was acquired.
*/
static void close_checked (FILE *file) {
  /* Close and verify an acquired stream */
  if (file != NULL) {
    CHECK(fclose (file) == 0);
  }
}


/**
\defgroup mw_cv_fs_clib_test_funcs Standard C Library
\brief C library stream validation test functions
\details
These tests validate the C library stream interface on FAT and EFS backends.
@{
*/

/**
\brief Test the fopen function.
\details
\par Method
1. Remove <tt>OPEN0.TST</tt> if it exists.
2. Open it with mode <tt>w</tt>; verify that the returned stream is not <tt>NULL</tt>.
3. Close the stream.
\par Expected
- Mode <tt>w</tt> creates a missing file and returns a valid stream.
*/
void MW_CV_FS_Clib_fopen_0 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Remove stale file state */
  MW_CV_FS_DeleteFile ("OPEN0.TST");

  /* Create the file through a write stream */
  file = fopen ("OPEN0.TST", "w");
  CHECK(file != NULL);
  close_checked (file);

  /* Remove the created file */
  MW_CV_FS_DeleteFile ("OPEN0.TST");
}


/**
\brief Test the fopen function.
\details
\par Method
1. Create <tt>OPEN1.TST</tt> with known content and close it.
2. Open it with mode <tt>r</tt>.
\par Expected
- The existing file opens successfully and its content is unchanged.
*/
void MW_CV_FS_Clib_fopen_1 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a file with deterministic content */
  create ("OPEN1.TST", 4U);

  /* Open and close the existing file for reading */
  file = fopen ("OPEN1.TST", "r");
  CHECK(file != NULL);
  close_checked (file);

  /* Verify that opening did not alter the content */
  CHECK(MW_CV_FS_VerifyFile ("OPEN1.TST", pattern, 4U));

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("OPEN1.TST");
}


/**
\brief Test the fopen function.
\details
\par Method
1. Create <tt>OPEN2.TST</tt> with known data and close it.
2. Reopen it with mode <tt>w</tt> and close it without writing.
3. Reopen it with mode <tt>r</tt> and attempt to read one byte.
\par Expected
- Reopening an existing file with mode <tt>w</tt> truncates it.
- Reading from the truncated file returns no data and indicates end-of-file.
*/
void MW_CV_FS_Clib_fopen_2 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create content, then reopen with write mode to verify truncation */
  create ("OPEN2.TST", 4U);

  file = fopen ("OPEN2.TST", "w");
  CHECK(file != NULL);
  close_checked (file);

  file = fopen ("OPEN2.TST", "r");
  CHECK(file != NULL);

  if (file != NULL) {
    CHECK(fgetc (file) == EOF);
  }
  close_checked (file);

  MW_CV_FS_DeleteFile ("OPEN2.TST");
}


/**
\brief Test the fopen function.
\details
\par Method
1. Remove <tt>OPEN3.TST</tt> if it exists.
2. Open it with mode <tt>a</tt>, write block A, and close it.
3. Reopen it with mode <tt>a</tt>, write block B, and close it.
4. Reopen it with mode <tt>r</tt> and read the complete content.
\par Expected
- Mode <tt>a</tt> creates a missing file.
- The final content is block A followed by block B; existing data is not overwritten.
*/
void MW_CV_FS_Clib_fopen_3 (void) {
  FILE   *file;
  uint8_t expected[8];

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  (void)memcpy (expected, pattern, 4U);
  (void)memcpy (&expected[4], &pattern[8], 4U);

  /* Append in separate sessions to verify that existing data is preserved */
  MW_CV_FS_DeleteFile ("OPEN3.TST");
  file = fopen ("OPEN3.TST", "a");
  CHECK(file != NULL);

  if (file != NULL) {
    CHECK(fwrite (pattern, 1U, 4U, file) == 4U);
  }
  close_checked (file);

  file = fopen ("OPEN3.TST", "a");
  CHECK(file != NULL);

  if (file != NULL) {
    CHECK(fwrite (&pattern[8], 1U, 4U, file) == 4U);
  }
  close_checked (file);

  CHECK(MW_CV_FS_VerifyFile ("OPEN3.TST", expected, sizeof (expected)));
  MW_CV_FS_DeleteFile ("OPEN3.TST");
}


/**
\brief Test the fopen function.
\details
\par Method
1. Create and write a file through an absolute drive-prefixed path with mode <tt>wb</tt>, including <tt>0x00</tt>,
<tt>0x0A</tt>, <tt>0x0D</tt>, <tt>0x80</tt>, and <tt>0xFF</tt>.
2. Reopen the same file through a relative path on the current drive with mode <tt>rb</tt> and verify the bytes exactly.
\par Expected
- Modes <tt>wb</tt> and <tt>rb</tt> succeed and preserve every byte.
- Absolute and current-drive-relative paths identify the same file.
*/
void MW_CV_FS_Clib_fopen_4 (void) {
  char          path[32];
  FILE         *file;
  const uint8_t binary[] = {0x00U, 0x0AU, 0x0DU, 0x80U, 0xFFU};

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Write text-sensitive values through an absolute binary-stream path */
  MW_CV_FS_MakePath (path, sizeof (path), "OPEN4.TST");
  MW_CV_FS_DeleteFile (path);

  file = fopen (path, "wb");
  CHECK(file != NULL);

  if (file != NULL) {
    CHECK(fwrite (binary, 1U, sizeof (binary), file) == sizeof (binary));
  }
  close_checked (file);

  CHECK(MW_CV_FS_VerifyFile ("OPEN4.TST", binary, sizeof (binary)));

  MW_CV_FS_DeleteFile (path);
}


/**
\brief Test the fopen function.
\details
\par Method
1. Create two files with distinct deterministic content.
2. Open both files for reading and also open a second read handle to one of them.
3. Interleave reads and close the streams in a different order from the open order.
\par Expected
- Every permitted read handle opens successfully and maintains an independent position.
- Each stream returns only its file's expected content.
*/
void MW_CV_FS_Clib_fopen_5 (void) {
  FILE *a, *b, *a2;
  int   ca, cb, ca2;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  create ("OPEN5A.TST", 8U);
  create ("OPEN5B.TST", 8U);

  /* Open independent and duplicate paths to verify per-stream positions */
  a  = fopen ("OPEN5A.TST", "r");
  b  = fopen ("OPEN5B.TST", "r");
  a2 = fopen ("OPEN5A.TST", "r");
  CHECK((a != NULL) && (b != NULL) && (a2 != NULL));

  if ((a != NULL) && (b != NULL) && (a2 != NULL)) {
    ca  = fgetc (a);
    cb  = fgetc (b);
    ca2 = fgetc (a2);
    CHECK(ca == pattern[0]);
    CHECK(cb == pattern[0]);
    CHECK(ca2 == pattern[0]);
  }
  close_checked (b);
  close_checked (a2);
  close_checked (a);
  MW_CV_FS_DeleteFile ("OPEN5A.TST");
  MW_CV_FS_DeleteFile ("OPEN5B.TST");
}


/**
\brief Test the fopen function.
\details
\par Method
1. Repeatedly create or open, optionally read or write one byte, and close the same test file for a bounded iteration
count.
2. Reopen the file once more and verify its final content.
\par Expected
- Every cycle succeeds without progressively losing handles.
- The final open and content verification succeed.
*/
void MW_CV_FS_Clib_fopen_6 (void) {
  FILE    *file;
  uint32_t i;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Cycle one stream repeatedly before confirming the file remains accessible */
  MW_CV_FS_DeleteFile ("OPEN6.TST");

  for (i = 0U; i < 64U; i++) {
    file = fopen ("OPEN6.TST", "w");
    CHECK(file != NULL);

    if (file == NULL) {
      break;
    }
    CHECK(fputc ((int)i, file) == (int)i);
    CHECK(fclose (file) == 0);
  }

  file = fopen ("OPEN6.TST", "r");
  CHECK(file != NULL);
  close_checked (file);

  MW_CV_FS_DeleteFile ("OPEN6.TST");
}


/**
\brief Test the fopen function.
\details
\par Method
1. Derive the configured maximum number of simultaneously open files for the active backend.
2. Open that many distinct files for reading and keep every stream open.
3. Attempt one additional open, then close one stream and retry.
\par Expected
- Opens through the configured limit succeed.
- The extra open fails with <tt>RT_ERR_MAXFILES</tt> and does not invalidate existing streams.
- An open succeeds again after one stream is closed.
*/
void MW_CV_FS_Clib_fopen_7 (void) {
  FILE    *file[CV_MAX_OPEN_FILES + 1U];
  char     name[16];
  uint32_t i;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  (void)memset (file, 0, sizeof (file));

  for (i = 0U; i <= CV_MAX_OPEN_FILES; i++) {
    /* Prepare name: OPXX.TST */
    (void)snprintf (name, sizeof (name), "OP%02u.TST", (unsigned)i);
    /* Create the file with prepared name*/
    create (name, 1U);
  }

  /* Fill the stream table and verify that one additional open is rejected */
  for (i = 0U; i < CV_MAX_OPEN_FILES; i++) {
    /* Prepare name: OPXX.TST */
    (void)snprintf (name, sizeof (name), "OP%02u.TST", (unsigned)i);
    /* Check that the file can be opened */
    file[i] = fopen (name, "r");
    CHECK(file[i] != NULL);
  }
  /* Attempt one additional open beyond the limit */
  (void)snprintf (name, sizeof (name), "OP%02u.TST", (unsigned)CV_MAX_OPEN_FILES);
  errno = 0;
  file[CV_MAX_OPEN_FILES] = fopen (name, "r");
  CHECK(file[CV_MAX_OPEN_FILES] == NULL);
  CHECK(errno == RT_ERR_MAXFILES);

  /* Release one slot, prove it is reusable, then close every remaining stream */
  close_checked (file[0]);
  file[0] = fopen (name, "r");
  CHECK(file[0] != NULL);

  for (i = 0U; i < CV_MAX_OPEN_FILES; i++) {
    close_checked (file[i]);
  }

  for (i = 0U; i <= CV_MAX_OPEN_FILES; i++) {
    (void)snprintf (name, sizeof (name), "OP%02u.TST", (unsigned)i);
    MW_CV_FS_DeleteFile (name);
  }
}


/**
\brief Test the fopen function.
\details
\par Method
1. Remove <tt>OPEN8.TST</tt> if it exists.
2. Attempt to open it with mode <tt>r</tt>.
\par Expected
- <tt>fopen</tt> returns <tt>NULL</tt>.
- <tt>errno</tt> is set to <tt>RT_ERR_NOTFOUND</tt>.
- The failed request does not create the file.
*/
void MW_CV_FS_Clib_fopen_8 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Ensure the requested file is absent */
  MW_CV_FS_DeleteFile ("OPEN8.TST");

  /* Attempt a read-only open and verify errno */
  errno = 0;
  file  = fopen ("OPEN8.TST", "r");
  CHECK(file == NULL);
  CHECK(errno == RT_ERR_NOTFOUND);
}


/**
\brief Test the fopen function.
\details
\par Method
1. Attempt to open a file on a nonexistent drive with modes <tt>rb</tt> and <tt>wb</tt> in independent operations.
\par Expected
- Each request returns <tt>NULL</tt> and sets <tt>errno</tt> to <tt>RT_ERR_NOTFOUND</tt>.
- The failed requests do not alter files on the configured drive.
*/
void MW_CV_FS_Clib_fopen_9 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Attempt a read from an invalid drive */
  errno = 0;
  file  = fopen (MW_CV_FS_INVALID_DRIVE "OPEN9.TST", "r");
  CHECK(file == NULL);
  CHECK(errno == RT_ERR_NOTFOUND);

  /* Attempt a write to an invalid drive */
  errno = 0;
  file  = fopen (MW_CV_FS_INVALID_DRIVE "OPEN9.TST", "w");
  CHECK(file == NULL);
  CHECK(errno == RT_ERR_NOTFOUND);
}


/**
\brief Test the fclose function.
\details
\par Method
1. Open a new empty <tt>CLOSE0.TST</tt> with mode <tt>w</tt>.
2. Call <tt>fclose</tt> immediately.
\par Expected
- <tt>fclose</tt> returns <tt>0</tt> for the valid stream.
*/
void MW_CV_FS_Clib_fclose_0 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create an empty write stream */
  file = fopen ("CLOSE0.TST", "w");
  CHECK(file != NULL);

  /* Close and verify the acquired stream */
  if (file != NULL) {
    CHECK(fclose (file) == 0);
  }

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("CLOSE0.TST");
}


/**
\brief Test the fclose function.
\details
\par Method
1. Open <tt>CLOSE1.TST</tt> with mode <tt>w</tt> and write known data.
2. Call <tt>fclose</tt>.
3. Reopen the file with mode <tt>r</tt> and verify the complete content.
\par Expected
- <tt>fclose</tt> returns <tt>0</tt>.
- Buffered data is flushed and persists after the stream is closed.
*/
void MW_CV_FS_Clib_fclose_1 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Open a file and write buffered content */
  file = fopen ("CLOSE1.TST", "w");
  CHECK(file != NULL);

  if (file != NULL) {
    CHECK(fwrite (pattern, 1U, 8U, file) == 8U);
    CHECK(fclose (file) == 0);
  }

  /* Verify that close persisted the complete payload */
  CHECK(MW_CV_FS_VerifyFile ("CLOSE1.TST", pattern, 8U));

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("CLOSE1.TST");
}


/**
\brief Test the fclose function.
\details
\par Method
1. Open a nonempty file with mode <tt>r</tt> and read only part of its content.
2. Close the stream, reopen the file, and verify the complete content.
\par Expected
- Closing the partially consumed read stream returns <tt>0</tt>.
- Closing the read stream does not alter file content.
*/
void MW_CV_FS_Clib_fclose_2 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create and partially consume a read stream */
  create ("CLOSE2.TST", 8U);
  file = fopen ("CLOSE2.TST", "r");
  CHECK(file != NULL);

  if (file != NULL) {
    CHECK(fgetc (file) == pattern[0]);
    CHECK(fclose (file) == 0);
  }

  /* Verify that close did not alter the file */
  CHECK(MW_CV_FS_VerifyFile ("CLOSE2.TST", pattern, 8U));

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("CLOSE2.TST");
}


/**
\brief Test the fwrite function.
\details
\par Method
1. Open <tt>WRITE0.TST</tt> with mode <tt>wb</tt>.
2. Write one byte using <tt>fwrite</tt> with <tt>size == 1</tt> and <tt>nmemb == 1</tt>.
3. Close and reopen the file with mode <tt>rb</tt> and verify the byte.
\par Expected
- <tt>fwrite</tt> returns <tt>1</tt> and the byte persists.
*/
void MW_CV_FS_Clib_fwrite_0 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Open a binary write stream */
  file = fopen ("WRITE0.TST", "wb");
  CHECK(file != NULL);

  if (file != NULL) {
    CHECK(fwrite (pattern, 1U, 1U, file) == 1U);
    close_checked (file);
  }

  /* Verify the persisted byte */
  CHECK(MW_CV_FS_VerifyFile ("WRITE0.TST", pattern, 1U));

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("WRITE0.TST");
}


/**
\brief Test the fwrite function.
\details
\par Method
1. Open <tt>WRITE1.TST</tt> with mode <tt>wb</tt>.
2. Write three distinct blocks with separate <tt>fwrite</tt> calls.
3. Close, reopen with mode <tt>rb</tt>, and read the complete file.
\par Expected
- Each call returns its requested element count.
- The file contains the three blocks in call order with no gaps or overlap.
*/
void MW_CV_FS_Clib_fwrite_1 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Write unequal chunks and verify their concatenated byte sequence */
  file = fopen ("WRITE1.TST", "wb");
  CHECK(file != NULL);

  if (file != NULL) {
    CHECK(fwrite (pattern, 1U, 3U, file) == 3U);
    CHECK(fwrite (&pattern[3], 1U, 4U, file) == 4U);
    CHECK(fwrite (&pattern[7], 1U, 5U, file) == 5U);
    close_checked (file);
  }

  CHECK(MW_CV_FS_VerifyFile ("WRITE1.TST", pattern, 12U));
  MW_CV_FS_DeleteFile ("WRITE1.TST");
}


/**
\brief Test the fwrite function.
\details
\par Method
1. Write deterministic data using element sizes and counts that total <tt>7</tt>, <tt>511</tt>, <tt>512</tt>, and
<tt>513</tt> bytes across independent iterations or files.
2. Close and reopen each file with mode <tt>rb</tt>.
3. Read back and compare every byte.
\par Expected
- <tt>fwrite</tt> returns the requested number of complete elements for each write.
- File size and content exactly match the requested data, including the 513-byte sector-crossing transfer.
*/
void MW_CV_FS_Clib_fwrite_2 (void) {
  const size_t sizes[] = {7U, 511U, 512U, 513U};
  FILE        *file;
  uint32_t     i;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Exercise writes immediately below, at, and above a sector-sized boundary */
  for (i = 0U; i < 4U; i++) {
    file = fopen ("WRITE2.TST", "wb");
    CHECK(file != NULL);

    if (file != NULL) {
      CHECK(fwrite (pattern, sizes[i], 1U, file) == 1U);
      close_checked (file);
    }

    CHECK(MW_CV_FS_VerifyFile ("WRITE2.TST", pattern, sizes[i]));
  }

  MW_CV_FS_DeleteFile ("WRITE2.TST");
}


/**
\brief Test the fwrite function.
\details
\par Method
1. Open <tt>WRITE3.TST</tt> with mode <tt>wb</tt>.
2. Call <tt>fwrite</tt> once with <tt>size == 0</tt> and once with <tt>nmemb == 0</tt> using an otherwise valid buffer
and stream.
3. Close and reopen the file.
\par Expected
- Both calls return <tt>0</tt>.
- Neither call changes the file position or file content.
*/
void MW_CV_FS_Clib_fwrite_3 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Verify that zero-sized writes neither fail nor extend the stream */
  file = fopen ("WRITE3.TST", "wb");
  CHECK(file != NULL);

  if (file != NULL) {
    CHECK(fwrite (pattern, 0U, 4U, file) == 0U);
    CHECK(fwrite (pattern, 4U, 0U, file) == 0U);
    close_checked (file);
  }

  CHECK(MW_CV_FS_VerifyFile ("WRITE3.TST", pattern, 0U));
  MW_CV_FS_DeleteFile ("WRITE3.TST");
}


/**
\brief Test the fwrite function.
\details
\par Method
1. Open one file with mode <tt>w</tt> and another existing file with mode <tt>a</tt>.
2. Interleave bounded writes of distinct patterns to the two streams.
3. Close both, reopen for reading, and verify both complete payloads.
\par Expected
- File positions and buffered data remain isolated between streams.
- The write-mode file contains only its new pattern, while the append-mode file retains its prefix followed by the
appended pattern.
*/
void MW_CV_FS_Clib_fwrite_4 (void) {
  FILE   *a, *b;
  uint8_t expected[8];

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  (void)memcpy (expected, pattern, 4U);
  (void)memcpy (&expected[4], &pattern[8], 4U);

  /* Interleave independent writes before verifying both resulting payloads */
  create ("WRITE4B.TST", 4U);
  a = fopen ("WRITE4A.TST", "w");
  b = fopen ("WRITE4B.TST", "a");
  CHECK((a != NULL) && (b != NULL));

  if ((a != NULL) && (b != NULL)) {
    CHECK(fwrite (&pattern[8], 1U, 4U, a) == 4U);
    CHECK(fwrite (&pattern[8], 1U, 4U, b) == 4U);
  }

  close_checked (a);
  close_checked (b);

  CHECK(MW_CV_FS_VerifyFile ("WRITE4A.TST", &pattern[8], 4U));
  CHECK(MW_CV_FS_VerifyFile ("WRITE4B.TST", expected, 8U));
  MW_CV_FS_DeleteFile ("WRITE4A.TST");
  MW_CV_FS_DeleteFile ("WRITE4B.TST");
}


/**
\brief Test the fread function.
\details
\par Method
1. Create <tt>READ0.TST</tt> containing one known byte.
2. Open it with mode <tt>rb</tt>.
3. Read one byte using <tt>fread</tt> with <tt>size == 1</tt> and <tt>nmemb == 1</tt>.
\par Expected
- <tt>fread</tt> returns <tt>1</tt> and the returned byte matches the source.
*/
void MW_CV_FS_Clib_fread_0 (void) {
  FILE   *file;
  uint8_t value = 0U;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create and open a one-byte file */
  create ("READ0.TST", 1U);
  file = fopen ("READ0.TST", "rb");
  CHECK(file != NULL);

  /* Read and verify the stored byte */
  if (file != NULL) {
    CHECK(fread (&value, 1U, 1U, file) == 1U);
    CHECK(value == pattern[0]);
  }

  /* Close the stream and remove the file */
  close_checked (file);
  MW_CV_FS_DeleteFile ("READ0.TST");
}


/**
\brief Test the fread function.
\details
\par Method
1. Create <tt>READ1.TST</tt> with three distinct blocks and open it with mode <tt>rb</tt>.
2. Read the blocks with three separate <tt>fread</tt> calls.
\par Expected
- Each call returns the requested element count.
- The returned blocks match the file in order with no gaps or overlap.
*/
void MW_CV_FS_Clib_fread_1 (void) {
  FILE   *file;
  uint8_t buf[12];

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Read unequal chunks into adjacent regions and verify the assembled buffer */
  create ("READ1.TST", 12U);
  file = fopen ("READ1.TST", "rb");
  CHECK(file != NULL);

  if (file != NULL) {
    CHECK(fread (buf, 1U, 3U, file) == 3U);
    CHECK(fread (&buf[3], 1U, 4U, file) == 4U);
    CHECK(fread (&buf[7], 1U, 5U, file) == 5U);
    CHECK(memcmp (buf, pattern, 12U) == 0);
  }

  close_checked (file);
  MW_CV_FS_DeleteFile ("READ1.TST");
}


/**
\brief Test the fread function.
\details
\par Method
1. Create files containing deterministic data of <tt>7</tt>, <tt>511</tt>, <tt>512</tt>, and <tt>513</tt> bytes.
2. Read each file using multiple element sizes and counts.
3. Include a transfer that crosses the 512-byte boundary.
\par Expected
- <tt>fread</tt> returns the number of complete elements read.
- Every returned byte matches the source data.
*/
void MW_CV_FS_Clib_fread_2 (void) {
  const size_t sizes[] = {7U, 511U, 512U, 513U};
  uint8_t      buf[513];
  FILE        *file;
  uint32_t     i;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Exercise reads immediately below, at, and above a sector-sized boundary */
  for (i = 0U; i < 4U; i++) {
    create ("READ2.TST", sizes[i]);
    file = fopen ("READ2.TST", "rb");
    CHECK(file != NULL);

    if (file != NULL) {
      CHECK(fread (buf, sizes[i], 1U, file) == 1U);
      CHECK(memcmp (buf, pattern, sizes[i]) == 0);
    }
    close_checked (file);
  }

  MW_CV_FS_DeleteFile ("READ2.TST");
}


/**
\brief Test the fread function.
\details
\par Method
1. Create <tt>READ3.TST</tt> with 10 known bytes and open it with mode <tt>rb</tt>.
2. Request more than 10 elements with <tt>size == 1</tt>.
3. Call <tt>fread</tt> again after the end of the file.
\par Expected
- The first call returns <tt>10</tt> and the ten bytes are correct.
- The next call returns <tt>0</tt>.
- <tt>feof</tt> reports a set end-of-file indicator and <tt>ferror</tt> remains clear.
*/
void MW_CV_FS_Clib_fread_3 (void) {
  FILE   *file;
  uint8_t buf[16];

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Request beyond the remaining data and verify the short count and EOF state */
  create ("READ3.TST", 10U);
  file = fopen ("READ3.TST", "rb");
  CHECK(file != NULL);

  if (file != NULL) {
    CHECK(fread (buf, 1U, sizeof (buf), file) == 10U);
    CHECK(memcmp (buf, pattern, 10U) == 0);
    CHECK(fread (buf, 1U, 1U, file) == 0U);
    CHECK(feof (file) != 0);
    CHECK(ferror (file) == 0);
  }
  close_checked (file);
  MW_CV_FS_DeleteFile ("READ3.TST");
}


/**
\brief Test the fread function.
\details
\par Method
1. Open a nonempty file with mode <tt>rb</tt>.
2. Call <tt>fread</tt> once with <tt>size == 0</tt> and once with <tt>nmemb == 0</tt> using an otherwise valid buffer
and stream.
3. Perform a normal read immediately afterward.
\par Expected
- Both zero-length calls return <tt>0</tt> without setting end-of-file or error indicators.
- The subsequent normal read starts at the beginning of the file, proving the position did not change.
*/
void MW_CV_FS_Clib_fread_4 (void) {
  FILE   *file;
  uint8_t value = 0U;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Verify that zero-sized reads preserve the stream position and destination */
  create ("READ4.TST", 4U);
  file = fopen ("READ4.TST", "rb");
  CHECK(file != NULL);

  if (file != NULL) {
    CHECK(fread (&value, 0U, 4U, file) == 0U);
    CHECK(fread (&value, 4U, 0U, file) == 0U);
    CHECK(feof (file) == 0);
    CHECK(ferror (file) == 0);
    CHECK(fread (&value, 1U, 1U, file) == 1U);
    CHECK(value == pattern[0]);
  }
  close_checked (file);
  MW_CV_FS_DeleteFile ("READ4.TST");
}


/**
\brief Test the fgetc function.
\details
\par Method
1. Create <tt>GETC0.TST</tt> containing one known byte.
2. Open it with mode <tt>rb</tt> and call <tt>fgetc</tt> once.
\par Expected
- <tt>fgetc</tt> returns the stored byte converted to <tt>unsigned char</tt> and then to <tt>int</tt>.
*/
void MW_CV_FS_Clib_fgetc_0 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create and open a one-byte file */
  create ("GETC0.TST", 1U);
  file = fopen ("GETC0.TST", "rb");
  CHECK(file != NULL);

  /* Read and verify the stored byte */
  if (file != NULL) {
    CHECK(fgetc (file) == pattern[0]);
  }
  /* Close the stream and remove the file */
  close_checked (file);
  MW_CV_FS_DeleteFile ("GETC0.TST");
}


/**
\brief Test the fgetc function.
\details
\par Method
1. Create <tt>GETC1.TST</tt> containing <tt>0x00</tt>, <tt>0x01</tt>, <tt>0x7F</tt>, <tt>0x80</tt>, and <tt>0xFF</tt>.
2. Open it with mode <tt>rb</tt> and call <tt>fgetc</tt> once per byte.
\par Expected
- Each call returns the next byte in sequence.
- Values <tt>0x80</tt> and <tt>0xFF</tt> are returned as data and are not confused with <tt>EOF</tt>.
*/
void MW_CV_FS_Clib_fgetc_1 (void) {
  const uint8_t data[] = {0x00U, 0x01U, 0x7FU, 0x80U, 0xFFU};
  FILE         *file;
  uint32_t      i;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create and open the representative byte sequence */
  MW_CV_FS_DeleteFile ("GETC1.TST");
  CHECK(MW_CV_FS_CreateFile ("GETC1.TST", data, sizeof (data)));
  file = fopen ("GETC1.TST", "rb");
  CHECK(file != NULL);

  /* Read and verify every unsigned byte value */
  if (file != NULL) {
    for (i = 0U; i < sizeof (data); i++) {
      CHECK(fgetc (file) == data[i]);
    }
  }
  /* Close the stream and remove the file */
  close_checked (file);
  MW_CV_FS_DeleteFile ("GETC1.TST");
}


/**
\brief Test the fgetc function.
\details
\par Method
1. Create a one-byte file and open it with mode <tt>rb</tt>.
2. Read the byte, then call <tt>fgetc</tt> twice more.
\par Expected
- The first call returns the stored byte.
- Calls at and after end-of-file return <tt>EOF</tt>.
- <tt>feof</tt> is set and <tt>ferror</tt> remains clear.
*/
void MW_CV_FS_Clib_fgetc_2 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create and open a one-byte file */
  create ("GETC2.TST", 1U);
  file = fopen ("GETC2.TST", "rb");
  CHECK(file != NULL);

  /* Read through end-of-file and verify stream indicators */
  if (file != NULL) {
    CHECK(fgetc (file) == pattern[0]);
    CHECK(fgetc (file) == EOF);
    CHECK(fgetc (file) == EOF);
    CHECK(feof (file) != 0);
    CHECK(ferror (file) == 0);
  }
  /* Close the stream and remove the file */
  close_checked (file);
  MW_CV_FS_DeleteFile ("GETC2.TST");
}


/**
\brief Test the fseek function.
\details
\par Method
1. Create <tt>SEEK0.TST</tt> with known content and open it with mode <tt>rb</tt>.
2. Read one byte.
3. Seek to offset <tt>0</tt> using <tt>SEEK_SET</tt> and read one byte again.
\par Expected
- <tt>fseek</tt> returns <tt>0</tt> and the following read returns the first byte.
*/
void MW_CV_FS_Clib_fseek_0 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create and open a known file */
  create ("SEEK0.TST", 8U);
  file = fopen ("SEEK0.TST", "rb");
  CHECK(file != NULL);

  /* Read, rewind, and verify the restored position */
  if (file != NULL) {
    CHECK(fgetc (file) == pattern[0]);
    CHECK(fseek (file, 0L, SEEK_SET) == 0);
    CHECK(fgetc (file) == pattern[0]);
  }
  /* Close the stream and remove the file */
  close_checked (file);
  MW_CV_FS_DeleteFile ("SEEK0.TST");
}


/**
\brief Test the fseek function.
\details
\par Method
1. Seek to offsets <tt>1</tt>, a middle position, and the last valid byte in a known file.
2. Read one byte after each seek.
\par Expected
- Every valid <tt>fseek</tt> returns <tt>0</tt>.
- Each following read returns the byte at the requested absolute offset.
*/
void MW_CV_FS_Clib_fseek_1 (void) {
  const long offsets[] = {1L, 7L, 15L};
  FILE      *file;
  uint32_t   i;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Seek to representative absolute offsets and verify each resulting byte */
  create ("SEEK1.TST", 16U);
  file = fopen ("SEEK1.TST", "rb");
  CHECK(file != NULL);

  if (file != NULL) {
    for (i = 0U; i < 3U; i++) {
      CHECK(fseek (file, offsets[i], SEEK_SET) == 0);
      CHECK(fgetc (file) == pattern[offsets[i]]);
    }
  }
  close_checked (file);
  MW_CV_FS_DeleteFile ("SEEK1.TST");
}


/**
\brief Test the fseek function.
\details
\par Method
1. Read several bytes from a known file.
2. Seek forward with a positive offset from <tt>SEEK_CUR</tt> and verify the next byte.
3. Seek backward with a negative offset from <tt>SEEK_CUR</tt> and verify the next byte.
\par Expected
- Valid forward and backward seeks return <tt>0</tt>.
- Subsequent reads use the expected relative positions.
*/
void MW_CV_FS_Clib_fseek_2 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Combine forward and backward relative seeks around a known position */
  create ("SEEK2.TST", 16U);
  file = fopen ("SEEK2.TST", "rb");
  CHECK(file != NULL);

  if (file != NULL) {
    CHECK(fseek (file, 4L, SEEK_SET) == 0);
    CHECK(fseek (file, 3L, SEEK_CUR) == 0);
    CHECK(fgetc (file) == pattern[7]);
    CHECK(fseek (file, -3L, SEEK_CUR) == 0);
    CHECK(fgetc (file) == pattern[5]);
  }
  close_checked (file);
  MW_CV_FS_DeleteFile ("SEEK2.TST");
}


/**
\brief Test the fseek function.
\details
\par Method
1. Open a nonempty known file with mode <tt>rb</tt>.
2. Seek to the last byte with offset <tt>-1</tt> from <tt>SEEK_END</tt> and read it.
3. Seek to offset <tt>0</tt> from <tt>SEEK_END</tt> and attempt a read.
\par Expected
- Both valid seeks return <tt>0</tt>.
- The first read returns the final byte.
- Reading at the end position returns <tt>EOF</tt> and sets the end-of-file indicator.
*/
void MW_CV_FS_Clib_fseek_3 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Seek backward from end-of-file and verify the final-byte position */
  create ("SEEK3.TST", 16U);
  file = fopen ("SEEK3.TST", "rb");
  CHECK(file != NULL);

  if (file != NULL) {
    CHECK(fseek (file, -1L, SEEK_END) == 0);
    CHECK(fgetc (file) == pattern[15]);
    CHECK(fseek (file, 0L, SEEK_END) == 0);
    CHECK(fgetc (file) == EOF);
  }
  close_checked (file);
  MW_CV_FS_DeleteFile ("SEEK3.TST");
}


/**
\brief Test the fseek function.
\details
\par Method
1. Read a file through end-of-file and verify that <tt>feof</tt> is set.
2. Seek to offset <tt>0</tt> using <tt>SEEK_SET</tt>.
3. Read the first byte again.
\par Expected
- <tt>fseek</tt> returns <tt>0</tt> and clears the end-of-file indicator.
- The subsequent read returns the first byte.
*/
void MW_CV_FS_Clib_fseek_4 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Attempt invalid seeks, then prove that the stream remains usable */
  create ("SEEK4.TST", 2U);
  file = fopen ("SEEK4.TST", "rb");
  CHECK(file != NULL);

  if (file != NULL) {
    while (fgetc (file) != EOF) {
    }
    CHECK(feof (file) != 0);
    CHECK(fseek (file, 0L, SEEK_SET) == 0);
    CHECK(feof (file) == 0);
    CHECK(fgetc (file) == pattern[0]);
  }
  close_checked (file);
  MW_CV_FS_DeleteFile ("SEEK4.TST");
}


/**
\brief Test the fseek function.
\details
\par Method
1. Open a known file with mode <tt>rb</tt>.
2. Attempt to seek before the beginning of the file.
3. Close and reopen the file, then verify its content.
\par Expected
- The invalid seek returns a nonzero value.
- The operation does not modify file content.
*/
void MW_CV_FS_Clib_fseek_5 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create and open a known file */
  create ("SEEK5.TST", 8U);
  file = fopen ("SEEK5.TST", "rb");
  CHECK(file != NULL);

  /* Verify rejection of a position before the file start */
  if (file != NULL) {
    CHECK(fseek (file, -1L, SEEK_SET) != 0);
  }
  /* Close and verify the unchanged file */
  close_checked (file);
  CHECK(MW_CV_FS_VerifyFile ("SEEK5.TST", pattern, 8U));
  /* Remove the test file */
  MW_CV_FS_DeleteFile ("SEEK5.TST");
}

#if ((MW_CV_FS_DRIVE_M != 0U) || (MW_CV_FS_DRIVE_N != 0U) || (MW_CV_FS_DRIVE_R != 0U))
/*
\brief Validate every 8.3 name-length combination for one character value.
\details Creates, reads, appends, and verifies every generated short file name, then removes the complete set.
\param[in] name_char Character used to populate the base name and extension.
*/
static void check_short_names (char name_char) {
  char     name[13];
  FILE    *file;
  uint8_t  expected[2];
  uint32_t base_len;
  uint32_t ext_len;

  /* Cover every valid 8.3 base and extension length for one name character */
  for (base_len = 1U; base_len <= 8U; base_len++) {
    /* Clear name buffer */
    (void)memset (name, 0, sizeof (name));

    /* Populate the base name with the specified character */
    (void)memset (name, name_char, base_len);
    name[base_len] = '.';

    for (ext_len = 1U; ext_len <= 3U; ext_len++) {
      /* Populate the extension with the specified character */
      name[base_len + ext_len]      = name_char;
      /* Null-terminate the name */
      name[base_len + ext_len + 1U] = '\0';

      /* Create the signature expected on file read */
      expected[0] = (uint8_t)((base_len << 4) | ext_len);
      expected[1] = (uint8_t)~expected[0];

      /* Delete potentially existing file */
      MW_CV_FS_DeleteFile (name);

      /* Create a new file with the generated name */
      file = fopen (name, "w");
      CHECK(file != NULL);

      if (file != NULL) {
        /* Write the expected signature to the file */
        CHECK(fputc (expected[0], file) == expected[0]);
      }

      /* Close the file after writing */
      close_checked (file);

      /* Open created file for reading */
      file = fopen (name, "r");
      CHECK(file != NULL);

      if (file != NULL) {
        /* Read and verify the content of the file */
        CHECK(fgetc (file) == expected[0]);
        CHECK(fgetc (file) == EOF);
      }

      /* Close the file after reading */
      close_checked (file);

      /* Open created file for appending */
      file = fopen (name, "a");
      CHECK(file != NULL);

      if (file != NULL) {
        /* Append a complementary signature to the file */
        CHECK(fputc (expected[1], file) == expected[1]);
      }

      /* Close the file after appending */
      close_checked (file);

      /* Verify the content of the file against the expected signature */
      CHECK(MW_CV_FS_VerifyFile (name, expected, sizeof (expected)));
    }
  }

  /* Keep all generated files until the complete set has been verified, then remove them */
  for (base_len = 1U; base_len <= 8U; base_len++) {
    /* Prepare base name */
    (void)memset (name, 0, sizeof (name));
    (void)memset (name, name_char, base_len);
    name[base_len] = '.';

    for (ext_len = 1U; ext_len <= 3U; ext_len++) {
      /* Prepare extension */
      name[base_len + ext_len]      = name_char;
      name[base_len + ext_len + 1U] = '\0';

      /* Delete the file */
      MW_CV_FS_DeleteFile (name);
    }
  }
}


/**
\brief Test the fopen function.
\details
\par Method
1. Create a file with known content and close it.
2. Open it with <tt>r+</tt> and verify that the stream is valid and the content was not truncated.
\par Expected
- <tt>r+</tt> returns a valid stream and preserves the existing content.
*/
void MW_CV_FS_FAT_Clib_fopen_0 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create and open the file for update */
  create ("FATOP0.TST", 8U);
  file = fopen ("FATOP0.TST", "r+");
  CHECK(file != NULL);

  /* Verify that existing content was preserved */
  if (file != NULL) {
    CHECK(fgetc (file) == pattern[0]);
  }
  /* Close the stream and remove the file */
  close_checked (file);
  MW_CV_FS_DeleteFile ("FATOP0.TST");
}


/**
\brief Test the fopen function.
\details
\par Method
1. Open a file with <tt>w+</tt> and verify that reading and writing are permitted with the required positioning
operation between them.
2. Repeat with an existing file and verify truncation.
\par Expected
- <tt>w+</tt> creates or truncates the file and permits both reading and writing.
*/
void MW_CV_FS_FAT_Clib_fopen_1 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create and reopen the file with truncating update mode */
  create ("FATOP1.TST", 8U);
  file = fopen ("FATOP1.TST", "w+");
  CHECK(file != NULL);

  /* Write, reposition, and read through the same stream */
  if (file != NULL) {
    CHECK(fwrite (pattern, 1U, 4U, file) == 4U);
    CHECK(fseek (file, 0L, SEEK_SET) == 0);
    CHECK(fgetc (file) == pattern[0]);
  }
  /* Close the stream and remove the file */
  close_checked (file);
  MW_CV_FS_DeleteFile ("FATOP1.TST");
}


/**
\brief Test the fopen function.
\details
\par Method
1. Open a file with <tt>a+</tt>, write known data, and reposition for reading.
2. Write another block after repositioning away from the end.
3. Reopen and verify the complete content.
\par Expected
- <tt>a+</tt> permits reading, but every write is appended at end-of-file.
*/
void MW_CV_FS_FAT_Clib_fopen_2 (void) {
  FILE   *file;
  uint8_t expected[8];

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Append through an update stream, then verify the complete payload */
  (void)memcpy (expected, pattern, 4U);
  (void)memcpy (&expected[4], &pattern[8], 4U);
  create ("FATOP2.TST", 4U);
  file = fopen ("FATOP2.TST", "a+");
  CHECK(file != NULL);

  if (file != NULL) {
    CHECK(fseek (file, 0L, SEEK_SET) == 0);
    CHECK(fwrite (&pattern[8], 1U, 4U, file) == 4U);
  }
  close_checked (file);
  CHECK(MW_CV_FS_VerifyFile ("FATOP2.TST", expected, 8U));
  MW_CV_FS_DeleteFile ("FATOP2.TST");
}


/**
\brief Test the fopen function.
\details
\par Method
1. Open a file for writing through a path containing one or more missing directories.
2. Close and reopen the file for reading through the same path.
3. Verify its content and remove the file and directories during cleanup.
\par Expected
- <tt>fopen</tt> creates the missing FAT subdirectories as documented.
- The created file is accessible through its complete path.
*/
void MW_CV_FS_FAT_Clib_fopen_3 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Remove stale directory state */
  (void)frmdir ("FOD3", "/S");

  /* Create a file through missing intermediate directories */
  file = fopen ("FOD3\\SUB\\F.TST", "w");
  CHECK(file != NULL);
  close_checked (file);
  /* Reopen the generated path for reading */
  file = fopen ("FOD3\\SUB\\F.TST", "r");
  CHECK(file != NULL);
  close_checked (file);
  /* Remove the complete directory tree */
  (void)frmdir ("FOD3", "/S");
}


/**
\brief Test the fopen function.
\details
\par Method
1. Generate all 24 lowercase FAT short-name length combinations: base lengths one through eight and extension
lengths one through three.
2. Create each name with mode <tt>w</tt>, reopen it with mode <tt>r</tt>, and append with mode <tt>a</tt>.
3. Verify the two-byte payload after every sequence.
\par Expected
- Every valid lowercase 8.3 name opens successfully in all three modes.
- Each name retains only its own expected payload.
*/
void MW_CV_FS_FAT_Clib_fopen_4 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Validate every lowercase 8.3 length combination */
  check_short_names ('a');
}


/**
\brief Test the fopen function.
\details
\par Method
1. Generate all 24 uppercase FAT short-name length combinations: base lengths one through eight and extension
lengths one through three.
2. Create each name with mode <tt>w</tt>, reopen it with mode <tt>r</tt>, and append with mode <tt>a</tt>.
3. Verify the two-byte payload after every sequence.
\par Expected
- Every valid uppercase 8.3 name opens successfully in all three modes.
- Each name retains only its own expected payload.
*/
void MW_CV_FS_FAT_Clib_fopen_5 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Validate every uppercase 8.3 length combination */
  check_short_names ('A');
}


/**
\brief Test the fopen function.
\details
\par Method
1. Create four names that share prefixes and cross the FAT short-name/long-name boundary.
2. Store the complete logical name as each file's payload.
3. After all files exist simultaneously, reopen each name and verify its payload.
\par Expected
- Every distinct supported name remains independently addressable.
- Each file contains exactly the payload associated with its logical name.
*/
void MW_CV_FS_FAT_Clib_fopen_6 (void) {
  const char *names[] = {"ThisIsLongNameFile.fil", "ThisIsLongNameFil.fil", "thisislong.fil", "ThisIsLo.fil"};
  FILE       *file;
  uint32_t    i;
  size_t      size;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create each long-name variant before checking independent lookup */
  for (i = 0U; i < (sizeof (names) / sizeof (names[0])); i++) {
    MW_CV_FS_DeleteFile (names[i]);
  }

  for (i = 0U; i < (sizeof (names) / sizeof (names[0])); i++) {
    size = strlen (names[i]);
    file = fopen (names[i], "w");
    CHECK(file != NULL);

    if (file != NULL) {
      CHECK(fwrite (names[i], 1U, size, file) == size);
    }
    close_checked (file);
  }

  for (i = 0U; i < (sizeof (names) / sizeof (names[0])); i++) {
    size = strlen (names[i]);
    CHECK(MW_CV_FS_VerifyFile (names[i], (const uint8_t *)names[i], size));
    MW_CV_FS_DeleteFile (names[i]);
  }
}


/**
\brief Test the fopen function.
\details
\par Method
1. Create an empty FAT directory.
2. Pass the directory name to <tt>fopen</tt> with mode <tt>w</tt>.
3. Remove the directory after checking the result.
\par Expected
- <tt>fopen</tt> returns <tt>NULL</tt> and sets <tt>errno</tt> to <tt>RT_ERR_NOTFOUND</tt>.
- The directory remains a directory and can be removed normally.
*/
void MW_CV_FS_FAT_Clib_fopen_7 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  (void)frmdir ("FOD7", "/S");
  /* Create an empty FAT directory */
  CHECK(fmkdir ("FOD7") == fsOK);

  errno = 0;
  /* Create a file with the same name as the directory */
  file  = fopen ("FOD7", "w");
  CHECK(file == NULL);
  CHECK(errno == RT_ERR_NOTFOUND);

  /* Close the file and remove the directory */
  close_checked (file);
  CHECK(frmdir ("FOD7", NULL) == fsOK);
}


/**
\brief Test the fseek function.
\details
\par Method
1. Create a file with known content and reopen it with <tt>r+</tt>.
2. Seek within the file and overwrite a known range.
3. Close, reopen with <tt>r</tt>, and verify changed and unchanged ranges.
\par Expected
- Valid seeks in the FAT update stream return <tt>0</tt>.
- In-range overwrite affects only the selected range.
*/
void MW_CV_FS_FAT_Clib_fseek_0 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create and open a FAT update stream */
  create ("FATSK0.TST", 8U);
  file = fopen ("FATSK0.TST", "r+");
  CHECK(file != NULL);

  /* Seek and overwrite a bounded range */
  if (file != NULL) {
    CHECK(fseek (file, 3L, SEEK_SET) == 0);
    CHECK(fwrite (&pattern[10], 1U, 2U, file) == 2U);
  }
  /* Close the stream and remove the file */
  close_checked (file);
  MW_CV_FS_DeleteFile ("FATSK0.TST");
}


/**
\brief Test the fseek function.
\details
\par Method
1. Create a dedicated small file and reopen it with an update mode.
2. Seek to a documented position beyond end-of-file and write one byte.
3. Close and reopen the file, then verify its size and retained original content.
\par Expected
- The seek follows documented FAT behavior.
- Existing content is not corrupted and the resulting position and file size are correct.
*/
void MW_CV_FS_FAT_Clib_fseek_1 (void) {
  FILE      *file;
  fsFileInfo info = {0};

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Extend a FAT file through an update stream, then verify its new size */
  create ("FATSK1.TST", 4U);
  file = fopen ("FATSK1.TST", "r+");
  CHECK(file != NULL);

  if (file != NULL) {
    CHECK(fseek (file, 8L, SEEK_SET) == 0);
    CHECK(fputc (0x55, file) == 0x55);
  }
  close_checked (file);
  CHECK(ffind ("FATSK1.TST", &info) == fsOK);
  CHECK(info.size == 9U);
  MW_CV_FS_DeleteFile ("FATSK1.TST");
}

#endif

#if (MW_CV_FS_DRIVE_F != 0U)
/**
\brief Test the fopen function.
\details
\par Method
1. Create a known file with mode <tt>w</tt> and close it.
2. Attempt to open it with <tt>r+</tt>.
3. Reopen it with <tt>r</tt> and verify that its content was not corrupted.
\par Expected
- The unsupported <tt>r+</tt> request returns <tt>NULL</tt> and leaves existing content intact.
*/
void MW_CV_FS_EFS_Clib_fopen_0 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a file and reject read-update mode */
  create ("EFSOP0.TST", 2U);
  file = fopen ("EFSOP0.TST", "r+");
  CHECK(file == NULL);
  /* Remove the unchanged file */
  MW_CV_FS_DeleteFile ("EFSOP0.TST");
}


/**
\brief Test the fopen function.
\details
\par Method
1. Attempt independent <tt>w+</tt> and <tt>a+</tt> opens using dedicated file names.
2. Verify that neither request creates or changes a file.
\par Expected
- Both unsupported update-mode requests return <tt>NULL</tt> without filesystem changes.
*/
void MW_CV_FS_EFS_Clib_fopen_1 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Reject write-update mode */
  file = fopen ("EFSOP1.TST", "w+");
  CHECK(file == NULL);
  /* Reject append-update mode */
  file = fopen ("EFSOP1.TST", "a+");
  CHECK(file == NULL);
  /* Remove any unexpected file state */
  MW_CV_FS_DeleteFile ("EFSOP1.TST");
}


/**
\brief Test the fopen function.
\details
\par Method
1. Attempt to create a file through a path containing a directory component.
2. Verify that no file or directory was created.
3. Confirm that a root-level file can still be created and read.
\par Expected
- The directory-path request returns <tt>NULL</tt> because EFS does not support directories.
- A valid root-level operation remains functional after the rejected request.
*/
void MW_CV_FS_EFS_Clib_fopen_2 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Reject a path containing an EFS directory component */
  file = fopen ("DIR\\EFSOP2.TST", "w");
  CHECK(file == NULL);
  /* Verify that a root-level file remains usable */
  create ("EFSOP2.TST", 1U);
  MW_CV_FS_DeleteFile ("EFSOP2.TST");
}


/**
\brief Test the fopen function.
\details
\par Method
1. Create and reopen a root-level file whose name is exactly 31 characters long.
2. Attempt to create a root-level file whose name is 32 characters long.
\par Expected
- The 31-character EFS filename succeeds.
- The 32-character filename is rejected without affecting the valid file.
*/
void MW_CV_FS_EFS_Clib_fopen_3 (void) {
  const char *valid = "1234567890123456789012345678901", *invalid = "12345678901234567890123456789012";
  FILE       *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create and close the maximum-length valid name */
  MW_CV_FS_DeleteFile (valid);
  file = fopen (valid, "w");
  CHECK(file != NULL);
  close_checked (file);
  /* Reject the name beyond the EFS length limit */
  file = fopen (invalid, "w");
  CHECK(file == NULL);
  /* Remove the valid test file */
  MW_CV_FS_DeleteFile (valid);
}


/**
\brief Test the fopen function.
\details
\par Method
1. Create several valid root-level names that share a long prefix and differ only in their final characters.
2. Store a distinct deterministic payload in each file.
3. Reopen every file and verify its payload.
\par Expected
- Every valid name remains independently addressable.
- Creating or opening one name does not select or corrupt a neighboring name.
*/
void MW_CV_FS_EFS_Clib_fopen_4 (void) {
  const char *a = "SIMILAR_NAME_1234567890_A.TST", *b = "SIMILAR_NAME_1234567890_B.TST";
  FILE       *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create two similar maximum-length names */
  create (a, 1U);
  file = fopen (b, "w");
  CHECK(file != NULL);
  close_checked (file);
  /* Verify that the first file remains independently accessible */
  CHECK(MW_CV_FS_VerifyFile (a, pattern, 1U));

  /* Remove both test files */
  MW_CV_FS_DeleteFile (a);
  MW_CV_FS_DeleteFile (b);
}


/**
\brief Test the fseek function.
\details
\par Method
1. Open a file with mode <tt>w</tt>, write known data, and attempt an in-range <tt>fseek</tt>.
2. Close and reopen the file with mode <tt>r</tt>.
3. Verify the file remains readable and contains only successfully written data.
\par Expected
- Seeking in the EFS write stream returns a nonzero value.
- The rejected seek does not corrupt the file.
*/
void MW_CV_FS_EFS_Clib_fseek_0 (void) {
  FILE *file;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Open an EFS write stream */
  file = fopen ("EFSSK0.TST", "w");
  CHECK(file != NULL);

  /* Write data and reject repositioning */
  if (file != NULL) {
    CHECK(fwrite (pattern, 1U, 4U, file) == 4U);
    CHECK(fseek (file, 0L, SEEK_SET) != 0);
  }
  /* Close and verify the successfully written data */
  close_checked (file);
  CHECK(MW_CV_FS_VerifyFile ("EFSSK0.TST", pattern, 4U));
  /* Remove the test file */
  MW_CV_FS_DeleteFile ("EFSSK0.TST");
}

#endif

/**
@}
*/

#endif /* File System C library validation */
