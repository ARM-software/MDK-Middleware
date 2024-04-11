/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_os.h
 * Purpose: File System RTOS Support
 *----------------------------------------------------------------------------*/

#include "rl_fs_lib.h"

/* Include CMSIS RTOS API */
#if !defined (RTE_CMSIS_RTOS) && defined (RTE_CMSIS_RTOS2)
  #include "cmsis_os2.h"
#else
  #error "FileSystem requires CMSIS-RTOS2."
#endif

#if defined (RTE_CMSIS_RTOS2_RTX5)
  #include "rtx_os.h"
#endif

/*----------------------------------------------------------------------------
 *  Drive F0: define fs_nor0_mtx object
 *---------------------------------------------------------------------------*/
#if (NOR0_ENABLE)

#if defined (RTE_CMSIS_RTOS2_RTX5)
/* CMSIS RTOS2 RTX5 */
static osRtxMutex_t  fs_nor0_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
static
const osMutexAttr_t  fs_nor0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_nor0_mtx_cb, sizeof(osRtxMutex_t) };

#else
/* CMSIS RTOS2 (dynamic memory allocation) */
static
const osMutexAttr_t  fs_nor0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
#endif

#define fs_nor0_mtx &fs_nor0_mtx_at

#endif /* NOR0_ENABLE */


/*----------------------------------------------------------------------------
 *  Drive F1: define fs_nor1_mtx object
 *---------------------------------------------------------------------------*/
#if (NOR1_ENABLE)

#if defined (RTE_CMSIS_RTOS2_RTX5)
/* CMSIS RTOS2 RTX5 */
static osRtxMutex_t  fs_nor1_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
static
const osMutexAttr_t  fs_nor1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_nor1_mtx_cb, sizeof(osRtxMutex_t) };

#else
/* CMSIS RTOS2 (dynamic memory allocation) */
static
const osMutexAttr_t  fs_nor1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
#endif

#define fs_nor1_mtx &fs_nor1_mtx_at

#endif /* NOR1_ENABLE */


/*----------------------------------------------------------------------------
 *  Drive M0: define fs_mc0_mtx object
 *---------------------------------------------------------------------------*/
#if (MC0_ENABLE)

#if defined (RTE_CMSIS_RTOS2_RTX5)
/* CMSIS RTOS2 RTX5 */
static osRtxMutex_t fs_mc0_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
static
const osMutexAttr_t fs_mc0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_mc0_mtx_cb, sizeof(osRtxMutex_t) };

#else
/* CMSIS RTOS2 (dynamic memory allocation) */
static
const osMutexAttr_t fs_mc0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
#endif

#define fs_mc0_mtx &fs_mc0_mtx_at

#endif /* MC0_ENABLE */


/*----------------------------------------------------------------------------
 *  Drive M1: define fs_mc1_mtx object
 *---------------------------------------------------------------------------*/
#if (MC1_ENABLE)

#if defined (RTE_CMSIS_RTOS2_RTX5)
/* CMSIS RTOS2 RTX5 */
static osRtxMutex_t fs_mc1_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
static
const osMutexAttr_t fs_mc1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_mc1_mtx_cb, sizeof(osRtxMutex_t) };

#else
/* CMSIS RTOS2 (dynamic memory allocation) */
static
const osMutexAttr_t fs_mc1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
#endif

#define fs_mc1_mtx &fs_mc1_mtx_at

#endif /* MC1_ENABLE */


/*----------------------------------------------------------------------------
 *  Drive N0: define fs_nand0_mtx object
 *---------------------------------------------------------------------------*/
#if (NAND0_ENABLE)

#if defined (RTE_CMSIS_RTOS2_RTX5)
/* CMSIS RTOS2 RTX5 */
static osRtxMutex_t   fs_nand0_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
static
const osMutexAttr_t   fs_nand0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_nand0_mtx_cb, sizeof(osRtxMutex_t) };

#else
/* CMSIS RTOS2 (dynamic memory allocation) */
static
const osMutexAttr_t   fs_nand0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
#endif

#define fs_nand0_mtx &fs_nand0_mtx_at

#endif /* NAND0_ENABLE */


/*----------------------------------------------------------------------------
 *  Drive N1: define fs_nand1_mtx object
 *---------------------------------------------------------------------------*/
#if (NAND1_ENABLE)

#if defined (RTE_CMSIS_RTOS2_RTX5)
/* CMSIS RTOS2 RTX5 */
static osRtxMutex_t   fs_nand1_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
static
const osMutexAttr_t   fs_nand1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_nand1_mtx_cb, sizeof(osRtxMutex_t) };

#else
/* CMSIS RTOS2 (dynamic memory allocation) */
static
const osMutexAttr_t   fs_nand1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
#endif

#define fs_nand1_mtx &fs_nand1_mtx_at

#endif /* NAND1_ENABLE */


/*----------------------------------------------------------------------------
 *  Drive R0: define fs_ram0_mtx object
 *---------------------------------------------------------------------------*/
#if (RAM0_ENABLE)

#if defined (RTE_CMSIS_RTOS2_RTX5)
/* CMSIS RTOS2 RTX5 */
static osRtxMutex_t  fs_ram0_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
static
const osMutexAttr_t  fs_ram0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_ram0_mtx_cb, sizeof(osRtxMutex_t) };

#else
/* CMSIS RTOS2 (dynamic memory allocation) */
static
const osMutexAttr_t  fs_ram0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
#endif

#define fs_ram0_mtx &fs_ram0_mtx_at

#endif /* RAM0_ENABLE */


/*----------------------------------------------------------------------------
 *  Drive R1: define fs_ram1_mtx object
 *---------------------------------------------------------------------------*/
#if (RAM1_ENABLE)

#if defined (RTE_CMSIS_RTOS2_RTX5)
/* CMSIS RTOS2 RTX5 */
static osRtxMutex_t  fs_ram1_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
static
const osMutexAttr_t  fs_ram1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_ram1_mtx_cb, sizeof(osRtxMutex_t) };

#else
/* CMSIS RTOS2 (dynamic memory allocation) */
static
const osMutexAttr_t  fs_ram1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
#endif

#define fs_ram1_mtx &fs_ram1_mtx_at

#endif /* RAM1_ENABLE */


/*----------------------------------------------------------------------------
 *  Drive U0: define fs_usb0_mtx object
 *---------------------------------------------------------------------------*/
#if (USB0_ENABLE)

#if defined (RTE_CMSIS_RTOS2_RTX5)
/* CMSIS RTOS2 RTX5 */
static osRtxMutex_t  fs_usb0_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
static
const osMutexAttr_t  fs_usb0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_usb0_mtx_cb, sizeof(osRtxMutex_t) };

#else
/* CMSIS RTOS2 (dynamic memory allocation) */
static
const osMutexAttr_t  fs_usb0_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
#endif

#define fs_usb0_mtx &fs_usb0_mtx_at

#endif /* USB0_ENABLE */


/*----------------------------------------------------------------------------
 *  Drive U1: define fs_usb1_mtx object
 *---------------------------------------------------------------------------*/
#if (USB1_ENABLE)

#if defined (RTE_CMSIS_RTOS2_RTX5)
/* CMSIS RTOS2 RTX5 */
static osRtxMutex_t  fs_usb1_mtx_cb  __attribute__((section(".bss.os.mutex.cb")));
static
const osMutexAttr_t  fs_usb1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit|osMutexRobust, &fs_usb1_mtx_cb, sizeof(osRtxMutex_t) };

#else
/* CMSIS RTOS2 (dynamic memory allocation) */
static
const osMutexAttr_t  fs_usb1_mtx_at = { NULL, osMutexRecursive|osMutexPrioInherit, NULL, 0 };
#endif

#define fs_usb1_mtx &fs_usb1_mtx_at

#endif /* USB1_ENABLE */


/*-----------------------------------------------------------------------------
 *  RTOS abstraction for FileSystem
 *----------------------------------------------------------------------------*/
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
