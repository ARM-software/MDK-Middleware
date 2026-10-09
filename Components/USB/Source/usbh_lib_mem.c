/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2026 Arm Limited (or its affiliates).
 * All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_lib_mem.c
 * Purpose: USB Host - Memory Management module
 *----------------------------------------------------------------------------*/

#include "usbh_lib_mem.h"

#include <string.h>

#include "usbh_config.h"


/************************** Important *****************************************/
/* Size of memory pool must be at least =                                     */
/* (maximum number of elements * 8) + (sum of all element data) + 4           */
/* Example: two 8 byte packets => (2*8)+(8+8)+4 = 36                          */
/******************************************************************************/


/************************** Variable and Memory Definitions *******************/

#define USBH_MIN_MEM_POOL_SIZE          64U

#ifdef __DEBUG
static uint32_t mem_load   [4];         /* Memory Pool Load for debugging     */
static uint8_t  mem_counter[4];         /* Number of Allocated Memory Blocks  */
#endif


/************************** Module Functions **********************************/

/// \brief       Initialize memory pool
/// \param[in]   idx    Index of memory pool
/// \return      true   initialization succeeded
/// \return      false  initialization failed
bool USBH_MemoryInitializeLib (uint8_t idx) {
  MEMP_t   *ptr_init;
  uintptr_t addr;

  // Check parameters
  if (idx >= usbh_hc_num) {
    return false;
  }
  if ((usbh_mem_pool_ptr[idx] == NULL) || (usbh_mem_pool_size_ptr[idx] == NULL)) {
    return false;
  }
  if (*usbh_mem_pool_size_ptr[idx] < USBH_MIN_MEM_POOL_SIZE) {
    return false;
  }

  memset ((void *)usbh_mem_pool_ptr[idx], 0, *usbh_mem_pool_size_ptr[idx]);

  ptr_init                = (MEMP_t *)usbh_mem_pool_ptr[idx];
  addr                    = (uintptr_t)usbh_mem_pool_ptr[idx] + (uintptr_t)(*usbh_mem_pool_size_ptr[idx]) - sizeof (struct mem *);
  ptr_init->next          = (MEMP_t *)addr;
  ptr_init->next->next    = NULL;
  ptr_init->len           = 0U;
#ifdef __DEBUG
  mem_load[idx]           = 0U;
  mem_counter[idx]        = 0U;
#endif

  return true;
}


/// \brief       Uninitialize memory pool
/// \param[in]   idx    Index of memory pool
/// \return      true   uninitialization succeeded
/// \return      false  uninitialization failed
bool USBH_MemoryUninitializeLib (uint8_t idx) {

  // Check parameters
  if (idx >= usbh_hc_num) {
    return false;
  }
  if ((usbh_mem_pool_ptr[idx] == NULL) || (usbh_mem_pool_size_ptr[idx] == NULL)) {
    return false;
  }

  memset ((void *)usbh_mem_pool_ptr[idx], 0, *usbh_mem_pool_size_ptr[idx]);

#ifdef __DEBUG
  mem_load[idx]           = 0U;
  mem_counter[idx]        = 0U;
#endif

  return true;
}


/// \brief       Get memory pool size
/// \param[in]   idx    Index of memory pool
/// \return      size of memory pool in bytes
uint32_t USBH_MemoryGetPoolSize (uint8_t idx) {

  // Check parameters
  if (idx >= usbh_hc_num) {
    return 0U;
  }
  if (usbh_mem_pool_size_ptr[idx] == NULL) {  
    return 0U;
  }

  return (*usbh_mem_pool_size_ptr[idx]);
}


/// \brief       Check if memory is in pool range
/// \param[in]   idx      Index of memory pool
/// \param[in]   ptr_mem  Pointer to memory to check if it is in pool
/// \return      true     is in pool
/// \return      false    is not in pool
bool USBH_MemoryIsInPool (uint8_t idx, const uint8_t *ptr_mem) {

  // Check parameters
  if (idx >= usbh_hc_num) {
    return false;
  }
  if (ptr_mem == NULL) {
    return false;
  }
  if ((usbh_mem_pool_ptr[idx] == NULL) || (usbh_mem_pool_size_ptr[idx] == NULL)) {
    return false;
  }
  if (*usbh_mem_pool_size_ptr[idx] < USBH_MIN_MEM_POOL_SIZE) {
    return false;
  }

  if (((uintptr_t)ptr_mem >= ((uintptr_t)usbh_mem_pool_ptr[idx] + sizeof(struct mem *) + sizeof(uintptr_t))) &&
      ((uintptr_t)ptr_mem <  ((uintptr_t)usbh_mem_pool_ptr[idx] + *usbh_mem_pool_size_ptr[idx] - sizeof(uintptr_t)))) {
    return true;
  }

  return false;
}


/// \brief       Allocate a piece of memory from the memory pool
/// \param[in]   idx      Index of memory pool
/// \param[in]   sz       Size of memory to be allocated from the memory pool
/// \return               Pointer to allocated memory or failed
/// \return        value != 0: pointer to allocated memory
/// \return        value = 0:  no memory available
uint8_t *USBH_MemoryAllocateLib (uint8_t idx, uint32_t sz) {
  MEMP_t   *ptr_search, *ptr_new;
  uint8_t  *ptr_data;
  uint32_t  total_sz;
  uint32_t  hole_sz;
  uintptr_t addr;

  // Check parameters
  if (idx >= usbh_hc_num) {
    return NULL;
  }
  if (sz == 0U) {
    return NULL;
  }
  if (sz >= (UINT32_MAX - (sizeof(struct mem *) + sizeof(uintptr_t) + 3U))) {
    return NULL;
  }
  if ((usbh_mem_pool_ptr[idx] == NULL) || (usbh_mem_pool_size_ptr[idx] == NULL)) {
    return NULL;
  }
  if (*usbh_mem_pool_size_ptr[idx] < USBH_MIN_MEM_POOL_SIZE) {
    return NULL;
  }

  // Add the header offset to 'sz'
  total_sz = sz + sizeof(struct mem *) + sizeof(uintptr_t);
  // Make sure that block is 4-byte aligned
  total_sz = (total_sz + 3U) & ~0x00000003U;

  ptr_search = (MEMP_t *)usbh_mem_pool_ptr[idx];
  if (ptr_search->next == NULL) {
    // If memory pool was not initialized
    return NULL;
  }

  for (;;) {
    if (((uintptr_t)(ptr_search->next) - (uintptr_t)ptr_search) >= UINT32_MAX) {
      return NULL;
    }
    hole_sz  = (uint32_t)((uintptr_t)(ptr_search->next) - (uintptr_t)ptr_search);
    if (ptr_search->len > hole_sz) {
      // Corrupted memory list
      return NULL;
    }
    hole_sz -= ptr_search->len;
    // Check if the hole size is big enough
    if (hole_sz >= total_sz) {
      break;
    }

    ptr_search = ptr_search->next;
    if (ptr_search->next == NULL) {
      // Failed, we are at the end of the list
      return NULL;
    }
  }

  if (ptr_search->len == 0U) {
    // No block is allocated, set the Length of the first element
    ptr_search->len  = total_sz;
    ptr_data         = (uint8_t *)ptr_search->content;
  } else {
    // Insert a new list element into the memory list
    addr             = (uintptr_t)ptr_search + ptr_search->len;
    ptr_new          = (MEMP_t *)addr;
    ptr_new->next    = ptr_search->next;
    ptr_search->next = ptr_new;
    ptr_new->len     = total_sz;
    ptr_data         = (uint8_t *)ptr_new->content;
  }
#ifdef __DEBUG
  mem_load[idx]     += total_sz;
  mem_counter[idx]++;
#endif
  return ptr_data;
}


/// \brief       Free memory and return it to the memory pool
/// \param[in]   idx      Index of memory pool
/// \param[in]   ptr_mem  Pointer to allocated memory to be set free
/// \return      true     memory deallocated successfully
/// \return      false    memory deallocation failed
bool USBH_MemoryFreeLib (uint8_t idx, uint8_t *ptr_mem) {
  const MEMP_t   *ptr_return;
        MEMP_t   *ptr_prev, *ptr_search;
        uintptr_t addr;

  // Check parameters
  if (idx >= usbh_hc_num) {
    return false;
  }
  if (ptr_mem == NULL) {
    return false;
  }
  if (USBH_MemoryIsInPool(idx, ptr_mem) == false) {
    return false;
  }

  addr       = (uintptr_t)ptr_mem - (sizeof(struct mem *) + sizeof(uintptr_t));
  ptr_return = (MEMP_t *)addr;

  // Set list header
  ptr_search = (MEMP_t *)usbh_mem_pool_ptr[idx];
  if (ptr_search == NULL) {
    return false;
  }
  ptr_prev   = ptr_search;
  while (ptr_search != ptr_return) {
    ptr_prev   = ptr_search;
    ptr_search = ptr_search->next;
    if (ptr_search == NULL) {
      // This is not a valid Memory Block pointer
      return false;
    }
    if (((uintptr_t)ptr_search <  ((uintptr_t)usbh_mem_pool_ptr[idx] + sizeof(struct mem *) + sizeof(uintptr_t))) ||
        ((uintptr_t)ptr_search >= ((uintptr_t)usbh_mem_pool_ptr[idx] + *usbh_mem_pool_size_ptr[idx] - sizeof(uintptr_t)))) {
      // If ptr_search is not within the valid memory pool range
      return false;
    }
    if ((((uintptr_t)(ptr_search->next)) & 3U) != 0U) {
      // Error Link pointer overwritten, memory corrupt
      return false;
    }
  }
#ifdef __DEBUG
  mem_load[idx]     -= ptr_search->len;
  mem_counter[idx]--;
#endif
  if (ptr_search == (MEMP_t *)usbh_mem_pool_ptr[idx]) {
    // First element to be set free, only set Length to 0
    ptr_search->len  = 0U;
  } else {
    // Discard list element
    ptr_prev->next   = ptr_search->next;
    memset(ptr_search, 0, (sizeof(struct mem *) + sizeof(uintptr_t)));
  }

  return true;
}
