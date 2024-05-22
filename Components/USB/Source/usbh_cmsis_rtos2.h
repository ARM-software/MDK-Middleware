/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_cmsis_rtos2.h
 * Purpose: USB Host (USBH) - RTOS abstraction header
 *----------------------------------------------------------------------------*/

#ifndef USBH_CMSIS_RTOS2_H_
#define USBH_CMSIS_RTOS2_H_

#include <stdint.h>

#include "RTE_Components.h"

#ifdef    RTE_CMSIS_RTOS2
#include "cmsis_os2.h"
#endif
#ifdef    RTE_CMSIS_RTOS2_RTX5
#include "rtx_os.h"
#endif

// External functions

/// \brief Create a thread
/// \param[in]     thread               thread
/// \param[in]     index                parameter dependent on thread (controller index or class instance)
/// \return
///                value != 0:          handle to created thread
///                value = 0:           thread creation failed
extern void *USBH_ThreadCreate (usbhThread_t thread, uint8_t index);

/// \brief Get handle to currently running thread
/// \return
///                value != 0:          handle to currently running thread
///                value = 0:           error
extern void *USBH_ThreadGetHandle (void);

/// \brief Terminate execution of a thread
/// \param[in]     thread_hndl          thread handle
/// \return
///                value 0:             thread terminated successfully
///                value < 0:           thread termination failed
extern int32_t USBH_ThreadTerminate (void *thread_hndl);

/// \brief Delay execution of a thread for specified number of milliseconds
/// \param[in]     millisec             number of milliseconds
/// \return
///                value 0:             delay finished
///                value < 0:           delay failed
extern int32_t USBH_Delay (uint32_t millisec);

/// \brief Create and initialize a single-shot timer for connection debouncing
/// \param[in]     ctrl                 controller index
/// \return
///                value != 0:          handle to created timer
///                value = 0:           timer creation failed
extern void *USBH_TimerCreate (uint8_t ctrl);

/// \brief Start or restart a timer
/// \param[in]     timer_hndl           timer handle
/// \return
///                value 0:             timer started or restarted successfully
///                value < 0:           timer start or restart failed
extern int32_t USBH_TimerStart (void *timer_hndl, uint32_t millisec);

/// \brief Stop a timer
/// \param[in]     timer_hndl           timer handle
/// \return
///                value 0:             timer stopped successfully
///                value < 0:           timer stop failed
extern int32_t USBH_TimerStop (void *timer_hndl);

/// \brief Delete a timer
/// \param[in]     timer_hndl           timer handle
/// \return
///                value 0:             timer deleted successfully
///                value < 0:           timer deletion failed
extern int32_t USBH_TimerDelete (void *timer_hndl);

/// \brief Set the specified flags of a thread
/// \param[in]     thread_hndl          thread handle
/// \param[in]     flags                flags to be set
/// \return
///                value 0:             flags set successfully
///                value >= 0x80000000: setting of flags failed
extern uint32_t USBH_ThreadFlagsSet (void *thread_hndl, uint32_t flags);

/// \brief Wait for any USB related flag of currently running thread to become signaled
/// \param[in]     millisec             time-out in milliseconds, or 0 in case of no time-out
/// \return
///                value < 0x80000000:  flags
///                value 0:             time-out
///                value >= 0x80000000: error
extern uint32_t USBH_ThreadFlagsWait (uint32_t millisec);

/// \brief Create and initialize a mutex
/// \param[in]     mutex                mutex
/// \param[in]     ctrl                 controller index
/// \return
///                value != 0:          handle to created mutex
///                value = 0:           mutex creation failed
extern void *USBH_MutexCreate (usbhMutex_t mutex, uint8_t ctrl);

/// \brief Acquire a mutex or timeout if it is locked
/// \param[in]     mutex_hndl           mutex handle
/// \param[in]     millisec             time-out in milliseconds, or 0 in case of no time-out
/// \return
///                value 0:             mutex acquired successfully
///                value < 0:           mutex acquire failed
extern int32_t USBH_MutexAcquire (void *mutex_hndl, uint32_t millisec);

/// Release a mutex
/// \param[in]     mutex_hndl           mutex handle
/// \return
///                value 0:             mutex released successfully
///                value < 0:           mutex release failed
extern int32_t USBH_MutexRelease (void *mutex_hndl);

/// \brief Delete a mutex
/// \param[in]     mutex_hndl           mutex handle
/// \return
///                value 0:             mutex deleted successfully
///                value < 0:           mutex deletion failed
extern int32_t USBH_MutexDelete (void *mutex_hndl);

/// \brief Create and initialize a binary semaphore
/// \param[in]     semaphore            semaphore
/// \param[in]     ctrl                 controller index
/// \return
///                value != 0:          handle to created semaphore
///                value = 0:           semaphore creation failed
extern void *USBH_SemaphoreCreate (usbhSemaphore_t semaphore, uint8_t ctrl);

/// \brief Wait for a semaphore token to become available and acquire it
/// \param[in]     semaphore_hndl       semaphore handle
/// \param[in]     millisec             time-out in milliseconds, or 0 in case of no time-out
/// \return
///                value 0:             token acquired successfully
///                value < 0:           token acquire failed
extern int32_t USBH_SemaphoreAcquire (void *semaphore_hndl, uint32_t millisec);

/// \brief Release a semaphore token
/// \param[in]     semaphore_hndl       semaphore handle
/// \return
///                value 0:             token released successfully
///                value < 0:           token release failed
extern int32_t USBH_SemaphoreRelease (void *semaphore_hndl);

/// \brief Delete a semaphore
/// \param[in]     semaphore_hndl       semaphore handle
/// \return
///                value 0:             semaphore deleted successfully
///                value < 0:           semaphore deletion failed
extern int32_t USBH_SemaphoreDelete (void *semaphore_hndl);

#endif  // USBH_CMSIS_RTOS2_H_
