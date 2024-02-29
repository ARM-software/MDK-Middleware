/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_igmp.c
 * Purpose: Internet Group Management
 *----------------------------------------------------------------------------*/

#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_mem.h"
#include "net_eth.h"
#include "net_ip4.h"
#include "net_igmp.h"
#include "net_dbg.h"

/* Global variables */
NET_IGMP_CTRL net_eth0_igmp_control;
NET_IGMP_CTRL net_eth1_igmp_control;
NET_IGMP_CTRL net_wifi0_igmp_control;
NET_IGMP_CTRL net_wifi1_igmp_control;

/* Local constants */
static const uint8_t IP_AllHosts[NET_ADDR_IP4_LEN]   = { 224, 0, 0, 1 };
static const uint8_t IP_AllRouters[NET_ADDR_IP4_LEN] = { 224, 0, 0, 2 };

/* Code shortening macros */
#define ctrl         (h->Ctrl)

/* Local Functions */
static void igmp_host_run (NET_IGMP_CFG *h);
static void igmp_send_report (NET_IGMP_CFG *h, const uint8_t *ip_addr);
static void igmp_send_leave (NET_IGMP_CFG *h, const uint8_t *ip_addr);
static NET_IGMP_INFO *igmp_map_group (NET_IGMP_CFG *h, const uint8_t *ip_addr);
static NET_IGMP_INFO *igmp_alloc_group (NET_IGMP_CFG *h, const uint8_t *ip_addr);
static bool igmp_is_group_valid (const uint8_t *ip4_addr);
#ifdef DEBUG_STDIO
 static void debug_inf2 (const char *msg, const uint8_t *ip_addr);
#endif

/**
  \brief       Initialize IGMP hosts.
  \return      none.
*/
void net_igmp_host_init (void) {
  NET_IGMP_INFO *igmp_t;
  NET_IGMP_CFG *const *p;
  uint32_t i;

  for (p = &net_igmp_list[0]; *p; p++) {
    DEBUGF (IGMP,"Init_table %s, %d entries\n",(*p)->If->Name,(*p)->TabSize);
    EvrNetIGMP_InitManager ((*p)->If->Id, (*p)->TabSize);
    /* Clear membership table */
    for (i = 0, igmp_t = &(*p)->Table[0]; i < (*p)->TabSize; igmp_t++, i++) {
      igmp_t->State = IGMP_STATE_NON;
    }
    /* Default active mode is IGMPv2 */
    (*p)->Ctrl->Mode   = IGMP_MODE_V2;
    (*p)->Ctrl->T200ms = 1;
  }
}

/**
  \brief       De-initialize IGMP hosts.
  \return      none.
*/
void net_igmp_host_uninit (void) {
  NET_IGMP_CFG *const *p;

  for (p = &net_igmp_list[0]; *p; p++) {
    DEBUGF (IGMP,"Uninit_manager %s\n",(*p)->If->Name);
    EvrNetIGMP_UninitManager ((*p)->If->Id);
    /* Clear membership table and manager control */
    memset ((*p)->Table, 0, sizeof(*(*p)->Table) * (*p)->TabSize);
    memset ((*p)->Ctrl, 0, sizeof(*(*p)->Ctrl));
  }
}

/**
  \brief       Join to a host group specified with IP address.
  \param[in]   if_id     interface identifier.
  \param[in]   ip4_addr  group IP address.
  \return      status code as defined with netStatus.
*/
__WEAK netStatus netIGMP_Join (uint32_t if_id, const uint8_t *ip4_addr) {
  NET_IF_CFG *net_if = net_if_map_lan (if_id);
  NET_IGMP_CFG  *h;
  NET_IGMP_INFO *igmp_t;

  START_LOCK (netStatus);

  /* Check interface */
  if (!(net_if && net_if->Ip4Cfg && net_if->Ip4Cfg->IgmpCfg)) {
    RETURN (netInvalidParameter);
  }
  /* Check group IP address */
  if (!igmp_is_group_valid (ip4_addr)) {
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip4Cfg->IgmpCfg;

  DEBUGF (IGMP,"Join %s, Group %s\n",h->If->Name,net_addr4_ntoa(ip4_addr));
  EvrNetIGMP_Join (h->If->Id, ip4_addr);
  igmp_t = igmp_map_group (h, ip4_addr);
  if (igmp_t != NULL) {
    /* Host is already a member of this group */
    DEBUGF (IGMP," Already in group %d\n",igmp_t->Id);
    EvrNetIGMP_AlreadyInGroup (h->If->Id, igmp_t->Id);
    RETURN (netOK);
  }
  igmp_t = igmp_alloc_group (h, ip4_addr);
  if (igmp_t == NULL) {
    ERRORF (IGMP,"Join %s, No free table entries\n",h->If->Name);
    EvrNetIGMP_NoFreeEntries (h->If->Id, h->TabSize & 0xFF);
    RETURN (netError);
  }
  DEBUGF (IGMP," Sending report\n");
  EvrNetIGMP_SendReport (h->If->Id, igmp_t->IpAddr);
  igmp_send_report (h, igmp_t->IpAddr);
  igmp_t->State = IGMP_STATE_DELAYING;
  igmp_t->Flags = IGMP_FLAG_LAST;
  /* Our IGMP tick interval is 200 ms */
  igmp_t->Tout  = net_rand_max (IGMP_UNSOL_TOUT) >> 1;

  /* Multicast addresses changed */
  h->If->State->ConfigMcast = true;
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Leave a host group specified with IP address.
  \param[in]   if_id     interface identifier.
  \param[in]   ip4_addr  group IP address.
  \return      status code as defined with netStatus.
*/
__WEAK netStatus netIGMP_Leave (uint32_t if_id, const uint8_t *ip4_addr) {
  NET_IF_CFG *net_if = net_if_map_lan (if_id);
  NET_IGMP_CFG  *h;
  NET_IGMP_INFO *igmp_t;

  START_LOCK (netStatus);

  /* Check interface */
  if (!(net_if && net_if->Ip4Cfg && net_if->Ip4Cfg->IgmpCfg)) {
    RETURN (netInvalidParameter);
  }
  /* Check group IP address */
  if (!igmp_is_group_valid (ip4_addr)) {
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip4Cfg->IgmpCfg;

  DEBUGF (IGMP,"Leave %s, Group %s\n",h->If->Name,net_addr4_ntoa(ip4_addr));
  EvrNetIGMP_Leave (h->If->Id, ip4_addr);
  igmp_t = igmp_map_group (h, ip4_addr);
  if (igmp_t == NULL) {
    /* Host is not a member of this group */
    ERRORF (IGMP,"Leave %s, Not in group\n",h->If->Name);
    EvrNetIGMP_NotInGroup (h->If->Id, ip4_addr);
    RETURN (netError);
  }
  if ((ctrl->Mode == IGMP_MODE_V2) && (igmp_t->Flags & IGMP_FLAG_LAST)) {
    DEBUGF (IGMP," Sending leave\n");
    EvrNetIGMP_SendLeave (h->If->Id, ip4_addr);
    igmp_send_leave (h, ip4_addr);
  }
  igmp_t->State = IGMP_STATE_NON;
  igmp_t->Tout  = 0;
  igmp_t->Flags = 0;

  /* Multicast addresses changed */
  h->If->State->ConfigMcast = true;
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Check if local host is a member of provided group.
  \param[in]   net_if    network interface descriptor.
  \param[in]   ip4_addr  multicast IPv4 address to be checked.
  \return      status:
               - true  = local host is member of a group,
               - false = local host is not member.
*/
bool net_igmp_is_member (NET_IF_CFG *net_if, const uint8_t *ip4_addr) {
  NET_IGMP_CFG  *h = net_if->Ip4Cfg->IgmpCfg;
  NET_IGMP_INFO *igmp_t;

  /* Check if IGMP enabled */
  if (h == NULL) {
    return (false);
  }
  /* Check if All Hosts group */
  if (net_addr4_comp (ip4_addr, IP_AllHosts)) {
    return (true);
  }
  igmp_t = igmp_map_group (h, ip4_addr);
  if (igmp_t != NULL) {
    return (true);
  }
  return (false);
}

/**
  \brief       Collect multicast MAC addresses of active IGMP groups.
  \param[in]   net_if  network interface descriptor.
  \param[out]  buf     buffer to write multicast MAC array to.
  \return      number of MAC addresses written.
  \note        This function collects active multicast MAC addresses and writes
               them to an array in the buffer. The collection is then used to
               set up ethernet hardware MAC address filtering.
*/
uint32_t net_igmp_collect_mcast (NET_IF_CFG *net_if, uint8_t *buf) {
  NET_IGMP_CFG  *h = net_if->Ip4Cfg->IgmpCfg;
  NET_IGMP_INFO *igmp_t;
  uint32_t i,j,n;

  /* Check if IGMP enabled */
  if (h == NULL) {
    return (0);
  }

  net_addr4_to_mac (IP_AllHosts, &buf[0]);
  n = 1;

  /* Only IP addresses of active groups */
  for (i = 0, igmp_t = &h->Table[0]; i < h->TabSize; igmp_t++, i++) {
    if (igmp_t->State == IGMP_STATE_NON) {
      continue;
    }
    net_addr4_to_mac (igmp_t->IpAddr, &buf[n * NET_ADDR_ETH_LEN]);
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
  /* Minimum value is 1 */
  return (n);
}

/**
  \brief       Process received IGMP message.
  \param[in]   net_if  network interface descriptor.
  \param[in]   frame   network frame.
  \return      none.
*/
void net_igmp_process (NET_IF_CFG *net_if, NET_FRAME *frame) {
  NET_IGMP_CFG  *h = net_if->Ip4Cfg->IgmpCfg;
  NET_IGMP_INFO *igmp_t;
  NET_IGMP_HEADER *igmp_frm;
  uint32_t i,n,max_time;

  /* Check if IGMP enabled */
  if (h == NULL) {
    return;
  }

  DEBUGF (IGMP,"*** Process_frame %s ***\n",h->If->Name);
  if (frame->length < IGMP_HEADER_LEN) {
    ERRORF (IGMP,"Process %s, Frame too short\n",h->If->Name);
    EvrNetIGMP_FrameTooShort (h->If->Id, frame->length, IGMP_HEADER_LEN);
    return;
  }
  igmp_frm = __ALIGN_CAST(NET_IGMP_HEADER *)&frame->data[frame->index];
  EvrNetIGMP_ReceiveFrame (h->If->Id, igmp_frm->VerType, frame->length);

  /* Check if IGMP frame is valid */
  /* Checksum is always computed over the whole IP payload [RFC2236 page 2] */
  if (net_ip4_chksum_buf (igmp_frm, frame->length) != 0) {
    ERRORF (IGMP,"Process %s, Checksum failed\n",h->If->Name);
    EvrNetIGMP_ChecksumFailed (h->If->Id, frame->length);
    return;
  }
  /* Frame is valid, check the Message Type */
  switch (igmp_frm->VerType) {
    case IGMP_QUERY:
      /* Host Membership Query message */
      if ((get_u32 (igmp_frm->GrpIpAddr)) != 0) {
        /* IGMPv2 Group Specific query */
        DEBUG_INF2 (" Type: Group-Specific Query",igmp_frm->GrpIpAddr);
        EvrNetIGMP_GroupSpecificQuery (h->If->Id, igmp_frm->GrpIpAddr);
        /* Destination IP and Group IP must match */
        if (!net_addr4_comp (IP4_FRAME(frame)->DstAddr, igmp_frm->GrpIpAddr)) {
          ERRORF (IGMP,"Query %s, %s IP wrong\n",h->If->Name,
                       (igmp_frm->GrpIpAddr[0] & 0xF0) == 0xE0 ? "Dst." : "Group");
          EvrNetIGMP_DestAddressWrong (h->If->Id, IP4_FRAME(frame)->DstAddr);
          break;
        }
        igmp_t = igmp_map_group (h, igmp_frm->GrpIpAddr);
        if (igmp_t == NULL) {
          /* Not member of this group */
          break;
        }
        DEBUGF (IGMP," Max. Time : %d\n",igmp_frm->MaxTime);
        DEBUGF (IGMP," Delayed report scheduled\n");
        EvrNetIGMP_DelayedReportScheduled (h->If->Id, igmp_frm->MaxTime);
        igmp_t->State = IGMP_STATE_DELAYING;
        /* Our IGMP tick interval is 200 ms */
        igmp_t->Tout  = net_rand_max (igmp_frm->MaxTime) >> 1;
        break;
      }
      /* General Query (Group address 0.0.0.0) */
      DEBUG_INF2 (" Type: General Query",igmp_frm->GrpIpAddr);
      EvrNetIGMP_GeneralQuery (h->If->Id, igmp_frm->GrpIpAddr);
      if (!net_addr4_comp (IP4_FRAME(frame)->DstAddr, IP_AllHosts)) {
        ERRORF (IGMP,"Query %s, Dst.IP wrong\n",h->If->Name);
        EvrNetIGMP_DestAddressWrong (h->If->Id, IP4_FRAME(frame)->DstAddr);
        break;
      }
      max_time = igmp_frm->MaxTime;
      if (max_time == 0) {
        /* IGMPv1 Query received, switch the mode */
        if (ctrl->Mode != IGMP_MODE_V1) {
          DEBUGF (IGMP," V1-Query, start IGMPv1 mode\n");
          EvrNetIGMP_StartModeIGMPv1 (h->If->Id);
          ctrl->Mode = IGMP_MODE_V1;
        }
        /* Start/Restart IGMPv1 mode timer */
        ctrl->Tout = IGMP_V1_TOUT >> 1;
        max_time   = IGMP_UNSOL_TOUT;
      }
      else {
        DEBUGF (IGMP," Max. Time : %d\n",max_time);
        EvrNetIGMP_MaxTimeForReport (h->If->Id, max_time & 0xFF);
      }
      /* Our IGMP tick interval is 200 ms */
      max_time >>= 1;
      n = 0;
      /* Generate reports for all active groups */
      for (i = 0, igmp_t = &h->Table[0]; i < h->TabSize; igmp_t++, i++) {
        switch (igmp_t->State) {
          case IGMP_STATE_DELAYING:
            if (igmp_t->Tout > max_time) {
              /* Reset timer for IGMPv2 */
              igmp_t->Tout = net_rand_max (max_time) & 0xFF;
            }
            n++;
            break;
          case IGMP_STATE_IDLE:
            /* A report will be sent delayed from net_igmp_host_run() */
            igmp_t->State = IGMP_STATE_DELAYING;
            igmp_t->Tout  = net_rand_max (max_time) & 0xFF;
            n++;
            break;
        }
      }
      if (n != 0) {
        DEBUGF (IGMP," Scheduled %d group reports\n",n);
        EvrNetIGMP_GroupReportsScheduled (h->If->Id, n);
        break;
      }
      DEBUGF (IGMP," No report scheduled\n");
      EvrNetIGMP_NoReportScheduled (h->If->Id);
      break;
    case IGMP_REPORT_V1:
      /* IGMPv1 Membership Report message */
    case IGMP_REPORT_V2:
      /* IGMPv2 Membership Report message */
      DEBUG_INF2 (" Type: Report",igmp_frm->GrpIpAddr);
      EvrNetIGMP_ReportReceived (h->If->Id, igmp_frm->GrpIpAddr);
      igmp_t = igmp_map_group (h, igmp_frm->GrpIpAddr);
      if (igmp_t == NULL) {
        break;
      }
      if (igmp_t->State == IGMP_STATE_DELAYING) {
        /* A router is informed about this group, cancel Report */
        DEBUGF (IGMP," Own report canceled\n");
        EvrNetIGMP_OwnReportCanceled (h->If->Id, igmp_t->Id);
        igmp_t->State  = IGMP_STATE_IDLE;
        igmp_t->Flags &= ~IGMP_FLAG_LAST;
        igmp_t->Tout   = 0;
      }
      break;
    default:
      /* Silently ignore other IGMP messages */
      break;
  }
}

/**
  \brief       Run main process of IGMP manager.
  \return      none.
*/
void net_igmp_host_run (void) {
  NET_IGMP_CFG *const *p;

  for (p = &net_igmp_list[0]; *p; p++) {
    igmp_host_run (*p);
  }
}

/**
  \brief       Run main process of IGMP manager.
  \param[in]   h  IGMP instance handle.
  \return      none.
*/
static void igmp_host_run (NET_IGMP_CFG *h) {
  NET_IGMP_INFO *igmp_t;
  uint32_t i;

  if (!(sys->Flags & SYS_FLAG_TICK)) {
    return;
  }
  /* Prescaler for 200ms intervals */
  if (--ctrl->T200ms) {
    return;
  }
  ctrl->T200ms = SYS_TICK_T200MS;
  if (ctrl->Tout) {
    if (--ctrl->Tout == 0) {
      /* IGMPv1 Router present timeout */
      DEBUGF (IGMP,"Timeout %s, start IGMPv2 mode\n",h->If->Name);
      EvrNetIGMP_StartModeIGMPv2 (h->If->Id);
      ctrl->Mode = IGMP_MODE_V2;
    }
  }

  /* Polling Interval is 200 ms */
  for (i = 0; i < h->TabSize; i++) {
    igmp_t = &h->Table[i];
    if (igmp_t->State != IGMP_STATE_DELAYING) {
      continue;
    }
    if (igmp_t->Tout != 0) {
      igmp_t->Tout--;
    }
    else {
      /* On timeout send a Report */
      DEBUGF (IGMP,"Send_report %s, Group %s\n",h->If->Name,
                                                net_addr4_ntoa(igmp_t->IpAddr));
      EvrNetIGMP_SendDelayedReport (h->If->Id, igmp_t->IpAddr);
      igmp_send_report (h, igmp_t->IpAddr);
      igmp_t->State  = IGMP_STATE_IDLE;
      igmp_t->Flags |= IGMP_FLAG_LAST;
    }
  }
}

/**
  \brief       Construct IGMP Report message and sent it.
  \param[in]   h        IGMP instance handle.
  \param[in]   ip_addr  group IP address.
  \return      none.
*/
static void igmp_send_report (NET_IGMP_CFG *h, const uint8_t *ip_addr) {
  NET_FRAME *frame;
  NET_IGMP_HEADER *igmp_frm;

  if (ctrl->Mode == IGMP_MODE_V1) {
    /* No IP Router Alert option */
    frame = net_mem_alloc (IP4_DATA_OFFS + IGMP_HEADER_LEN);
    frame->index = IP4_DATA_OFFS;
    igmp_frm = __ALIGN_CAST(NET_IGMP_HEADER *)&frame->data[IP4_DATA_OFFS];
    igmp_frm->VerType = IGMP_REPORT_V1;
  }
  else {
    /* Reserve space for IP Router Alert option */
    frame = net_mem_alloc (IP4_DATA_OFFS + 4 + IGMP_HEADER_LEN);
    frame->index = IP4_DATA_OFFS + 4;
    igmp_frm = __ALIGN_CAST(NET_IGMP_HEADER *)&frame->data[IP4_DATA_OFFS + 4];
    igmp_frm->VerType = IGMP_REPORT_V2;
  }
  igmp_frm->MaxTime = 0;
  igmp_frm->Chksum  = 0;
  net_addr4_copy (igmp_frm->GrpIpAddr, ip_addr);
  igmp_frm->Chksum  = htons(net_ip4_chksum_buf (igmp_frm, IGMP_HEADER_LEN));

  /* Calculate the total length of the IGMP frame */
  frame->length = IGMP_HEADER_LEN;
  net_ip4_send_frame (h->If, frame, ip_addr, IP4_PROT_IGMP, 0, 1);
  net_mem_free (frame);
}

/**
  \brief       Construct IGMPv2 Leave message and sent it.
  \param[in]   h        IGMP instance handle.
  \param[in]   ip_addr  group IP address.
  \return      none.
  \note        This is for IGMP v2 only!
*/
static void igmp_send_leave (NET_IGMP_CFG *h, const uint8_t *ip_addr) {
  NET_FRAME *frame;
  NET_IGMP_HEADER *igmp_frm;

  frame = net_mem_alloc (IP4_DATA_OFFS + 4 + IGMP_HEADER_LEN);
  igmp_frm = __ALIGN_CAST(NET_IGMP_HEADER *)&frame->data[IP4_DATA_OFFS + 4];
  igmp_frm->VerType = IGMP_LEAVE_V2;
  igmp_frm->MaxTime = 0;
  igmp_frm->Chksum  = 0;
  net_addr4_copy (igmp_frm->GrpIpAddr, ip_addr);
  igmp_frm->Chksum  = htons(net_ip4_chksum_buf (igmp_frm, IGMP_HEADER_LEN));

  /* Calculate the total length of the IGMP frame */
  frame->length = IGMP_HEADER_LEN;
  frame->index  = IP4_DATA_OFFS + 4;
  net_ip4_send_frame (h->If, frame, IP_AllRouters, IP4_PROT_IGMP, 0, 1);
  net_mem_free (frame);
}

/**
  \brief       Map group IP address to IGMP table entry.
  \param[in]   h        IGMP instance handle.
  \param[in]   ip_addr  group IP address.
  \return      pointer to IGMP table entry or NULL if not found.
*/
static NET_IGMP_INFO *igmp_map_group (NET_IGMP_CFG *h, const uint8_t *ip_addr) {
  NET_IGMP_INFO *igmp_t;
  uint32_t i;

  for (i = 0, igmp_t = &h->Table[0]; i < h->TabSize; igmp_t++, i++) {
    if (igmp_t->State != IGMP_STATE_NON) {
      if (net_addr4_comp (ip_addr, igmp_t->IpAddr)) {
        return (igmp_t);
      }
    }
  }
  return (NULL);
}

/**
  \brief       Allocate a free entry in IGMP table.
  \param[in]   h        IGMP instance handle.
  \param[in]   ip_addr  group IP address.
  \return      pointer to IGMP table entry or NULL if failed.
*/
static NET_IGMP_INFO *igmp_alloc_group (NET_IGMP_CFG *h, const uint8_t *ip_addr) {
  NET_IGMP_INFO *igmp_t;
  uint32_t i;

  for (i = 1, igmp_t = &h->Table[0]; i <= h->TabSize; igmp_t++, i++) {
    if (igmp_t->State == IGMP_STATE_NON) {
      net_addr4_copy (igmp_t->IpAddr, ip_addr);
      igmp_t->Id = i & 0xFF;
      return (igmp_t);
    }
  }
  return (NULL);
}

/**
  \brief       Check if multicast IP address is valid.
  \param[in]   ip4_addr  group IP address.
  \return      status:
               - true  = address is valid,
               - false = not valid.
*/
static bool igmp_is_group_valid (const uint8_t *ip4_addr) {
  if (ip4_addr == NULL) {
    return (false);
  }
  if ((ip4_addr[0] & 0xF0) != 0xE0) {
    /* Not multicast address */
    return (false);
  }
  if (net_addr4_comp (ip4_addr, IP_AllHosts)   ||
      net_addr4_comp (ip4_addr, IP_AllRouters)) {
    return (false);
  }
  return (true);
}

#ifdef DEBUG_STDIO
/**
  \brief       Debug print IGMP address information.
  \param[in]   msg      Info message to print.
  \param[in]   ip_addr  IP address to print.
  \return      none.
*/
static void debug_inf2 (const char *msg, const uint8_t *ip_addr) {
  DEBUGF (IGMP,"%s\n",msg);
  DEBUGF (IGMP," IP Address: %s\n",net_addr4_ntoa(ip_addr));
}
#endif
