/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_mem.c
 * Purpose: Dynamic Memory Management
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_mem.h"
#include "net_common.h"
#include "net_dbg.h"

/* Local variables */
static NET_MEM_CTRL  mem_control;
#define mem        (&mem_control)

/* Local functions */
static void mem_lock (void);
static void mem_unlock (void);

/**
  \brief       Initialize dynamic memory pool.
*/
void net_mem_init (void) {
  NET_MEMP *init_ptr;

  DEBUGF (MEM,"Init MemPool %d bytes\n",sysc->MemSize);
  init_ptr       = (NET_MEMP *)sysc->MemPool;
  init_ptr->next = (NET_MEMP *)((uint32_t)sysc->MemPool + sysc->MemSize - sizeof (NET_MEMP *));
  init_ptr->next->next = NULL;
  init_ptr->len  = 0;
  mem->load      = 0;
  mem->count     = 0;
  mem->mutex     = netos_mutex_create (1);

  /* Usage limit for non critical allocations.     */
  /*   Limit[0] - ethernet & bsd receive buffering */
  /*   Limit[1] - TCP send buffering (low priority)*/
  if (sysc->MemSize < 65536) {
    /* Limit[0] at 75% of mpool */
    /* Limit[1] at 50% of mpool */
    mem->limit[0] = sysc->MemSize * 3 / 4;
    mem->limit[1] = sysc->MemSize / 2;
  }
  else {
    /* Limit[0] at mpsize-16k */
    /* Limit[1] at mpsize-32k */
    mem->limit[0] = sysc->MemSize - 16384;
    mem->limit[1] = sysc->MemSize - 32768;
  }
  if (sysc->TxQueUsed) {
    /* PPP or SLIP enabled, keep space for TX queue */
    mem->limit[1] = mem->limit[0] / 2;
  }
  DEBUGF (MEM," base=0x%X (limit_0=%d, limit_1=%d)\n",
               (uint32_t)sysc->MemPool, mem->limit[0], mem->limit[1]);
  EvrNetMEM_InitMemory (sysc->MemPool, sysc->MemSize, mem->limit[0], mem->limit[1]);
}

/**
  \brief       De-initialize dynamic memory pool.
*/
void net_mem_uninit (void) {
  DEBUGF (MEM,"Uninit MemPool\n");
  EvrNetMEM_UninitMemory ();

  memset (sysc->MemPool, 0, sysc->MemSize);
  netos_mutex_delete (mem->mutex);
  memset (mem, 0, sizeof (*mem));
}

/**
  \brief       Allocate memory for the network frame.
  \param[in]   byte_size  buffer size in bytes.
  \return      Pointer to the allocated memory.
*/
NET_FRAME *net_mem_alloc (uint32_t byte_size) {
  NET_MEMP *search_ptr, *new_ptr;
  NET_FRAME *frame;
  uint32_t hole_size, req_size;

  /* Make sure that block is 4-byte aligned */
  req_size = (byte_size & ~0xC0000000) + MEM_HEADER_LEN + NET_HEADER_LEN;
  req_size = (req_size + 3) & ~3u;

  mem_lock ();

  DEBUGF (MEM,"Alloc %d bytes\n",req_size);
  if (byte_size & 0xC0000000) {
    /* Ethernet or BSD socket, keep at least 25% of the Pool free. */
    if ((mem->load + req_size) > mem->limit[0]) {
      DEBUGF (MEM," Failed, limit_0 exceeded\n");
      EvrNetMEM_AllocLimitExceeded (req_size, mem->load, mem->count);
      mem_unlock ();
      return (NULL);
    }
  }

  search_ptr = (NET_MEMP *)sysc->MemPool;
  while (true) {
    hole_size  = (uint32_t)search_ptr->next - (uint32_t)search_ptr;
    hole_size -= search_ptr->len;
    /* Check if the hole size is big enough */
    if (hole_size >= req_size) {
      break;
    }

    search_ptr = search_ptr->next;
    if (search_ptr->next == NULL) {
      /* Failed, we are at the end of the list */
      ERRORF (MEM,"Alloc, No memory (used=%d, blocks=%d)\n",mem->load,mem->count);
      EvrNetMEM_AllocOutOfMemory (req_size, mem->load, mem->count);
      mem_unlock ();
      if (!(byte_size & 0xC0000000)) {
        /* No sys_error trap for Ethernet or BSD socket */
        net_sys_error (NET_ERROR_MEM_ALLOC);
      }
      return (NULL);
    }
  }

  if (search_ptr->len == 0) {
    /* No block is allocated, set the Length of the first element */
    search_ptr->len = req_size;
    frame = __FRAME(search_ptr->content);
  }
  else {
    /* Insert a new list element into the memory list */
    new_ptr = (NET_MEMP *)((uint32_t)search_ptr + search_ptr->len);
    new_ptr->next    = search_ptr->next;
    search_ptr->next = new_ptr;
    new_ptr->len     = req_size;
    frame = __FRAME(new_ptr->content);
  }
  mem->load += req_size;
  mem->count++;
  DEBUGF (MEM," 0x%X (used=%d, blocks=%d)\n",(uint32_t)__MEMP(frame),mem->load,mem->count);
  EvrNetMEM_AllocMemory (__MEMP(frame), req_size, mem->load, mem->count);

  mem_unlock ();

  frame->length = byte_size & 0xFFFF;
  frame->index  = 0;
  return (frame);
}

/**
  \brief       Shrink allocated memory block.
  \param[in]   mem_ptr   pointer to allocated memory.
  \param[in]   new_size  size of utilized memory.
  \note        This function is used, when full size memory block
               is allocated, but only small part utilized.
*/
void net_mem_shrink (NET_FRAME *mem_ptr, uint32_t new_size) {
  NET_MEMP *block_ptr;

  if (mem_ptr == NULL) {
    return;
  }

  mem_ptr->length = new_size & 0xFFFF;
  new_size += (MEM_HEADER_LEN + NET_HEADER_LEN);
  new_size  = (new_size + 3U) & ~3U;
  block_ptr = __MEMP(mem_ptr);

  if (new_size < block_ptr->len) {
    mem_lock ();

    DEBUGF (MEM,"Shrink 0x%X (new_size=%d)\n",(uint32_t)block_ptr,new_size);
    EvrNetMEM_ShrinkMemory (block_ptr, new_size);
    mem->load     -= (block_ptr->len - new_size);
    block_ptr->len = new_size;

    mem_unlock ();
  }
}

/**
  \brief       Release allocated memory.
  \param[in]   mem_ptr  pointer to the allocated memory.
*/
void net_mem_free (NET_FRAME *mem_ptr) {
  NET_MEMP *prev_ptr, *search_ptr, *return_ptr, *limit_ptr;

  if (mem_ptr == NULL) {
    return;
  }

  mem_lock ();

  return_ptr = __MEMP(mem_ptr);

  /* Set list header */
  search_ptr = (NET_MEMP *)sysc->MemPool;
  limit_ptr  = (NET_MEMP *)((uint32_t)search_ptr + sysc->MemSize);
  prev_ptr   = NULL;
  while (search_ptr != return_ptr) {
    prev_ptr   = search_ptr;
    search_ptr = search_ptr->next;
    if (search_ptr == NULL) {
      /* This is not a valid Memory Block pointer */
      ERRORF (MEM,"Free, 0x%X invalid block\n",(uint32_t)return_ptr);
      EvrNetMEM_FreeInvalidBlock (return_ptr);

      mem_unlock ();
      net_sys_error (NET_ERROR_MEM_FREE);
      return;
    }
    if ((search_ptr <= (NET_MEMP *)&prev_ptr->content) ||
        (search_ptr >= limit_ptr) || ((uint32_t)search_ptr & 3)) {
      /* Pointer out of bounds or not aligned */
      ERRORF (MEM,"Free, 0x%X link corrupted\n",(uint32_t)search_ptr);
      EvrNetMEM_FreeLinkCorrupted (search_ptr);

      mem_unlock ();
      net_sys_error (NET_ERROR_MEM_CORRUPT);
      return;
    }
  }
  mem->load -= search_ptr->len;
  mem->count--;

  DEBUGF (MEM,"Free %d bytes\n",search_ptr->len);
  DEBUGF (MEM," 0x%X (used=%d, blocks=%d)\n",(uint32_t)search_ptr,mem->load,mem->count);
  EvrNetMEM_FreeMemory (search_ptr, search_ptr->len, mem->load, mem->count);
  if (search_ptr == (NET_MEMP *)sysc->MemPool) {
    /* First element to be freed, only set length to 0 */
    search_ptr->len = 0;
  }
  else {
    /* Discard list element */
    prev_ptr->next = search_ptr->next;
  }

  mem_unlock ();
}

/**
  \brief       Check if memory available for allocation.
  \param[in]   level  priority level of requester.
  \return      status:
               - true  = memory is available,
               - false = no memory available.
*/
bool net_mem_avail (int32_t level) {
  if (mem->load < mem->limit[level]) {
    return (true);
  }
  return (false);
}

/**
  \brief       Acquire memory protection mutex.
*/
static void mem_lock (void) {
  netos_lock (mem->mutex);
}

/**
  \brief       Release memory protection mutex.
*/
static void mem_unlock (void) {
  netos_unlock (mem->mutex);
}
