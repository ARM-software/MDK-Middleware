/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_mld.c
 * Purpose: Multicast Listener Discovery for IPv6
 *----------------------------------------------------------------------------*/

#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_eth.h"
#include "net_ip6.h"
#include "net_icmp.h"
#include "net_icmp6.h"
#include "net_addr.h"
#include "net_common.h"
#include "net_mem.h"
#include "net_mld.h"
#include "net_dbg.h"

/* Global variables */
NET_MLD_CTRL net_eth0_mld_control;
NET_MLD_CTRL net_eth1_mld_control;
NET_MLD_CTRL net_wifi0_mld_control;
NET_MLD_CTRL net_wifi1_mld_control;

/* Code shortening macros */
#define ctrl         (h->Ctrl)

/* Local Functions */
static void mld_node_run (NET_MLD_CFG *h);
static void mld_send_message (NET_MLD_CFG *h, uint8_t type, const uint8_t *ip6_addr);
static NET_MLD_INFO *mld_table_find (NET_MLD_CFG *h, const uint8_t *ip6_addr);
static NET_MLD_INFO *mld_table_add (NET_MLD_CFG *h, const uint8_t *ip6_addr);
static bool mld_is_addr_valid (const uint8_t *ip6_addr);

/**
  \brief       Initialize MLD nodes.
*/
void net_mld_node_init (void) {
  NET_MLD_INFO *mld_t;
  NET_MLD_CFG *const *p;
  int32_t i;

  for (p = &net_mld_list[0]; *p; p++) {
    DEBUGF (MLD,"Init_table %s, %d entries\n",(*p)->If->Name,(*p)->TabSize);
    EvrNetMLD_InitNode ((*p)->If->Id, (*p)->TabSize);
    /* Clear listener table */
    for (i = 0, mld_t = &(*p)->Table[0]; i < (*p)->TabSize; mld_t++, i++) {
      mld_t->State = MLD_STATE_NON;
    }
    (*p)->Ctrl->Tout = 0;
  }
}

/**
  \brief       De-initialize MLD nodes.
*/
void net_mld_node_uninit (void) {
  NET_MLD_CFG *const *p;

  for (p = &net_mld_list[0]; *p; p++) {
    DEBUGF (MLD,"Uninit_node %s\n",(*p)->If->Name);
    EvrNetMLD_UninitNode ((*p)->If->Id);
    /* Clear listener table and node control */
    memset ((*p)->Table, 0, sizeof(*(*p)->Table) * (*p)->TabSize);
    memset ((*p)->Ctrl, 0, sizeof(*(*p)->Ctrl));
  }
}

/**
  \brief       Join multicast group and start listening.
  \param[in]   if_id     interface identifier.
  \param[in]   ip6_addr  listening IPv6 address.
  \return      status code as defined with netStatus.
*/
__WEAK netStatus netMLD_Join (uint32_t if_id, const uint8_t *ip6_addr) {
  NET_IF_CFG *net_if = net_if_map_lan (if_id);
  NET_MLD_CFG  *h;
  NET_MLD_INFO *mld_t;

  START_LOCK (netStatus);

  /* Check interface */
  if (!(net_if && net_if->Ip6Cfg && net_if->Ip6Cfg->MldCfg)) {
    RETURN (netInvalidParameter);
  }
  /* Check listening IPv6 address */
  if (!mld_is_addr_valid (ip6_addr)) {
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip6Cfg->MldCfg;

  DEBUGF (MLD,"Join %s, Group %s\n",h->If->Name,net_addr6_ntoa(ip6_addr));
  EvrNetMLD_Join (h->If->Id, ip6_addr);
  mld_t = mld_table_find (h, ip6_addr);
  if (mld_t != NULL) {
    /* Node is already listening on this address */
    DEBUGF (MLD," Already in group %d\n",mld_t->Id);
    EvrNetMLD_AlreadyInGroup (h->If->Id, mld_t->Id);
    RETURN (netOK);
  }
  mld_t = mld_table_add (h, ip6_addr);
  if (mld_t == NULL) {
    ERRORF (MLD,"Join %s, No free table entries\n",h->If->Name);
    EvrNetMLD_NoFreeEntries (h->If->Id, h->TabSize & 0xFF);
    RETURN (netError);
  }
  DEBUGF (MLD," Sending report\n");
  EvrNetMLD_SendReport (h->If->Id, mld_t->IpAddr);
  mld_send_message (h, ICMP6_MCAST_REPORT, mld_t->IpAddr);
  mld_t->State = MLD_STATE_DELAYING;
  mld_t->Flags = MLD_FLAG_LAST;
  mld_t->Tout  = net_rand_max (MLD_UNSOL_TOUT);

  /* Multicast addresses changed */
  h->If->State->ConfigMcast = true;
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Leave multicast group and stop listening.
  \param[in]   if_id     interface identifier.
  \param[in]   ip6_addr  listening IPv6 address.
  \return      status code as defined with netStatus.
*/
__WEAK netStatus netMLD_Leave (uint32_t if_id, const uint8_t *ip6_addr) {
  NET_IF_CFG *net_if = net_if_map_lan (if_id);
  NET_MLD_CFG  *h;
  NET_MLD_INFO *mld_t;

  START_LOCK (netStatus);

  /* Check interface */
  if (!(net_if && net_if->Ip6Cfg && net_if->Ip6Cfg->MldCfg)) {
    RETURN (netInvalidParameter);
  }
  /* Check listening IPv6 address */
  if (!mld_is_addr_valid (ip6_addr)) {
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip6Cfg->MldCfg;

  DEBUGF (MLD,"Leave %s, Group %s\n",h->If->Name,net_addr6_ntoa(ip6_addr));
  EvrNetMLD_Leave (h->If->Id, ip6_addr);
  mld_t = mld_table_find (h, ip6_addr);
  if (mld_t == NULL) {
    /* Node is not listening on this address */
    ERRORF (MLD,"Leave %s, Not in group\n",h->If->Name);
    EvrNetMLD_NotInGroup (h->If->Id, ip6_addr);
    RETURN (netError);
  }
  if (mld_t->Flags & MLD_FLAG_LAST) {
    DEBUGF (MLD," Sending leave\n");
    EvrNetMLD_SendLeave (h->If->Id, ip6_addr);
    mld_send_message (h, ICMP6_MCAST_DONE, ip6_addr);
  }
  mld_t->State = MLD_STATE_NON;
  mld_t->Tout  = 0;
  mld_t->Flags = 0;

  /* Multicast addresses changed */
  h->If->State->ConfigMcast = true;
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Check if local node is listening on this address.
  \param[in]   net_if    network interface descriptor.
  \param[in]   ip6_addr  multicast IPv6 address to be checked.
  \return      status:
               - true  = listening,
               - false = not listening.
*/
bool net_mld_listening (NET_IF_CFG *net_if, const uint8_t *ip6_addr) {
  NET_MLD_CFG  *h = net_if->Ip6Cfg->MldCfg;
  NET_MLD_INFO *mld_t;

  /* Check if MLD enabled */
  if (h == NULL) {
    return (false);
  }
  mld_t = mld_table_find (h, ip6_addr);
  if (mld_t != NULL) {
    return (true);
  }
  return (false);
}

/**
  \brief       Collect multicast MAC addresses of listening nodes.
  \param[in]   net_if  network interface descriptor.
  \param[out]  buf     buffer to write multicast MAC array to.
  \return      number of MAC addresses written.
  \note        This function collects active multicast MAC addresses and writes
               them to an array in the buffer. The collection is then used to
               set up ethernet hardware MAC address filtering.
*/
uint32_t net_mld_collect_mcast (NET_IF_CFG *net_if, uint8_t *buf) {
  NET_MLD_CFG  *h = net_if->Ip6Cfg->MldCfg;
  NET_MLD_INFO *mld_t;
  int32_t i,j,n;

  /* Check if MLD enabled */
  if (h == NULL) {
    return (0);
  }

  n = 0;

  /* Only IP addresses of active groups */
  for (i = 0, mld_t = &h->Table[0]; i < h->TabSize; mld_t++, i++) {
    if (mld_t->State == MLD_STATE_NON) {
      continue;
    }
    net_addr6_to_mac (mld_t->IpAddr, &buf[n * NET_ADDR_ETH_LEN]);
    /* Check if this MAC already exists */
    for (j = 0; j < n; j++) {
      if (net_mac_comp (&buf[j * NET_ADDR_ETH_LEN], &buf[n * NET_ADDR_ETH_LEN])) {
        break;
      }
    }
    if (j == n) {
      /* Unique MAC, keep it */
      n++;
    }
  }
  /* Minimum value is 0 */
  return ((uint32_t)n);
}

/**
  \brief       Process received MLD message.
  \param[in]   net_if  network interface descriptor.
  \param[in]   frame   network frame.
*/
void net_mld_process (NET_IF_CFG *net_if, NET_FRAME *frame) {
  NET_MLD_CFG  *h = net_if->Ip6Cfg->MldCfg;
  NET_MLD_INFO *mld_t;
  NET_MLD_HEADER *mld_hdr;
  int32_t i,n,max_time;

  /* Check if MLD enabled */
  if (h == NULL) {
    return;
  }

  /* Check the frame length */
  if (frame->length < MLD_HEADER_LEN) {
    ERRORF (MLD,"Process %s, Frame too short\n",h->If->Name);
    EvrNetMLD_FrameTooShort (h->If->Id, frame->length, MLD_HEADER_LEN);
    return;
  }

  /* Checksum validated in ICMP6 */
  mld_hdr = __ALIGN_CAST(NET_MLD_HEADER *)&frame->data[frame->index];

  switch (mld_hdr->Type) {
    case ICMP6_MCAST_QUERY:
      /* Node Membership Query message */
      if (!(net_addr6_is_unspec (mld_hdr->McastAddr))) {
        /* Multicast-address-specific query */
        DEBUGF (MLD,"Process %s, Addr.Specific Query\n",h->If->Name);
        DEBUGF (MLD," Address: %s\n",net_addr6_ntoa(mld_hdr->McastAddr));
        EvrNetMLD_AddressSpecificQuery (h->If->Id, mld_hdr->McastAddr);
        /* Destination IPv6 and Group IPv6 must match */
        if (!net_addr6_comp (IP6_FRAME(frame)->DstAddr, mld_hdr->McastAddr)) {
          ERRORF (MLD,"Query %s, %s wrong\n",h->If->Name,
                       (mld_hdr->McastAddr[0] & 0xF0) == 0xE0 ? "Dst.IP" : "Group");
          EvrNetMLD_DestAddressWrong (h->If->Id, IP6_FRAME(frame)->DstAddr);
          break;
        }
        mld_t = mld_table_find (h, mld_hdr->McastAddr);
        if (mld_t == NULL) {
          /* Not member of this group */
          break;
        }
        max_time = ntohs (mld_hdr->MaxDelay);
        DEBUGF (MLD," MaxTime: %d ms\n",max_time);
        DEBUGF (MLD," Delayed report scheduled\n");
        EvrNetMLD_DelayedReportScheduled (h->If->Id, max_time & 0xFFFF);
        mld_t->State = MLD_STATE_DELAYING;
        /* Tick interval is 100 ms */
        mld_t->Tout  = net_rand_max (max_time / 100) & 0xFFFF;
        break;
      }
      /* General Query (Multicast address 0:0:0:0:0:0:0:0) */
      DEBUGF (MLD,"Process %s, General Query\n",h->If->Name);
      EvrNetMLD_GeneralQuery (h->If->Id, mld_hdr->McastAddr);
      if (!net_addr6_comp (IP6_FRAME(frame)->DstAddr, net_addr6_all_nodes)) {
        ERRORF (MLD,"Query %s, Dst.IP wrong\n",h->If->Name);
        EvrNetMLD_DestAddressWrong (h->If->Id, IP6_FRAME(frame)->DstAddr);
        break;
      }
      max_time = ntohs (mld_hdr->MaxDelay);
      DEBUGF (MLD," MaxTime: %d ms\n",max_time);
      EvrNetMLD_MaxTimeForReport (h->If->Id, max_time & 0xFFFF);

      /* Tick interval is 100 ms */
      max_time /= 100;
      n = 0;
      /* Generate reports for all active groups */
      for (i = 0, mld_t = &h->Table[0]; i < h->TabSize; mld_t++, i++) {
        switch (mld_t->State) {
          case MLD_STATE_DELAYING:
            if (mld_t->Tout > max_time) {
              /* Reset timer for MLD */
              mld_t->Tout = net_rand_max (max_time) & 0xFFFF;
            }
            n++;
            break;
          case MLD_STATE_IDLE:
            /* A report will be sent delayed from net_mld_node_run() */
            mld_t->State = MLD_STATE_DELAYING;
            mld_t->Tout  = net_rand_max (max_time) & 0xFFFF;
            n++;
            break;
        }
      }
      if (n != 0) {
        DEBUGF (MLD," Scheduled %d group reports\n",n);
        EvrNetMLD_GroupReportsScheduled (h->If->Id, n & 0xFF);
        break;
      }
      DEBUGF (MLD," No report scheduled\n");
      EvrNetMLD_NoReportScheduled (h->If->Id);
      break;

    case ICMP6_MCAST_REPORT:
      /* Node Membership Report message */
      DEBUGF (MLD,"Process %s, Report\n",h->If->Name);
      DEBUGF (MLD," Address: %s\n",net_addr6_ntoa(mld_hdr->McastAddr));
      EvrNetMLD_ReportReceived (h->If->Id, mld_hdr->McastAddr);
      mld_t = mld_table_find (h, mld_hdr->McastAddr);
      if (mld_t == NULL) {
        break;
      }
      if (mld_t->State == MLD_STATE_DELAYING) {
        /* A router is informed about this group, cancel Report */
        DEBUGF (MLD," Own report canceled\n");
        EvrNetMLD_OwnReportCanceled (h->If->Id, mld_t->Id);
        mld_t->State  = MLD_STATE_IDLE;
        mld_t->Flags &= ~MLD_FLAG_LAST;
        mld_t->Tout   = 0;
      }
      break;

    default:
      /* Silently ignore other MLD messages */
      break;
  }
}

/**
  \brief       Run MLD node main process.
*/
void net_mld_node_run (void) {
  NET_MLD_CFG *const *p;

  for (p = &net_mld_list[0]; *p; p++) {
    mld_node_run (*p);
  }
}

/**
  \brief       Run MLD node main process.
  \param[in]   h  MLD instance handle.
*/
static void mld_node_run (NET_MLD_CFG *h) {
  NET_MLD_INFO *mld_t;
  int32_t i;

  if (!(sys->Flags & SYS_FLAG_TICK)) {
    return;
  }

  /* Polling interval is 100 ms */
  for (i = 0; i < h->TabSize; i++) {
    mld_t = &h->Table[i];
    if (mld_t->State != MLD_STATE_DELAYING) {
      continue;
    }
    if (mld_t->Tout != 0) {
      mld_t->Tout--;
    }
    else {
      /* On timeout send report message */
      DEBUGF (MLD,"Send_report %s, Group %s\n",h->If->Name,
                                               net_addr6_ntoa(mld_t->IpAddr));
      EvrNetMLD_SendDelayedReport (h->If->Id, mld_t->IpAddr);
      mld_send_message (h, ICMP6_MCAST_REPORT, mld_t->IpAddr);
      mld_t->State  = MLD_STATE_IDLE;
      mld_t->Flags |= MLD_FLAG_LAST;
    }
  }
}

/**
  \brief       Construct MLD message and sent it.
  \param[in]   h         MLD instance handle.
  \param[in]   type      message type.
  \param[in]   ip6_addr  listening address.
*/
static void mld_send_message (NET_MLD_CFG *h,
                              uint8_t type, const uint8_t *ip6_addr) {
  const uint8_t  *dst_addr;
  NET_MLD_HEADER *mld_frm;
  NET_FRAME *frame;

  /* Reserve space for IPv6 Router Alert option */
  frame = net_mem_alloc (IP6_DATA_OFFS + 8 + MLD_HEADER_LEN);
  frame->index = IP6_DATA_OFFS + 8;
  mld_frm = __ALIGN_CAST(NET_MLD_HEADER *)&frame->data[frame->index];

  mld_frm->MaxDelay = 0;
  mld_frm->Reserved = 0;
  net_addr6_copy (mld_frm->McastAddr, ip6_addr);

  /* Calculate total length of the MLD frame */
  frame->length = MLD_HEADER_LEN;
  dst_addr = (type == ICMP6_MCAST_DONE) ? net_addr6_all_routers : ip6_addr;
  net_icmp6_send (h->If, frame, h->If->localm6->LLAddr, dst_addr, type);
  net_mem_free (frame);
}

/**
  \brief       Find IPv6 address in MLD listener table.
  \param[in]   h         MLD instance handle.
  \param[in]   ip6_addr  listening IPv6 address.
  \return      pointer to table entry or NULL if not found.
*/
static NET_MLD_INFO *mld_table_find (NET_MLD_CFG *h, const uint8_t *ip6_addr) {
  NET_MLD_INFO *mld_t;
  int32_t i;

  for (i = 0, mld_t = &h->Table[0]; i < h->TabSize; mld_t++, i++) {
    if (mld_t->State != MLD_STATE_NON) {
      if (net_addr6_comp (ip6_addr, mld_t->IpAddr)) {
        return (mld_t);
      }
    }
  }
  /* Not found */
  return (NULL);
}

/**
  \brief       Add IPv6 address to MLD listener table.
  \param[in]   h         MLD instance handle.
  \param[in]   ip6_addr  listening IPv6 address.
  \return      pointer to table entry or NULL if failed.
*/
static NET_MLD_INFO *mld_table_add (NET_MLD_CFG *h, const uint8_t *ip6_addr) {
  NET_MLD_INFO *mld_t;
  int32_t i;

  for (i = 1, mld_t = &h->Table[0]; i <= h->TabSize; mld_t++, i++) {
    if (mld_t->State == MLD_STATE_NON) {
      net_addr6_copy (mld_t->IpAddr, ip6_addr);
      mld_t->Id = i & 0xFF;
      return (mld_t);
    }
  }
  /* All entries used */
  return (NULL);
}

/**
  \brief       Check if multicast IPv6 address is valid.
  \param[in]   ip6_addr  listening IPv6 address.
  \return      status:
               - true  = address is valid,
               - false = not valid.
*/
static bool mld_is_addr_valid (const uint8_t *ip6_addr) {
  if (ip6_addr == NULL) {
    return (false);
  }
  if (ip6_addr[0] != 0xFF) {
    /* Not multicast address "FF00::/8" */
    return (false);
  }
  if(((get_u32 (&ip6_addr[0]) & 0xFFF0FFFF) == 0xFF000000) &&
      (get_u32 (&ip6_addr[4])  == 0x00000000)              &&
      (get_u32 (&ip6_addr[8])  == 0x00000000)              &&
      (get_u32 (&ip6_addr[12]) == 0x00000000)) {
    /* Reserved multicast address "FF0x::" */
    return (false);
  }
  if ((get_u32 (&ip6_addr[0])  == 0xFF020000) &&
      (get_u32 (&ip6_addr[4])  == 0x00000000) &&
      (get_u32 (&ip6_addr[8])  == 0x00000001) &&
      (get_u32 (&ip6_addr[12]) >= 0xFF000000)) {
    /* Solicited-node multicast address "FF02::1:FFxx:xxxx"*/
    return (false);
  }
  if (net_addr6_comp (ip6_addr, net_addr6_all_nodes)   ||
      net_addr6_comp (ip6_addr, net_addr6_all_routers)) {
    return (false);
  }
  return (true);
}
