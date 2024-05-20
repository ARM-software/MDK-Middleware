/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ip4_frag.c
 * Purpose: IPv4 Fragmentation and Reassembly
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "net_lib.h"
#include "net_sys.h"
#include "net_mem.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_ip4.h"
#include "net_ip4_frag.h"
#include "net_udp.h"
#include "net_dbg.h"

/* Global constants */

/* Local variables */

/* Local Functions */
static NET_IP_FRAG_INFO *frag_map_session (NET_FRAME *frame);
static void que_free_list (NET_IP_FRAG_INFO *frag_s);

/**
  \brief       Initialize IPv4 reassembly.
*/
void net_ip4_frag_init (void) {
  /* Clear IP4 reassembly control table */
  memset (fragc->Scb, 0, sizeof (*fragc->Scb) * fragc->NumSess);
}

/**
  \brief       De-initialize IPv4 reassembly.
*/
void net_ip4_frag_uninit (void) {
  memset (fragc->Scb, 0, sizeof (*fragc->Scb) * fragc->NumSess);
}

/**
  \brief       Reassemble incoming IPv4 fragment.
  \param[in]   frame  incoming IPv4 datagram fragment.
  \return      new assembled frame or NULL if not completed yet.
  \note        The ethernet header and IP4_QUE() variables overlap.
               After the frame is added to IP4_QUE() reassembly, the 
               ethernet header MAC addresses are destroyed.
*/
__WEAK NET_FRAME *net_ip4_frag_add (NET_FRAME *frame) {
  NET_IP_FRAG_INFO *frag_s;
  NET_FRAME *q,*prev,*new_frame;
  uint32_t max_size;
  uint16_t offs;

  /* Check which protocols can be fragmented */
  switch (IP4_FRAME(frame)->Prot) {
    case IP4_PROT_TCP:
      /* Accept TCP frames with 1460 byte MSS */
      max_size = 1500 - IP4_HEADER_LEN;
      break;
    case IP4_PROT_ICMP:
    case IP4_PROT_UDP:
      /* Accept UDP frames with 2000 byte payload */
      max_size = 2000 + UDP_HEADER_LEN;
      break;
    default:
      /* Dump all other protocol fragments */
dump_frame:
      net_mem_free (frame);
      return (NULL);
  }

  frag_s = frag_map_session (frame);
  frag_s->Tout = fragc->ReassTout;

  /* Check boundary limits */
  offs = (ntohs(IP4_FRAME(frame)->FragOffs) & IP4_FRAG_MASK) * 8;
  if ((offs + frame->length) > max_size) {
    /* Out of bounds, free the session */
    frag_s->Tout = 0;
    que_free_list (frag_s);
    goto dump_frame;
  }
  /* This overlaps the ethernet header */
  IP4_QUE(frame)->start = offs;
  IP4_QUE(frame)->end   = offs + frame->length;

  /* Add frame to the list */
  if (frag_s->frag_list == NULL) {
    /* Add a first fragment to the queue */
    frag_s->frag_list    = frame;
    IP4_QUE(frame)->next = NULL;
    frag_s->Len          = frame->length;
  }
  else {
    /* Insert a frame into an ordered list of datagram fragments, */
    /* check for overlaps and discard the overlapping fragments.  */
    for (prev = NULL, q = frag_s->frag_list; ; prev = q, q = IP4_QUE(q)->next) {
      if (IP4_QUE(frame)->start < IP4_QUE(q)->start) {
        /* Insert new fragment before this */
        if ((IP4_QUE(frame)->end) > IP4_QUE(q)->start) {
          /* Overlaps with this fragment */
          goto dump_frame;
        }
        if (prev != NULL) {
          if (IP4_QUE(frame)->start < IP4_QUE(prev)->end) {
            /* Overlaps with the previous fragment */
            goto dump_frame;
          } 
          IP4_QUE(prev)->next = frame;
        }
        else {
          /* Fragment with the lowest offset */
          frag_s->frag_list = frame;
        }
        IP4_QUE(frame)->next = q;
        break;
      }
      if (IP4_QUE(q)->next == NULL) {
        /* Append new fragment at the end of the list */
        if (IP4_QUE(frame)->start < IP4_QUE(q)->end) {
          /* Overlaps with the current fragment */
          goto dump_frame;
        }
        IP4_QUE(q)->next     = frame;
        IP4_QUE(frame)->next = NULL;
        break;
      }
    }
    /* Update total datagram length */
    frag_s->Len += frame->length;
  }

  /* Check if the reassembly is complete */
  if (IP4_QUE(frag_s->frag_list)->start != 0) {
    /* First fragment offset should be 0 */
    return (NULL);
  }
  /* Walk through the list and find the last fragment */
  for (q = frag_s->frag_list; ; q = IP4_QUE(q)->next) {
    if (IP4_QUE(q)->next == NULL) {
      /* This is the last fragment */
      break;
    }
  }
  /* End offset should match the total length */
  if (IP4_QUE(q)->end != frag_s->Len) {
    return (NULL);
  }
  /* Last fragment should have MF bit cleared */
  if (IP4_FRAME(q)->FragOffs & HTONS(IP4_MF_FLAG)) {
    return (NULL);
  }

  /* Create new flat frame and copy the data to this frame */
  new_frame = net_mem_alloc ((IP4_DATA_OFFS + frag_s->Len) | 0x40000000);
  if (new_frame != NULL) {
    /* Copy IP header first */
    offs = PHY_HEADER_LEN;
    memcpy (&new_frame->data[offs], &frag_s->frag_list->data[offs], IP4_HEADER_LEN);
    /* Walk through the list and copy the data fragments to new frame */
    for (q = frag_s->frag_list; q != NULL; q = IP4_QUE(q)->next) {
      offs = IP4_DATA_OFFS + IP4_QUE(q)->start;
      memcpy (&new_frame->data[offs], &q->data[q->index], q->length);
    }
    new_frame->index  = IP4_DATA_OFFS;
    new_frame->length = frag_s->Len;
  }
  /* Release this frame fragments */
  frag_s->Tout = 0;
  que_free_list (frag_s);
  /* Checksum offload does not work for fragmented frames */
  sys->RxOffload &= ~(SYS_OFFL_UDP4_RX | SYS_OFFL_TCP4_RX | SYS_OFFL_ICMP4_RX);
  return (new_frame);
}

/**
  \brief       Split outgoing IPv4 datagram to small fragments.
  \param[in]   frame  outgoing IPv4 datagram.
  \param[in]   mtu    maximum transmission unit.
  \return      pointer to generated frame fragment.
  \note        Parameter 'frame' holds the following information:
               - frame->index:  data offset,
               - frame->length: total length including IP header.
*/
__WEAK NET_FRAME *net_ip4_frag_get (NET_FRAME *frame, uint16_t mtu) {
  static NET_FRAME *dfrag;
  uint16_t dlen,frag;

  /* Check if this is last call */
  if (frame == NULL) {
    /* Release 'dfrag' frame buffer */
    if (dfrag) net_mem_free (dfrag);
    dfrag = NULL;
    return (NULL);
  }
  /* Check if this is first call */
  if (frame->index == 0) {
    if ((mtu == 0) || (frame->length <= mtu)) {
      /* Fragmentation disabled or not needed */
      return (frame);
    }
    /* Fragmentation needed, allocate frame buffer */
    if (dfrag) net_mem_free (dfrag);
    /* This buffer will be reused for all generated fragments */
    dfrag = net_mem_alloc ((PHY_HEADER_LEN + mtu) | 0x40000000);
    if (dfrag == NULL) {
      /* No memory, dump this frame */
      return (NULL);
    }
    /* Copy IP header and use it for all fragments */
    memcpy (&dfrag->data[PHY_HEADER_LEN],
            &frame->data[PHY_HEADER_LEN], IP4_HEADER_LEN);
  }
  /* Length must be a multiple of 8 */
  dlen = (mtu - IP4_HEADER_LEN) & ~7u;
  frag = frame->index / 8;
  if (dlen < (frame->length - IP4_HEADER_LEN - frame->index)) {
    /* Need to send more fragments, set MF flag */
    frag |= IP4_MF_FLAG;
  }
  else {
    /* This is the last fragment to send */
    dlen = frame->length - IP4_HEADER_LEN - frame->index;
  }
  IP4_FRAME(dfrag)->Len      = htons(IP4_HEADER_LEN + dlen);
  IP4_FRAME(dfrag)->FragOffs = htons(frag);
  IP4_FRAME(dfrag)->Chksum   = 0;
  memcpy (&dfrag->data[IP4_DATA_OFFS],
          &frame->data[IP4_DATA_OFFS + frame->index], dlen);
  dfrag->length = IP4_HEADER_LEN + dlen;
  frame->index += dlen;
  return (dfrag);
}

/**
  \brief       Run IPv4 reassembly main function.
*/
void net_ip4_frag_run (void) {
  NET_IP_FRAG_INFO *frag_s;

  if (!(sys->Flags & SYS_FLAG_TICK)) {
    return;
  }
  /* Update reassembly TTLs every system tick (100ms) */
  for (frag_s = &fragc->Scb[0]; frag_s < &fragc->Scb[fragc->NumSess]; frag_s++) {
    if (frag_s->Tout != 0) {
      frag_s->Tout--;
      if (frag_s->Tout != 0) {
        /* A timeout not yet expired, continue */
        continue;
      }
      /* Timeout expired, release fragments */
      que_free_list (frag_s);
    }
  }
}

/**
  \brief       Map received frame to reassembly session.
  \param[in]   frame  IPv4 datagram fragment.
  \return      pointer to reassembly session,
               - NULL= not found.
*/
static NET_IP_FRAG_INFO *frag_map_session (NET_FRAME *frame) {
  NET_IP_FRAG_INFO *frag_s,*new_s;

  /* Find existing reassembly session */
  new_s = NULL;
  for (frag_s = &fragc->Scb[0]; frag_s < &fragc->Scb[fragc->NumSess]; frag_s++) {
    if (frag_s->frag_list == NULL) {
      /* Not yet allocated */
      if (new_s == NULL) new_s = frag_s;
      continue;
    }
    /* Buffer id is composed of source, destination, protocol and identification */
    /* Those values are in IP header of the first datagram fragment in the list  */
    if ((IP4_FRAME(frame)->Id   == IP4_FRAME(frag_s->frag_list)->Id)     &&
        (IP4_FRAME(frame)->Prot == IP4_FRAME(frag_s->frag_list)->Prot)   &&
        net_addr4_comp (IP4_FRAME(frame)->SrcAddr, IP4_FRAME(frag_s->frag_list)->SrcAddr) &&
        net_addr4_comp (IP4_FRAME(frame)->DstAddr, IP4_FRAME(frag_s->frag_list)->DstAddr)) {
      /* Existing reassembly session found */
      return (frag_s);
    }
  }

  /* Existing session not found, need to allocate a new one */
  if (new_s == NULL) {
    /* No sessions are free, reuse the oldest */
    uint32_t tout = 0xFFFFFFFF;
    for (frag_s = &fragc->Scb[0]; frag_s < &fragc->Scb[fragc->NumSess]; frag_s++) {
      if (frag_s->Tout < tout) {
        tout  = frag_s->Tout;
        new_s = frag_s;
      }
    }
    /* Flush outdated reassembly buffers */
    que_free_list (new_s);
  }
  return (new_s);
}

/**
  \brief       Release all reassembly buffers.
  \param[in]   frag_s  reassembly session descriptor.
*/
static void que_free_list (NET_IP_FRAG_INFO *frag_s) {
  NET_FRAME *frame, *next;

  frame = frag_s->frag_list;
  frag_s->frag_list = NULL;
  for ( ; frame; frame = next) {
    next = IP4_QUE(frame)->next;
    net_mem_free (frame);
  }
}
