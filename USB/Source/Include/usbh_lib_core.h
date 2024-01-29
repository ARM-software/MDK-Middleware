/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2021 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_lib_core.h
 * Purpose: USB Host - Core module header file
 *----------------------------------------------------------------------------*/

#ifndef __USBH_LIB_CORE_H__
#define __USBH_LIB_CORE_H__

#include <stdint.h>
#include <stdbool.h>

#include "rl_usb.h"
#include "rl_usbh_lib.h"

#include "Driver_USBH.h"


#define ARM_USBH_PIPE_EVENT_MASK              (0xFFU)

#define ARM_USBH_EVENT_ABORT                  (ARM_USBH_EVENT_BUS_ERROR <<  1)

#define ARM_USBH_EVENT_CORE_CONNECT_DEBOUNCED (ARM_USBH_EVENT_BUS_ERROR <<  2)
#define ARM_USBH_EVENT_CORE_DO_RECOVERY       (ARM_USBH_EVENT_BUS_ERROR <<  3)
#define ARM_USBH_EVENT_CORE_RECOVERY_OK       (ARM_USBH_EVENT_BUS_ERROR <<  4)
#define ARM_USBH_EVENT_CORE_RECOVERY_FAIL     (ARM_USBH_EVENT_BUS_ERROR <<  5)

#define ARM_USBH_EVENT_PORT                   (1UL                      << 12)


/// \brief Get a pointer to first free Pipe
/// \param[in]     ctrl                 index of USB Host controller.
/// \return        != 0                 valid pointer to free Pipe Handle to Thread ID array entry
/// \return        == 0                 no free Pipe Handle to Thread ID array entry
extern USBH_PIPE *USBH_GetFree_PIPE (uint8_t ctrl);

/// \brief Get Thread ID of Pipe
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     hw_handle            hardware handle.
/// \return        != 0                 valid Thread ID
/// \return        == 0                 Thread ID for requested Pipe Hw Handle was not found
extern void *USBH_Get_Thread_ID_of_Pipe (uint8_t ctrl, ARM_USBH_PIPE_HANDLE hw_handle);

/// \brief Get Pipe Callback of Pipe
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     hw_handle            hardware handle.
/// \return        != 0                 valid pointer to Pipe event callback function
/// \return        == 0                 invalid pointer to Pipe event callback function
extern USBH_PipeEvent_t USBH_Get_PipeCallback_of_Pipe (uint8_t ctrl, ARM_USBH_PIPE_HANDLE hw_handle);

/// \brief Initialize memory pool
/// \param[in]     ctrl                 index of USB Host controller.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_MemoryInitialize (uint8_t ctrl);

/// \brief De-initialize memory pool
/// \param[in]     ctrl                 index of USB Host controller.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_MemoryUninitialize (uint8_t ctrl);

/// \brief Allocate a chunk of memory from the initialized pool
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[out]    ptr                  pointer to pointer to allocated memory chunk.
/// \param[in]     size                 size of memory chunk to be allocated.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_MemoryAllocate (uint8_t ctrl, uint8_t **ptr, uint32_t size);

/// \brief Release a piece of memory back to memory pool
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     ptr                  pointer to allocated memory chunk to be released.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_MemoryFree (uint8_t ctrl, uint8_t *ptr);

/// \brief Try to recover USB Device (reset and re-enumerate)
/// \param[in]     ptr_dev              pointer to USB Device instance.
/// \param[in]     port                 port index (0 .. 127).
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_RecoverDevice (USBH_DEV *ptr_dev);

/// \brief De-bounce timer for connect events
/// \param[in]     arg                  index (instance) of USB Host controller.
/// \return                             none.
extern void USBH_ConnectDebounce (void const * arg);

#endif  // __USBH_LIB_CORE_H__
