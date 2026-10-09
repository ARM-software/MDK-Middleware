/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - File System - Performance
 * Copyright (c) 2026 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_FS_Performance.c
 * Purpose: MDK Middleware - Component Validation - File System - performance
 *          tests module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Config.h"

#if ((MW_CV_FS != 0U) && (MW_CV_FS_PERFORMANCE != 0U))

#include <stdio.h>
#include <string.h>

#include "cmsis_os2.h"
#include "MW_CV_TestReport.h"
#include "MW_CV_FS.h"
#include "MW_CV_FS_Performance.h"

#define CHECK(expr) ASSERT_TRUE((expr), "%s", #expr)

#define PERF_BUFFER_SIZE    8192U
#define PERF_TRANSFER_BYTES (1024U * 1024U)
#define PERF_FILE_NAME      "FS_PERF.BIN"

static uint32_t perf_pattern_words[PERF_BUFFER_SIZE / sizeof (uint32_t)];
static uint32_t perf_data_words[PERF_BUFFER_SIZE / sizeof (uint32_t)];
static uint32_t perf_start_ticks;
static uint64_t perf_elapsed_ticks;


/* Reset the local high-resolution CMSIS-RTOS2 measurement */
static void perf_timer_reset (void) {
  /* Clear accumulated measurement ticks */
  perf_elapsed_ticks = 0U;
}


/* Start one measured interval */
static void perf_timer_start (void) {
  /* Capture the current system-timer count */
  perf_start_ticks = osKernelGetSysTimerCount();
}


/* Accumulate one measured interval, including a 32-bit system-timer wrap */
static void perf_timer_stop (void) {
  /* Add the unsigned tick delta to the current measurement */
  perf_elapsed_ticks += (uint32_t)(osKernelGetSysTimerCount() - perf_start_ticks);
}


/* Convert the accumulated CMSIS-RTOS2 system-timer ticks to microseconds */
static uint32_t perf_timer_get_time (void) {
  uint64_t time;
  uint32_t frequency;

  /* Read the system-timer frequency */
  frequency = osKernelGetSysTimerFreq();

  /* Reject an unavailable timer frequency */
  if (frequency == 0U) {
    return 0U;
  }

  /* Convert accumulated ticks to microseconds */
  time = (perf_elapsed_ticks * 1000000ULL) / frequency;

  /* Saturate the public 32-bit result */
  if (time > 0xFFFFFFFFULL) {
    time = 0xFFFFFFFFULL;
  }

  /* Return the converted measurement */
  return (uint32_t)time;
}


/*
\brief Convert a measured transfer to binary kilobytes per second.
\param[in] bytes Transferred byte count.
\param[in] time_us Measured duration in microseconds.
\return Transfer rate in KiB/s, or zero when the duration is zero.
*/
static uint32_t transfer_rate (uint64_t bytes, uint32_t time_us) {
  /* Avoid division by an unavailable measurement */
  if (time_us == 0U) {
    return 0U;
  }

  /* Convert bytes per microsecond to binary kilobytes per second */
  return (uint32_t)((bytes * 1000000ULL) / ((uint64_t)time_us * 1024ULL));
}


/*
\brief Fill the performance buffer with deterministic binary data.
*/
static void prepare_data (void) {
  /* Fill the write buffer with deterministic data */
  MW_CV_FS_FillPattern ((uint8_t *)perf_pattern_words, PERF_BUFFER_SIZE, 0U);
}


/**
\defgroup mw_cv_fs_performance_test_funcs Performance Tests
\brief File System throughput validation test functions
\details
These tests measure successful transfers without imposing platform-specific minimum rates.
@{
*/

#if (MW_CV_FS_CLIB != 0U)
/**
\brief Measure C library FileSystem performance.
\details
\par Method
Format the drive, then write and read a one-MiB file using <tt>fwrite</tt> and <tt>fread</tt>
transfer sizes from 512 to 8192 bytes.
Data preparation and verification are excluded from the measured intervals.
\par Expected
- Formatting succeeds.
- Every transfer and stream operation succeeds.
- Read data matches the written pattern.
- Nonzero write and read durations and their corresponding KiB/s rates are reported for each transfer size.
*/
void MW_CV_FS_Performance_Clib (void) {
  uint8_t  *pattern = (uint8_t *)perf_pattern_words;
  uint8_t  *buffer  = (uint8_t *)perf_data_words;
  FILE     *file;
  size_t    count;
  int32_t   status;
  uint32_t  chunk;
  uint32_t  offset;
  uint32_t  read_time;
  uint32_t  write_time;
  bool      ok;

  if (MW_CV_FS_Format (NULL) != fsOK) {
    CHECK(false);
    return;
  }

  prepare_data();

  /* Iterate over different chunk sizes, double chunk size each time */
  for (chunk = 512U; chunk <= PERF_BUFFER_SIZE; chunk *= 2U) {
    /* Delete any existing measurement file before starting a new test iteration */
    MW_CV_FS_DeleteFile (PERF_FILE_NAME);

    /* Open a new file for writing the performance test data */
    file = fopen (PERF_FILE_NAME, "wb");
    CHECK(file != NULL);
    if (file == NULL) {
      return;
    }

    /* Write the performance test data to the file */
    ok = true;
    perf_timer_reset();
    for (offset = 0U; offset < PERF_TRANSFER_BYTES; offset += chunk) {
      perf_timer_start();
      count = fwrite (pattern, 1U, chunk, file);
      perf_timer_stop();
      if (count != chunk) {
        CHECK(count == chunk);
        ok = false;
        break;
      }
    }

    /* Finalize the write operation by flushing and closing the file */
    perf_timer_start();
    status = fflush (file);
    perf_timer_stop();
    CHECK(status == 0);
    if (status != 0) {
      ok = false;
    }

    status = fclose (file);
    CHECK(status == 0);
    if (status != 0) {
      ok = false;
    }

    write_time = perf_timer_get_time();
    CHECK(write_time != 0U);
    if ((!ok) || (write_time == 0U)) {
      MW_CV_FS_DeleteFile (PERF_FILE_NAME);
      return;
    }

    /* Open the performance test file for reading */
    file = fopen (PERF_FILE_NAME, "rb");
    CHECK(file != NULL);
    if (file == NULL) {
      MW_CV_FS_DeleteFile (PERF_FILE_NAME);
      return;
    }

    /* Read the performance test data from the file and verify its integrity */
    ok = true;
    perf_timer_reset();
    for (offset = 0U; offset < PERF_TRANSFER_BYTES; offset += chunk) {
      (void)memset (buffer, 0, chunk);
      perf_timer_start();
      count = fread (buffer, 1U, chunk, file);
      perf_timer_stop();
      if (count != chunk) {
        CHECK(count == chunk);
        ok = false;
        break;
      }

      if (memcmp (buffer, pattern, chunk) != 0) {
        CHECK(false);
        ok = false;
        break;
      }
    }

    /* Close the file after reading */
    status = fclose (file);
    CHECK(status == 0);
    if (status != 0) {
      ok = false;
    }

    read_time = perf_timer_get_time();
    CHECK(read_time != 0U);
    if ((!ok) || (read_time == 0U)) {
      MW_CV_FS_DeleteFile (PERF_FILE_NAME);
      return;
    }

    /* Report the results for the current chunk size */
    DETAIL_INFO("C library: block %u bytes, write %u KiB/s, read %u KiB/s",
                (unsigned)chunk,
                (unsigned)transfer_rate (PERF_TRANSFER_BYTES, write_time),
                (unsigned)transfer_rate (PERF_TRANSFER_BYTES, read_time));
  }

  MW_CV_FS_DeleteFile (PERF_FILE_NAME);
}
#endif


#if defined(__DOXYGEN__) || ((MW_CV_FS_IOC != 0U) && (MW_CV_FS_DRIVE_F == 0U))
/**
\brief Measure FileSystem IOC performance.
\details
\par Method
Format and lock the FAT drive, then measure sequential sector writes and reads from sector zero.
The formatted volume is disposable because this test runs after all functional and stress tests.
\par Expected
- Formatting succeeds and the media contains enough sectors for at least one transfer.
- Every sector transfer succeeds and read data matches the written pattern.
- Nonzero write and read durations and their corresponding KiB/s rates are reported.
*/
void MW_CV_FS_Performance_IOC (void) {
  uint8_t    *pattern = (uint8_t *)perf_pattern_words;
  uint8_t    *buffer  = (uint8_t *)perf_data_words;
  fsMediaInfo info;
  fsStatus    status;
  uint64_t    transferred;
  uint32_t    bytes;
  uint32_t    chunk_sectors;
  uint32_t    iterations;
  uint32_t    max_sectors;
  uint32_t    read_time;
  uint32_t    sector;
  uint32_t    write_time;
  uint32_t    i;
  int32_t     id;

  status = MW_CV_FS_Format (NULL);
  CHECK(status == fsOK);
  if (status != fsOK) {
    return;
  }
  id = fs_ioc_get_id (MW_CV_FS_DRIVE);
  CHECK(id >= 0);
  if (id < 0) {
    return;
  }

  status = fs_ioc_lock (id);
  CHECK(status == fsOK);
  if (status != fsOK) {
    return;
  }

  status = fs_ioc_read_info (id, &info);
  CHECK(status == fsOK);
  if (status != fsOK) {
    CHECK(fs_ioc_unlock (id) == fsOK);
    return;
  }

  CHECK((info.read_blen != 0U) && (info.read_blen == info.write_blen));
  if ((info.read_blen == 0U) || (info.read_blen != info.write_blen)) {
    CHECK(fs_ioc_unlock (id) == fsOK);
    return;
  }

  /* Determine the maximum number of sectors that fit in the performance buffer */
  max_sectors = PERF_BUFFER_SIZE / info.read_blen;
  if (max_sectors > info.block_cnt) {
    max_sectors = info.block_cnt;
  }
  CHECK(max_sectors != 0U);
  if (max_sectors == 0U) {
    CHECK(fs_ioc_unlock (id) == fsOK);
    return;
  }

  prepare_data();

  /* Iterate over chunk sizes, doubling each time until the maximum number of sectors is reached */
  for (chunk_sectors = 1U; chunk_sectors <= max_sectors; chunk_sectors *= 2U) {
    /* Calculate the number of bytes for the current chunk size */
    bytes = chunk_sectors * info.read_blen;

    /* Calculate the number of iterations needed to transfer PERF_TRANSFER_BYTES with the current chunk size */
    iterations = (PERF_TRANSFER_BYTES + bytes - 1U) / bytes;
    if (iterations > (info.block_cnt / chunk_sectors)) {
      iterations = info.block_cnt / chunk_sectors;
    }

    /* Calculate the total number of bytes that will be transferred with the current chunk size and iterations */
    transferred = (uint64_t)iterations * bytes;

    /* Measure write performance for the current chunk size */
    sector = 0U;
    perf_timer_reset();
    for (i = 0U; i < iterations; i++) {
      perf_timer_start();
      status = fs_ioc_write_sector (id, sector, pattern, chunk_sectors);
      perf_timer_stop();
      if (status != fsOK) {
        CHECK(status == fsOK);
        break;
      }
      sector += chunk_sectors;
    }
    write_time = perf_timer_get_time();

    if (status == fsOK) {
      /* Measure read performance for the current chunk size */
      sector = 0U;
      perf_timer_reset();
      for (i = 0U; i < iterations; i++) {
        perf_timer_start();
        status = fs_ioc_read_sector (id, sector, buffer, chunk_sectors);
        perf_timer_stop();
        if (status != fsOK) {
          CHECK(status == fsOK);
          break;
        }
        if (memcmp (buffer, pattern, bytes) != 0) {
          CHECK(false);
          status = fsError;
          break;
        }
        sector += chunk_sectors;
      }
      read_time = perf_timer_get_time();
    }
    else {
      read_time = 0U;
    }

    if (status == fsOK) {
      CHECK(write_time != 0U);
      CHECK(read_time != 0U);

      if ((write_time != 0U) && (read_time != 0U)) {
        /* Report the results for the current chunk size */
        DETAIL_INFO("IOC: block %u bytes, write %u KiB/s, read %u KiB/s",
                    (unsigned)bytes,
                    (unsigned)transfer_rate (transferred, write_time),
                    (unsigned)transfer_rate (transferred, read_time));
      }
    }
    if (status != fsOK) {
      break;
    }

    if (chunk_sectors > (max_sectors / 2U)) {
      break;
    }
  }

  CHECK(fs_ioc_unlock (id) == fsOK);
}
#endif

/** @} */

#endif /* FileSystem performance */
