/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_Framework.c
 * Purpose: MDK Middleware - Component Validation - Framework module
 *----------------------------------------------------------------------------*/

#include "MW_CV.h"

#include "cmsis_os2.h"
#include "cmsis_compiler.h"

#include "MW_CV_Framework.h"
#include "MW_CV_TestSuite.h"
#include "MW_CV_TestReport.h"


static uint32_t    unit_test_time_in_sec;
static osTimerId_t timer_id;

/* Timer callback function */
void TimerCallback (void *argument) {
  (void)argument;

  unit_test_time_in_sec++;
}


/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\defgroup mw_cv_framework_funcs Framework Functions
\brief The MDK Middleware Component Validation Framework.
\details

@{
*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief This is the exit point of the test framework.
\details
This function is provided for debug reference, so debug session can end when this function is reached.
*/
void __attribute__((noinline)) MW_CV_End (void) {
  __NOP();
}


/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief This is the entry point of the test framework.
\details
This function executes test functions and writes Test Report to standard output.
Program flow:
  -# Call Init function if it is provided by user
  -# Write Test Report beginning to the standard output
  -# Process test list entries, one by one, until the end of the list:
      - If Unit Test entry:
        - Write Test Report Test Case end to the standard output, of last active Test Case
        - Write Test Report Unit Test end to the standard output, of last active Unit Test
        - Write Test Report Unit Test beginning to the standard output, and clear Unit Test statistics
      - If Test Case entry:
        - Write Test Report Test Case end to the standard output, of last active Test Case
        - Execute test function, if Test Case execute flag is true and test function exists
          (test function writes asserts to standard output during execution)
        - Test Report Test Case beginning is written to the standard output
  -# Write Test Report Test Case end to the standard output, of the last active Test Case
  -# Write Test Report Unit Test end to the standard output, of the last active Unit Test
  -# Write Test Report end to the standard output
*/
void MW_CV (void) {
  TEST_LIST_t *ptr_entry;
  uint32_t     entry_idx, unit_test_idx, test_case_idx, ver;

  entry_idx     = 0U;
  unit_test_idx = 255U;
  test_case_idx = 255U;

  if (test_suite.num_of_test == 0U) {
    TestReportBegin("No tests selected!",
                    test_suite.date,
                    test_suite.time,
                    test_suite.file);
    TestReportEnd();
    goto exit;
  }

  timer_id = osTimerNew (TimerCallback, osTimerPeriodic, NULL, NULL);

  /* Init callback allowing user initialization before test suite starts      */
  if (test_suite.Init != NULL) {
    test_suite.Init();                          /* Call user function Init    */
  }

  TestReportBegin(test_suite.report_title,      /* Begin test report          */
                  test_suite.date,
                  test_suite.time,
                  test_suite.file);

  /* Process test list                                                        */
  for (entry_idx = 0; entry_idx < test_suite.num_of_test; entry_idx++) {
    ptr_entry = &test_suite.test_list[entry_idx];
    switch (ptr_entry->type) {
      case TEST_UNIT_TYPE:
        if (test_case_idx != 255U) {
          TestReportTestCaseEnd();              /* End test report test case  */
        }
        test_case_idx = 255U;
        if (unit_test_idx == 255U) {
          unit_test_idx = 0U;
        } else {
          osTimerStop(timer_id);
          TestReportUnitTestEnd(unit_test_time_in_sec); /* End unit test      */
        }
        if (ptr_entry->GetVersion != NULL) {
          ver = ptr_entry->GetVersion();
        } else {
          ver = 0U;
        }
        unit_test_idx++;
        TestReportUnitTestBegin(unit_test_idx,  /* Begin test report unit test*/
                                ptr_entry->name,
                                ver);
        unit_test_time_in_sec = 0U;
        osTimerStart(timer_id, 1000U);
        break;
      case TEST_CASE_TYPE:
        if (test_case_idx == 255U) {
          test_case_idx = 0U;
        } else {
          TestReportTestCaseEnd();              /* End test report test case  */
        }
        test_case_idx++;
        TestReportTestCaseBegin(test_case_idx,  /* Begin test report test case*/
                                ptr_entry->name);
        if ((ptr_entry->execute) &&             /* If test case is for execute*/
            (ptr_entry->TestFunc != NULL)) {    /* If test function is valid  */
          ptr_entry->TestFunc();                /* Execute test case function */
        }
        break;
      default:
        MW_CV_End();
    }
  }
  TestReportTestCaseEnd();                      /* End test report test case  */
  osTimerStop(timer_id);
  TestReportUnitTestEnd(unit_test_time_in_sec); /* End test report unit test  */
  TestReportEnd();                              /* End test report            */

exit:
  MW_CV_End();                                  /* Test framework exit point  */
}

/**
@}
*/
