/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_cmsis_rtos2.h
 * Purpose: USB Device (USBD) - RTOS abstraction header
 *----------------------------------------------------------------------------*/

#ifndef USBD_CMSIS_RTOS2_H_
#define USBD_CMSIS_RTOS2_H_

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
/// \param[in]     index                parameter dependent on thread (device index or class instance or endpoint index)
/// \return
///                value != 0:          handle to created thread
///                value = 0:           thread creation failed
extern void *USBD_ThreadCreate (usbdThread_t thread, uint8_t index);

/// \brief Terminate execution of a thread
/// \param[in]     thread_hndl          thread handle
/// \return
///                value 0:             thread terminated successfully
///                value < 0:           thread termination failed
extern int32_t USBD_ThreadTerminate (void *thread_hndl);

/// \brief Delay execution of a thread for specified number of milliseconds
/// \param[in]     millisec             number of milliseconds
/// \return
///                value 0:             delay finished
///                value < 0:           delay failed
extern int32_t USBD_Delay (uint32_t millisec);

/// \brief Create and initialize a periodic timer for HID class handling
/// \param[in]     instance             class instance
/// \return
///                value != 0:          handle to created timer
///                value = 0:           timer creation failed
extern void *USBD_TimerCreate (uint8_t instance);

/// \brief Start or restart a timer
/// \param[in]     timer_hndl           timer handle
/// \return
///                value 0:             timer started or restarted successfully
///                value < 0:           timer start or restart failed
extern int32_t USBD_TimerStart (void *timer_hndl, uint32_t millisec);

/// \brief Stop a timer
/// \param[in]     timer_hndl           timer handle
/// \return
///                value 0:             timer stopped successfully
///                value < 0:           timer stop failed
extern int32_t USBD_TimerStop (void *timer_hndl);

/// \brief Delete a timer
/// \param[in]     timer_hndl           timer handle
/// \return
///                value 0:             timer deleted successfully
///                value < 0:           timer deletion failed
extern int32_t USBD_TimerDelete (void *timer_hndl);

/// \brief Set the specified flags of a thread
/// \param[in]     thread_hndl          thread handle
/// \param[in]     flags                flags to be set
/// \return
///                value 0:             flags set successfully
///                value >= 0x80000000: setting of flags failed
extern uint32_t USBD_ThreadFlagsSet (void *thread_hndl, uint32_t flags);

/// \brief Wait for any USB related flag of currently running thread to become signaled
/// \param[in]     millisec             time-out in milliseconds, or 0 in case of no time-out
/// \return
///                value < 0x80000000:  flags
///                value 0:             time-out
///                value >= 0x80000000: error
extern uint32_t USBD_ThreadFlagsWait (uint32_t millisec);

/// \brief Create and initialize a binary semaphore
/// \param[in]     semaphore            semaphore
/// \param[in]     index1               semaphore dependent index1 (device index or class instance)
/// \param[in]     index2               semaphore dependent index2 (endpoint index)
/// \return
///                value != 0:          handle to created semaphore
///                value = 0:           semaphore creation failed
extern void *USBD_SemaphoreCreate (usbdSemaphore_t semaphore, uint8_t index1, uint8_t index2);

/// \brief Wait for a semaphore token to become available and acquire it
/// \param[in]     semaphore_hndl       semaphore handle
/// \param[in]     millisec             time-out in milliseconds, or 0 in case of no time-out
/// \return
///                value 0:             token acquired successfully
///                value < 0:           token acquire failed
extern int32_t USBD_SemaphoreAcquire (void *semaphore_hndl, uint32_t millisec);

/// \brief Release a semaphore token
/// \param[in]     semaphore_hndl       semaphore handle
/// \return
///                value 0:             token released successfully
///                value < 0:           token release failed
extern int32_t USBD_SemaphoreRelease (void *semaphore_hndl);

/// \brief Delete a semaphore
/// \param[in]     semaphore_hndl       semaphore handle
/// \return
///                value 0:             semaphore deleted successfully
///                value < 0:           semaphore deletion failed
extern int32_t USBD_SemaphoreDelete (void *semaphore_hndl);

#endif  // USBD_CMSIS_RTOS2_H_
