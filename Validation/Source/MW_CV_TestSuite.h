/*------------------------------------------------------------------------------
* MDK Middleware - Component Validation
* Copyright (c) 2024-2025 Arm Limited (or its affiliates). All rights reserved.
*------------------------------------------------------------------------------
* Name:    MW_CV_TestSuite.h
* Purpose: MDK Middleware - Component Validation - Tests Suite header
*----------------------------------------------------------------------------*/

#ifndef MW_CV_TEST_SUITE_H_
#define MW_CV_TEST_SUITE_H_

#include "MW_CV_Framework.h"

/* Test suite description structure                                           */
typedef struct {
  char         *report_title;           /* Report title                       */
  char         *date;                   /* Compilation date                   */
  char         *time;                   /* Compilation time                   */
  char         *file;                   /* Test module file                   */
  void (* const Init)(void);            /* Init function callback             */

  TEST_LIST_t  *test_list;              /* List of test entries (unit or case)*/
  uint32_t      num_of_test;            /* Number of test list entries        */
} const TEST_SUITE_t;

// Module exported global structure
extern TEST_SUITE_t test_suite;

#endif // MW_CV_TEST_SUITE_H_
