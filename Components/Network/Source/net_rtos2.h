/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_rtos2.h
 * Purpose: Network CMSIS-RTOS2 abstraction layer
 *----------------------------------------------------------------------------*/

#include "cmsis_os2.h"
#include "net_lib.h"

/* Network core resources */
#ifdef RTE_CMSIS_RTOS2_RTX5
  #include "rtx_os.h"
  static osRtxMutex_t  net_lock_cb    __attribute__((section(".bss.os.mutex.cb")));
  static osRtxMutex_t  mem_lock_cb    __attribute__((section(".bss.os.mutex.cb")));
  static osRtxTimer_t  net_timer_cb   __attribute__((section(".bss.os.timer.cb")));
  static osRtxThread_t net_thread_cb  __attribute__((section(".bss.os.thread.cb")));
  static uint64_t      net_stack[NET_THREAD_STACK_SIZE/8];
  #define __NET_LOCK   &net_lock_cb,   sizeof(net_lock_cb)
  #define __MEM_LOCK   &mem_lock_cb,   sizeof(mem_lock_cb)
  #define __NET_TICK   &net_timer_cb,  sizeof(net_timer_cb)
  #define __NET_TCB    &net_thread_cb, sizeof(net_thread_cb)
  #define __NET_STACK  &net_stack,     sizeof(net_stack)
#else
  #define __NET_LOCK   NULL, 0
  #define __MEM_LOCK   NULL, 0
  #define __NET_TICK   NULL, 0
  #define __NET_TCB    NULL, 0
  #define __NET_STACK  NULL, NET_THREAD_STACK_SIZE
#endif

static const osMutexAttr_t net_lock_attr = {
  "netCore_Mutex",
  osMutexPrioInherit | osMutexRecursive | osMutexRobust,
  __NET_LOCK
};

static const osMutexAttr_t mem_lock_attr = {
  "netMemory_Mutex",
  osMutexPrioInherit | osMutexRobust,
  __MEM_LOCK
};

static const osTimerAttr_t net_timer_attr = {
  "netCore_Timer",
  0,
  __NET_TICK
};

static const osThreadAttr_t net_thread_attr = {
  "netCore_Thread",
  osThreadDetached,
  __NET_TCB,
  __NET_STACK,
  NET_THREAD_PRIORITY,
  0,
  0
};

/* ETH0 interface resources */
#if (ETH0_ENABLE)
#ifdef RTE_CMSIS_RTOS2_RTX5
  static osRtxSemaphore_t eth0_lock_cb   __attribute__((section(".bss.os.semaphore.cb")));
  static osRtxThread_t    eth0_thread_cb __attribute__((section(".bss.os.thread.cb")));
  static uint64_t         eth0_stack[ETH0_THREAD_STACK_SIZE/8];
  #define __ETH0_LOCK     &eth0_lock_cb,   sizeof(eth0_lock_cb)
  #define __ETH0_TCB      &eth0_thread_cb, sizeof(eth0_thread_cb)
  #define __ETH0_STACK    &eth0_stack,     sizeof(eth0_stack)
#else
  #define __ETH0_LOCK     NULL, 0
  #define __ETH0_TCB      NULL, 0
  #define __ETH0_STACK    NULL, ETH0_THREAD_STACK_SIZE
#endif

static const osSemaphoreAttr_t eth0_lock_attr = {
  "netEth0_Semaphore",
  0,
  __ETH0_LOCK
};

static const osThreadAttr_t eth0_thread_attr = {
  "netEth0_Thread",
  osThreadDetached,
  __ETH0_TCB,
  __ETH0_STACK,
  ETH0_THREAD_PRIORITY,
  0,
  0
};
#endif

/* ETH1 interface resources */
#if (ETH1_ENABLE)
#ifdef RTE_CMSIS_RTOS2_RTX5
  static osRtxSemaphore_t eth1_lock_cb   __attribute__((section(".bss.os.semaphore.cb")));
  static osRtxThread_t    eth1_thread_cb __attribute__((section(".bss.os.thread.cb")));
  static uint64_t         eth1_stack[ETH1_THREAD_STACK_SIZE/8];
  #define __ETH1_LOCK     &eth1_lock_cb,   sizeof(eth1_lock_cb)
  #define __ETH1_TCB      &eth1_thread_cb, sizeof(eth1_thread_cb)
  #define __ETH1_STACK    &eth1_stack,     sizeof(eth1_stack)
#else
  #define __ETH1_LOCK     NULL, 0
  #define __ETH1_TCB      NULL, 0
  #define __ETH1_STACK    NULL, ETH1_THREAD_STACK_SIZE
#endif

static const osSemaphoreAttr_t eth1_lock_attr = {
  "netEth1_Semaphore",
  0,
  __ETH1_LOCK
};

static const osThreadAttr_t eth1_thread_attr = {
  "netEth1_Thread",
  osThreadDetached,
  __ETH1_TCB,
  __ETH1_STACK,
  ETH1_THREAD_PRIORITY,
  0,
  0
};
#endif

/* WIFI0 interface resources */
#if (WIFI0_ENABLE)
#ifdef RTE_CMSIS_RTOS2_RTX5
  static osRtxSemaphore_t wifi0_lock_cb   __attribute__((section(".bss.os.semaphore.cb")));
  static osRtxThread_t    wifi0_thread_cb __attribute__((section(".bss.os.thread.cb")));
  static uint64_t         wifi0_stack[WIFI0_THREAD_STACK_SIZE/8];
  #define __WIFI0_LOCK    &wifi0_lock_cb,   sizeof(wifi0_lock_cb)
  #define __WIFI0_TCB     &wifi0_thread_cb, sizeof(wifi0_thread_cb)
  #define __WIFI0_STACK   &wifi0_stack,     sizeof(wifi0_stack)
#else
  #define __WIFI0_LOCK    NULL, 0
  #define __WIFI0_TCB     NULL, 0
  #define __WIFI0_STACK   NULL, WIFI0_THREAD_STACK_SIZE
#endif

static const osSemaphoreAttr_t wifi0_lock_attr = {
  "netWiFi0_Semaphore",
  0,
  __WIFI0_LOCK
};

static const osThreadAttr_t wifi0_thread_attr = {
  "netWiFi0_Thread",
  osThreadDetached,
  __WIFI0_TCB,
  __WIFI0_STACK,
  WIFI0_THREAD_PRIORITY,
  0,
  0
};
#endif

/* WIFI1 interface resources */
#if (WIFI1_ENABLE)
#ifdef RTE_CMSIS_RTOS2_RTX5
  static osRtxSemaphore_t wifi1_lock_cb   __attribute__((section(".bss.os.semaphore.cb")));
  static osRtxThread_t    wifi1_thread_cb __attribute__((section(".bss.os.thread.cb")));
  static uint64_t         wifi1_stack[WIFI1_THREAD_STACK_SIZE/8];
  #define __WIFI1_LOCK    &wifi1_lock_cb,   sizeof(wifi1_lock_cb)
  #define __WIFI1_TCB     &wifi1_thread_cb, sizeof(wifi1_thread_cb)
  #define __WIFI1_STACK   &wifi1_stack,     sizeof(wifi1_stack)
#else
  #define __WIFI1_LOCK    NULL, 0
  #define __WIFI1_TCB     NULL, 0
  #define __WIFI1_STACK   NULL, WIFI1_THREAD_STACK_SIZE
#endif

static const osSemaphoreAttr_t wifi1_lock_attr = {
  "netWiFi1_Semaphore",
  0,
  __WIFI1_LOCK
};

static const osThreadAttr_t wifi1_thread_attr = {
  "netWiFi1_Thread",
  osThreadDetached,
  __WIFI1_TCB,
  __WIFI1_STACK,
  WIFI1_THREAD_PRIORITY,
  0,
  0
};
#endif

/* PPP interface resources */
#if (PPP_ENABLE)
#ifdef RTE_CMSIS_RTOS2_RTX5
  static osRtxSemaphore_t ppp0_lock_cb    __attribute__((section(".bss.os.semaphore.cb")));
  static osRtxThread_t    ppp0_thread_cb  __attribute__((section(".bss.os.thread.cb")));
  static uint64_t         ppp0_stack[PPP_THREAD_STACK_SIZE/8];
  #define __PPP0_LOCK     &ppp0_lock_cb,   sizeof(ppp0_lock_cb)
  #define __PPP0_TCB      &ppp0_thread_cb, sizeof(ppp0_thread_cb)
  #define __PPP0_STACK    &ppp0_stack,     sizeof(ppp0_stack)
#else
  #define __PPP0_LOCK     NULL, 0
  #define __PPP0_TCB      NULL, 0
  #define __PPP0_STACK    NULL, PPP_THREAD_STACK_SIZE
#endif

static const osSemaphoreAttr_t ppp0_lock_attr = {
  "netPPP_Semaphore",
  0,
  __PPP0_LOCK
};

static const osThreadAttr_t ppp0_thread_attr = {
  "netPPP_Thread",
  osThreadDetached,
  __PPP0_TCB,
  __PPP0_STACK,
  PPP_THREAD_PRIORITY,
  0,
  0
};
#endif

/* SLIP interface resources */
#if (SLIP_ENABLE)
#ifdef RTE_CMSIS_RTOS2_RTX5
  static osRtxSemaphore_t slip0_lock_cb   __attribute__((section(".bss.os.semaphore.cb")));
  static osRtxThread_t    slip0_thread_cb __attribute__((section(".bss.os.thread.cb")));
  static uint64_t         slip0_stack[SLIP_THREAD_STACK_SIZE/8];
  #define __SLIP0_LOCK    &slip0_lock_cb,   sizeof(slip0_lock_cb)
  #define __SLIP0_TCB     &slip0_thread_cb, sizeof(slip0_thread_cb)
  #define __SLIP0_STACK   &slip0_stack,     sizeof(slip0_stack)
#else
  #define __SLIP0_LOCK    NULL, 0
  #define __SLIP0_TCB     NULL, 0
  #define __SLIP0_STACK   NULL, SLIP_THREAD_STACK_SIZE
#endif

static const osSemaphoreAttr_t slip0_lock_attr = {
  "netSLIP_Semaphore",
  0,
  __SLIP0_LOCK
};

static const osThreadAttr_t slip0_thread_attr = {
  "netSLIP_Thread",
  osThreadDetached,
  __SLIP0_TCB,
  __SLIP0_STACK,
  SLIP_THREAD_PRIORITY,
  0,
  0
};
#endif

static uint32_t k_mul;

/* Convert timeout to system ticks */
static uint32_t ms2tick (uint32_t ms) {
  uint64_t val;

  if (ms == osWaitForever) {
    return (ms);
  }
  val = ((uint64_t)ms * k_mul + 1023) >> 10;
  if (val < 0xFFFFFFFF) {
    return ((uint32_t)val);
  }
  return (0xFFFFFFFE);
}

/* Initialize OS abstraction layer */
void netos_init (void) {
  k_mul = (osKernelGetTickFreq () << 10) / 1000;
}

/* Create network core thread */
NETOS_ID netos_thread_create (void) {
  return (osThreadNew (&netCore_Thread, NULL, &net_thread_attr));
}

/* Delete network thread */
void netos_thread_delete (NETOS_ID thread) {
  osThreadTerminate (thread);
}

/* Get running thread identifier */
NETOS_ID netos_thread_id (void) {
  return (osThreadGetId ());
}

/* Pass control to next ready thread */
void netos_thread_pass (void) {
  osThreadYield ();
}

/* Create periodic tick timer */
NETOS_ID netos_timer_create (void) {
  return (osTimerNew (&net_sys_tick, osTimerPeriodic, NULL, &net_timer_attr));
}

/* Delete periodic tick timer */
void netos_timer_delete (NETOS_ID timer) {
  osTimerDelete (timer);
}

/* Start periodic tick timer */
void netos_timer_start (NETOS_ID timer, uint32_t interval_ms) {
  osTimerStart (timer, ms2tick(interval_ms));
}

/* Create network protection mutex */
NETOS_ID netos_mutex_create (uint8_t sys_id) {
  switch (sys_id) {
    case 0:  return (osMutexNew (&net_lock_attr));
    default: return (osMutexNew (&mem_lock_attr));
  }
}

/* Delete network protection mutex */
void netos_mutex_delete (NETOS_ID mutex) {
  osMutexDelete (mutex);
}

/* Lock network protection mutex */
void netos_lock (NETOS_ID mutex) {
  osMutexAcquire (mutex, osWaitForever);
}

/* Unlock network protection mutex */
void netos_unlock (NETOS_ID mutex) {
  osMutexRelease (mutex);
}

/* Wait for thread signal/event flag */
void netos_flag_wait (uint32_t flag, uint32_t ms) {
  osThreadFlagsWait (flag, osFlagsWaitAny, ms2tick(ms));
}

/* Set thread signal/event flag */
void netos_flag_set (NETOS_ID thread, uint32_t flag) {
  osThreadFlagsSet (thread, flag);
}

/* Clear thread signal/event flag */
void netos_flag_clear (NETOS_ID thread, uint32_t flag) {
  (void)thread;
  osThreadFlagsClear (flag);
}

/* Delay thread execution */
void netos_delay (uint32_t ms) {
  osDelay (ms2tick(ms));
}

/* Create network interface thread and semaphore */
NETOS_ID netif_create (uint32_t if_id, NETOS_ID *semaphore) {
  switch (if_id) {
#if (ETH0_ENABLE)
    case NET_IF_CLASS_ETH:
      *semaphore = osSemaphoreNew (1, 1, &eth0_lock_attr);
      return (osThreadNew (&netETH_Thread,
                           (void *)(uint32_t)&eth0_ll_config, &eth0_thread_attr));
#endif
#if (ETH1_ENABLE)
    case NET_IF_CLASS_ETH+1:
      *semaphore = osSemaphoreNew (1, 1, &eth1_lock_attr);
      return (osThreadNew (&netETH_Thread,
                           (void *)(uint32_t)&eth1_ll_config, &eth1_thread_attr));
#endif
#if (WIFI0_ENABLE)
    case NET_IF_CLASS_WIFI:
      *semaphore = osSemaphoreNew (1, 1, &wifi0_lock_attr);
      return (osThreadNew (&netWiFi_Thread,
                           (void *)(uint32_t)&wifi0_ll_config, &wifi0_thread_attr));
#endif
#if (WIFI1_ENABLE)
    case NET_IF_CLASS_WIFI+1:
      *semaphore = osSemaphoreNew (1, 1, &wifi1_lock_attr);
      return (osThreadNew (&netWiFi_Thread,
                           (void *)(uint32_t)&wifi1_ll_config, &wifi1_thread_attr));
#endif
#if (PPP_ENABLE)
    case NET_IF_CLASS_PPP:
      *semaphore = osSemaphoreNew (1, 1, &ppp0_lock_attr);
      return (osThreadNew (&netPPP_Thread,
                           (void *)(uint32_t)&ppp0_ll_config, &ppp0_thread_attr));
#endif
#if (SLIP_ENABLE)
    case NET_IF_CLASS_SLIP:
      *semaphore = osSemaphoreNew (1, 1, &slip0_lock_attr);
      return (osThreadNew (&netSLIP_Thread,
                           (void *)(uint32_t)&slip0_ll_config, &slip0_thread_attr));
#endif
  }
  return (NULL);
}

/* Delete network interface thread and semaphore */
void netif_delete (NETOS_ID thread, NETOS_ID semaphore) {
  osSemaphoreDelete (semaphore);
  osThreadTerminate (thread);
}

/* Lock interface protection semaphore */
void netif_lock (NETOS_ID semaphore) {
  osSemaphoreAcquire (semaphore, osWaitForever);
}

/* Unlock interface protection semaphore */
void netif_unlock (NETOS_ID semaphore) {
  osSemaphoreRelease (semaphore);
}
