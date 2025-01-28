/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_Framework.h
 * Purpose: MDK Middleware - Component Validation - Framework header
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_FRAMEWORK_H_
#define MW_CV_FRAMEWORK_H_

#include <stdint.h>

/* Test list entry type definitions                                           */
#define TEST_UNIT_TYPE                  0U
#define TEST_CASE_TYPE                  1U

/* Test list entry definition macros                                          */
#define TEST_UNIT_DEF(name,get_ver_func)        { TEST_UNIT_TYPE,   0U, 0U,        0U, get_ver_func, name }
#define TEST_CASE_DEF(test_func,name,exec)      { TEST_CASE_TYPE, exec, 0U, test_func,           0U, name }

/* Test list entry structure                                                  */
typedef struct {
  uint8_t       type;                   /* Entry type                         */
  uint8_t       execute;                /* Execute setting                    */
  uint16_t      reserved;               /* Reserved for alignment             */
  void       (* TestFunc)(void);        /* Pointer to Test Case function      */
  uint32_t   (* GetVersion)(void);      /* Pointer to GetVersion function     */
  char         *name;                   /* Pointer to unit or test name string*/
} const TEST_LIST_t;

#endif // MW_CV_FRAMEWORK_H_
