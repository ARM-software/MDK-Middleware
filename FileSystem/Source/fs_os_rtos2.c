/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_rtos2.h
 * Purpose: File System RTOS abstraction for CMSIS-RTOS2
 *----------------------------------------------------------------------------*/

#include "rl_fs_lib.h"
#include "cmsis_os2.h"

/*
  Create and initialize a mutex object
*/
FS_MUTEX fs_mutex_new (const void *arg) {
  return ((FS_MUTEX)osMutexNew (arg));
}
/*
  Acquire a mutex.
*/
uint32_t fs_mutex_acquire (FS_MUTEX mutex) {
  uint32_t status = 0U;

  if (osMutexAcquire ((osMutexId_t)mutex, osWaitForever) != osOK) {
    status = 1U;
  }
  return (status);
}
/*
  Release a mutex.
*/
uint32_t fs_mutex_release (FS_MUTEX mutex) {
  uint32_t status = 0U;

  if (osMutexRelease ((osMutexId_t)mutex) != osOK) {
    status = 1U;
  }
  return (status);
}
/*
  Delete a mutex object.
*/
uint32_t fs_mutex_delete (FS_MUTEX mutex) {
  uint32_t status = 0U;

  if (osMutexDelete ((osMutexId_t)mutex) != osOK) {
    status = 1U;
  }
  return (status);
}
/*
  Get the RTOS kernel tick frequency
*/
uint32_t fs_get_rtos_tick_freq (void) {
  return osKernelGetTickFreq();
}
/*
  Number of RTOS ticks in a millisecond
*/
uint32_t fs_ms_rtos_tick;
/*
  Wait for Timeout
*/
uint32_t fs_set_rtos_delay (uint32_t millisec) {
  osDelay (millisec * fs_ms_rtos_tick);
  return (0);
}
/*
  Get the RTOS kernel system timer count.
*/
uint32_t fs_get_sys_tick (void) {
  return (osKernelGetSysTimerCount());
}
/*
  Convert a microseconds value to a RTOS kernel system timer value.
*/
uint32_t fs_get_sys_tick_us (uint32_t microsec) {
  return ((uint32_t)(((uint64_t)microsec * osKernelGetSysTimerFreq()) / 1000000U));
}
