/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_TestReport.h
 * Purpose: MDK Middleware - Component Validation - Test Report header
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_TEST_REPORT_H_
#define MW_CV_TEST_REPORT_H_

#include <stdbool.h>
#include <stdint.h>

/* Macros usable in tests */
#define ASSERT_TRUE(...)                TestReportAssertTrue(__FILE__, __LINE__, __VA_ARGS__);
#define DETAIL_INFO(...)                TestReportDetailInfo(__VA_ARGS__);

/* Module exported global functions */
extern void TestReportBegin             (const char *title, const char *date, const char *time, const char *file);
extern void TestReportEnd               (void);
extern void TestReportUnitTestBegin     (uint8_t id, const char *name, uint32_t ver);
extern void TestReportUnitTestEnd       (uint32_t time_in_sec);
extern void TestReportTestCaseBegin     (uint32_t idx, const char *name);
extern void TestReportTestCaseEnd       (void);
extern void TestReportAssertTrue        (const char *module_name, uint32_t module_line, bool cond, const char *format, ...);
extern void TestReportDetailInfo        (const char *format, ...);

#endif // MW_CV_TEST_REPORT_H_
