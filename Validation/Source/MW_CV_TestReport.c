/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_TestReport.c
 * Purpose: MDK Middleware - Component Validation - Test Report module
 *----------------------------------------------------------------------------*/

#include "MW_CV_TestReport.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <cmsis_compiler.h>


/*-----------------------------------------------------------------------------
 * Local module variables
 *----------------------------------------------------------------------------*/
static uint32_t tests;                  /* Unit Test total test cases count   */
static uint32_t passed;                 /* Unit Test total test cases passed  */
static uint32_t failed;                 /* Unit Test total test cases failed  */
static uint32_t asserts_passed;         /* Test Case assertions passed        */
static uint32_t asserts_failed;         /* Test Case assertions failed        */

/*-----------------------------------------------------------------------------
 * Test Report begin
 *----------------------------------------------------------------------------*/
void TestReportBegin (const char *title, const char *date, const char *time, const char *file) {
  (void)file;

  printf("%s   %s   %s \n\n", title, date, time);
}

/*-----------------------------------------------------------------------------
 * Test Report end
 *----------------------------------------------------------------------------*/
void TestReportEnd (void) {
  fflush(stdout);
}

/*-----------------------------------------------------------------------------
 * Test Report Unit Test begin
 *----------------------------------------------------------------------------*/
void TestReportUnitTestBegin (uint8_t id, const char *name, uint32_t ver) {
  uint32_t major, minor, patch;

  tests    = 0U;
  passed   = 0U;
  failed   = 0U;

  major =  ver / 10000000U;
  minor = (ver % 10000000U) / 10000U;
  patch = (ver %    10000U);

  printf("Unit Test %02u: %-32s v%u.%u.%u\n", (unsigned int)id, name, (unsigned int)major, (unsigned int)minor, (unsigned int)patch);
}

/*-----------------------------------------------------------------------------
 * Test Report Unit Test end
 *----------------------------------------------------------------------------*/
void TestReportUnitTestEnd (uint32_t time_in_sec) {
  const char *result_str;

  (void)time_in_sec;

  if (failed > 0U) {                    /* If any Test Case failed            */
    result_str = "FAILED";
  } else if (passed > 0U) {             /* If any Test Case passed            */
    result_str = "PASSED";
  } else {                              /* If no Test Case was executed       */
    result_str = "SKIPPED";
  }

  printf("\nUnit Test Time:    %u min %u sec", (unsigned int)(time_in_sec / 60), (unsigned int)(time_in_sec % 60));
  printf("\nUnit Test Summary: %u Tests, %u Executed, %u Passed, %u Failed.\n",
         (unsigned int)tests,
         (unsigned int)(passed+failed),
         (unsigned int)passed,
         (unsigned int)failed);
  printf("Unit Test Result:  %s\n\n\n", result_str);

  fflush(stdout);
}

/*-----------------------------------------------------------------------------
 * Test Report Test Case begin
 *----------------------------------------------------------------------------*/
void TestReportTestCaseBegin (uint32_t idx, const char *name) {

  asserts_passed = 0U;
  asserts_failed = 0U;

  printf("Test Case %02u: %-41s ", (unsigned int)idx, name);
}

/*-----------------------------------------------------------------------------
 * Test Report Test Case end
 *----------------------------------------------------------------------------*/
void TestReportTestCaseEnd (void) {
  const char *result_str;

  tests++;
  if (asserts_failed > 0U) {            /* If any assert in Test Case failed  */
    failed++;
    result_str = "FAILED";
  } else if (asserts_passed > 0U) {     /* If any assert in Test Case passed  */
    passed++;
    result_str = "PASSED";
  } else {                              /* If no assert during Test Case      */
    result_str = "SKIPPED";
  }

  if (result_str[0] == 'F' || result_str[0] == 'W') {
    printf("\n                                                        ");
  }
  printf("(%s)\n", result_str);

  fflush(stdout);
}

/*-----------------------------------------------------------------------------
 * Test Report Assert True
 *----------------------------------------------------------------------------*/
void TestReportAssertTrue (const char *module_name, uint32_t module_line, bool cond, const char *format, ...) {
  const char *cp;
  va_list args;

  if (cond == true) {
    asserts_passed++;
  } else {
    asserts_failed++;
  }

  cp = strrchr(module_name, '/');       /* Skip path from module name         */
  if (cp != NULL) {
    module_name = cp + 1;
  }

  if (cond == false) {                  /* If assertion has failed            */
    printf("\n - %s (%u)", module_name, (unsigned int)module_line);
    if (*format != 0U) {
      va_start(args, format);
      printf(": ");
      vprintf(format, args);
      va_end(args);
    }
  }
}

/*-----------------------------------------------------------------------------
 * Test Report Detail Info (debug message or result without assert)
 *----------------------------------------------------------------------------*/
void TestReportDetailInfo (const char *format, ...) {
  va_list args;

  if (*format != 0U) {
    va_start(args, format);
    printf(": ");
    vprintf(format, args);
    printf("\n");
    printf("                                                        ");
    va_end(args);
  }
}
