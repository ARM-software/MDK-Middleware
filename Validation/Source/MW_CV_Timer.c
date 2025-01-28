/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_Timer.c
 * Purpose: MDK Middleware - Component Validation - Timer module
 *----------------------------------------------------------------------------*/

#include "MW_CV_Timer.h"

#include "cmsis_os2.h"


static uint32_t StartTimeTicks = 0U;    // Start time in ticks
static uint32_t MeasuredTime   = 0U;    // Measured time in microseconds

// Reset Timer
void MW_CV_TimerReset (void) {
  MeasuredTime = 0U;
}

// Start Timer
void MW_CV_TimerStart (void) {
  StartTimeTicks = osKernelGetTickCount();
}

// Stop Timer
void MW_CV_TimerStop (void) {
  MeasuredTime += (uint32_t)(((osKernelGetTickCount() - StartTimeTicks) * 1000000ULL) / osKernelGetTickFreq());
}

// Get Time in us
uint32_t MW_CV_TimerGetTime (void) {
  return MeasuredTime;
}
