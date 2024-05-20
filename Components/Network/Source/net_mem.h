/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_mem.h
 * Purpose: Memory Management Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_MEM_H
#define __NET_MEM_H

#include "net_lib.h"

#define MEM_HEADER_LEN      8           // Memory manager header size

/* Memory Pool management structure */
typedef struct net_memp {
  struct net_memp *next;                // Next Memory Block in the list
  uint32_t         len;                 // Length of data block
  uint8_t          content[];           // Memory Block Data content
} NET_MEMP;

/* Memory Control info */
typedef struct net_mem_ctrl {         
  uint32_t load;                        // Memory pool load (usage)
  uint32_t limit[2];                    // Limits for non critical allocations
  NETOS_ID mutex;                       // Memory manager lock mutex
  uint32_t count;                       // Number of allocated blocks
} NET_MEM_CTRL;

#define __MEMP(frame)       ((NET_MEMP *)((uint32_t)(frame) - MEM_HEADER_LEN))

/* Functions */
extern void       net_mem_init (void);
extern void       net_mem_uninit (void);
extern NET_FRAME *net_mem_alloc (uint32_t byte_size);
extern void       net_mem_shrink (NET_FRAME *mem_ptr, uint32_t new_size);
extern void       net_mem_free (NET_FRAME *mem_ptr);
extern bool       net_mem_avail (int32_t level);
#define net_mem_avail_rx()  net_mem_avail(0)
#define net_mem_avail_tx()  net_mem_avail(1)

#endif /* __NET_MEM_H */
