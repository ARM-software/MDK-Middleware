/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_rtos2.h
 * Purpose: File System RTOS abstraction for CMSIS-RTOS2
 *----------------------------------------------------------------------------*/

#include "fs_core.h"

#if (NOR0_ENABLE)

  #if defined (FS_RTOS_RTX5)
  /* CMSIS RTOS2 RTX5 */
  static osRtxMutex_t  fs_nor0_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
  static
  const osMutexAttr_t  fs_nor0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_nor0_mtx_cb, sizeof(osRtxMutex_t) };
  #else
  /* CMSIS RTOS2 (dynamic memory allocation) */
  static
  const osMutexAttr_t  fs_nor0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
  #endif
#endif

#if (NOR1_ENABLE)
  #if defined (FS_RTOS_RTX5)
  /* CMSIS RTOS2 RTX5 */
  static osRtxMutex_t  fs_nor1_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
  static
  const osMutexAttr_t  fs_nor1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_nor1_mtx_cb, sizeof(osRtxMutex_t) };
  #else
  /* CMSIS RTOS2 (dynamic memory allocation) */
  static
  const osMutexAttr_t  fs_nor1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
  #endif
#endif

#if (MC0_ENABLE)
  #if defined (FS_RTOS_RTX5)
  /* CMSIS RTOS2 RTX5 */
  static osRtxMutex_t fs_mc0_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
  static
  const osMutexAttr_t fs_mc0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_mc0_mtx_cb, sizeof(osRtxMutex_t) };
  #else
  /* CMSIS RTOS2 (dynamic memory allocation) */
  static
  const osMutexAttr_t fs_mc0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
  #endif
#endif

#if (MC1_ENABLE)
  #if defined (FS_RTOS_RTX5)
  /* CMSIS RTOS2 RTX5 */
  static osRtxMutex_t fs_mc1_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
  static
  const osMutexAttr_t fs_mc1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_mc1_mtx_cb, sizeof(osRtxMutex_t) };
  #else
  /* CMSIS RTOS2 (dynamic memory allocation) */
  static
  const osMutexAttr_t fs_mc1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
  #endif
#endif

#if (NAND0_ENABLE)
  #if defined (FS_RTOS_RTX5)
  /* CMSIS RTOS2 RTX5 */
  static osRtxMutex_t   fs_nand0_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
  static
  const osMutexAttr_t   fs_nand0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_nand0_mtx_cb, sizeof(osRtxMutex_t) };
  #else
  /* CMSIS RTOS2 (dynamic memory allocation) */
  static
  const osMutexAttr_t   fs_nand0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
  #endif
#endif

#if (NAND1_ENABLE)
  #if defined (FS_RTOS_RTX5)
  /* CMSIS RTOS2 RTX5 */
  static osRtxMutex_t   fs_nand1_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
  static
  const osMutexAttr_t   fs_nand1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_nand1_mtx_cb, sizeof(osRtxMutex_t) };
  #else
  /* CMSIS RTOS2 (dynamic memory allocation) */
  static
  const osMutexAttr_t   fs_nand1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
  #endif
#endif

#if (RAM0_ENABLE)
  #if defined (FS_RTOS_RTX5)
  /* CMSIS RTOS2 RTX5 */
  static osRtxMutex_t  fs_ram0_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
  static
  const osMutexAttr_t  fs_ram0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_ram0_mtx_cb, sizeof(osRtxMutex_t) };
  #else
  /* CMSIS RTOS2 (dynamic memory allocation) */
  static
  const osMutexAttr_t  fs_ram0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
  #endif
#endif

#if (RAM1_ENABLE)
  #if defined (FS_RTOS_RTX5)
  /* CMSIS RTOS2 RTX5 */
  static osRtxMutex_t  fs_ram1_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
  static
  const osMutexAttr_t  fs_ram1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_ram1_mtx_cb, sizeof(osRtxMutex_t) };
  #else
  /* CMSIS RTOS2 (dynamic memory allocation) */
  static
  const osMutexAttr_t  fs_ram1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
  #endif
#endif

#if (USB0_ENABLE)
  #if defined (FS_RTOS_RTX5)
  /* CMSIS RTOS2 RTX5 */
  static osRtxMutex_t  fs_usb0_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
  static
  const osMutexAttr_t  fs_usb0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_usb0_mtx_cb, sizeof(osRtxMutex_t) };
  #else
  /* CMSIS RTOS2 (dynamic memory allocation) */
  static
  const osMutexAttr_t  fs_usb0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
  #endif
#endif

#if (USB1_ENABLE)
  #if defined (FS_RTOS_RTX5)
  /* CMSIS RTOS2 RTX5 */
  static osRtxMutex_t  fs_usb1_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
  static
  const osMutexAttr_t  fs_usb1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_usb1_mtx_cb, sizeof(osRtxMutex_t) };
  #else
  /* CMSIS RTOS2 (dynamic memory allocation) */
  static
  const osMutexAttr_t  fs_usb1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
  #endif
#endif

/*
  Create and initialize a mutex object
*/
FS_MUTEX fs_mutex_new (const void *arg) {
  const char *drive = (const char *)arg;
  const osMutexAttr_t *attr;

  if (drive == NULL) {
    attr = NULL;
  }
#if (NOR0_ENABLE)
  else if (drive[0] == 'F' && drive[1] == '0') {
    attr = &fs_nor0_mtx_at;
  }
#endif
#if (NOR1_ENABLE)
  else if (drive[0] == 'F' && drive[1] == '1') {
    attr = &fs_nor1_mtx_at;
  }
#endif
#if (MC0_ENABLE)
  else if (drive[0] == 'M' && drive[1] == '0') {
    attr = &fs_mc0_mtx_at;
  }
#endif
#if (MC1_ENABLE)
  else if (drive[0] == 'M' && drive[1] == '1') {
    attr = &fs_mc1_mtx_at;
  }
#endif
#if (NAND0_ENABLE)
  else if (drive[0] == 'N' && drive[1] == '0') {
    attr = &fs_nand0_mtx_at;
  }
#endif
#if (NAND1_ENABLE)
  else if (drive[0] == 'N' && drive[1] == '1') {
    attr = &fs_nand1_mtx_at;
  }
#endif
#if (RAM0_ENABLE)
  else if (drive[0] == 'R' && drive[1] == '0') {
    attr = &fs_ram0_mtx_at;
  }
#endif
#if (RAM1_ENABLE)
  else if (drive[0] == 'R' && drive[1] == '1') {
    attr = &fs_ram1_mtx_at;
  }
#endif
#if (USB0_ENABLE)
  else if (drive[0] == 'U' && drive[1] == '0') {
    attr = &fs_usb0_mtx_at;
  }
#endif
#if (USB1_ENABLE)
  else if (drive[0] == 'U' && drive[1] == '1') {
    attr = &fs_usb1_mtx_at;
  }
#endif
  else {
    /* Unknown drive */
    attr = NULL;
  }

  return ((FS_MUTEX)osMutexNew (attr));
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
