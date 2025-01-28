/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation
 * Copyright (c) 2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_Main.c
 * Purpose: Middleware - Component Validation Application Main
 *----------------------------------------------------------------------------*/

#include "main.h"

#include "cmsis_os2.h"

#include "MW_CV.h"

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (4096U)
static uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
static const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

/*-----------------------------------------------------------------------------
 * Application main thread
 *----------------------------------------------------------------------------*/
void app_main_thread (void *argument) {
  (void)argument;

  MW_CV();                              // Execute tests

  osThreadExit();
}

/*-----------------------------------------------------------------------------
 *        Application main function
 *----------------------------------------------------------------------------*/
int app_main (void) {
  osKernelInitialize();
  osThreadNew(app_main_thread, NULL, &app_main_attr);
  osKernelStart();
  return 0;
}
