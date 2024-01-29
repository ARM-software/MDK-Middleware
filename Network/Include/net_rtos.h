/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_rtos.h
 * Purpose: Network CMSIS-RTOS abstraction layer
 *----------------------------------------------------------------------------*/

#include "cmsis_os.h"
#include "rl_net_lib.h"

/* Avoid syntax-checker errors in editor */
#ifndef NET_THREAD_STACK_SIZE
  #define NET_THREAD_STACK_SIZE 1024
  #define NET_THREAD_PRIORITY   osPriorityNormal
#endif

#if (osCMSIS < 0x20000U)
  #define STATIC        static
#else
  #define STATIC
#endif

/* Network core resources */
extern const osMutexDef_t os_mutex_def_net_lock;
extern const osMutexDef_t os_mutex_def_mem_lock;
extern const osTimerDef_t os_timer_def_net_tick;
extern const osThreadDef_t os_thread_def_netCore_Thread;
STATIC osMutexDef(net_lock);
STATIC osMutexDef(mem_lock);
STATIC osTimerDef(net_tick, net_sys_tick);
       osThreadDef(netCore_Thread, NET_THREAD_PRIORITY, 1, NET_THREAD_STACK_SIZE);

/* Ethernet interface resources */
#if (ETH0_ENABLE || ETH1_ENABLE)
static void eth_thread (void const *arg) {
  netETH_Thread (arg);
}
#endif
#if (ETH0_ENABLE)
extern const osThreadDef_t os_thread_def_eth0_thread;
extern const osSemaphoreDef_t os_semaphore_def_eth0_lock;
static void eth0_thread (void const *arg) __attribute__((alias("eth_thread")));
STATIC osSemaphoreDef(eth0_lock);
       osThreadDef(eth0_thread, ETH0_THREAD_PRIORITY, 1, ETH0_THREAD_STACK_SIZE);
#endif
#if (ETH1_ENABLE)
extern const osThreadDef_t os_thread_def_eth1_thread;
extern const osSemaphoreDef_t os_semaphore_def_eth1_lock;
static void eth1_thread (void const *arg) __attribute__((alias("eth_thread")));
STATIC osSemaphoreDef(eth1_lock);
       osThreadDef(eth1_thread, ETH1_THREAD_PRIORITY, 1, ETH1_THREAD_STACK_SIZE);
#endif

/* WiFi interface resources */
#if (WIFI0_ENABLE || WIFI1_ENABLE)
static void wifi_thread (void const *arg) {
  netWiFi_Thread (arg);
}
#endif
#if (WIFI0_ENABLE)
extern const osThreadDef_t os_thread_def_wifi0_thread;
extern const osSemaphoreDef_t os_semaphore_def_wifi0_lock;
static void wifi0_thread (void const *arg) __attribute__((alias("wifi_thread")));
STATIC osSemaphoreDef(wifi0_lock);
       osThreadDef(wifi0_thread, WIFI0_THREAD_PRIORITY, 1, WIFI0_THREAD_STACK_SIZE);
#endif
#if (WIFI1_ENABLE)
extern const osThreadDef_t os_thread_def_wifi1_thread;
extern const osSemaphoreDef_t os_semaphore_def_wifi1_lock;
static void wifi1_thread (void const *arg) __attribute__((alias("wifi_thread")));
STATIC osSemaphoreDef(wifi1_lock);
       osThreadDef(wifi1_thread, WIFI1_THREAD_PRIORITY, 1, WIFI1_THREAD_STACK_SIZE);
#endif

/* PPP interface resources */
#if (PPP_ENABLE)
extern const osThreadDef_t os_thread_def_netPPP_Thread;
extern const osSemaphoreDef_t os_semaphore_def_ppp0_lock;
STATIC osSemaphoreDef(ppp0_lock);
       osThreadDef(netPPP_Thread, PPP_THREAD_PRIORITY, 1, PPP_THREAD_STACK_SIZE);
#endif

/* SLIP interface resources */
#if (SLIP_ENABLE)
extern const osThreadDef_t os_thread_def_netSLIP_Thread;
extern const osSemaphoreDef_t os_semaphore_def_slip0_lock;
STATIC osSemaphoreDef(slip0_lock);
       osThreadDef(netSLIP_Thread, SLIP_THREAD_PRIORITY, 1, SLIP_THREAD_STACK_SIZE);
#endif


/* Initialize OS abstraction layer */
void netos_init (void) {
  return;
}

/* Create network core thread */
NETOS_ID netos_thread_create (void) {
  return (osThreadCreate (osThread(netCore_Thread), NULL));
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
  return (osTimerCreate (osTimer(net_tick), osTimerPeriodic, NULL));
}

/* Delete periodic tick timer */
void netos_timer_delete (NETOS_ID timer) {
  osTimerDelete (timer);
}

/* Start periodic tick timer */
void netos_timer_start (NETOS_ID timer, uint32_t interval_ms) {
  osTimerStart (timer, interval_ms);
}

/* Create network protection mutex */
NETOS_ID netos_mutex_create (uint8_t sys_id) {
  switch (sys_id) {
    case 0:  return (osMutexCreate (osMutex(net_lock)));
    default: return (osMutexCreate (osMutex(mem_lock)));
  }
}

/* Delete network protection mutex */
void netos_mutex_delete (NETOS_ID mutex) {
  osMutexDelete (mutex);
}

/* Lock network protection mutex */
void netos_lock (NETOS_ID mutex) {
  osMutexWait (mutex, osWaitForever);
}

/* Unlock network protection mutex */
void netos_unlock (NETOS_ID mutex) {
  osMutexRelease (mutex);
}

/* Wait for thread signal/event flag */
void netos_flag_wait (uint32_t flag, uint32_t ms) {
  osSignalWait ((int32_t)flag, ms);
}

/* Set thread signal/event flag */
void netos_flag_set (NETOS_ID thread, uint32_t flag) {
  osSignalSet (thread, (int32_t)flag);
}

/* Clear thread signal/event flag */
void netos_flag_clear (NETOS_ID thread, uint32_t flag) {
  osSignalClear (thread, (int32_t)flag);
}

/* Delay thread execution */
void netos_delay (uint32_t ms) {
  osDelay (ms);
}

/* Create network interface (thread and semaphore) */
NETOS_ID netif_create (uint32_t if_id, NETOS_ID *semaphore) {
  switch (if_id) {
#if (ETH0_ENABLE)
    case NET_IF_CLASS_ETH:
      *semaphore = osSemaphoreCreate (osSemaphore(eth0_lock), 1);
      return (osThreadCreate (osThread(eth0_thread), (void *)(uint32_t)&eth0_ll_config));
#endif
#if (ETH1_ENABLE)
    case NET_IF_CLASS_ETH+1:
      *semaphore = osSemaphoreCreate (osSemaphore(eth1_lock), 1);
      return (osThreadCreate (osThread(eth1_thread), (void *)(uint32_t)&eth1_ll_config));
#endif
#if (WIFI0_ENABLE)
    case NET_IF_CLASS_WIFI:
      *semaphore = osSemaphoreCreate (osSemaphore(wifi0_lock), 1);
      return (osThreadCreate (osThread(wifi0_thread), (void *)(uint32_t)&wifi0_ll_config));
#endif
#if (WIFI1_ENABLE)
    case NET_IF_CLASS_WIFI+1:
      *semaphore = osSemaphoreCreate (osSemaphore(wifi1_lock), 1);
      return (osThreadCreate (osThread(wifi1_thread), (void *)(uint32_t)&wifi1_ll_config));
#endif
#if (PPP_ENABLE)
    case NET_IF_CLASS_PPP:
      *semaphore = osSemaphoreCreate (osSemaphore(ppp0_lock), 1);
      return (osThreadCreate (osThread(netPPP_Thread), (void *)(uint32_t)&ppp0_ll_config));
#endif
#if (SLIP_ENABLE)
    case NET_IF_CLASS_SLIP:
      *semaphore = osSemaphoreCreate (osSemaphore(slip0_lock), 1);
      return (osThreadCreate (osThread(netSLIP_Thread), (void *)(uint32_t)&slip0_ll_config));
#endif
  }
  return (NULL);
}

/* Delete network interface (thread and semaphore) */
void netif_delete (NETOS_ID thread, NETOS_ID semaphore) {
  osSemaphoreDelete (semaphore);
  osThreadTerminate (thread);
}

/* Lock interface protection semaphore */
void netif_lock (NETOS_ID semaphore) {
  osSemaphoreWait (semaphore, osWaitForever);
}

/* Unlock interface protection semaphore */
void netif_unlock (NETOS_ID semaphore) {
  osSemaphoreRelease (semaphore);
}
