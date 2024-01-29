/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_lib_mem.h
 * Purpose: USB Host - Memory Management module header file
 *----------------------------------------------------------------------------*/

#ifndef __USBH_LIB_MEM_H__
#define __USBH_LIB_MEM_H__

#include <stdint.h>
#include <stdbool.h>


typedef struct  mem {                   // Memory Pool management structure
  struct mem   *next;                   // Next Memory Block in the list
  uint32_t      len;                    // Length of following block
  uint8_t       content[4];             // Memory Block Data content
} MEMP_t;


/// \brief       Initialize memory pool
/// \param[in]   idx    Index of memory pool
/// \return      true   initialization succeeded
/// \return      false  initialization failed
extern bool USBH_MemoryInitializeLib (uint8_t idx);

/// \brief       Uninitialize memory pool
/// \param[in]   idx    Index of memory pool
/// \return      true   uninitialization succeeded
/// \return      false  uninitialization failed
extern bool USBH_MemoryUninitializeLib (uint8_t idx);

/// \brief       Get memory pool size
/// \param[in]   idx    Index of memory pool
/// \return      size of memory pool in bytes
extern uint32_t USBH_MemoryGetPoolSize (uint8_t idx);

/// \brief       Check if memory is in pool range
/// \param[in]   idx      Index of memory pool
/// \param[in]   ptr_mem  Pointer to memory to check if it is in pool
/// \return      true     is in pool
/// \return      false    is not in pool
extern bool USBH_MemoryIsInPool (uint8_t idx, const uint8_t *ptr_mem);

/// \brief       Allocate a piece of memory from the memory pool
/// \param[in]   idx      Index of memory pool
/// \param[in]   sz       Size of memory to be allocated from the memory pool
/// \return               Pointer to allocated memory or failed
/// \return        value != 0: pointer to allocated memory
/// \return        value = 0:  no memory available
extern uint8_t *USBH_MemoryAllocateLib (uint8_t idx, uint32_t sz);

/// \brief       Free memory and return it to the memory pool
/// \param[in]   idx      Index of memory pool
/// \param[in]   ptr_mem  Pointer to allocated memory to be set free
/// \return      true     memory deallocated successfully
/// \return      false    memory deallocation failed
extern bool USBH_MemoryFreeLib (uint8_t idx, uint8_t *ptr_mem);

#endif  // __USBH_LIB_MEM_H__
