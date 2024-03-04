/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_arp.c
 * Purpose: Address Resolution for Ethernet
 *----------------------------------------------------------------------------*/

#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_eth.h"
#include "net_ip4.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_mem.h"
#include "net_arp.h"
#include "net_dbg.h"

/* Global variables */
NET_ARP_CTRL net_eth0_arp_control;
NET_ARP_CTRL net_eth1_arp_control;
NET_ARP_CTRL net_wifi0_arp_control;
NET_ARP_CTRL net_wifi1_arp_control;

/* Code shortening macros */
#define ctrl        ( h->Ctrl)
#define LocM        (*h->If->localm)

/* Local Functions */
static void  arp_send_reply (NET_ARP_CFG *h, NET_ARP_HEADER *arp_r, uint16_t opcode);
static void  arp_proc_response (NET_ARP_CFG *h, NET_ARP_HEADER *arp_r);
static void  arp_send_request (NET_ARP_CFG *h, const uint8_t *dst_mac,
                               const uint8_t *dst_ip, const uint8_t *src_ip);
static NET_ARP_INFO  *arp_cache_alloc (NET_ARP_CFG *h);
static netStatus arp_probe_start (NET_ARP_CFG *h,
                                  const uint8_t *ip4_addr, netARP_cb_t cb_func);
static const uint8_t *arp_find_mac (NET_IF_CFG *net_if, const uint8_t *ip4_addr);
static const uint8_t *arp_find_ip (NET_ARP_CFG *h, const uint8_t *mac_addr);
static void  arp_probe_run (NET_ARP_CFG *h);
static void  arp_cache_run (NET_ARP_CFG *h);
static bool  arp_is_cacheable (NET_ARP_CFG *h, const uint8_t *ip4_addr);
static bool  arp_sender_valid (NET_ARP_CFG *h, NET_ARP_HEADER *arp_r);
static void  arp_que_send (NET_ARP_CFG *h, NET_ARP_INFO *arp_t);
static void  arp_que_free (NET_ARP_INFO *arp_t);
#ifdef DEBUG_STDIO
 #define D_MAC  0
 #define D_IP   1
 static const char *type_ascii (uint8_t type);
 static void debug_info (const NET_ARP_HEADER *arp_hdr);
 static void debug_inf2 (uint32_t type, const uint8_t *addr);
#endif

/**
  \brief       Initialize ARP cache.
  \return      none.
*/
void net_arp_cache_init (void) {
  NET_ARP_INFO *arp_t;
  NET_ARP_CFG *const *p;
  uint32_t i;

  for (p = &net_arp_list[0]; *p; p++) {
    DEBUGF (ARP,"Init_cache %s, %d entries\n",(*p)->If->Name,(*p)->TabSize);
    EvrNetARP_InitCache ((*p)->If->Id, (*p)->TabSize);
    for (i = 0, arp_t = &(*p)->Table[0]; i < (*p)->TabSize; arp_t++, i++) {
      arp_t->State   = ARP_STATE_FREE;
      arp_t->tx_list = NULL;
    }
    memset ((*p)->Ctrl, 0, sizeof (*(*p)->Ctrl));
  }
}

/**
  \brief       De-initialize ARP cache.
  \return      none.
*/
void net_arp_cache_uninit (void) {
  NET_ARP_CFG *const *p;

  for (p = &net_arp_list[0]; *p; p++) {
    DEBUGF (ARP,"Uninit_cache %s\n",(*p)->If->Name);
    EvrNetARP_UninitCache ((*p)->If->Id);
    /* Clear cache table and resolver control */
    memset ((*p)->Table, 0, sizeof(*(*p)->Table) * (*p)->TabSize);
    net_sys_resume (&(*p)->Ctrl->xThread);
    memset ((*p)->Ctrl, 0, sizeof (*(*p)->Ctrl));
    (*p)->Ctrl->xRetv = netTimeout;
  }
}

/**
  \brief       Process received ARP message.
  \param[in]   net_if  network interface descriptor.
  \param[in]   frame   network frame.
  \return      none.
*/
void net_arp_process (NET_IF_CFG *net_if, NET_FRAME *frame) {
  NET_ARP_CFG  *h = net_if->Ip4Cfg->ArpCfg;
  NET_ARP_INFO *arp_t;
  uint32_t i;

  DEBUGF (ARP,"*** Process_frame %s ***\n",h->If->Name);
  if (frame->length < (PHY_HEADER_LEN + ARP_HEADER_LEN)) {
    ERRORF (ARP,"Process %s, Frame too short\n",h->If->Name);
    EvrNetARP_FrameTooShort (h->If->Id, frame->length, (PHY_HEADER_LEN + ARP_HEADER_LEN));
    return;
  }
  /* Check if ARP frame is valid */
  if (ARP_FRAME(frame)->HwType   != HTONS(HW_TYPE_ETH) ||
      ARP_FRAME(frame)->ProtType != HTONS(ETH_PROT_IP4)||
      ARP_FRAME(frame)->HwLen    != NET_ADDR_ETH_LEN   ||
      ARP_FRAME(frame)->ProtLen  != NET_ADDR_IP4_LEN) {
    ERRORF (ARP,"Process %s, Frame corrupted\n",h->If->Name);
    EvrNetARP_FrameCorrupted (h->If->Id);
    return;
  }

  DEBUG_INFO (ARP_FRAME(frame));
  EvrNetARP_ReceiveFrame (h->If->Id, ntohs(ARP_FRAME(frame)->OpCode), frame->length);
  EvrNetARP_ShowFrameHeader (ARP_FRAME(frame));

  /* Frame is valid, check the Opcode */
  switch (ntohs(ARP_FRAME(frame)->OpCode)) {
    case ARP_RQ:
      /* ARP request */
      if (!arp_sender_valid (h, ARP_FRAME(frame))) {
        DEBUGF (ARP," Discarded, SendAddr invalid\n");
        EvrNetARP_SenderAddressInvalid (h->If->Id);
        return;
      }
      if (net_addr4_comp (ARP_FRAME(frame)->TargIpAddr, LocM.IpAddr)) {
        /* Target IP is our's, send reply */
        net_arp_cache_add (h->If, ARP_FRAME(frame)->SendIpAddr,
                                  ARP_FRAME(frame)->SendHwAddr);
        arp_send_reply (h, ARP_FRAME(frame), ARP_REP);
        return;
      }
      if (net_addr4_comp (ARP_FRAME(frame)->TargIpAddr, ARP_FRAME(frame)->SendIpAddr)) {
        /* This is Gratuitous ARP */
        for (i = 0, arp_t = &h->Table[0]; i < h->TabSize; arp_t++, i++) {
          if (arp_t->State < ARP_STATE_RESOLVED) {
            continue;
          }
          if (net_mac_comp (arp_t->MacAddr, ARP_FRAME(frame)->SendHwAddr)) {
            /* MAC is cached, update entry */
            DEBUGF (ARP," Cache, Entry %d update\n",arp_t->Id);
            EvrNetARP_CacheEntryUpdate (h->If->Id, arp_t->Id);
            net_addr4_copy (arp_t->IpAddr, ARP_FRAME(frame)->SendIpAddr);
            arp_t->Tout    = h->CacheTout;
            arp_t->Retries = h->MaxRetry;
            arp_t->State   = ARP_STATE_RESOLVED;
            return;
          }
        }
      }
      goto wrong_ip;

    case InARP_RQ:
      /* Inverse ARP request */
      if (!arp_sender_valid (h, ARP_FRAME(frame))) {
        DEBUGF (ARP," Discarded, SendAddr invalid\n");
        EvrNetARP_SenderAddressInvalid (h->If->Id);
        return;
      }
      if (net_mac_comp (ARP_FRAME(frame)->TargHwAddr, h->If->MacAddr)) {
        /* Target MAC is our's, send a reply */
        net_arp_cache_add (h->If, ARP_FRAME(frame)->SendIpAddr,
                                  ARP_FRAME(frame)->SendHwAddr);
        arp_send_reply (h, ARP_FRAME(frame), InARP_RSP);
        return;
      }
      goto wrong_mac;

    case ARP_REP:
      /* ARP reply */
      if (get_u32 (ARP_FRAME(frame)->TargIpAddr) == 0) {
        /* Target IP is 0.0.0.0, this is a probe response.          */
        /* Win7 broadcasts a probe reply to gratuitous ARP request. */
        if (ctrl->ProbeIp && net_addr4_comp (ARP_FRAME(frame)->SendIpAddr, ctrl->ProbeIp)){
          /* Probe response to our AutoIP ARP probe request */
          ctrl->ProbeIp[0] = 0;
          break;
        }
        if ((ctrl->Flags & ARP_FLAG_PROBE_BUSY) &&
            net_addr4_comp (ARP_FRAME(frame)->SendIpAddr, ctrl->TargAddr)) {
          /* Probe response to user ARP probe request */
          DEBUGF (ARP," Probe Response received\n");
          EvrNetARP_ProbeResponseReceived (h->If->Id, ARP_FRAME(frame)->SendIpAddr);
          ctrl->Flags &= ~ARP_FLAG_PROBE_BUSY;
          if (ctrl->cb_func) {
            ctrl->cb_func (netARP_EventSuccess);
            break;
          }
          ctrl->xRetv = netOK;
          net_sys_resume (&ctrl->xThread);
        }
        break;
      }
      if (net_addr4_comp (ARP_FRAME(frame)->TargIpAddr, LocM.IpAddr)) {
        /* Target IP is our's, process response */
        arp_proc_response (h, ARP_FRAME(frame));
        return;
      }
wrong_ip:
      DEBUGF (ARP," Discarded, Wrong target IP\n");
      EvrNetARP_WrongIpAddress (h->If->Id, ARP_FRAME(frame)->TargIpAddr);
      break;

    case InARP_RSP:
      /* Inverse ARP response */
      if (net_mac_comp (ARP_FRAME(frame)->TargHwAddr, h->If->MacAddr)) {
        /* Target MAC is our's, process response */
        arp_proc_response (h, ARP_FRAME(frame));
        return;
      }
wrong_mac:
      DEBUGF (ARP," Discarded, Wrong target MAC\n");
      EvrNetARP_WrongMacAddress (h->If->Id, ARP_FRAME(frame)->TargHwAddr);
      break;

    case RARP_RQ:
      /* Reverse ARP request */
    case RARP_REP:
      /* Reverse ARP response */
    default:
      /* Unknown ARP opcode */
      DEBUGF (ARP," Discarded, Wrong Opcode\n");
      EvrNetARP_OpcodeUnknown (h->If->Id, ntohs(ARP_FRAME(frame)->OpCode));
      break;
  }
}

/**
  \brief       Construct and send ARP reply.
  \param[in]   h       ARP instance handle.
  \param[in]   arp_r   received ARP request.
  \param[in]   opcode  reply opcode.
  \return      none.
*/
static void arp_send_reply (NET_ARP_CFG *h, NET_ARP_HEADER *arp_r, uint16_t opcode) {
  NET_FRAME *frame;

  DEBUGF (ARP,"Send_reply %s\n",h->If->Name);
  EvrNetARP_SendReply (h->If->Id, opcode);
  frame = net_mem_alloc (PHY_HEADER_LEN + ARP_HEADER_LEN);

  /* Construct the Ethernet header */
  net_mac_copy (ETH_FRAME(frame)->DstAddr, arp_r->SendHwAddr);
  net_mac_copy (ETH_FRAME(frame)->SrcAddr, h->If->MacAddr);
  ETH_FRAME(frame)->Prot = HTONS(ETH_PROT_ARP);

  ARP_FRAME(frame)->HwType   = HTONS(HW_TYPE_ETH);
  ARP_FRAME(frame)->ProtType = HTONS(ETH_PROT_IP4);
  ARP_FRAME(frame)->HwLen    = NET_ADDR_ETH_LEN;
  ARP_FRAME(frame)->ProtLen  = NET_ADDR_IP4_LEN;
  ARP_FRAME(frame)->OpCode   = htons(opcode);
  net_mac_copy   (ARP_FRAME(frame)->TargHwAddr, arp_r->SendHwAddr);
  net_addr4_copy (ARP_FRAME(frame)->TargIpAddr, arp_r->SendIpAddr);
  net_mac_copy   (ARP_FRAME(frame)->SendHwAddr, h->If->MacAddr);
  net_addr4_copy (ARP_FRAME(frame)->SendIpAddr, LocM.IpAddr);

  DEBUG_INFO (ARP_FRAME(frame));
  EvrNetARP_ShowFrameHeader (ARP_FRAME(frame));
  h->If->output_lan (h->If->Id & 0xFF, frame);
  net_mem_free (frame);
}

/**
  \brief       Process received response to our ARP/InARP request.
  \param[in]   h      ARP instance handle.
  \param[in]   arp_r  received ARP response.
  \return      none.
*/
static void arp_proc_response (NET_ARP_CFG *h, NET_ARP_HEADER *arp_r) {
  NET_ARP_INFO *arp_t;
  uint32_t i;

  /* Are we waiting for that reply? */
  for (i = 0, arp_t = &h->Table[0]; i < h->TabSize; arp_t++, i++) {
    if ((arp_t->State != ARP_STATE_PENDING) &&
        (arp_t->State != ARP_STATE_REFRESH)) {
      continue;
    }
    if ((arp_r->OpCode) == HTONS(InARP_RSP)) {
      /* Inverse ARP reply */
      if (!net_mac_comp (arp_r->SendHwAddr, arp_t->MacAddr)) {
        continue;
      }
      /* This remote MAC is cached, so refresh it */
      net_addr4_copy (arp_t->IpAddr, arp_r->SendIpAddr);
      goto update;
    }
    /* Regular ARP reply */
    if (net_addr4_comp (arp_r->SendIpAddr, arp_t->IpAddr)) {
      /* This remote IP is cached, so refresh it */
      net_mac_copy (arp_t->MacAddr, arp_r->SendHwAddr);
      arp_que_send (h, arp_t);
update:
      DEBUGF (ARP," Cache, Entry %d refreshed\n",arp_t->Id);
      EvrNetARP_CacheEntryRefreshed (h->If->Id, arp_t->Id);
      if ((arp_t->Type  == ARP_TYPE_INUSE_IP) &&
          (arp_t->State == ARP_STATE_REFRESH)) {
        /* Inuse entry refreshed only once after use */
        arp_t->Type = ARP_TYPE_TEMP_IP;
      }
      arp_t->Tout    = h->CacheTout;
      arp_t->Retries = h->MaxRetry;
      arp_t->State   = ARP_STATE_RESOLVED;
      return;
    }
  }
  ERRORF (ARP,"Process %s, Wrong response\n",h->If->Name);
  EvrNetARP_WrongResponse (h->If->Id);
}

/**
  \brief       Construct and send an ARP message.
  \param[in]   h        ARP instance handle.
  \param[in]   dst_mac  destination MAC address.
  \param[in]   dst_ip   destination IP address.
  \param[in]   src_ip   source IP address.
  \return      none.
  \note        Parameters define type of message:
               - ARP request     (NULL, rem_ip, loc_ip)
               - ARP probe       (NULL, probe_ip, NULL)
               - gratuitous ARP  (NULL, loc_ip, loc_ip)
               - inverse ARP     (dst_mac, NULL, loc_ip)
*/
static void arp_send_request (NET_ARP_CFG *h, const uint8_t *dst_mac,
                              const uint8_t *dst_ip, const uint8_t *src_ip) {
  NET_FRAME *frame;

  DEBUGF (ARP,"Send_request %s\n",h->If->Name);
  EvrNetARP_SendRequest (h->If->Id, dst_mac ? InARP_RQ : ARP_RQ);
  frame = net_mem_alloc (PHY_HEADER_LEN + ARP_HEADER_LEN);

  /* Construct the Ethernet header for:   */
  /*  - ARP request   (broadcasted)       */
  /*  - InARP request (known MAC address) */
  net_mac_copy (ETH_FRAME(frame)->DstAddr, dst_mac ? dst_mac : net_addr_bcast);
  net_mac_copy (ETH_FRAME(frame)->SrcAddr, h->If->MacAddr);
  ETH_FRAME(frame)->Prot = HTONS(ETH_PROT_ARP);

  /* Construct the request frame */
  ARP_FRAME(frame)->HwType   = HTONS(HW_TYPE_ETH);
  ARP_FRAME(frame)->ProtType = HTONS(ETH_PROT_IP4);
  ARP_FRAME(frame)->HwLen    = NET_ADDR_ETH_LEN;
  ARP_FRAME(frame)->ProtLen  = NET_ADDR_IP4_LEN;
  ARP_FRAME(frame)->OpCode   = dst_mac ? HTONS(InARP_RQ) : HTONS(ARP_RQ);

  /* Setup the MAC and IP addresses */
  net_mac_copy (ARP_FRAME(frame)->SendHwAddr, h->If->MacAddr);
  net_mac_copy (ARP_FRAME(frame)->TargHwAddr, dst_mac ? dst_mac : net_addr_unspec);
  net_addr4_copy (ARP_FRAME(frame)->SendIpAddr, src_ip ? src_ip : net_addr_unspec);
  net_addr4_copy (ARP_FRAME(frame)->TargIpAddr, dst_ip ? dst_ip : net_addr_unspec);

  DEBUG_INFO (ARP_FRAME(frame));
  EvrNetARP_ShowFrameHeader (ARP_FRAME(frame));
  h->If->output_lan (h->If->Id & 0xFF, frame);
  net_mem_free (frame);
}

/**
  \brief       Send ARP probe for duplicate IP address detection.
  \param[in]   net_if    network interface descriptor.
  \param[in]   ip4_addr  requested IP address.
  \return      none.
*/
void net_arp_probe (NET_IF_CFG *net_if, uint8_t *ip4_addr) {
  NET_ARP_CFG *h = net_if->Ip4Cfg->ArpCfg;

  ctrl->ProbeIp = ip4_addr;
  arp_send_request (h, NULL, ip4_addr, NULL);
}

/**
  \brief       Send gratuitous ARP on IP address change.
  \param[in]   net_if  network interface descriptor.
  \return      none.
*/
void net_arp_notify (NET_IF_CFG *net_if) {
  NET_ARP_CFG *h = net_if->Ip4Cfg->ArpCfg;

  ctrl->ProbeIp = NULL;
  if (h->Notify != 0) {
    /* When enabled in configuration */
    arp_send_request (h, NULL, LocM.IpAddr, LocM.IpAddr);
  }
}

/**
  \brief       Allocate an ARP cache entry.
  \param[in]   h  ARP instance handle.
  \return      pointer to allocated cache entry.
  \note        Choose the least destructive entry to use:
                1.unused/free entry
                2.oldest stable temp entry
*/
static NET_ARP_INFO *arp_cache_alloc (NET_ARP_CFG *h) {
  NET_ARP_INFO *arp_t;
  uint32_t i,i2,t2;

  /* Scan the cache and find an unused entry */
  for (i = 1, arp_t = &h->Table[0]; i <= h->TabSize; arp_t++, i++) {
    if (arp_t->State == ARP_STATE_FREE) {
      arp_t->Id = i & 0xFF;
      return (arp_t);
    }
  }

  /* Cache is full, find the oldest temp entry */
  /* The oldest entry will expire first        */
  i2 = 0;
  t2 = h->CacheTout;
  for (i = 1, arp_t = &h->Table[0]; i <= h->TabSize; arp_t++, i++) {
    if ((arp_t->State == ARP_STATE_RESOLVED) &&
        (arp_t->Type  == ARP_TYPE_TEMP_IP)) {
      if (arp_t->Tout <= t2) {
        /* This temp entry is older */
        t2 = arp_t->Tout;
        i2 = i;
      }
    }
  }
  /* If temp entry found, re-use it */
  if (i2 != 0) {
    arp_t = &h->Table[i2-1];
    arp_t->Id = i2 & 0xFF;
    return (arp_t);
  }
  return (NULL);
}

/**
  \brief       Add ethernet host to ARP cache, refresh if existing.
  \param[in]   net_if    network interface descriptor.
  \param[in]   ip4_addr  host IP address.
  \param[in]   mac_addr  host MAC address.
  \return      none.
*/
void net_arp_cache_add (NET_IF_CFG *net_if,
                        const uint8_t *ip4_addr, const uint8_t *mac_addr) {
  NET_ARP_CFG  *h = net_if->Ip4Cfg->ArpCfg;
  NET_ARP_INFO *arp_t;
  uint32_t i;

  if (!arp_is_cacheable (h, ip4_addr)) {
    /* Ignore non-cacheable IPs */
    return;
  }
  for (i = 0, arp_t = &h->Table[0]; i < h->TabSize; arp_t++, i++) {
    if (arp_t->State == ARP_STATE_FREE) {
      continue;
    }
    if (net_addr4_comp (arp_t->IpAddr, ip4_addr)) {
      /* IP address is found in cache so update it */
      DEBUGF (ARP,"Cache %s, Entry %d updated\n",h->If->Name,arp_t->Id);
      DEBUG_INF2 (D_IP, ip4_addr);
      EvrNetARP_CacheIpRefreshed (h->If->Id, ip4_addr);
      goto refresh;
    }
  }

  /* Address was not in cache */
  DEBUGF (ARP,"Cache_add %s\n",h->If->Name);
  DEBUG_INF2 (D_IP, ip4_addr);
  DEBUG_INF2 (D_MAC, mac_addr);
  EvrNetARP_CacheAdd (h->If->Id, ip4_addr, mac_addr);

  if (!net_ip4_is_onlink (h->If, ip4_addr)) {
    /* This IP Address belongs to external WAN */
    if (get_u32 (LocM.DefGW) == 0) {
      /* Default Gateway is not specified */
      ERRORF (ARP,"Cache_add %s, Gateway unknown\n",h->If->Name);
      EvrNetARP_GatewayUnknown (h->If->Id, LocM.DefGW);
      return;
    }
    for (i = 0, arp_t = &h->Table[0]; i < h->TabSize; arp_t++, i++) {
      if (arp_t->State == ARP_STATE_FREE) {
        continue;
      }
      if (net_mac_comp (arp_t->MacAddr, mac_addr)) {
        /* Gateway MAC Address is found in cache so refresh it */
        DEBUGF (ARP," Cache, Entry %d updated\n",arp_t->Id);
        EvrNetARP_CacheEntryRefreshed (h->If->Id, arp_t->Id);
        goto refresh_gw;
      }
    }
  }

  arp_t = arp_cache_alloc (h);
  if (arp_t == NULL) {
    /* No Entries left, return */
    ERRORF (ARP,"Cache_add %s, Cache full\n",h->If->Name);
    EvrNetARP_CacheAllocFailed (h->If->Id);
    return;
  }
  DEBUGF (ARP," Entry %d added\n",arp_t->Id);
  EvrNetARP_CacheEntryAdded (h->If->Id, arp_t->Id);
  arp_t->Type    = ARP_TYPE_TEMP_IP;
  arp_que_free (arp_t);
  net_addr4_copy (arp_t->IpAddr, ip4_addr);
refresh:
  net_mac_copy (arp_t->MacAddr, mac_addr);
refresh_gw:
  arp_t->Tout    = h->CacheTout;
  arp_t->Retries = h->MaxRetry;
  arp_t->State   = ARP_STATE_RESOLVED;
}

/**
  \brief       Pre-cache ethernet host and start ARP resolving.
  \param[in]   net_if    network interface descriptor.
  \param[in]   ip4_addr  host IP address.
  \param[in]   mac_addr  host MAC address.
  \return      none.
  \note        Improves networking performance on Ethernet interface.
*/
void net_arp_cache_early (NET_IF_CFG *net_if,
                          const uint8_t *ip4_addr, const uint8_t *mac_addr) {
  NET_ARP_CFG  *h = net_if->Ip4Cfg->ArpCfg;
  NET_ARP_INFO *arp_t;
  uint32_t i;

  for (i = 0, arp_t = &h->Table[0]; i < h->TabSize; arp_t++, i++) {
    if (arp_t->State == ARP_STATE_FREE) {
      continue;
    }
    if (net_addr4_comp (arp_t->IpAddr, ip4_addr)) {
      /* Address found in cache, do nothing */
      return;
    }
  }
  if (!arp_is_cacheable (h, ip4_addr)     ||
      !net_ip4_is_onlink (h->If, ip4_addr)) {
    /* Ignore IPs that can not be cached */
    return;
  }

  DEBUGF (ARP,"Cache_early %s\n",h->If->Name);
  DEBUG_INF2 (D_IP, ip4_addr);
  DEBUG_INF2 (D_MAC, mac_addr);
  EvrNetARP_CacheEarly (h->If->Id, ip4_addr, mac_addr);

  /* IP address not in cache */
  arp_t = arp_cache_alloc (h);
  if (arp_t != NULL) {
    DEBUGF (ARP," Entry %d added\n",arp_t->Id);
    EvrNetARP_CacheEntryAdded (h->If->Id, arp_t->Id);
    arp_que_free (arp_t);
    net_addr4_copy (arp_t->IpAddr, ip4_addr);
    net_mac_copy (arp_t->MacAddr, mac_addr);
    arp_t->Type    = ARP_TYPE_TEMP_IP;
    arp_t->Tout    = h->ResendTout;
    arp_t->Retries = h->MaxRetry;
    /* Start ARP refreshing */
    arp_t->State   = ARP_STATE_REFRESH;
    arp_send_request (h, NULL, arp_t->IpAddr, LocM.IpAddr);
  }
}

/**
  \brief       Find IP address in ARP cache, start ARP resolver if not found.
  \param[in]   net_if    network interface descriptor.
  \param[in]   ip4_addr  requested IP address.
  \return      pointer to cache entry.
*/
NET_ARP_INFO *net_arp_cache_find (NET_IF_CFG *net_if, const uint8_t *ip4_addr) {
  NET_ARP_CFG  *h = net_if->Ip4Cfg->ArpCfg;
  NET_ARP_INFO *arp_t;
  uint8_t type = ARP_TYPE_INUSE_IP;
  uint32_t i;

  DEBUGF (ARP,"Cache_find %s\n",h->If->Name);
  DEBUG_INF2 (D_IP, ip4_addr);
  EvrNetARP_CacheFind (h->If->Id, ip4_addr);
  if (!net_ip4_is_onlink (h->If, ip4_addr)) {
    DEBUGF (ARP," External, Use gateway\n");
    if (get_u32 (LocM.DefGW) == 0) {
      /* Default Gateway is not specified */
      ERRORF (ARP,"Cache_find %s, Gateway unknown\n",h->If->Name);
      EvrNetARP_GatewayUnknown (h->If->Id, LocM.DefGW);
      return (NULL);
    }
    EvrNetARP_UsingGateway (h->If->Id, LocM.DefGW);
    ip4_addr = LocM.DefGW;
    type     = ARP_TYPE_FIXED_IP;
  }
  /* Check if IP already cached */
  for (i = 0, arp_t = &h->Table[0]; i < h->TabSize; arp_t++, i++) {
    if (arp_t->State == ARP_STATE_FREE) {
      continue;
    }
    if (net_addr4_comp (arp_t->IpAddr, ip4_addr)) {
      /* IP address found in cache, now check if it is valid */
      if (arp_t->State < ARP_STATE_RESOLVED) {
        ERRORF (ARP,"Cache_find %s, Unresolved MAC\n",h->If->Name);
        EvrNetARP_UnresolvedMacAddress (h->If->Id, ip4_addr);
        return (arp_t);
      }
      DEBUGF (ARP," Entry %d found\n",arp_t->Id);
      EvrNetARP_EntryFound (h->If->Id, arp_t->Id);
      if (arp_t->Type == ARP_TYPE_TEMP_IP) {
        /* Inuse IP is refreshed once, then removed from cache */
        arp_t->Type = ARP_TYPE_INUSE_IP;
      }
      return (arp_t);
    }
  }
  /* IP not cached */
  arp_t = arp_cache_alloc (h);
  if (arp_t != NULL) {
    DEBUGF (ARP," Entry %d added\n",arp_t->Id);
    EvrNetARP_CacheEntryAdded (h->If->Id, arp_t->Id);
    arp_t->Type  = type;
    arp_que_free (arp_t);
    net_addr4_copy (arp_t->IpAddr, ip4_addr);
    net_mac_copy (arp_t->MacAddr, net_addr_bcast);
    /* Start ARP resolution */
    arp_t->State = ARP_STATE_RESERVED;
  }
  return (arp_t);
}

/**
  \brief       Resolve MAC address from ARP cache, or send ARP request.
  \param[in]   if_id     interface identifier.
  \param[in]   ip4_addr  requested IP address.
  \param[in]   type      address cache type.
  \return      status code as defined with netStatus.
*/
netStatus netARP_CacheIP (uint32_t if_id,
                          const uint8_t *ip4_addr, netARP_CacheType type) {
  NET_IF_CFG   *net_if = net_if_map_lan (if_id);
  NET_ARP_CFG  *h;
  NET_ARP_INFO *arp_t;
  uint8_t ctype = (type == netARP_CacheFixedIP) ? ARP_TYPE_FIXED_IP : ARP_TYPE_TEMP_IP;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip4Cfg) || (ip4_addr == NULL)) {
    ERRORF (ARP,"CacheIP, Invalid parameter\n");
    EvrNetARP_CacheIpInvalidParameter (if_id & 0xFFFF);
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip4Cfg->ArpCfg;

  DEBUGF (ARP,"CacheIP %s\n",h->If->Name);
  DEBUG_INF2 (D_IP, ip4_addr);
  EvrNetARP_CacheIp (h->If->Id, ip4_addr);

  if (!arp_is_cacheable (h, ip4_addr)) {
    /* Invalid IP address, non-cacheable */
    ERRORF (ARP,"CacheIP %s, Invalid IP\n",h->If->Name);
    EvrNetARP_CacheIpInvalidParameter(h->If->Id);
    RETURN (netInvalidParameter);
  }
  arp_t = net_arp_cache_find (h->If, ip4_addr);
  if (arp_t == NULL) {
    /* Requested IP is external but gateway unspecified */
    RETURN (netError);
  }
  if (net_addr4_comp (arp_t->IpAddr, ip4_addr)) {
    /* If this IP is onlink, change also the type */
    arp_t->Type = ctype;
  }
  if (arp_t->State < ARP_STATE_RESOLVED) {
    /* MAC resolving in progress */
    RETURN (netBusy);
  }
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Resolve IP address from ARP cache, or send InARP request.
  \param[in]   if_id     interface identifier.
  \param[in]   mac_addr  requested MAC address.
  \return      status code as defined with netStatus.
*/
netStatus netARP_CacheMAC (uint32_t if_id, const uint8_t *mac_addr) {
  NET_IF_CFG   *net_if = net_if_map_lan (if_id);
  NET_ARP_CFG  *h;
  NET_ARP_INFO *arp_t;
  uint32_t i;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip4Cfg) || (mac_addr == NULL)) {
    ERRORF (ARP,"CacheMAC, Invalid parameter\n");
    EvrNetARP_CacheMacInvalidParameter (if_id & 0xFFFF);
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip4Cfg->ArpCfg;

  DEBUGF (ARP,"CacheMAC %s\n",h->If->Name);
  DEBUG_INF2 (D_MAC, mac_addr);
  EvrNetARP_CacheMac (h->If->Id, mac_addr);

  if (net_mac_comp (mac_addr, net_addr_unspec) ||
      net_mac_comp (mac_addr, net_addr_bcast)  ||
      (get_u32 (mac_addr) >> 8) == 0x01005E) {
    /* Invalid MAC address */
    ERRORF (ARP,"CacheMAC %s, Invalid MAC\n",h->If->Name);
    EvrNetARP_CacheMacInvalidParameter (h->If->Id);
    RETURN (netInvalidParameter);
  }
  /* Check if MAC already cached */
  for (i = 0, arp_t = &h->Table[0]; i < h->TabSize; arp_t++, i++) {
    if (arp_t->State == ARP_STATE_FREE) {
      continue;
    }
    if (net_mac_comp (arp_t->MacAddr, mac_addr)) {
      /* MAC address found in cache, now check if it is valid */
      if (arp_t->State < ARP_STATE_RESOLVED) {
        ERRORF (ARP,"CacheMAC %s, Unresolved IP\n",h->If->Name);
        EvrNetARP_UnresolvedIpAddress (h->If->Id, mac_addr);
        RETURN (netBusy);
      }
      DEBUGF (ARP," Entry %d found\n",arp_t->Id);
      EvrNetARP_EntryFound (h->If->Id, arp_t->Id);
      RETURN (netOK);
    }
  }
  if (get_u32 (LocM.IpAddr) == 0) {
    ERRORF (ARP,"CacheMAC %s, Local-IP undefined\n",h->If->Name);
    EvrNetARP_UnresolvedIpAddress (h->If->Id, mac_addr);
    RETURN (netError);
  }
  /* MAC not cached */
  arp_t = arp_cache_alloc (h);
  if (arp_t != NULL) {
    DEBUGF (ARP," Entry %d added\n",arp_t->Id);
    EvrNetARP_CacheEntryAdded (h->If->Id, arp_t->Id);
    arp_t->Type  = ARP_TYPE_TEMP_IP;
    arp_que_free (arp_t);
    set_u32 (arp_t->IpAddr, 0);
    net_mac_copy (arp_t->MacAddr, mac_addr);
    /* Start Inverse ARP resolution */
    arp_t->State = ARP_STATE_RESERVED;
  }
  RETURN (netBusy);

  END_LOCK;
}

/**
  \brief       Get IP address from the ARP cache.
  \param[in]   if_id     interface identifier.
  \param[in]   mac_addr  requested MAC address.
  \param[out]  ip4_addr  resolved IP address.
  \return      status code as defined with netStatus.
*/
netStatus netARP_GetIP (uint32_t if_id,
                        const uint8_t *mac_addr, uint8_t *ip4_addr) {
  NET_IF_CFG   *net_if = net_if_map_lan (if_id);
  NET_ARP_CFG  *h;
  const uint8_t *sp;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip4Cfg) || (mac_addr == NULL) || (ip4_addr == NULL)) {
    ERRORF (ARP,"GetIP, Invalid parameter\n");
    EvrNetARP_GetIpInvalidParameter (if_id & 0xFFFF);
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip4Cfg->ArpCfg;

  DEBUGF (ARP,"GetIP %s\n",h->If->Name);
  DEBUG_INF2 (D_MAC, mac_addr);
  sp = arp_find_ip (h, mac_addr);
  if (sp == NULL) {
    DEBUGF (ARP," Not found\n");
    EvrNetARP_GetIpEntryNotFound (h->If->Id, mac_addr);
    RETURN (netError);
  }

  /* MAC found in cache, return IP */
  DEBUG_INF2 (D_IP, sp);
  EvrNetARP_GetIp (h->If->Id, mac_addr, sp);
  net_addr4_copy (ip4_addr, sp);
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Get MAC address from the ARP cache.
  \param[in]   if_id     interface identifier.
  \param[in]   ip4_addr  requested IPv4 address.
  \param[out]  mac_addr  resolved MAC address.
  \return      status code as defined with netStatus.
*/
netStatus netARP_GetMAC (uint32_t if_id,
                         const uint8_t *ip4_addr, uint8_t *mac_addr) {
  NET_IF_CFG   *net_if = net_if_map_lan (if_id);
  NET_ARP_CFG  *h;
  const uint8_t *sp;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip4Cfg) || (ip4_addr == NULL) || (mac_addr == NULL)) {
    ERRORF (ARP,"GetMAC, Invalid parameter\n");
    EvrNetARP_GetMacInvalidParameter (if_id & 0xFFFF);
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip4Cfg->ArpCfg;

  DEBUGF (ARP,"GetMAC %s\n",h->If->Name);
  DEBUG_INF2 (D_IP, ip4_addr);
  sp = arp_find_mac (h->If, ip4_addr);
  if (sp == NULL) {
    DEBUGF (ARP," Not found\n");
    EvrNetARP_GetMacEntryNotFound (h->If->Id, ip4_addr);
    RETURN (netError);
  }

  /* IP found in cache, return MAC */
  DEBUG_INF2 (D_MAC, sp);
  EvrNetARP_GetMac (h->If->Id, ip4_addr, sp);
  net_mac_copy (mac_addr, sp);
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Probe IP address if already in use.
  \param[in]   if_id     interface identifier.
  \param[in]   ip4_addr  requested IPv4 address.
  \param[out]  cb_func   callback notification function.
  \return      status code as defined with netStatus.
*/
netStatus netARP_Probe (uint32_t if_id,
                        const uint8_t *ip4_addr, netARP_cb_t cb_func) {
  NET_IF_CFG  *net_if = net_if_map_lan (if_id);
  NET_ARP_CFG *h;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip4Cfg) || (ip4_addr == NULL) || (cb_func == NULL)) {
    ERRORF (ARP,"Probe, Invalid parameter\n");
    EvrNetARP_ProbeInvalidParameter (if_id & 0xFFFF);
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip4Cfg->ArpCfg;
  RETURN (arp_probe_start (h, ip4_addr, cb_func));

  END_LOCK;
}

/**
  \brief       Probe IP address in blocking mode.
  \param[in]   if_id     interface identifier.
  \param[in]   ip4_addr  requested IPv4 address.
  \return      status code as defined with netStatus.
*/
netStatus netARP_ProbeX (uint32_t if_id, const uint8_t *ip4_addr) {
  NET_IF_CFG *net_if = net_if_map_lan (if_id);
  NET_ARP_CFG *h;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip4Cfg) || (ip4_addr == NULL)) {
    ERRORF (ARP,"Probe, Invalid parameter\n");
    EvrNetARP_ProbeInvalidParameter (if_id & 0xFFFF);
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip4Cfg->ArpCfg;

  for (;;) {
    /* Check if blocking probe is busy */
    if (!ctrl->xThread) {
      netStatus res = arp_probe_start (h, ip4_addr, NULL);
      if (res == netOK) {
        break;
      }
      if (res != netBusy) {
        /* res: - netInvalidParameter */
        RETURN (res);
      }
    }
    /* Wait a while and try again */
    net_sys_unlock ();
    netos_delay (100);
    net_sys_lock ();
  }
  /* Suspend this thread, wait for net_sys_resume() */
  net_sys_suspend (&ctrl->xThread);
  RETURN (ctrl->xRetv);

  END_LOCK;
}

/**
  \brief       Add static entry to the ARP cache.
  \param[in]   if_id     interface identifier.
  \param[in]   ip4_addr  host IP address.
  \param[in]   mac_addr  host MAC address.
  \return      status code as defined with netStatus.
*/
netStatus netARP_AddCache (uint32_t if_id,
                           const uint8_t *ip4_addr, const uint8_t *mac_addr) {
  NET_IF_CFG *net_if = net_if_map_lan (if_id);
  NET_ARP_CFG *h;
  NET_ARP_INFO *arp_t;
  uint32_t i;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip4Cfg) || (ip4_addr == NULL) || (mac_addr == NULL)) {
    ERRORF (ARP,"AddCache, Invalid parameter\n");
    EvrNetARP_AddCacheInvalidParam (if_id & 0xFFFF);
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip4Cfg->ArpCfg;

  DEBUGF (ARP,"AddCache %s\n",h->If->Name);
  DEBUG_INF2 (D_IP, ip4_addr);
  DEBUG_INF2 (D_MAC, mac_addr);
  EvrNetARP_AddCache (h->If->Id, ip4_addr, mac_addr);

  if (!arp_is_cacheable (h, ip4_addr)) {
    /* Invalid IP address, non-cacheable */
    ERRORF (ARP,"AddCache %s, Invalid IP\n",h->If->Name);
    EvrNetARP_AddCacheInvalidIpAddress (h->If->Id);
    RETURN (netInvalidParameter);
  }

  if (net_mac_comp (mac_addr, net_addr_unspec) ||
      net_mac_comp (mac_addr, net_addr_bcast)  ||
      (get_u32 (mac_addr) >> 8) == 0x01005E) {
    /* Invalid MAC address */
    ERRORF (ARP,"AddCache %s, Invalid MAC\n",h->If->Name);
    EvrNetARP_AddCacheInvalidMacAddress (h->If->Id);
    RETURN (netInvalidParameter);
  }

  /* Check if this IP address already cached */
  for (i = 0, arp_t = &h->Table[0]; i < h->TabSize; arp_t++, i++) {
    if (arp_t->State == ARP_STATE_FREE) {
      continue;
    }
    if (net_addr4_comp (arp_t->IpAddr, ip4_addr)) {
      goto set;
    }
  }

  /* IP not cached */
  arp_t = arp_cache_alloc (h);
  if (arp_t != NULL) {
    DEBUGF (ARP," Entry %d added\n",arp_t->Id);
    EvrNetARP_CacheEntryAdded (h->If->Id, arp_t->Id);
    net_addr4_copy (arp_t->IpAddr, ip4_addr);
set:arp_que_free (arp_t);
    net_mac_copy (arp_t->MacAddr, mac_addr);
    arp_t->State = ARP_STATE_RESOLVED;
    arp_t->Type  = ARP_TYPE_STATIC_IP;
    arp_t->Tout  = h->CacheTout;
    RETURN (netOK);
  }

  RETURN (netError);

  END_LOCK;
}

/**
  \brief       Clear the ARP cache.
  \param[in]   if_id  interface identifier.
  \return      status code as defined with netStatus.
*/
netStatus netARP_ClearCache (uint32_t if_id) {
  NET_IF_CFG *net_if = net_if_map_lan (if_id);
  NET_ARP_CFG *h;
  NET_ARP_INFO *arp_t;
  uint32_t i,cnt;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip4Cfg)) {
    ERRORF (ARP,"ClearCache, Invalid parameter\n");
    EvrNetARP_ClearCacheInvalidParam (if_id & 0xFFFF);
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip4Cfg->ArpCfg;

  if (ctrl->Flags & ARP_FLAG_PROBE_BUSY) {
    ERRORF (ARP,"ClearCache %s, Client busy\n",h->If->Name);
    EvrNetARP_ClearCacheClientBusy (h->If->Id);
    RETURN (netBusy);
  }
  cnt = 0;
  for (i = 0, arp_t = &h->Table[0]; i < h->TabSize; arp_t++, i++) {
    if (arp_t->State != ARP_STATE_FREE) {
      arp_t->State = ARP_STATE_FREE;
      arp_que_free (arp_t);
      cnt++;
    }
  }
  ctrl->ProbeIp = NULL;
  ctrl->entry   = 0;

  DEBUGF (ARP,"ClearCache %s, %d deleted of %d\n",h->If->Name,cnt,h->TabSize);
  EvrNetARP_ClearCache (h->If->Id, cnt, h->TabSize);
  (void)cnt;
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Start probing the IP address.
  \param[in]   h         ARP instance handle.
  \param[in]   ip4_addr  requested IPv4 address.
  \param[out]  cb_func   callback notification function.
  \return      status code as defined with netStatus:
               - netOK
               - netInvalidParameter
               - netBusy
*/
static netStatus arp_probe_start (NET_ARP_CFG *h,
                                  const uint8_t *ip4_addr, netARP_cb_t cb_func) {
  uint32_t ip_tmp;

  DEBUGF (ARP,"Probe %s, Addr [%s]\n",h->If->Name,net_addr4_ntoa(ip4_addr));
  EvrNetARP_Probe (h->If->Id, ip4_addr);

  /* Check if IP address is valid for probe */
  ip_tmp = get_u32 (ip4_addr);
  if ((ip_tmp == 0x00000000) || (ip_tmp == 0xFFFFFFFF)   ||
      (ip4_addr[0] == 127)   || ((ip4_addr[0] & 0xF0) == 0xE0)) {
    /* Undefined IP: 0.0.0.0                     */
    /* Broadcast IP: 255.255.255.255             */
    /* Localhost IP: 127.0.0.0 - 127.255.255.255 */
    /* Multicast IP: 224.0.0.0 - 239.255.255.255 */
    ERRORF (ARP,"Probe %s, Invalid IP\n",h->If->Name);
    EvrNetARP_ProbeInvalidParameter (h->If->Id);
    return (netInvalidParameter);
  }
  if (ctrl->Flags & ARP_FLAG_PROBE_BUSY) {
    ERRORF (ARP,"Probe %s, Process busy\n",h->If->Name);
    EvrNetARP_ProbeBusy (h->If->Id);
    return (netBusy);
  }

  net_addr4_copy (ctrl->TargAddr, ip4_addr);
  ctrl->cb_func = cb_func;
  ctrl->Flags  |= ARP_FLAG_PROBE_BUSY;
  ctrl->Timer   = 1*SYS_TICK_TSEC;
  ctrl->Retries = 2;
  arp_send_request (h, NULL, ctrl->TargAddr, NULL);
  return (netOK);
}

/**
  \brief       Find MAC address in ARP cache.
  \param[in]   net_if    network interface descriptor.
  \param[in]   ip4_addr  requested IP address.
  \return      pointer to resolved MAC address.
*/
static const uint8_t *arp_find_mac (NET_IF_CFG *net_if, const uint8_t *ip4_addr) {
  NET_ARP_CFG  *h = net_if->Ip4Cfg->ArpCfg;
  NET_ARP_INFO *arp_t;
  uint32_t i;

  for (i = 0, arp_t = &h->Table[0]; i < h->TabSize; arp_t++, i++) {
    if (arp_t->State < ARP_STATE_RESOLVED) {
      continue;
    }
    if (net_addr4_comp (arp_t->IpAddr, ip4_addr)) {
      /* Requested IP found, return MAC address */
      return (arp_t->MacAddr);
    }
  }
  return (NULL);
}

/**
  \brief       Find IP address in ARP cache.
  \param[in]   h         ARP instance handle.
  \param[in]   mac_addr  requested MAC address.
  \return      pointer to resolved IP address.
*/
static const uint8_t *arp_find_ip (NET_ARP_CFG *h, const uint8_t *mac_addr) {
  NET_ARP_INFO *arp_t;
  uint32_t i;

  for (i = 0, arp_t = &h->Table[0]; i < h->TabSize; arp_t++, i++) {
    if (arp_t->State < ARP_STATE_RESOLVED) {
      continue;
    }
    if (net_mac_comp (arp_t->MacAddr, mac_addr)) {
      /* IP address found in cache */
      return (arp_t->IpAddr);
    }
  }
  return (NULL);
}

/**
  \brief       Run ARP Probe service.
  \param[in]   h  ARP instance handle.
  \return      none.
  \note        Executed on sys-tick (every 100ms).
*/
static void arp_probe_run (NET_ARP_CFG *h) {

  if (!(ctrl->Flags & ARP_FLAG_PROBE_BUSY)) {
    return;
  }
  /* Update interval is 1 tick */
  if (ctrl->Timer) {
    ctrl->Timer--;
    return;
  }
  if (ctrl->Retries) {
    ctrl->Retries--;
    ctrl->Timer = 1*SYS_TICK_TSEC-1;
    DEBUGF (ARP,"Probe %s, retransmit\n",h->If->Name);
    EvrNetARP_ProbeRetransmit (h->If->Id, ctrl->TargAddr);
    arp_send_request (h, NULL, ctrl->TargAddr, NULL);
    return;
  }
  DEBUGF (ARP,"Probe %s, timeout\n",h->If->Name);
  EvrNetARP_ProbeTimeout (h->If->Id, ctrl->TargAddr);
  ctrl->Flags &= ~ARP_FLAG_PROBE_BUSY;
  if (ctrl->cb_func) {
    ctrl->cb_func (netARP_EventTimeout);
    return;
  }
  ctrl->xRetv = netTimeout;
  net_sys_resume (&ctrl->xThread);
}

/**
  \brief       Enqueue the frame to transmit pending queue.
  \param[in]   net_if  network interface descriptor.
  \param[in]   arp_t   pointer to a cache entry control block.
  \param[in]   frame   network frame.
  \return      status:
               - true  = success,
               - false = failed.
  \description Transmit queue contains a list of frames, waiting to resolve
               the MAC address. After the MAC address is resolved, all saved
               packets are sent to the resolved destination MAC address.
*/
bool net_arp_enqueue (NET_IF_CFG *net_if, NET_ARP_INFO *arp_t, NET_FRAME *frame) {
  NET_ARP_CFG *h = net_if->Ip4Cfg->ArpCfg;
  NET_FRAME *txframe,*next;

  if (!(net_mem_avail_tx())) {
    /* System out of memory, ignore this packet */
    return (false);
  }
  txframe = net_mem_alloc (frame->length);
  memcpy (txframe->data, frame->data, frame->length);
  /* This overlaps the ethernet header */
  ARP_QUE(txframe)->next = NULL;

  /* Add txframe to queue tail */
  if (arp_t->tx_list == NULL) {
    /* First item in queue */
    arp_t->tx_list = txframe;
  }
  else {
    /* Append txframe to the end */
    next = arp_t->tx_list;
    for ( ; ARP_QUE(next)->next; next = ARP_QUE(next)->next);
    ARP_QUE(next)->next = txframe;
  }
  /* Send ARP request */
  if (arp_t->State == ARP_STATE_RESERVED) {
    DEBUGF (ARP,"Cache %s, Entry %d resolving\n",h->If->Name,arp_t->Id);
    EvrNetARP_ResolveEntry (h->If->Id, arp_t->Id);
    arp_t->State   = ARP_STATE_PENDING;
    arp_t->Retries = h->MaxRetry;
    arp_t->Tout    = h->ResendTout;
    arp_send_request (h, NULL, arp_t->IpAddr, LocM.IpAddr);
  }
  return (true);
}

/**
  \brief       Run main process of ARP cache.
  \return      none.
*/
void net_arp_cache_run (void) {
  NET_ARP_CFG *const *p;

  for (p = &net_arp_list[0]; *p; p++) {
    arp_cache_run (*p);
  }
}

/**
  \brief       Run main process of ARP cache.
  \param[in]   h  ARP instance handle.
  \return      none.
  \note        Expired temporary cache entries are removed,
               expired fixed IP entries are refreshed.
*/
static void arp_cache_run (NET_ARP_CFG *h) {
  NET_ARP_INFO *arp_t;

  if (ctrl->entry == 0) {
    /* Take a snapshot of time tick flags. This prevents missed time ticks,*/
    /* if processing is split to multiple calls to arp cache run function. */
    ctrl->sys_flags = sys->Flags;
    if (!(ctrl->sys_flags & SYS_FLAG_TICK)) {
      return;
    }
    arp_probe_run (h);
  }
  sys->Busy = true;

  /* Every 100 ms all entries are processed. If there are actions taken     */
  /* (a packet is sent), processing is interrupted and the function returns.*/
  /* Remaining entries are processed in subsequent call to this function.   */
  for (++ctrl->entry; ctrl->entry <= h->TabSize; ctrl->entry++) {
    arp_t = &h->Table[ctrl->entry-1];
    switch (arp_t->State) {
      case ARP_STATE_FREE:
        break;

      case ARP_STATE_RESERVED:
        DEBUGF (ARP,"Cache %s, Entry %d resolving\n",h->If->Name,ctrl->entry);
        EvrNetARP_ResolveEntry (h->If->Id, ctrl->entry);
        arp_t->State   = ARP_STATE_PENDING;
        arp_t->Retries = h->MaxRetry;
        goto request;

      case ARP_STATE_PENDING:
      case ARP_STATE_REFRESH:
        /* Counting rate is 1 tick */
        if (arp_t->Tout != 0) {
          if (--arp_t->Tout != 0) {
            /* A timeout not yet expired */
            break;
          }
        }
        if (arp_t->Retries != 0) {
          /* Resend ARP request */
          uint32_t ip;
          DEBUGF (ARP,"Cache %s, Entry %d resend\n",h->If->Name,ctrl->entry);
          EvrNetARP_ResolveEntry (h->If->Id, ctrl->entry);
          arp_t->Retries--;
request:  arp_t->Tout = h->ResendTout;
          ip = get_u32 (arp_t->IpAddr);
          /* Type of request depends on IP value: */
          /*   ip != 0, send regular ARP request */
          /*   ip == 0, send inverse ARP request */
          arp_send_request (h, ip ? NULL : arp_t->MacAddr,
                               ip ? arp_t->IpAddr : NULL, LocM.IpAddr);
          return;
        }
        DEBUGF (ARP,"Cache %s, Entry %d released\n",h->If->Name,ctrl->entry);
        EvrNetARP_EntryReleased (h->If->Id, ctrl->entry);
        arp_t->State = ARP_STATE_FREE;
        arp_que_free (arp_t);
        break;

      case ARP_STATE_RESOLVED:
        if (!(ctrl->sys_flags & SYS_FLAG_SEC)) {
          break;
        }
        /* Counting rate is 1 second */
        if (arp_t->Tout != 0) {
          arp_t->Tout--;
          break;
        }
        /* This Entry has timed out */
        if (arp_t->Type == ARP_TYPE_STATIC_IP) {
          if (!net_ip4_is_onlink (h->If, arp_t->IpAddr)) {
            /* Not onlink anymore, delete it */
            arp_t->State = ARP_STATE_FREE;
            break;
          }
          arp_t->Tout = h->CacheTout;
          break;
        }
        if ((arp_t->Type != ARP_TYPE_FIXED_IP)  &&
            (net_addr4_comp (arp_t->IpAddr, LocM.DefGW))) {
          /* Classify network gateway as a fixed IP */
          arp_t->Type = ARP_TYPE_FIXED_IP;
        }
        DEBUGF (ARP,"Cache %s, Entry %d timeout\n",h->If->Name,ctrl->entry);
        DEBUGF (ARP," Type %s\n",type_ascii(arp_t->Type));
        DEBUG_INF2 (D_IP, arp_t->IpAddr);
        DEBUG_INF2 (D_MAC, arp_t->MacAddr);
        EvrNetARP_CacheEntryTimeout (h->If->Id, ctrl->entry,
                                     arp_t->IpAddr, arp_t->MacAddr, arp_t->Type);

        /* Fixed-IP is always refreshed */
        /* InUse-IP is refreshed once   */
        if ((arp_t->Type == ARP_TYPE_FIXED_IP) ||
            (arp_t->Type == ARP_TYPE_INUSE_IP)) {
          DEBUGF (ARP," Refresh entry\n");
          EvrNetARP_RefreshEntry (h->If->Id, ctrl->entry);
          arp_t->State   = ARP_STATE_REFRESH;
          arp_t->Retries = h->MaxRetry;
          arp_t->Tout    = h->ResendTout;
          arp_send_request (h, NULL, arp_t->IpAddr, LocM.IpAddr);
          return;
        }
        /* Temp-IP is released from cache */
        DEBUGF (ARP," Entry released\n");
        EvrNetARP_EntryReleased (h->If->Id, ctrl->entry);
        arp_t->State = ARP_STATE_FREE;
        arp_que_free (arp_t);
        break;
    }
  }
  /* Prepare for the next run */
  ctrl->entry = 0;
}

/**
  \brief       Check if ARP sender is valid.
  \param[in]   h      ARP instance handle.
  \param[in]   arp_r  received ARP request.
  \return      check status:
               - true  = is valid,
               - false = not valid.
*/
static bool arp_sender_valid (NET_ARP_CFG *h, NET_ARP_HEADER *arp_r) {
  uint32_t ip_tmp;

  /* Check MAC address of the sender */
  if (net_mac_comp (arp_r->SendHwAddr, net_addr_bcast)  ||
      net_mac_comp (arp_r->SendHwAddr, net_addr_unspec) ||
      (get_u32 (arp_r->SendHwAddr) >> 8) == 0x01005E) {
    /* Broadcast:   FF-FF-FF-FF-FF-FF */
    /* Multicast:   01-00-5E-xx-xx-xx */
    /* Unspecified: 00-00-00-00-00-00 */
    return (false);
  }

  /* Check IP address of the sender */
  if ( (arp_r->SendIpAddr[0] == 127) ||
      ((arp_r->SendIpAddr[0] & 0xF0) == 0xE0)) {
    /* Localhost: 127.0.0.0 - 127.255.255.255 */
    /* Multicast: 224.0.0.0 - 239.255.255.255 */
    return (false);
  }
  ip_tmp = get_u32 (arp_r->SendIpAddr);
  if ( (ip_tmp == 0xFFFFFFFF) ||
      ((ip_tmp == 0) && (arp_r->OpCode != HTONS(ARP_RQ)))) {
    /* Bcast (255.255.255.255)               */
    /* Undefined (0.0.0.0) and not ARP probe */
    return (false);
  }
  if (net_addr4_comp (arp_r->SendIpAddr, LocM.IpAddr)) {
    /* Sender is using our IP address */
    return (false);
  }
  return (true);
}

/**
  \brief       Check if IP address is cacheable.
  \param[in]   h         ARP instance handle.
  \param[in]   ip4_addr  requested IP address.
  \return      check status:
               - true  = address is cacheable,
               - false = not cacheable.
*/
static bool arp_is_cacheable (NET_ARP_CFG *h, const uint8_t *ip4_addr) {
  uint32_t ip_tmp,ip_lm,net_mask;

  if ((ip4_addr[0] == 127) || ((ip4_addr[0] & 0xF0) == 0xE0)) {
    /* Localhost: 127.0.0.0 - 127.255.255.255 */
    /* Multicast: 224.0.0.0 - 239.255.255.255 */
    return (false);
  }

  /* 'ip_addr' may not be aligned */
  ip_tmp   = get_u32 (ip4_addr);

  if ((ip_tmp == 0) || (ip_tmp == 0xFFFFFFFF)) {
    /* Undefined (0.0.0.0) or Bcast (255.255.255.255) */
    return (false);
  }

  /* Check if IP address on-link */
  ip_lm    = ntohl(__ALIGNED_UINT32(LocM.IpAddr));
  net_mask = ntohl(__ALIGNED_UINT32(LocM.NetMask));

  if ((ip_tmp ^ ip_lm) & net_mask) {
    /* Network address is different */
    return (false);
  }
  if ((ip_tmp | net_mask) == 0xFFFFFFFF) {
    /* This is subnet broadcast */
    return (false);
  }
  return (true);
}

/**
  \brief       Send all enqueued frames.
  \param[in]   h      ARP instance handle.
  \param[in]   arp_t  pointer to a cache entry control block.
  \return      none.
*/
static void arp_que_send (NET_ARP_CFG *h, NET_ARP_INFO *arp_t) {
  NET_FRAME *frame, *next;

  frame = arp_t->tx_list;
  arp_t->tx_list = NULL;
  for ( ; frame; frame = next) {
    next = ARP_QUE(frame)->next;
    /* Set source and destination MAC address */
    net_mac_copy (ETH_FRAME(frame)->SrcAddr, h->If->MacAddr);
    net_mac_copy (ETH_FRAME(frame)->DstAddr, arp_t->MacAddr);
    h->If->output_lan (h->If->Id & 0xFF, frame);
    net_mem_free (frame);
  }
}

/**
  \brief       Release all enqueued frames from memory.
  \param[in]   arp_t  pointer to a cache entry control block.
  \return      none.
*/
static void arp_que_free (NET_ARP_INFO *arp_t) {
  NET_FRAME *frame, *next;

  frame = arp_t->tx_list;
  arp_t->tx_list = NULL;
  for ( ; frame; frame = next) {
    next = ARP_QUE(frame)->next;
    net_mem_free (frame);
  }
}

#ifdef DEBUG_STDIO
/**
  \brief       Convert cache entry type to ascii.
  \param[in]   type  cache entry type.
  \return      pointer to text representation of cache entry type.
*/
static const char *type_ascii (uint8_t type) {
  switch (type) {
    case ARP_TYPE_FIXED_IP:
      return ("Static");
    case ARP_TYPE_TEMP_IP:
      return ("Dynamic");
    case ARP_TYPE_INUSE_IP:
      return ("Inuse-Dynamic");
  }
  return ("Unknown");
}

/**
  \brief       Debug print ARP header information.
  \param[in]   arp_hdr  ARP frame header.
  \return      none.
*/
static void debug_info (const NET_ARP_HEADER *arp_hdr) {
  static const char *const p_asc[] = {
    "ARP-REQUEST",
    "ARP-REPLY",
    "RARP-REQUEST",
    "RARP-REPLY",
    "InARP-REQUEST",
    "InARP-RESPONSE"
  };
  static const uint16_t p_bin[] = {
    ARP_RQ,
    ARP_REP,
    RARP_RQ,
    RARP_REP,
    InARP_RQ,
    InARP_RSP
  };
  uint16_t opcode;
  uint32_t i;

  /* Print user friendly ARP opcodes. */
  opcode = ntohs(arp_hdr->OpCode);
  for (i = 0; i < sizeof (p_bin)/sizeof(uint16_t); i++) {
    if (p_bin[i] == opcode) {
      DEBUGF (ARP," Opcode %s\n",p_asc[i]);
      goto d;
    }
  }
  DEBUGF (ARP," Opcode 0x%04X\n",ntohs(arp_hdr->OpCode));
d:DEBUGF (ARP," SendIp  %s\n",net_addr4_ntoa(arp_hdr->SendIpAddr));
  DEBUGF (ARP," TargIp  %s\n",net_addr4_ntoa(arp_hdr->TargIpAddr));
  DEBUGF (ARP," SendMac %s\n",net_mac_ntoa(arp_hdr->SendHwAddr));
  DEBUGF (ARP," TargMac %s\n",net_mac_ntoa(arp_hdr->TargHwAddr));
}

/**
  \brief       Debug print MAC or IP address.
  \param[in]   type  type of address.
  \param[in]   addr  address to print.
  \return      none.
*/
static void debug_inf2 (uint32_t type, const uint8_t *addr) {

  switch (type) {
    case D_MAC:
      DEBUGF (ARP," MAC %s\n",net_mac_ntoa(addr));
      break;
    case D_IP:
      DEBUGF (ARP," IP4 %s\n",net_addr4_ntoa(addr));
      break;
  }
}
#endif
