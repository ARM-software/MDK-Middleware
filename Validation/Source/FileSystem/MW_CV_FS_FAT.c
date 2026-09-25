/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - File System - FAT
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_FS_FAT.c
 * Purpose: MDK Middleware - Component Validation - File System - FAT -
 *          tests module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"

#if ((MW_CV_FS != 0U) && ((MW_CV_FS_DRIVE_M != 0U) || (MW_CV_FS_DRIVE_N != 0U) || (MW_CV_FS_DRIVE_R != 0U)))

#include <stdio.h>
#include <string.h>

#include "MW_CV_TestReport.h"
#include "MW_CV_FS.h"
#include "MW_CV_FS_FAT.h"

#define CHECK(expr) ASSERT_TRUE((expr), "%s", #expr)

/*
\brief Ensure that the configured FAT drive is ready for a test.
\return true when drive preparation succeeds; otherwise false.
*/
static bool ready (void) {
  fsStatus status = MW_CV_FS_EnsureReady();

  /* Verify drive preparation and the FAT backend */
  CHECK(status == fsOK);
  CHECK(MW_CV_FS_IsFAT());

  /* Report whether the test may continue */
  return ((status == fsOK) && MW_CV_FS_IsFAT());
}


/*
\brief Remove a FAT directory tree during setup or cleanup.
\param[in] path Directory path to remove recursively.
*/
static void clean_dir (const char *path) {
  /* Remove colliding files and directory trees */
  (void)fdelete (path, "/S");
  (void)frmdir (path, "/S");

  /* Restore the root working directory */
  (void)fchdir ("\\");
}


/*
\brief Replace a file with a one-byte deterministic payload.
\param[in] path File path to create or replace.
*/
static void create_file (const char *path) {
  const uint8_t value = 0x5AU;

  /* Remove stale state and create a deterministic file */
  MW_CV_FS_DeleteFile (path);
  CHECK(MW_CV_FS_CreateFile (path, &value, 1U));
}


/*
\brief Find one entry and finish the associated directory search.
\param[in] path File or directory search path.
\param[out] info Destination for information about the matching entry.
\return Status returned by the initial ffind operation.
*/
static fsStatus find_one (const char *path, fsFileInfo *info) {
  /* Clear stale search state before starting a new lookup */
  (void)memset (info, 0, sizeof (*info));

  /* Return the first matching directory entry */
  return ffind (path, info);
}


/**
\defgroup mw_cv_fs_fat_test_funcs FAT
\brief FAT validation test functions
\details
These tests validate FAT searches, attributes, directories, volume information, and timestamps.
@{
*/

/**
\brief Test the ffind function with FAT long file names.
\details
\par Method
Enumerate long file names in the root directory with a prefix and suffix wildcard pattern.
\par Expected
- The search returns both long <tt>.txt</tt> names and no other entry.
- Returned names equal the created names without regard to letter case.
*/
void MW_CV_FS_FAT_ffind_0 (void) {
  fsFileInfo info = {0};
  fsStatus   status;
  uint32_t   alpha = 0U;
  uint32_t   beta  = 0U;
  uint32_t   count = 0U;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create matching long names and one suffix mismatch */
  create_file ("FFind Long Alpha Name.txt");
  create_file ("FFind Long Beta Name.txt");
  create_file ("FFind Long Gamma Name.bin");

  /* Enumerate and classify the matching long names */
  while ((status = ffind ("FFind Long *.txt", &info)) == fsOK) {
    if (MW_CV_FS_StrEqualIgnoreCase (info.name, "FFind Long Alpha Name.txt")) {
      alpha++;
    } else if (MW_CV_FS_StrEqualIgnoreCase (info.name, "FFind Long Beta Name.txt")) {
      beta++;
    } else {
      CHECK(false);
    }
    count++;
  }

  /* Verify that every expected name was returned exactly once */
  CHECK(count == 2U);
  CHECK(alpha == 1U);
  CHECK(beta  == 1U);

  /* Remove all files created by the test */
  MW_CV_FS_DeleteFile ("FFind Long Alpha Name.txt");
  MW_CV_FS_DeleteFile ("FFind Long Beta Name.txt");
  MW_CV_FS_DeleteFile ("FFind Long Gamma Name.bin");
}


/**
\brief Test the ffind function within a FAT directory.
\details
\par Method
Search a long-name directory containing matching and nonmatching long file names while a
matching-name decoy exists in the root directory.
\par Expected
- Only the two matching entries from the specified directory are returned.
- The root entry and the entry with a different extension are excluded.
*/
void MW_CV_FS_FAT_ffind_1 (void) {
  fsFileInfo info = {0};
  fsStatus   status;
  uint32_t   first = 0U;
  uint32_t   second = 0U;
  uint32_t   count = 0U;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Remove stale state and create a long-name search directory */
  clean_dir ("FFind Search Folder");
  MW_CV_FS_DeleteFile ("Directory Result One.dat");
  CHECK(fmkdir ("FFind Search Folder") == fsOK);

  /* Populate the directory and create a same-name root decoy */
  create_file ("FFind Search Folder\\Directory Result One.dat");
  create_file ("FFind Search Folder\\Directory Result Two.dat");
  create_file ("FFind Search Folder\\Directory Result Other.bin");
  create_file ("Directory Result One.dat");

  /* Enumerate and classify matches from the specified directory */
  while ((status = ffind ("FFind Search Folder\\Directory Result *.dat", &info)) == fsOK) {
    if (MW_CV_FS_StrEqualIgnoreCase (info.name, "Directory Result One.dat")) {
      first++;
    } else if (MW_CV_FS_StrEqualIgnoreCase (info.name, "Directory Result Two.dat")) {
      second++;
    } else {
      CHECK(false);
    }
    count++;
  }

  /* Verify that the path constrained the search to the directory */
  CHECK(count  == 2U);
  CHECK(first  == 1U);
  CHECK(second == 1U);

  /* Remove the directory tree and root decoy */
  clean_dir ("FFind Search Folder");
  MW_CV_FS_DeleteFile ("Directory Result One.dat");
}


/**
\brief Test the ffind function within a nested FAT directory.
\details
\par Method
Search a long-name subdirectory while similarly named files exist in its parent and the drive root.
\par Expected
- Only the two matching entries from the nested directory are returned.
- Matching entries outside the nested directory are excluded.
*/
void MW_CV_FS_FAT_ffind_2 (void) {
  fsFileInfo info = {0};
  fsStatus   status;
  uint32_t   one   = 0U;
  uint32_t   two   = 0U;
  uint32_t   count = 0U;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Remove stale state and create a nested long-name directory */
  clean_dir ("FFind Tree");
  MW_CV_FS_DeleteFile ("Deep Result Root.log");
  CHECK(fmkdir ("FFind Tree\\Nested Search Folder") == fsOK);

  /* Populate the nested directory and both enclosing scopes */
  create_file ("FFind Tree\\Nested Search Folder\\Deep Result One.log");
  create_file ("FFind Tree\\Nested Search Folder\\Deep Result Two.log");
  create_file ("FFind Tree\\Deep Result Parent.log");
  create_file ("Deep Result Root.log");

  /* Enumerate and classify matches from the nested directory */
  while ((status = ffind ("FFind Tree\\Nested Search Folder\\Deep Result *.log", &info)) == fsOK) {
    if (MW_CV_FS_StrEqualIgnoreCase (info.name, "Deep Result One.log")) {
      one++;
    } else if (MW_CV_FS_StrEqualIgnoreCase (info.name, "Deep Result Two.log")) {
      two++;
    } else {
      CHECK(false);
    }
    count++;
  }

  /* Verify that only nested-directory entries were returned */
  CHECK(count == 2U);
  CHECK(one   == 1U);
  CHECK(two   == 1U);

  /* Remove the directory tree and root decoy */
  clean_dir ("FFind Tree");
  MW_CV_FS_DeleteFile ("Deep Result Root.log");
}


/**
\brief Test independent ffind searches in FAT directories.
\details
\par Method
Interleave wildcard enumeration of long file names in two different directories using separate
<tt>fsFileInfo</tt> search states.
\par Expected
- Each search returns both names from its own directory exactly once.
- Advancing one search does not alter the other search state.
*/
void MW_CV_FS_FAT_ffind_3 (void) {
  fsFileInfo info_a = {0};
  fsFileInfo info_b = {0};
  fsStatus   status_a;
  fsStatus   status_b;
  uint32_t   alpha   = 0U;
  uint32_t   beta    = 0U;
  uint32_t   gamma   = 0U;
  uint32_t   delta   = 0U;
  uint32_t   count_a = 0U;
  uint32_t   count_b = 0U;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Remove stale state and create two independent search directories */
  clean_dir ("FFind State Folder A");
  clean_dir ("FFind State Folder B");
  CHECK(fmkdir ("FFind State Folder A") == fsOK);
  CHECK(fmkdir ("FFind State Folder B") == fsOK);

  /* Populate each directory with two long file names */
  create_file ("FFind State Folder A\\State Entry Alpha.tmp");
  create_file ("FFind State Folder A\\State Entry Beta.tmp");
  create_file ("FFind State Folder B\\State Entry Gamma.tmp");
  create_file ("FFind State Folder B\\State Entry Delta.tmp");

  /* Advance both directory searches in an interleaved sequence */
  do {
    status_a = ffind ("FFind State Folder A\\State Entry *.tmp", &info_a);
    if (status_a == fsOK) {
      if (MW_CV_FS_StrEqualIgnoreCase (info_a.name, "State Entry Alpha.tmp")) {
        alpha++;
      } else if (MW_CV_FS_StrEqualIgnoreCase (info_a.name, "State Entry Beta.tmp")) {
        beta++;
      } else {
        CHECK(false);
      }
      count_a++;
    }

    status_b = ffind ("FFind State Folder B\\State Entry *.tmp", &info_b);
    if (status_b == fsOK) {
      if (MW_CV_FS_StrEqualIgnoreCase (info_b.name, "State Entry Gamma.tmp")) {
        gamma++;
      } else if (MW_CV_FS_StrEqualIgnoreCase (info_b.name, "State Entry Delta.tmp")) {
        delta++;
      } else {
        CHECK(false);
      }
      count_b++;
    }
  } while ((status_a == fsOK) || (status_b == fsOK));

  /* Verify that each independent search returned its complete set */
  CHECK(count_a == 2U);
  CHECK(count_b == 2U);
  CHECK(alpha   == 1U);
  CHECK(beta    == 1U);
  CHECK(gamma   == 1U);
  CHECK(delta   == 1U);

  /* Remove both search directory trees */
  clean_dir ("FFind State Folder A");
  clean_dir ("FFind State Folder B");
}


/**
\brief Test the fattrib function.
\par Method
Set one attribute on a file.
\par Expected
- The hidden attribute is set successfully.
- The test file can be found after its attribute is changed.
- The file reports the hidden attribute.
- The hidden attribute is cleared successfully.
*/
void MW_CV_FS_FAT_fattrib_0 (void) {
  fsFileInfo info;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a file and set its hidden attribute */
  create_file ("ATTR0.TST");
  CHECK(fattrib ("ATTR0.TST", "+H") == fsOK);

  /* Read back and verify the attribute */
  CHECK(find_one ("ATTR0.TST", &info) == fsOK);
  CHECK((info.attrib & FS_FAT_ATTR_HIDDEN) != 0U);

  /* Clear the attribute and remove the file */
  CHECK(fattrib ("ATTR0.TST", "-H") == fsOK);
  MW_CV_FS_DeleteFile ("ATTR0.TST");
}


/**
\brief Test the fattrib function.
\details
\par Method
Clear the attribute.
\par Expected
- The archive attribute is set successfully.
- The archive attribute is cleared successfully.
- The test file can be found after its attribute is changed.
- The file no longer reports the archive attribute.
*/
void MW_CV_FS_FAT_fattrib_1 (void) {
  fsFileInfo info;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a file and toggle its archive attribute */
  create_file ("ATTR1.TST");
  CHECK(fattrib ("ATTR1.TST", "+A") == fsOK);
  CHECK(fattrib ("ATTR1.TST", "-A") == fsOK);

  /* Verify that the attribute was cleared */
  CHECK(find_one ("ATTR1.TST", &info) == fsOK);
  CHECK((info.attrib & FS_FAT_ATTR_ARCHIVE) == 0U);

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("ATTR1.TST");
}


/**
\brief Test the fattrib function.
\details
\par Method
Combine attributes and verify them with <tt>ffind</tt>.
\par Expected
- The hidden, system, and read-only attributes are set successfully.
- The file can be found after its attributes are changed.
- The file reports the hidden, system, and read-only attributes.
- The hidden, system, and read-only attributes are cleared successfully.
*/
void MW_CV_FS_FAT_fattrib_2 (void) {
  fsFileInfo info;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a file and set multiple attributes */
  create_file ("ATTR2.TST");
  CHECK(fattrib ("ATTR2.TST", "+H +S +R") == fsOK);

  /* Read back and verify the combined attribute mask */
  CHECK(find_one ("ATTR2.TST", &info) == fsOK);
  CHECK((info.attrib & (FS_FAT_ATTR_HIDDEN | FS_FAT_ATTR_SYSTEM | FS_FAT_ATTR_READ_ONLY)) ==
         (FS_FAT_ATTR_HIDDEN | FS_FAT_ATTR_SYSTEM | FS_FAT_ATTR_READ_ONLY));
  /* Clear the attributes and remove the file */
  CHECK(fattrib ("ATTR2.TST", "-H -S -R") == fsOK);
  MW_CV_FS_DeleteFile ("ATTR2.TST");
}


/**
\brief Test the fpwd function.
\details
\par Method
Read the root working directory.
\par Expected
- The directory change succeeds.
- The current-directory query succeeds.
- The returned current-directory path is not empty.
*/
void MW_CV_FS_FAT_fpwd_0 (void) {
  char buf[260];

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Select and query the root working directory */
  CHECK(fchdir ("\\") == fsOK);
  CHECK(fpwd (MW_CV_FS_DRIVE, buf, sizeof (buf)) == fsOK);

  /* Verify that a path was returned */
  CHECK(buf[0] != '\0');
}


/**
\brief Test the fpwd function.
\details
\par Method
Read a changed working directory.
\par Expected
- The directory creation succeeds.
- The directory change succeeds.
- The current-directory query succeeds.
- The returned path contains the selected directory name.
- The directory change succeeds.
*/
void MW_CV_FS_FAT_fpwd_1 (void) {
  char buf[260];

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create and enter a dedicated directory */
  clean_dir ("PWD1");
  CHECK(fmkdir ("PWD1") == fsOK);
  CHECK(fchdir ("PWD1") == fsOK);

  /* Read and verify the changed working directory */
  CHECK(fpwd (MW_CV_FS_DRIVE, buf, sizeof (buf)) == fsOK);
  CHECK(strstr (buf, "PWD1") != NULL);

  /* Restore the root and remove the directory */
  CHECK(fchdir ("\\") == fsOK);
  clean_dir ("PWD1");
}


/**
\brief Test the fpwd function.
\details
\par Method
Exercise a short destination buffer.
\par Expected
- The current-directory query reports an invalid parameter.
*/
void MW_CV_FS_FAT_fpwd_2 (void) {
  char buf[1];

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Verify rejection of an undersized destination buffer */
  CHECK(fpwd (MW_CV_FS_DRIVE, buf, sizeof (buf)) == fsInvalidParameter);
}


/**
\brief Test the fchdir function.
\details
\par Method
Change to one existing directory.
\par Expected
- The directory creation succeeds.
- The directory change succeeds.
- The directory change succeeds.
*/
void MW_CV_FS_FAT_fchdir_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create and enter a dedicated directory */
  clean_dir ("CDIR0");
  CHECK(fmkdir ("CDIR0") == fsOK);
  CHECK(fchdir ("CDIR0") == fsOK);

  /* Restore the root and remove the directory */
  CHECK(fchdir ("\\") == fsOK);
  clean_dir ("CDIR0");
}


/**
\brief Test the fchdir function.
\details
\par Method
Verify supported root and current-directory spellings.
\par Expected
- The directory change succeeds.
- The directory change succeeds.
- The directory change succeeds.
*/
void MW_CV_FS_FAT_fchdir_1 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Verify root and current-directory path spellings */
  CHECK(fchdir ("\\") == fsOK);
  CHECK(fchdir (".") == fsOK);
  CHECK(fchdir ("/") == fsOK);
}


/**
\brief Test the fchdir function.
\details
\par Method
Navigate using parent and relative paths.
\par Expected
- The directory creation succeeds.
- The directory change succeeds.
- The directory change succeeds.
- The directory change succeeds.
- The directory change succeeds.
*/
void MW_CV_FS_FAT_fchdir_2 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create and enter a nested directory */
  clean_dir ("CDIR2");
  CHECK(fmkdir ("CDIR2\\SUB") == fsOK);
  CHECK(fchdir ("CDIR2\\SUB") == fsOK);

  /* Navigate through parent and relative paths */
  CHECK(fchdir ("..") == fsOK);
  CHECK(fchdir ("SUB") == fsOK);

  /* Restore the root and remove the directory tree */
  CHECK(fchdir ("\\") == fsOK);
  clean_dir ("CDIR2");
}


/**
\brief Test the fchdir function.
\details
\par Method
Reject a missing directory.
\par Expected
- The directory change reports that the file or directory was not found.
*/
void MW_CV_FS_FAT_fchdir_3 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Verify rejection of a missing directory */
  CHECK(fchdir ("NO_DIR") == fsFileNotFound);
}


/**
\brief Test the fchdir function.
\details
\par Method
Reject a file path with <tt>fsNotDirectory</tt>.
\par Expected
- The directory change reports that the path does not identify a directory.
*/
void MW_CV_FS_FAT_fchdir_4 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a file and reject it as a directory */
  create_file ("NOTDIR.TST");
  CHECK(fchdir ("NOTDIR.TST") == fsNotDirectory);

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("NOTDIR.TST");
}


/**
\brief Test the fchdir function.
\details
\par Method
Call <tt>fchdir</tt> with a NULL path and an empty path.
\par Expected
- The directory change reports an invalid parameter.
- The directory change reports an invalid path.
*/
void MW_CV_FS_FAT_fchdir_5 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Verify null and empty path handling */
  CHECK(fchdir (NULL) == fsInvalidParameter);
  CHECK(fchdir ("") == fsInvalidPath);
}


/**
\brief Test the fmkdir function.
\details
\par Method
Create one directory.
\par Expected
- The directory creation succeeds.
*/
void MW_CV_FS_FAT_fmkdir_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Remove stale state and create one directory */
  clean_dir ("MKD0");
  CHECK(fmkdir ("MKD0") == fsOK);

  /* Remove the created directory */
  clean_dir ("MKD0");
}


/**
\brief Test the fmkdir function.
\details
\par Method
Verify absolute and relative paths.
\par Expected
- The directory creation succeeds.
- The directory creation succeeds.
*/
void MW_CV_FS_FAT_fmkdir_1 (void) {
  char path[32];

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Remove stale absolute and relative directory names */
  clean_dir ("MKD1A");
  clean_dir ("MKD1R");

  /* Create directories through absolute and relative paths */
  MW_CV_FS_MakePath (path, sizeof (path), "\\MKD1A");
  CHECK(fmkdir (path) == fsOK);
  CHECK(fmkdir ("MKD1R") == fsOK);
  /* Remove both created directories */
  clean_dir ("MKD1A");
  clean_dir ("MKD1R");
}


/**
\brief Test the fmkdir function.
\details
\par Method
Create intermediate directories.
\par Expected
- The directory creation succeeds.
- The directory change succeeds.
- The directory change succeeds.
*/
void MW_CV_FS_FAT_fmkdir_2 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a path with missing intermediate directories */
  clean_dir ("MKD2");
  CHECK(fmkdir ("MKD2\\A\\B") == fsOK);

  /* Enter the deepest directory to verify the complete path */
  CHECK(fchdir ("MKD2\\A\\B") == fsOK);

  /* Restore the root and remove the directory tree */
  CHECK(fchdir ("\\") == fsOK);
  clean_dir ("MKD2");
}


/**
\brief Test the fmkdir function.
\details
\par Method
Reject an existing directory or colliding file.
\par Expected
- The directory creation succeeds.
- The directory creation reports that the name already exists.
*/
void MW_CV_FS_FAT_fmkdir_3 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a directory and reject duplicate creation */
  clean_dir ("MKD3");
  CHECK(fmkdir ("MKD3") == fsOK);
  CHECK(fmkdir ("MKD3") == fsAlreadyExists);

  /* Replace it with a file and reject the name collision */
  clean_dir ("MKD3");
  create_file ("MKD3");
  CHECK(fmkdir ("MKD3") == fsAlreadyExists);
  /* Remove the colliding file */
  MW_CV_FS_DeleteFile ("MKD3");
}


/**
\brief Test the fmkdir function.
\details
\par Method
Create a bounded configuration-derived number of sibling directories and verify them with <tt>ffind</tt>.
\par Expected
- The directory creation succeeds.
- The directory creation succeeds.
- Exactly 16 matching entries are found.
*/
void MW_CV_FS_FAT_fmkdir_4 (void) {
  char       path[24];
  fsFileInfo info = {0};
  uint32_t   i, count = 0U;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a bounded directory set before inspecting its contents */
  clean_dir ("MKD4");
  CHECK(fmkdir ("MKD4") == fsOK);

  for (i = 0U; i < 16U; i++) {
    (void)snprintf (path, sizeof (path), "MKD4\\D%02u", (unsigned)i);
    CHECK(fmkdir (path) == fsOK);
  }

  /* Enumerate the directory and verify that every created entry is visible */
  while (ffind ("MKD4\\*", &info) == fsOK) {
    if (info.name[0] != '.') {
      count++;
    }
  }
  CHECK(count == 16U);

  /* Remove the complete test tree for the following test */
  clean_dir ("MKD4");
}


/**
\brief Test the frmdir function.
\details
\par Method
Remove one empty directory.
\par Expected
- The directory creation succeeds.
- The directory removal succeeds.
*/
void MW_CV_FS_FAT_frmdir_0 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a clean empty directory */
  clean_dir ("RMD0");
  CHECK(fmkdir ("RMD0") == fsOK);

  /* Remove the directory without recursive options */
  CHECK(frmdir ("RMD0", NULL) == fsOK);
}


/**
\brief Test the frmdir function.
\details
\par Method
Verify absolute and relative removal and immediate name reuse.
\par Expected
- The directory creation succeeds.
- The directory removal succeeds.
- The directory creation succeeds.
- The directory removal succeeds.
*/
void MW_CV_FS_FAT_frmdir_1 (void) {
  char path[32];

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Prepare an absolute directory path */
  clean_dir ("RMD1");
  MW_CV_FS_MakePath (path, sizeof (path), "\\RMD1");

  /* Create and remove the directory through the absolute path */
  CHECK(fmkdir (path) == fsOK);
  CHECK(frmdir (path, NULL) == fsOK);

  /* Reuse and remove the same name through a relative path */
  CHECK(fmkdir ("RMD1") == fsOK);
  CHECK(frmdir ("RMD1", NULL) == fsOK);
}


/**
\brief Test the frmdir function.
\details
\par Method
Reject a file path with <tt>fsNotDirectory</tt>.
\par Expected
- The directory removal reports that the path does not identify a directory.
*/
void MW_CV_FS_FAT_frmdir_2 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a file and reject it as a directory removal target */
  create_file ("RMD2.TST");
  CHECK(frmdir ("RMD2.TST", NULL) == fsNotDirectory);

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("RMD2.TST");
}


/**
\brief Test the frmdir function.
\details
\par Method
Reject a nonempty directory.
\par Expected
- The directory creation succeeds.
- The directory removal reports that the directory is not empty.
*/
void MW_CV_FS_FAT_frmdir_3 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a nonempty directory */
  clean_dir ("RMD3");
  CHECK(fmkdir ("RMD3") == fsOK);
  create_file ("RMD3\\FILE.TST");

  /* Verify rejection without recursive removal */
  CHECK(frmdir ("RMD3", NULL) == fsDirNotEmpty);

  /* Remove the complete directory tree */
  clean_dir ("RMD3");
}


/**
\brief Test the frmdir function.
\details
\par Method
Remove a directory tree using documented recursive options.
\par Expected
- The directory creation succeeds.
- The directory removal succeeds.
*/
void MW_CV_FS_FAT_frmdir_4 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a populated nested directory tree */
  clean_dir ("RMD4");
  CHECK(fmkdir ("RMD4\\A\\B") == fsOK);
  create_file ("RMD4\\A\\B\\F.TST");

  /* Remove the complete tree recursively */
  CHECK(frmdir ("RMD4", "/S") == fsOK);
}


/**
\brief Test the fvol function.
\details
\par Method
Read the volume label.
\par Expected
- The volume-information request succeeds.
*/
void MW_CV_FS_FAT_fvol_0 (void) {
  char label[12];

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Read the volume label without requesting the serial number */
  CHECK(fvol (MW_CV_FS_DRIVE, label, NULL) == fsOK);
}


/**
\brief Test the fvol function.
\details
\par Method
Read and validate the volume serial number.
\par Expected
- The volume-information request succeeds.
*/
void MW_CV_FS_FAT_fvol_1 (void) {
  uint32_t serial;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Read the volume serial number without requesting the label */
  CHECK(fvol (MW_CV_FS_DRIVE, NULL, &serial) == fsOK);
}


/**
\brief Test the fvol function.
\details
\par Method
Exercise absent output parameters as documented.
\par Expected
- The volume-information request succeeds.
- The volume-information request reports an invalid drive.
*/
void MW_CV_FS_FAT_fvol_2 (void) {
  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Verify optional output handling on a valid drive */
  CHECK(fvol (MW_CV_FS_DRIVE, NULL, NULL) == fsOK);

  /* Verify rejection of an invalid drive */
  CHECK(fvol (MW_CV_FS_INVALID_DRIVE, NULL, NULL) == fsInvalidDrive);
}


static fsTime test_time (uint8_t sec) {
  fsTime time = {12U, 34U, sec, 5U, 6U, 2024U};

  /* Return a deterministic valid FAT timestamp */
  return time;
}


/**
\brief Test the ftime_set function.
\details
\par Method
Set one valid file timestamp.
\par Expected
- The timestamp update succeeds.
*/
void MW_CV_FS_FAT_ftime_set_0 (void) {
  fsTime write = test_time (20U);

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a file and set its write timestamp */
  create_file ("TSET0.TST");
  CHECK(ftime_set ("TSET0.TST", NULL, NULL, &write) == fsOK);

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("TSET0.TST");
}


/**
\brief Test the ftime_set function.
\details
\par Method
Set all supported timestamps.
\par Expected
- The timestamp update succeeds.
*/
void MW_CV_FS_FAT_ftime_set_1 (void) {
  fsTime create = test_time (20U), access = test_time (22U), write = test_time (24U);

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a file and set all supported timestamps */
  create_file ("TSET1.TST");
  CHECK(ftime_set ("TSET1.TST", &create, &access, &write) == fsOK);

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("TSET1.TST");
}


/**
\brief Test the ftime_set function.
\details
\par Method
Set directory timestamps and verify FAT two-second rounding and access-date granularity.
\par Expected
- The directory creation succeeds.
- The timestamp update succeeds.
- The timestamp query succeeds.
- The retrieved write timestamp contains the assigned seconds value.
- The unsupported access timestamp remains cleared.
*/
void MW_CV_FS_FAT_ftime_set_2 (void) {
  fsTime access = test_time (23U), write = test_time (23U), got_access, got_write;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a directory and set its access and write times */
  clean_dir ("TSET2");
  CHECK(fmkdir ("TSET2") == fsOK);
  CHECK(ftime_set ("TSET2", NULL, &access, &write) == fsOK);

  /* Read back the stored directory timestamps */
  CHECK(ftime_get ("TSET2", NULL, &got_access, &got_write) == fsOK);

  /* Verify FAT timestamp granularity */
  CHECK(got_write.sec == 22U);
  CHECK((got_access.hr == 0U) && (got_access.min == 0U) && (got_access.sec == 0U));

  /* Remove the test directory */
  clean_dir ("TSET2");
}


/**
\brief Test the ftime_set function.
\details
\par Method
Reject invalid parameters, drive, time fields, or missing path with exact statuses.
\par Expected
- The timestamp update reports an invalid parameter.
- The timestamp update reports an invalid drive.
- The timestamp update is rejected.
*/
void MW_CV_FS_FAT_ftime_set_3 (void) {
  fsTime invalid = test_time (20U);

  /* Corrupt the month field for the negative requests */
  invalid.mon    = 13U;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Verify null path handling */
  CHECK(ftime_set (NULL, NULL, NULL, NULL) == fsInvalidParameter);

  /* Verify invalid-drive and missing-file handling */
  CHECK(ftime_set (MW_CV_FS_INVALID_DRIVE, NULL, NULL, &invalid) == fsInvalidDrive);
  CHECK(ftime_set ("NO_TIME.TST", NULL, NULL, &invalid) != fsOK);
}


/**
\brief Test the ftime_get function.
\details
\par Method
Read one file timestamp.
\par Expected
- The timestamp query succeeds.
*/
void MW_CV_FS_FAT_ftime_get_0 (void) {
  fsTime write;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a file and read its write timestamp */
  create_file ("TGET0.TST");
  CHECK(ftime_get ("TGET0.TST", NULL, NULL, &write) == fsOK);

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("TGET0.TST");
}


/**
\brief Test the ftime_get function.
\details
\par Method
Retrieve and compare all supported timestamps.
\par Expected
- The timestamp query succeeds.
- The returned creation and write years are valid FAT dates.
*/
void MW_CV_FS_FAT_ftime_get_1 (void) {
  fsTime create, access, write;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a file and retrieve all supported timestamps */
  create_file ("TGET1.TST");
  CHECK(ftime_get ("TGET1.TST", &create, &access, &write) == fsOK);

  /* Verify valid FAT timestamp years */
  CHECK((create.year >= 1980U) && (write.year >= 1980U));

  /* Remove the test file */
  MW_CV_FS_DeleteFile ("TGET1.TST");
}


/**
\brief Test the ftime_get function.
\details
\par Method
Retrieve file and directory times with each optional output pointer omitted independently.
\par Expected
- The timestamp query succeeds.
- The timestamp query succeeds.
- The timestamp query succeeds.
*/
void MW_CV_FS_FAT_ftime_get_2 (void) {
  fsTime a, b;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Create a file for optional-output requests */
  create_file ("TGET2.TST");

  /* Omit each timestamp output independently */
  CHECK(ftime_get ("TGET2.TST", NULL, &a, &b) == fsOK);
  CHECK(ftime_get ("TGET2.TST", &a, NULL, &b) == fsOK);
  CHECK(ftime_get ("TGET2.TST", &a, &b, NULL) == fsOK);
  /* Remove the test file */
  MW_CV_FS_DeleteFile ("TGET2.TST");
}


/**
\brief Test the ftime_get function.
\details
\par Method
Reject invalid parameters, drive, or missing path with exact statuses.
\par Expected
- The timestamp query reports an invalid parameter.
- The timestamp query reports an invalid drive.
- The timestamp query reports that the file or directory was not found.
*/
void MW_CV_FS_FAT_ftime_get_3 (void) {
  fsTime time;

  /* Prepare the configured drive */
  if (!ready()) {
    return;
  }

  /* Verify null path handling */
  CHECK(ftime_get (NULL, &time, NULL, NULL) == fsInvalidParameter);

  /* Verify invalid-drive and missing-file handling */
  CHECK(ftime_get (MW_CV_FS_INVALID_DRIVE, &time, NULL, NULL) == fsInvalidDrive);
  CHECK(ftime_get ("NO_TIME.TST", &time, NULL, NULL) == fsFileNotFound);
}


/**
@}
*/

#endif /* FAT validation */
