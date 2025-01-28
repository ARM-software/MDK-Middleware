/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation
 * Copyright (c) 2018-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_Timer.h
 * Purpose: MDK Middleware - Component Validation - Timer header
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_TIMER_H_
#define MW_CV_TIMER_H_

#include <stdint.h>


extern void     MW_CV_TimerReset  (void);
extern void     MW_CV_TimerStart  (void);
extern void     MW_CV_TimerStop   (void);
extern uint32_t MW_CV_TimerGetTime(void);

#endif // MW_CV_TIMER_H_
