/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ndp.c
 * Purpose: Neighbor Discovery for IPv6
 *----------------------------------------------------------------------------*/

#include "rl_net_lib.h"

#ifdef Network_IPv6
#include "net_sys.h"
#include "net_eth.h"
#include "net_ip6.h"
#include "net_icmp.h"
#include "net_icmp6.h"
#include "net_addr.h"
#include "net_common.h"
#include "net_mem.h"
#include "net_ndp.h"
#include "net_dbg.h"

/* Global variables */
NET_NDP_CTRL net_eth0_ndp_control;
NET_NDP_CTRL net_eth1_ndp_control;
NET_NDP_CTRL net_wifi0_ndp_control;
NET_NDP_CTRL net_wifi1_ndp_control;

/* Code shortening macros */
#define ctrl        ( h->Ctrl)
#define LocM6       (*h->If->localm6)
#define slaac       (&h->Ctrl->Slaac)

/* Local Functions */
static NET_NDP_INFO *ndp_cache_alloc (NET_NDP_CFG *h);
static const uint8_t *ndp_next_opt (const uint8_t *buf, const uint8_t *end);
static bool ndp_check_frame (NET_FRAME *frame, uint32_t opt_offs);
static bool ndp_is_onlink (NET_NDP_CFG *h, const uint8_t *ip6_addr);
static NET_NDP_INFO *ndp_cache_add (NET_NDP_CFG *h,
                                    const uint8_t *ip_addr, const uint8_t *mac_addr,
                                    uint16_t tout, uint8_t type);
static void ndp_cache_delete (NET_NDP_CFG *h, const uint8_t *ip_addr, uint8_t type);
static void ndp_send_reply (NET_NDP_CFG *h,
                            const uint8_t *ip_addr, const uint8_t *targ_addr);
static void ndp_send_request (NET_NDP_CFG *h, uint8_t type, const uint8_t *ip_addr);
static void ndp_proc_response (NET_NDP_CFG *h,
                               NET_ICMP_HEADER *icmp_r, uint32_t len);
static netStatus ndp_probe_start (NET_NDP_CFG *h,
                                  const uint8_t *ip6_addr, netNDP_cb_t cb_func);
static const uint8_t *ndp_find_ip (NET_NDP_CFG *h, const uint8_t *mac_addr);
static void ndp_router_add (NET_NDP_CFG *h, NET_NDP_INFO *ndp_t);
static void ndp_router_remove (NET_NDP_CFG *h, NET_NDP_INFO *ndp_t);
static void ndp_prefix_add (NET_NDP_CFG *h,
                            const uint8_t *prefix, uint32_t lt_valid);
static void ndp_slaac_run (NET_NDP_CFG *h);
static void ndp_probe_run (NET_NDP_CFG *h);
static void ndp_cache_run (NET_NDP_CFG *h);
static void ndp_que_send (NET_NDP_CFG *h, NET_NDP_INFO *ndp_t);
static void ndp_que_free (NET_NDP_INFO *ndp_t);
#ifdef Network_Debug_STDIO
 #define D_MAC  0
 #define D_IP   1
 static void debug_inf2 (uint32_t type, const uint8_t *addr);
#endif

/**
  \brief       Initialize NDP cache.
*/
void net_ndp_cache_init (void) {
  NET_NDP_INFO *ndp_t;
  NET_NDP_CFG *const *p;
  uint32_t i;

  for (p = &net_ndp_list[0]; *p; p++) {
    DEBUGF (NDP,"Init_cache %s, %d entries\n",(*p)->If->Name,(*p)->TabSize);
    EvrNetNDP_InitCache ((*p)->If->Id, (*p)->TabSize);
    for (i = 0, ndp_t = &(*p)->Table[0]; i < (*p)->TabSize; ndp_t++, i++) {
      ndp_t->State   = NDP_STATE_FREE;
      ndp_t->tx_list = NULL;
    }
    memset ((*p)->Ctrl, 0, sizeof (*(*p)->Ctrl));
    net_addr6_make_eui64 ((*p)->If->localm6->LLAddr, NULL, (*p)->If->MacAddr);
    DEBUGF (NDP," Make LLAddr [%s]\n",net_addr6_ntoa((*p)->If->localm6->LLAddr));
    EvrNetNDP_MakeLinkLocalAddress ((*p)->If->Id, (*p)->If->localm6->LLAddr);
  }
}

/**
  \brief       De-initialize NDP cache.
*/
void net_ndp_cache_uninit (void) {
  NET_NDP_CFG *const *p;

  for (p = &net_ndp_list[0]; *p; p++) {
    DEBUGF (NDP,"Uninit_cache %s\n",(*p)->If->Name);
    EvrNetNDP_UninitCache ((*p)->If->Id);
    /* Clear cache table and resolver control */
    memset ((*p)->Table, 0, sizeof (*(*p)->Table) * (*p)->TabSize);
    net_sys_resume (&(*p)->Ctrl->xThread);
    memset ((*p)->Ctrl, 0, sizeof (*(*p)->Ctrl));
    (*p)->Ctrl->xRetv = netTimeout;
  }
}

/**
  \brief       Process Neighbor Discovery message.
  \param[in]   net_if  network interface descriptor.
  \param[in]   frame   network frame.
*/
void net_ndp_process (NET_IF_CFG *net_if, NET_FRAME *frame) {
  NET_NDP_CFG *h = net_if->Ip6Cfg->NdpCfg;
  const uint8_t *opt,*end;
  uint16_t tout;
  uint32_t mtu;

  switch (ICMP6_FRAME(frame)->Type) {
    case ICMP6_ROUTER_ADVER:
      /* Router Advertisement message */
      DEBUGF (NDP,"Process %s, Router-Advertisement\n",h->If->Name);
      if (!ndp_check_frame (frame, NDP_OPT_OFFS_RA)) {
        /* Frame length and ICMP option length check failed */
        ERRORF (NDP,"Process %s, RA frame check failed\n",h->If->Name);
        EvrNetNDP_FrameCheckFailed (h->If->Id, ICMP6_ROUTER_ADVER);
        break;
      }
      if (!net_addr6_is_linklocal (IP6_FRAME(frame)->SrcAddr)) {
        /* Nodes must silently discard RA messages if source */
        /* IP address is not link-local.   [RFC4861 page 38] */
        DEBUGF (NDP," Discarded, SrcAddr not link-local\n");
        EvrNetNDP_AddressNotLinkLocal (h->If->Id, IP6_FRAME(frame)->SrcAddr);
        break;
      }
      DEBUGF (NDP," CurHopLimit=%d, Flags=0x%02X\n",
                   ICMP6_FRAME(frame)->Data[0],ICMP6_FRAME(frame)->Data[1]);
      /* Check Router lifetime */
      tout = get_u16 (&ICMP6_FRAME(frame)->Data[2]);
      DEBUGF (NDP," Router-Lifetime=%d\n",tout);
      EvrNetNDP_RouterAdvertisement (h->If->Id, ICMP6_FRAME(frame)->Data[0],
                                     ICMP6_FRAME(frame)->Data[1], tout);
      if (tout != 0) {
        /* Add to default router list */
        NET_NDP_INFO *ndp_t;
        ndp_t = ndp_cache_add (h, IP6_FRAME(frame)->SrcAddr,
                               ETH_FRAME(frame)->SrcAddr, tout, NDP_FLAG_ROUTER);
        if (ndp_t && (ndp_t == ctrl->DefRouter)) {
          /* This is active router */
          if (ICMP6_FRAME(frame)->Data[0] > 0) {
            /* Update HopLimit if not 0 [RFC4861 page 19] */
            LocM6.HopLimit = ICMP6_FRAME(frame)->Data[0];
          }
        }
      }
      else {
        /* Remove from default router list [RFC4861 page 20] */
        ndp_cache_delete (h, IP6_FRAME(frame)->SrcAddr, NDP_FLAG_ROUTER);
      }
      /* Process router options */
      DEBUGF (NDP,"Process %s, RA options\n",h->If->Name);
      end = &ICMP6_FRAME(frame)->Data[frame->length - ICMP_HEADER_LEN];
      for (opt = &ICMP6_FRAME(frame)->Data[NDP_OPT_OFFS_RA]; opt != NULL; ) {
        switch (opt[0]) {
          case NDP_OPT_MTU:
            /* Maximum transmission unit option */
            mtu = get_u32 (&opt[4]);
            if (mtu > 1500) mtu = 1500;
            if (mtu < 1280) mtu = 1280;
            LocM6.Mtu = mtu & 0xFFFF;
            DEBUGF (NDP," Option-Mtu=%d\n",mtu);
            EvrNetNDP_OptionMtu (h->If->Id, mtu);
            break;

          case NDP_OPT_SOURCE_ADDR:
            /* Source Link-layer address option */
            DEBUGF (NDP," Option-SrcAddr (%s)\n",net_mac_ntoa(&opt[2]));
            EvrNetNDP_OptionSourceMacAddress (h->If->Id, &opt[2]);
            break;

          case NDP_OPT_PREFIX_INFO:
            /* Prefix Information option */
            DEBUGF (NDP," Option-Prefix [%s/%d]\n",net_addr6_ntoa(&opt[16]),opt[2]);
            DEBUGF (NDP," + Lifetime Valid=%d, Prefrd=%d\n",
                          get_u32(&opt[4]),get_u32(&opt[8]));
            DEBUGF (NDP," + Flags=0x%02X\n",opt[3]);
            EvrNetNDP_OptionPrefixInfo (&opt[0]);
            /* Validate Prefix information     [RFC4861 page 28] */
            /* - option_length=4                                 */
            /* - prefix_len=64 (SLAAC uses only 64)              */
            /* - Autonomous flag must be set                     */
            /* - On-link flag must be set                        */
            /* - Prefix must not be link-local [RFC4862 page 17] */
            /* - Valid lifetime >= Preferred lifetime            */
            if ((opt[1] != 4) || (opt[2] != 64) || !(opt[3] & 0xC0) ||
                (get_u32(&opt[4]) < get_u32(&opt[8]))               ||
                (net_addr6_is_linklocal (&opt[16]))) {
              DEBUGF (NDP," Discarded, Prefix invalid\n");
              EvrNetNDP_PrefixInfoNotValid (h->If->Id);
              break;
            }
            ndp_prefix_add (h, &opt[16], get_u32(&opt[4]));
            break;
        }
        opt = ndp_next_opt (opt, end);
      }
      break;

    case ICMP6_NEIGHB_SOL:
      /* Neighbor Solicitation message */
      DEBUGF (NDP,"Process %s, Neighbor-Solicitation\n",h->If->Name);
      if (!ndp_check_frame (frame, NDP_OPT_OFFS_NS)) {
        /* Frame length and ICMP option length check failed */
        ERRORF (NDP,"Process %s, NS frame check failed\n",h->If->Name);
        EvrNetNDP_FrameCheckFailed (h->If->Id, ICMP6_NEIGHB_SOL);
        break;
      }
      DEBUGF (NDP," TargAddr [%s]\n",net_addr6_ntoa(&ICMP6_FRAME(frame)->Data[4]));
      if (!net_addr6_comp (&ICMP6_FRAME(frame)->Data[4], LocM6.LLAddr)   &&
          !net_addr6_comp (&ICMP6_FRAME(frame)->Data[4], LocM6.TempAddr) &&
          !net_addr6_comp (&ICMP6_FRAME(frame)->Data[4], LocM6.IpAddr)) {
        DEBUGF (NDP," Discarded, Wrong TargAddr\n");
        EvrNetNDP_WrongTargetAddress (h->If->Id, &ICMP6_FRAME(frame)->Data[4]);
        break;
      }
      EvrNetNDP_NeighborSolicitation (h->If->Id, &ICMP6_FRAME(frame)->Data[4]);
      /* Destination IP is our's address */
      if (net_addr6_is_unspec (IP6_FRAME(frame)->SrcAddr)) {
        /* Probe request message (source IP "::") */
        DEBUGF (NDP," Probe Request received\n");
        EvrNetNDP_ProbeRequest (h->If->Id);
        ndp_send_reply (h, NULL, &ICMP6_FRAME(frame)->Data[4]);
        break;
      }
      end = &ICMP6_FRAME(frame)->Data[frame->length - ICMP_HEADER_LEN];
      for (opt = &ICMP6_FRAME(frame)->Data[NDP_OPT_OFFS_NS]; opt != NULL; ) {
        switch (opt[0]) {
          case NDP_OPT_SOURCE_ADDR:
            /* Source Link-layer address option */
            DEBUGF (NDP," Option-SrcAddr (%s)\n",net_mac_ntoa(&opt[2]));
            EvrNetNDP_OptionSourceMacAddress (h->If->Id, &opt[2]);
            if (ndp_cache_add (h, IP6_FRAME(frame)->SrcAddr,
                                  &opt[2], h->CacheTout, NDP_FLAG_HOST)) {
              /* Source address is link-local address (MAC) */
              ndp_send_reply (h, IP6_FRAME(frame)->SrcAddr,
                                 &ICMP6_FRAME(frame)->Data[4]);
            }
            break;
        }
        opt = ndp_next_opt (opt, end);
      }
      break;

    case ICMP6_NEIGHB_ADVER:
      /* Neighbor Advertisement message */
      DEBUGF (NDP,"Process %s, Neighbor-Advertisement\n",h->If->Name);
      if (!ndp_check_frame (frame, NDP_OPT_OFFS_NA)) {
        /* Frame length and ICMP option length check failed */
        ERRORF (NDP,"Process %s, NA frame check failed\n",h->If->Name);
        EvrNetNDP_FrameCheckFailed (h->If->Id, ICMP6_NEIGHB_ADVER);
        break;
      }
      DEBUGF (NDP," TargAddr [%s]\n",net_addr6_ntoa(&ICMP6_FRAME(frame)->Data[4]));
      EvrNetNDP_NeighborAdvertisement (h->If->Id, &ICMP6_FRAME(frame)->Data[4]);
      if (net_addr6_comp (IP6_FRAME(frame)->DstAddr, net_addr6_all_nodes)) {
        /* Probe response message (destination IP "FF02::1") */
        if ((ctrl->Flags & NDP_IFLAG_PROBE_BUSY) &&
             net_addr6_comp (&ICMP6_FRAME(frame)->Data[4], ctrl->TargAddr)) {
          /* Probe response to user NDP probe request */
          DEBUGF (NDP," Probe Response received\n");
          EvrNetNDP_ProbeResponse (h->If->Id);
          ctrl->Flags &= ~NDP_IFLAG_PROBE_BUSY;
          if (ctrl->cb_func) {
            ctrl->cb_func (netNDP_EventSuccess);
            break;
          }
          ctrl->xRetv = netOK;
          net_sys_resume (&ctrl->xThread);
        }
        break;
      }
      /* Solicited Neighbor Advertisement message            */
      /* The Destination IPv6 should be the Source IPv6 from */
      /* invoking Neighbor Solicitation.   [RFC4861 page 24] */
      if (!net_addr6_comp (IP6_FRAME(frame)->DstAddr, LocM6.LLAddr)   &&
          !net_addr6_comp (IP6_FRAME(frame)->DstAddr, LocM6.TempAddr) &&
          !net_addr6_comp (IP6_FRAME(frame)->DstAddr, LocM6.IpAddr)) {
        DEBUGF (NDP," Discarded, Wrong DstAddr\n");
        EvrNetNDP_WrongDestinationAddress (h->If->Id, IP6_FRAME(frame)->DstAddr);
        break;
      }
      /* Destination IP is our address, process response */
      ndp_proc_response (h, ICMP6_FRAME(frame), frame->length);
      break;

    default:
      break;
  }
}

/**
  \brief       Send advertisement reply to solicitation message.
  \param[in]   h          NDP instance handle.
  \param[in]   ip_addr    destination IPv6 address or NULL for all nodes.
  \param[in]   targ_addr  target IPv6 address.
*/
static void ndp_send_reply (NET_NDP_CFG *h,
                            const uint8_t *ip_addr, const uint8_t *targ_addr) {
  NET_FRAME *frame;
  uint32_t flags;

  DEBUGF (NDP,"Send_reply %s\n",h->If->Name);
  DEBUGF (NDP," TargAddr [%s]\n",net_addr6_ntoa(targ_addr));
  EvrNetNDP_SendReply (h->If->Id, targ_addr);
  frame = net_mem_alloc (IP6_DATA_OFFS + ICMP_HEADER_LEN + NDP_OPT_OFFS_NA + 8);

  /* Unicast NA Flags:   Solicited + Override */
  /* Multicast NA Flags: Override only        */
  flags = (ip_addr) ? 0x60000000 : 0x20000000;
  set_u32 (&ICMP6_FRAME(frame)->Data[0], flags);
  DEBUGF (NDP," Flags=0x%08X\n",flags);
  EvrNetNDP_ViewFlags (h->If->Id, flags);

  /* Target IPv6 address is our's */
  net_addr6_copy (&ICMP6_FRAME(frame)->Data[4], targ_addr);

  /* Option: Target link-layer address is our's */
  ICMP6_FRAME(frame)->Data[NDP_OPT_OFFS_NA+0] = NDP_OPT_TARGET_ADDR;
  ICMP6_FRAME(frame)->Data[NDP_OPT_OFFS_NA+1] = 1;
  net_mac_copy (&ICMP6_FRAME(frame)->Data[NDP_OPT_OFFS_NA+2], h->If->MacAddr);

  frame->length = ICMP_HEADER_LEN + NDP_OPT_OFFS_NA + 8;
  /* Probe reply goes to all nodes */
  ip_addr = (ip_addr) ? ip_addr : net_addr6_all_nodes;
  net_icmp6_send (h->If, frame, targ_addr, ip_addr, ICMP6_NEIGHB_ADVER);
  net_mem_free (frame);
}

/**
  \brief       Construct and send solicitation request.
  \param[in]   h        NDP instance handle.
  \param[in]   type     request type.
  \param[in]   ip_addr  target IP address.
  \note        Request message contains fields:
               - RS:               LL_option
               - NS:      Target + LL_option
               - NS(DAD): Target
*/
static void ndp_send_request (NET_NDP_CFG *h,
                              uint8_t type, const uint8_t *ip_addr) {
  const uint8_t *src_addr;
  NET_FRAME *frame;
  uint32_t idx;

  DEBUGF (NDP,"Send_request %s\n",h->If->Name);
  DEBUGF (NDP," Type %s-Solicitation\n",
               (type == ICMP6_ROUTER_SOL)?"Router":"Neighbor");
  EvrNetNDP_SendRequest (h->If->Id, type);
  frame = net_mem_alloc (IP6_DATA_OFFS + ICMP_HEADER_LEN + 28);

  /* Reserved */
  set_u32 (&ICMP6_FRAME(frame)->Data[0], 0);

  /* Target IPv6 address */
  idx = 4;
  if ((type == ICMP6_NEIGHB_SOL) || (type == ICMP6_NBR_SOL_DAD)) {
    net_addr6_copy (&ICMP6_FRAME(frame)->Data[4], ip_addr);
    DEBUGF (NDP," TargAddr [%s]\n",net_addr6_ntoa(ip_addr));
    EvrNetNDP_ViewTargetAddress (h->If->Id, ip_addr);
    idx = 20;
  }

  /* Option: Target link-layer address */
  if ((type == ICMP6_ROUTER_SOL) || (type == ICMP6_NEIGHB_SOL)) {
    ICMP6_FRAME(frame)->Data[idx+0] = NDP_OPT_SOURCE_ADDR;
    ICMP6_FRAME(frame)->Data[idx+1] = 1;
    net_mac_copy (&ICMP6_FRAME(frame)->Data[idx+2], h->If->MacAddr);
    idx += 8;
  }
  frame->length = (ICMP_HEADER_LEN + idx) & 0xFFFF;

  if (type == ICMP6_NBR_SOL_DAD) {
    type     = ICMP6_NEIGHB_SOL;
    src_addr = net_addr_unspec;
  }
  else {
    src_addr = net_ip6_get_local (h->If, ip_addr);
  }
  net_icmp6_send (h->If, frame, src_addr, ip_addr, type);
  net_mem_free (frame);
}

/**
  \brief       Process neighbor advertisement response.
  \param[in]   h       NDP instance handle.
  \param[in]   icmp_r  ICMP response frame.
  \param[in]   len     length of the frame.
*/
static void ndp_proc_response (NET_NDP_CFG *h,
                               NET_ICMP_HEADER *icmp_r, uint32_t len) {
  NET_NDP_INFO *ndp_t;
  const uint8_t *opt,*end;
  uint32_t i;

  /* Are we waiting for that reply? */
  for (i = 0, ndp_t = &h->Table[0]; i < h->TabSize; ndp_t++, i++) {
    if ((ndp_t->State != NDP_STATE_PENDING) &&
        (ndp_t->State != NDP_STATE_REFRESH)) {
      continue;
    }
    if (!net_addr6_comp (&icmp_r->Data[4], ndp_t->IpAddr)) {
      continue;
    }
    /* The Override flag is ignored if the entry is in the INCOMPLETE state.*/
    /* In any other state, the advertisement should be ignored and must not */
    /* update the cache, if the Override flag is clear.  [RFC 4861 page 65] */
    if ((ndp_t->State != NDP_STATE_PENDING) && ((icmp_r->Data[0] & 0x20) == 0)) {
      /* State not INCOMPLETE and Override flag clear */
      return;
    }
    /* This remote IP is in cache */
    end = &icmp_r->Data[len - ICMP_HEADER_LEN];
    /* Process neighbor discovery options */
    for (opt = &icmp_r->Data[NDP_OPT_OFFS_NA]; opt != NULL; ) {
      if (opt[0] == NDP_OPT_TARGET_ADDR) {
        /* Target Link-layer address option */
        DEBUGF (NDP," Option-TargAddr (%s)\n",net_mac_ntoa(&opt[2]));
        EvrNetNDP_OptionTargetMacAddress (h->If->Id, &opt[2]);
        net_mac_copy (ndp_t->MacAddr, &opt[2]);
        ndp_que_send (h, ndp_t);
        DEBUGF (NDP," Cache, Entry %d updated\n",ndp_t->Id);
        EvrNetNDP_CacheEntryUpdate (h->If->Id, ndp_t->Id);
        if (ndp_t->Flags & NDP_FLAG_HOST_INUSE) {
          ndp_t->Flags &= ~NDP_FLAG_HOST_INUSE;
        }
        ndp_t->Tout    = h->CacheTout;
        ndp_t->Retries = h->MaxRetry;
        ndp_t->State   = NDP_STATE_RESOLVED;
        return;
      }
      opt = ndp_next_opt (opt, end);
    }
    break;
  }
  ERRORF (NDP,"Process %s, Wrong response\n",h->If->Name);
  EvrNetNDP_WrongResponse (h->If->Id, icmp_r->Type);
}

/**
  \brief       Allocate a NDP cache entry.
  \param[in]   h  NDP instance handle.
  \return      pointer to allocated cache entry.
  \note        Choose the least destructive entry to use:
                1.unused/free entry
                2.oldest stable temp entry
                3.oldest stable in-use entry
*/
static NET_NDP_INFO *ndp_cache_alloc (NET_NDP_CFG *h) {
  NET_NDP_INFO *ndp_t;
  uint32_t i,i1,i2,t1,t2;

  /* Scan the cache and find an unused entry */
  for (i = 1, ndp_t = &h->Table[0]; i <= h->TabSize; ndp_t++, i++) {
    if (ndp_t->State == NDP_STATE_FREE) {
      ndp_t->Id = i & 0xFF;
      return (ndp_t);
    }
  }

  /* Cache is full, find the oldest stable entry */
  /* The oldest entry will expire first          */
  i1 = i2 = 0;
  t1 = t2 = h->CacheTout;
  for (i = 1, ndp_t = &h->Table[0]; i <= h->TabSize; ndp_t++, i++) {
    if ((ndp_t->State != NDP_STATE_RESOLVED) || (ndp_t->Flags & NDP_FLAG_ROUTER)) {
      continue;
    }
    if (!(ndp_t->Flags & NDP_FLAG_HOST_INUSE)) {
      if (ndp_t->Tout <= t1) {
        /* This host entry is older */
        t1 = ndp_t->Tout;
        i1 = i;
      }
    }
    else {
      if (ndp_t->Tout <= t2) {
        /* This inuse entry is older */
        t2 = ndp_t->Tout;
        i2 = i;
      }
    }
  }

  /* If temp entry found, then use it, */
  /* otherwise try the inuse entries.  */
  i = (i1) ? i1 : i2;
  if (i != 0) {
    ndp_t = &h->Table[i-1];
    ndp_t->Id = i & 0xFF;
    return (ndp_t);
  }
  return (NULL);
}

/**
  \brief       Parse Router/Neighbor Discovery options.
  \param[in]   buf  buffer containing options.
  \param[in]   end  buffer end.
  \return      pointer to next option or NULL.
*/
static const uint8_t *ndp_next_opt (const uint8_t *buf, const uint8_t *end) {
  const uint8_t *next;

  if (buf < end) {
    /* Increase by option length */
    next = buf + (buf[1] * 8);
    if (next < end) {
      return (next);
    }
  }
  return (NULL);
}

/**
  \brief       Add a router to default router list.
  \param[in]   h      NDP instance handle.
  \param[in]   ndp_t  pointer to router cache entry.
*/
static void ndp_router_add (NET_NDP_CFG *h, NET_NDP_INFO *ndp_t) {
  if (ctrl->DefRouter == NULL) {
    /* Set default gateway of local machine */
    net_addr6_copy (LocM6.DefGW, ndp_t->IpAddr);
    ctrl->DefRouter = ndp_t;
    ctrl->AltRouter = NULL;
  }
  else if (ctrl->AltRouter == NULL) {
    ctrl->AltRouter = ndp_t;
  }
}

/**
  \brief       Remove a router from default router list.
  \param[in]   h      NDP instance handle.
  \param[in]   ndp_t  pointer to router cache entry.
*/
static void ndp_router_remove (NET_NDP_CFG *h, NET_NDP_INFO *ndp_t) {
  if (ndp_t == ctrl->AltRouter) {
    ctrl->AltRouter = NULL;
  }
  else if (ndp_t == ctrl->DefRouter) {
    /* Alternate router is now default */
    ndp_t           = ctrl->AltRouter;
    ctrl->AltRouter = NULL;
    ctrl->DefRouter = ndp_t;
    /* Set default gateway of local machine */
    net_addr6_copy (LocM6.DefGW, ndp_t ? ndp_t->IpAddr : net_addr_unspec);
  }
}

/**
  \brief       Add prefix to a prefix list.
  \param[in]   h         NDP instance handle.
  \param[in]   prefix    pointer to NDP prefix information.
  \param[in]   lt_valid  valid lifetime in seconds.
*/
static void ndp_prefix_add (NET_NDP_CFG *h,
                            const uint8_t *prefix, uint32_t lt_valid) {
  NET_NDP_PREFIX *pref_t;
  uint32_t i;

  /* Check auto-address first */
  if (slaac->AutoTout == 0) {
    /* Make new temporary address using prefix info */
    if (lt_valid != 0) {
      if (lt_valid < SLAAC_MIN_VALID) {
        lt_valid = SLAAC_MIN_VALID;
      }
      slaac->AutoTout = (lt_valid >> 6) & 0xFFFF;
      /* For safety clear prefix list */
      slaac->Prefix[0].Timer = 0;
      slaac->Prefix[1].Timer = 0;
      net_addr6_make_eui64 (LocM6.TempAddr, prefix, h->If->MacAddr);
      DEBUGF (NDP,"Make TempAddr %s\n",h->If->Name);
      DEBUGF (NDP," Addr [%s]\n",net_addr6_ntoa(LocM6.TempAddr));
      EvrNetNDP_MakeTempAddress (h->If->Id, LocM6.TempAddr);
      slaac->State = SLAAC_STATE_ACTIVE;
    }
    return;
  }

  /* Check if auto-address needs refresh */
  if (memcmp (LocM6.TempAddr, prefix, 8) == 0) {
    /* Update prefix timeout when: [RFC4862 page 19] */
    /* - received valid lifetime > 2 hours           */
    /* - received valid lifetime > remaining time    */
    if (lt_valid < SLAAC_MIN_VALID) {
      lt_valid = SLAAC_MIN_VALID;
    }
    if (lt_valid > ((uint32_t)slaac->AutoTout << 6)) {
      slaac->AutoTout = (lt_valid >> 6) & 0xFFFF;
    }
    slaac->State = SLAAC_STATE_ACTIVE;
    return;
  }

  /* Check if prefix needs refresh */
  for (i = 0, pref_t = &slaac->Prefix[0]; i < SLAAC_NUM_PREF; pref_t++, i++) {
    if ((pref_t->Timer != 0) && (memcmp (pref_t->Addr, prefix, 8) == 0)) {
      pref_t->Timer = (lt_valid >> 6) & 0xFFFF;
      return;
    }
  }

  /* Prefix not listed, add it to list now */
  for (i = 0, pref_t = &slaac->Prefix[0]; i < SLAAC_NUM_PREF; pref_t++, i++) {
    if (pref_t->Timer == 0) {
      pref_t->Timer = (lt_valid >> 6) & 0xFFFF;
      memcpy (pref_t->Addr, prefix, 8);
      return;
    }
  }
}

/**
  \brief       Check if IP6 address is on-link.
  \param[in]   h        NDP instance handle.
  \param[in]   ip_addr  destination IPv6 address.
  \return      check status:
               - true  = is on-link,
               - false = not on-link.
*/
static bool ndp_is_onlink (NET_NDP_CFG *h, const uint8_t *ip6_addr) {
  NET_NDP_PREFIX *pref_t;
  uint32_t i;

  if (get_u32 (&ip6_addr[0]) >= 0xFE800000) {
    /* Link-Local unicast or Multicast address */
    return (true);
  }
  /* Check SLAAC temp address and prefix list */
  if (slaac->AutoTout != 0) {
    if (memcmp (ip6_addr, LocM6.TempAddr, 8) == 0) {
      /* Auto-address prefix match */
      return (true);
    }
    for (i = 0, pref_t = &slaac->Prefix[0]; i < SLAAC_NUM_PREF; pref_t++, i++) {
      /* SLAAC prefix length is 64-bit only */
      if ((pref_t->Timer != 0) && (memcmp (ip6_addr, pref_t->Addr, 8) == 0)) {
        return (true);
      }
    }
  }
  /* Check static IP address too */
  return (net_addr6_is_onlink (ip6_addr, h->If->localm6));
}

/**
  \brief       Check NDP Discovery frame.
  \param[in]   frame     network frame.
  \param[in]   opt_offs  offset of option data.
  \return      check status:
               - true  = no error,
               - false = failed.
*/
static bool ndp_check_frame (NET_FRAME *frame, uint32_t opt_offs) {
  const uint8_t *buf,*end;

  /* Check frame size */
  if (frame->length < (ICMP_HEADER_LEN + opt_offs)) {
    return (false);
  }
  /* Check length of options */
  end = &ICMP6_FRAME(frame)->Data[frame->length - ICMP_HEADER_LEN];
  for (buf = &ICMP6_FRAME(frame)->Data[opt_offs]; buf < end; ) {
    if (buf[1] == 0) {
      /* Error, not allowed */
      return (false);
    }
    buf = buf + (buf[1] * 8);
  }
  if (buf != end) {
    /* Size mismatch */
    return (false);
  }
  return (true);
}

/**
  \brief       Add IPv6 address to neighbor cache or update it.
  \param[in]   h         NDP instance handle.
  \param[in]   ip_addr   neighbor IP address.
  \param[in]   mac_addr  neighbor MAC address.
  \param[in]   tout      timeout in seconds.
  \param[in]   type      entry type (host/router).
  \return      pointer to cache entry, NULL on error.
*/
static NET_NDP_INFO *ndp_cache_add (NET_NDP_CFG *h,
                                    const uint8_t *ip_addr, const uint8_t *mac_addr,
                                    uint16_t tout, uint8_t type) {
  NET_NDP_INFO *ndp_t;
  uint32_t i;

  for (i = 0, ndp_t = &h->Table[0]; i < h->TabSize; ndp_t++, i++) {
    if (ndp_t->State == NDP_STATE_FREE) {
      continue;
    }
    if (net_addr6_comp (ip_addr, ndp_t->IpAddr)) {
      /* Address is found in cache so update it */
      DEBUGF (NDP,"Cache %s, Entry %d updated\n",h->If->Name,i+1);
      DEBUG_INF2 (D_IP, ip_addr);
      EvrNetNDP_CacheIpRefreshed (h->If->Id, ip_addr);
      goto refresh;
    }
  }

  /* Address was not in cache */
  DEBUGF (NDP,"Cache_add %s\n",h->If->Name);
  DEBUG_INF2 (D_IP, ip_addr);
  DEBUG_INF2 (D_MAC, mac_addr);
  EvrNetNDP_CacheAdd (h->If->Id, ip_addr, mac_addr, type);

  if ((type == NDP_FLAG_ROUTER) && (ctrl->AltRouter != NULL)) {
    /* Max.number of routers cached, ignore this request */
    DEBUGF (NDP," Discarded, Router list full\n");
    EvrNetNDP_RouterListFull (h->If->Id, 2);
    return (NULL);
  }

  ndp_t = ndp_cache_alloc (h);
  if (ndp_t == NULL) {
    /* No Entries left, return */
    ERRORF (NDP,"Cache_add %s, Cache full\n",h->If->Name);
    EvrNetNDP_CacheAllocFailed (h->If->Id);
    return (NULL);
  }
  DEBUGF (NDP," Type %s, Lifetime=%d\n",
               (type == NDP_FLAG_ROUTER)?"Router":"Host",tout);
  DEBUGF (NDP," Entry %d added\n",ndp_t->Id);
  EvrNetNDP_CacheEntryAdded (h->If->Id, ndp_t->Id, tout);

  net_addr6_copy (ndp_t->IpAddr, ip_addr);
  if (type == NDP_FLAG_ROUTER) {
    ndp_router_add (h, ndp_t);
  }
refresh:
  net_mac_copy (ndp_t->MacAddr, mac_addr);
  ndp_t->Flags = type;
  ndp_t->Tout  = tout;
  ndp_t->State = NDP_STATE_RESOLVED;
  return (ndp_t);
}

/**
  \brief       Delete an IPv6 address from neighbor cache.
  \param[in]   h        NDP instance handle.
  \param[in]   ip_addr  neighbor IP address.
  \param[in]   type     entry type (host/router).
*/
static void ndp_cache_delete (NET_NDP_CFG *h, const uint8_t *ip_addr, uint8_t type) {
  NET_NDP_INFO *ndp_t;
  uint32_t i;

  for (i = 0, ndp_t = &h->Table[0]; i < h->TabSize; ndp_t++, i++) {
    if (ndp_t->State == NDP_STATE_FREE) {
      continue;
    }
    if (!(ndp_t->Flags & type)) {
      continue;
    }
    if (net_addr6_comp (ip_addr, ndp_t->IpAddr)) {
      /* Address is found in cache so delete it */
      DEBUGF (NDP,"Cache %s, Entry %d deleted\n",h->If->Name,i+1);
      DEBUG_INF2 (D_IP, ip_addr);
      EvrNetNDP_CacheEntryDeleted (h->If->Id, i+1, ip_addr, type);
      ndp_t->State = NDP_STATE_FREE;
      ndp_router_remove (h, ndp_t);
      ndp_que_free (ndp_t);
      break;
    }
  }
}

/**
  \brief       Refresh neighbor cache entry.
  \param[in]   net_if    network interface descriptor.
  \param[in]   ip6_addr  neighbor IP address.
  \param[in]   mac_addr  neighbor MAC address.
*/
void net_ndp_cache_refresh (NET_IF_CFG *net_if,
                            const uint8_t *ip6_addr, const uint8_t *mac_addr) {
  NET_NDP_CFG  *h = net_if->Ip6Cfg->NdpCfg;
  NET_NDP_INFO *ndp_t;
  uint32_t i;

  for (i = 0, ndp_t = &h->Table[0]; i < h->TabSize; ndp_t++, i++) {
    if (ndp_t->State <= NDP_STATE_RESERVED) {
      continue;
    }
    if (!net_mac_comp (mac_addr, ndp_t->MacAddr)) {
      continue;
    }
    if (net_addr6_comp (ip6_addr, ndp_t->IpAddr)) {
      /* Address is found in cache so refresh it */
      DEBUGF (NDP,"Cache %s, Entry %d refreshed\n",h->If->Name,ndp_t->Id);
      DEBUG_INF2 (D_IP, ip6_addr);
      EvrNetNDP_CacheEntryRefreshed (h->If->Id, ndp_t->Id, ip6_addr);
      ndp_t->Tout  = h->CacheTout;
      ndp_t->State = NDP_STATE_RESOLVED;
      return;
    }
  }
}

/**
  \brief       Find IPv6 address in neighbor cache.
  \param[in]   net_if    network interface descriptor.
  \param[in]   ip6_addr  IPv6 address.
  \return      pointer to cached MAC address.
*/
NET_NDP_INFO *net_ndp_cache_find (NET_IF_CFG *net_if, const uint8_t *ip6_addr) {
  NET_NDP_CFG  *h = net_if->Ip6Cfg->NdpCfg;
  NET_NDP_INFO *ndp_t;
  uint32_t i;

  DEBUGF (NDP,"Cache_find %s\n",h->If->Name);
  DEBUG_INF2 (D_IP, ip6_addr);
  EvrNetNDP_CacheFind (h->If->Id, ip6_addr);
  if (!ndp_is_onlink (h, ip6_addr)) {
    DEBUGF (NDP," External, Use gateway\n");
    if (net_addr6_is_unspec (LocM6.DefGW)) {
      /* Default Gateway unspecified or expired */
      ERRORF (NDP,"Cache_find %s, Gateway unknown\n",h->If->Name);
      EvrNetNDP_GatewayUnknown (h->If->Id, LocM6.DefGW);
      return (NULL);
    }
    if (ctrl->DefRouter != NULL) {
      /* Dynamic config enabled, return default router */
      EvrNetNDP_UsingGateway (h->If->Id, ctrl->DefRouter->IpAddr);
      if (ctrl->DefRouter->State == NDP_STATE_FREE) {
        /* Safety check, should not be free */
        return (NULL);
      }
      return (ctrl->DefRouter);
    }
    /* Use neighbor discovery to resolve MAC address */
    /* of Default GW in static configuration mode    */
    ip6_addr = LocM6.DefGW;
    if (!net_addr6_is_linklocal (ip6_addr)) {
      /* Default Gateway must be link-local */
      ERRORF (NDP,"Cache_find %s, Gateway not link-local\n",h->If->Name);
      EvrNetNDP_GatewayUnknown (h->If->Id, ip6_addr);
      return (NULL);
    }
    EvrNetNDP_UsingGateway (h->If->Id, ip6_addr);
  }
  /* Check if IP already cached */
  for (i = 0, ndp_t = &h->Table[0]; i < h->TabSize; ndp_t++, i++) {
    if (ndp_t->State == NDP_STATE_FREE) {
      continue;
    }
    if (net_addr6_comp (ndp_t->IpAddr, ip6_addr)) {
      /* IP address found in cache, now check if it is valid */
      if (ndp_t->State < NDP_STATE_RESOLVED) {
        ERRORF (NDP,"Cache_find %s, Unresolved MAC\n",h->If->Name);
        EvrNetNDP_UnresolvedMacAddress (h->If->Id, ip6_addr);
        return (ndp_t);
      }
      DEBUGF (NDP," Entry %d found\n",ndp_t->Id);
      EvrNetNDP_EntryFound (h->If->Id, ndp_t->Id);
      if (ndp_t->Flags & NDP_FLAG_HOST) {
        /* Inuse-IP refreshed once, then removed from cache */
        ndp_t->Flags |= NDP_FLAG_HOST_INUSE;
      }
      return (ndp_t);
    }
  }

  /* IP not cached, start neighbor discovery */
  ndp_t = ndp_cache_alloc (h);
  if (ndp_t != NULL) {
    DEBUGF (NDP," Entry %d added\n",ndp_t->Id);
    EvrNetNDP_CacheEntryAdded (h->If->Id, ndp_t->Id, 0);
    ndp_t->Flags = NDP_FLAG_HOST;
    ndp_que_free (ndp_t);
    net_addr6_copy (ndp_t->IpAddr, ip6_addr);
    net_mac_copy (ndp_t->MacAddr, net_addr_bcast);
    /* Start neighbor discovery */
    ndp_t->State = NDP_STATE_RESERVED;
  }
  return (ndp_t);
}

/**
  \brief       Resolve MAC address to neighbor cache.
  \param[in]   if_id     interface identifier.
  \param[in]   ip6_addr  IPv6 address.
  \return      status code as defined with netStatus.
  \note        If IP address not cached, send Neigbor Discovery.
*/
netStatus netNDP_CacheIP (uint32_t if_id, const uint8_t *ip6_addr) {
  NET_IF_CFG  *net_if = net_if_map_lan (if_id);
  NET_NDP_CFG *h;
  NET_NDP_INFO *ndp_t;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip6Cfg) || (ip6_addr == NULL)) {
    ERRORF (NDP,"CacheIP, Invalid parameter\n");
    EvrNetNDP_CacheIpInvalidParameter (if_id & 0xFFFF);
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip6Cfg->NdpCfg;

  DEBUGF (NDP,"CacheIP %s\n",h->If->Name);
  DEBUG_INF2 (D_IP, ip6_addr);
  EvrNetNDP_CacheIp (h->If->Id, ip6_addr);

  if (net_addr6_is_unspec (ip6_addr)) {
    /* Invalid IP address, non-cacheable */
    ERRORF (NDP,"CacheIP %s, Invalid IP\n",h->If->Name);
    EvrNetNDP_CacheIpInvalidParameter (h->If->Id);
    RETURN (netInvalidParameter);
  }
  ndp_t = net_ndp_cache_find (h->If, ip6_addr);
  if (ndp_t == NULL) {
    /* Requested IP is external but gateway unspecified */
    RETURN (netError);
  }
  if (ndp_t->State < NDP_STATE_RESOLVED) {
    /* MAC resolving in progress */
    RETURN (netBusy);
  }
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Resolve IPv6 address to neighbor cache.
  \param[in]   if_id     interface identifier.
  \param[in]   mac_addr  IPv6 address.
  \return      status code as defined with netStatus.
  \note        If MAC address not cached, send Inverse Neigbor Discovery.
               This function is not yet implemented!
*/
netStatus net_ndp_cache_mac (uint32_t if_id, const uint8_t *mac_addr) {
  NET_IF_CFG  *net_if = net_if_map_lan (if_id);
  NET_NDP_CFG *h;
  NET_NDP_INFO *ndp_t;
  uint32_t i;

  if (!(net_if && net_if->Ip6Cfg) || (mac_addr == NULL)) {
    ERRORF (NDP,"CacheMAC, Invalid parameter\n");
    return (netInvalidParameter);
  }
  h = net_if->Ip6Cfg->NdpCfg;

  DEBUGF (NDP,"CacheMAC %s\n",h->If->Name);
  DEBUG_INF2 (D_MAC, mac_addr);
  /* Check if MAC already cached */
  for (i = 0, ndp_t = &h->Table[0]; i < h->TabSize; ndp_t++, i++) {
    if (ndp_t->State == NDP_STATE_FREE) {
      continue;
    }
    if (net_mac_comp (ndp_t->MacAddr, mac_addr)) {
      /* MAC address found in cache, check if it is valid */
      if (ndp_t->State < NDP_STATE_RESOLVED) {
        ERRORF (NDP,"CacheMAC %s, Unresolved IP\n",h->If->Name);
        return (netError);
      }
      DEBUGF (NDP," Entry %d found\n",ndp_t->Id);
      return (netOK);
    }
  }
#if 0
  /* MAC not cached */
  arp_t = arp_cache_alloc ();
  if (arp_t != NULL) {
    DEBUGF (ARP," Entry %d added\n",arp_t->Id);
    arp_t->Type  = ARP_TYPE_TEMP_IP;
    arp_que_free (arp_t);
    set_u32 (arp_t->IpAddr, 0);
    net_mac_copy (arp_t->MacAddr, mac_addr);
    /* Start Inverse ARP resolution */
    arp_t->State = ARP_STATE_RESERVED;
  }
#endif
  return (netBusy);
}

/**
  \brief       Get IP address from neighbor cache.
  \param[in]   if_id     interface identifier.
  \param[in]   mac_addr  MAC address.
  \param[out]  ip6_addr  IPv6 address.
  \return      status code as defined with netStatus.
*/
netStatus netNDP_GetIP (uint32_t if_id, const uint8_t *mac_addr, uint8_t *ip6_addr) {
  NET_IF_CFG  *net_if = net_if_map_lan (if_id);
  NET_NDP_CFG *h;
  const uint8_t *sp;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip6Cfg) || (mac_addr == NULL) || (ip6_addr == NULL)) {
    ERRORF (NDP,"GetIP, Invalid parameter\n");
    EvrNetNDP_GetIpInvalidParameter (if_id & 0xFFFF);
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip6Cfg->NdpCfg;

  DEBUGF (NDP,"GetIP %s\n",h->If->Name);
  DEBUG_INF2 (D_MAC, mac_addr);
  sp = ndp_find_ip (h, mac_addr);
  if (sp == NULL) {
    DEBUGF (NDP," Not found\n");
    EvrNetNDP_GetIpEntryNotFound (h->If->Id, mac_addr);
    RETURN (netError);
  }

  /* MAC found in cache, return IP */
  DEBUG_INF2 (D_IP, sp);
  EvrNetNDP_GetIp (h->If->Id, mac_addr, sp);
  net_addr6_copy (ip6_addr, sp);
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Get MAC address from neighbor cache.
  \param[in]   if_id     interface identifier.
  \param[in]   ip6_addr  IPv6 address.
  \param[out]  mac_addr  MAC address.
  \return      status code as defined with netStatus.
*/
netStatus netNDP_GetMAC (uint32_t if_id, const uint8_t *ip6_addr, uint8_t *mac_addr) {
  NET_IF_CFG  *net_if = net_if_map_lan (if_id);
  NET_NDP_CFG *h;
  const uint8_t *sp;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip6Cfg) || (mac_addr == NULL) || (ip6_addr == NULL)) {
    ERRORF (NDP,"GetMAC, Invalid parameter\n");
    EvrNetNDP_GetMacInvalidParameter (if_id & 0xFFFF);
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip6Cfg->NdpCfg;

  DEBUGF (NDP,"GetMAC %s\n",h->If->Name);
  DEBUG_INF2 (D_IP, ip6_addr);
  sp = net_ndp_find_mac (h->If, ip6_addr);
  if (sp == NULL) {
    DEBUGF (NDP," Not found\n");
    EvrNetNDP_GetMacEntryNotFound (h->If->Id, ip6_addr);
    RETURN (netError);
  }

  /* IP found in cache, return MAC */
  DEBUG_INF2 (D_MAC, sp);
  EvrNetNDP_GetMac (h->If->Id, ip6_addr, sp);
  net_mac_copy (mac_addr, sp);
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Probe IP address if already in use.
  \param[in]   if_id     interface identifier.
  \param[in]   ip6_addr  requested IPv6 address.
  \param[out]  cb_func   callback notification function.
  \return      status code as defined with netStatus.
*/
netStatus netNDP_Probe (uint32_t if_id, const uint8_t *ip6_addr, netNDP_cb_t cb_func) {
  NET_IF_CFG  *net_if = net_if_map_lan (if_id);
  NET_NDP_CFG *h;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip6Cfg) || (ip6_addr == NULL) || (cb_func == NULL)) {
    ERRORF (NDP,"Probe, Invalid parameter\n");
    EvrNetNDP_ProbeInvalidParameter (if_id & 0xFFFF);
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip6Cfg->NdpCfg;
  RETURN (ndp_probe_start (h, ip6_addr, cb_func));

  END_LOCK;
}

/**
  \brief       Probe IP address in blocking mode.
  \param[in]   if_id     interface identifier.
  \param[in]   ip6_addr  requested IPv4 address.
  \return      status code as defined with netStatus.
*/
netStatus netNDP_ProbeX (uint32_t if_id, const uint8_t *ip6_addr) {
  NET_IF_CFG *net_if = net_if_map_lan (if_id);
  NET_NDP_CFG *h;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip6Cfg) || (ip6_addr == NULL)) {
    ERRORF (NDP,"Probe, Invalid parameter\n");
    EvrNetNDP_ProbeInvalidParameter (if_id & 0xFFFF);
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip6Cfg->NdpCfg;

  for (;;) {
    /* Check if blocking probe is busy */
    if (!ctrl->xThread) {
      netStatus res = ndp_probe_start (h, ip6_addr, NULL);
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
  \brief       Clear the NDP cache.
  \param[in]   if_id  interface identifier.
  \return      status code as defined with netStatus.
*/
netStatus netNDP_ClearCache (uint32_t if_id) {
  NET_IF_CFG *net_if = net_if_map_lan (if_id);
  NET_NDP_CFG *h;
  NET_NDP_INFO *ndp_t;
  uint32_t i,cnt;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip6Cfg)) {
    ERRORF (NDP,"ClearCache, Invalid parameter\n");
    EvrNetNDP_ClearCacheInvalidParam (if_id & 0xFFFF);
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip6Cfg->NdpCfg;

  if (ctrl->Flags & NDP_IFLAG_PROBE_BUSY) {
    ERRORF (NDP,"ClearCache %s, Client busy\n",h->If->Name);
    EvrNetNDP_ClearCacheClientBusy (h->If->Id);
    RETURN (netBusy);
  }
  cnt = 0;
  for (i = 0, ndp_t = &h->Table[0]; i < h->TabSize; ndp_t++, i++) {
    if (ndp_t->State != NDP_STATE_FREE) {
      ndp_t->State = NDP_STATE_FREE;
      ndp_que_free (ndp_t);
      cnt++;
    }
  }
  ctrl->DefRouter = NULL;
  ctrl->AltRouter = NULL;
  ctrl->entry     = 0;
  slaac->State    = SLAAC_STATE_INIT;

  DEBUGF (NDP,"ClearCache %s, %d deleted of %d\n",h->If->Name,cnt,h->TabSize);
  EvrNetNDP_ClearCache (h->If->Id, cnt, h->TabSize);
  (void)cnt;
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Start probing the IP address.
  \param[in]   h         NDP instance handle.
  \param[in]   ip6_addr  requested IPv6 address.
  \param[out]  cb_func   callback notification function.
  \return      status code as defined with netStatus:
               - netOK
               - netInvalidParameter
               - netBusy
*/
static netStatus ndp_probe_start (NET_NDP_CFG *h,
                                  const uint8_t *ip6_addr, netNDP_cb_t cb_func) {

  DEBUGF (NDP,"Probe %s, Addr [%s]\n",h->If->Name,net_addr6_ntoa(ip6_addr));
  EvrNetNDP_Probe (h->If->Id, ip6_addr);

  /* Check if IP6 address is valid for probe */
  if (net_addr6_is_unspec (ip6_addr)                ||
      net_addr6_comp (ip6_addr, net_addr6_loopback) ||
      get_u32 (ip6_addr) > 0xFE800000) {
    /* Undefined IP6 : "::"              */
    /* Loopback  IP6 : "::1"             */
    /* Link-local IP6: "FE80::(EUI-64)"  */
    /* Multicast IP6 : "FF00::/8"        */
    ERRORF (NDP,"Probe %s, Invalid IP\n",h->If->Name);
    EvrNetNDP_ProbeInvalidParameter (h->If->Id);
    return (netInvalidParameter);
  }
  if (ctrl->Flags & NDP_IFLAG_PROBE_BUSY) {
    ERRORF (NDP,"Probe %s, Process busy\n",h->If->Name);
    EvrNetNDP_ProbeBusy (h->If->Id);
    return (netBusy);
  }

  net_addr6_copy (ctrl->TargAddr, ip6_addr);
  ctrl->cb_func = cb_func;
  ctrl->Flags  |= NDP_IFLAG_PROBE_BUSY;
  ctrl->Timer   = 1*SYS_TICK_TSEC;
  ctrl->Retries = 2;
  ndp_send_request (h, ICMP6_NBR_SOL_DAD, ctrl->TargAddr);
  return (netOK);
}

/**
  \brief       Find MAC address in neighbor cache.
  \param[in]   net_if    network interface descriptor.
  \param[in]   ip6_addr  IPv6 address.
  \return      pointer to cached MAC address.
*/
const uint8_t *net_ndp_find_mac (NET_IF_CFG *net_if, const uint8_t *ip6_addr) {
  NET_NDP_CFG  *h = net_if->Ip6Cfg->NdpCfg;
  NET_NDP_INFO *ndp_t;
  uint32_t i;

  for (i = 0, ndp_t = &h->Table[0]; i < h->TabSize; ndp_t++, i++) {
    if (ndp_t->State < NDP_STATE_RESOLVED) {
      continue;
    }
    if (net_addr6_comp (ndp_t->IpAddr, ip6_addr)) {
      /* IP address found in cache */
      return (ndp_t->MacAddr);
    }
  }
  return (NULL);
}

/**
  \brief       Find IP address in neighbor cache.
  \param[in]   h         NDP instance handle.
  \param[in]   mac_addr  MAC address.
  \return      pointer to cached IP address.
*/
static const uint8_t *ndp_find_ip (NET_NDP_CFG *h, const uint8_t *mac_addr) {
  NET_NDP_INFO *ndp_t;
  uint32_t i;

  for (i = 0, ndp_t = &h->Table[0]; i < h->TabSize; ndp_t++, i++) {
    if (ndp_t->State < NDP_STATE_RESOLVED) {
      continue;
    }
    if (net_mac_comp (ndp_t->MacAddr, mac_addr)) {
      /* MAC address found in cache */
      return (ndp_t->IpAddr);
    }
  }
  return (NULL);
}

/**
  \brief       Run address autoconfiguration service.
  \param[in]   h  NDP instance handle.
  \note        Executed on sys-tick (every 100ms).
*/
static void ndp_slaac_run (NET_NDP_CFG *h) {
  NET_NDP_PREFIX *pref_t;
  uint32_t i;

  switch (slaac->State) {
    case SLAAC_STATE_INIT:
      /* Initialize, wait for Link-up */
      if (!h->If->State->LinkUp) {
        /* Link is down, do nothing */
        break;
      }
      if (h->If->State->MacNew) {
        /* Multicast addresses changed */
        h->If->State->MacNew      = false;
        h->If->State->ConfigMcast = true;
        /* Update link-local address, MAC has changed */
        net_addr6_make_eui64 (LocM6.LLAddr, NULL, h->If->MacAddr);
        DEBUGF (NDP,"Make %s, LLAddr [%s]\n",h->If->Name, net_addr6_ntoa(LocM6.LLAddr));
        EvrNetNDP_MakeLinkLocalAddress (h->If->Id, LocM6.LLAddr);
        /* Auto-address is unusable, need to reconfigure */
        net_addr6_copy (LocM6.TempAddr, net_addr_unspec);
        slaac->AutoTout = 0;
      }
      /* Random initial delay 0.1 - 1.0 sec */
      slaac->Timer   = net_rand_max(SYS_TICK_TSEC) + 1;
      slaac->State   = SLAAC_STATE_START;
      break;

    case SLAAC_STATE_START:
      /* Start delay before sending RS */
      if (--slaac->Timer != 0) {
        /* Timeout not yet expired */
        break;
      }
      slaac->State   = SLAAC_STATE_DISCOVER;
      slaac->Retries = 3;
      slaac->Timer   = 1;
      __FALLTHROUGH;

    case SLAAC_STATE_DISCOVER:
      /* Discovering IPv6 routers */
      if (!h->If->State->LinkUp || h->If->State->MacNew) {
        /* Link is down or MAC has changed, restart SLAAC */
        slaac->State = SLAAC_STATE_INIT;
        break;
      }
      if (--slaac->Timer != 0) {
        /* Timeout not yet expired */
        break;
      }
      if (slaac->Retries != 0) {
        /* Resend solicitation request */
        slaac->Retries--;
        slaac->Timer = 4*SYS_TICK_TSEC;
        ndp_send_request (h, ICMP6_ROUTER_SOL, net_addr6_all_routers);
        break;
      }
      slaac->State = SLAAC_STATE_ACTIVE;
      break;

    case SLAAC_STATE_ACTIVE:
      /* Wait for link-down or MAC address change */
      if (!h->If->State->LinkUp || h->If->State->MacNew) {
        slaac->State = SLAAC_STATE_INIT;
      }
      break;
  }

  /* Executed on every minute */
  if (!(sys->Flags & SYS_FLAG_MIN64)) {
    return;
  }

  if (slaac->AutoTout == 0) {
    /* No prefix existing yet */
    return;
  }

  /* Update prefix timeouts first */
  for (i = 0, pref_t = &slaac->Prefix[0]; i < SLAAC_NUM_PREF; pref_t++, i++) {
    if (pref_t->Timer != 0) {
      pref_t->Timer--;
    }
  }

  if (--slaac->AutoTout != 0) {
    /* Auto-address still valid */
    return;
  }
  /* Auto-address has expired, make new */
  for (i = 0, pref_t = &slaac->Prefix[0]; i < SLAAC_NUM_PREF; pref_t++, i++) {
    if (pref_t->Timer != 0) {
      /* Minimum address lifetime is 2 h */
      uint32_t tout = (uint32_t)pref_t->Timer << 6;
      if (tout < SLAAC_MIN_VALID) {
        tout = SLAAC_MIN_VALID;
      }
      slaac->AutoTout = (tout >> 6) & 0xFFFF;
      pref_t->Timer   = 0;
      net_addr6_make_eui64 (LocM6.TempAddr, pref_t->Addr, h->If->MacAddr);
      DEBUGF (NDP,"Make %s, TempAddr\n",h->If->Name);
      DEBUGF (NDP," Addr [%s]\n",net_addr6_ntoa(LocM6.TempAddr));
      EvrNetNDP_MakeTempAddress (h->If->Id, LocM6.TempAddr);
      return;
    }
  }
}

/**
  \brief       Run NDP Probe service.
  \param[in]   h  NDP instance handle.
  \note        Executed on sys-tick (every 100ms).
*/
static void ndp_probe_run (NET_NDP_CFG *h) {

  if (!(ctrl->Flags & NDP_IFLAG_PROBE_BUSY)) {
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
    DEBUGF (NDP,"Probe %s, Retransmit\n",h->If->Name);
    EvrNetNDP_ProbeRetransmit (h->If->Id, ctrl->TargAddr);
    ndp_send_request (h, ICMP6_NBR_SOL_DAD, ctrl->TargAddr);
    return;
  }
  DEBUGF (NDP,"Probe %s, Timeout\n",h->If->Name);
  EvrNetNDP_ProbeTimeout (h->If->Id, ctrl->TargAddr);
  ctrl->Flags &= ~NDP_IFLAG_PROBE_BUSY;
  if (ctrl->cb_func){
    ctrl->cb_func (netNDP_EventTimeout);
    return;
  }
  ctrl->xRetv = netTimeout;
  net_sys_resume (&ctrl->xThread);
}

/**
  \brief       Enqueue the frame to transmit pending queue.
  \param[in]   net_if  network interface descriptor.
  \param[in]   ndp_t   pointer to a cache entry control block.
  \param[in]   frame   network frame.
  \return      status:
               - true  = success,
               - false = failed.
  \description Transmit queue contains a list of frames, waiting to resolve
               the MAC address. After the MAC address is resolved, all saved
               packets are sent to the resolved destination MAC address.
*/
bool net_ndp_enqueue (NET_IF_CFG *net_if, NET_NDP_INFO *ndp_t, NET_FRAME *frame) {
  NET_NDP_CFG *h = net_if->Ip6Cfg->NdpCfg;
  NET_FRAME *txframe,*next;

  if (!(net_mem_avail_tx())) {
    /* System out of memory, ignore this packet */
    return (false);
  }
  txframe = net_mem_alloc (frame->length);
  memcpy (txframe->data, frame->data, frame->length);
  /* This overlaps the ethernet header */
  NDP_QUE(txframe)->next = NULL;

  /* Add txframe to queue tail */
  if (ndp_t->tx_list == NULL) {
    /* First item in queue */
    ndp_t->tx_list = txframe;
  }
  else {
    /* Append txframe to the end */
    next = ndp_t->tx_list;
    for ( ; NDP_QUE(next)->next; next = NDP_QUE(next)->next);
    NDP_QUE(next)->next = txframe;
  }
  /* Send NDP request */
  if (ndp_t->State == NDP_STATE_RESERVED) {
    DEBUGF (NDP,"Cache %s, Entry %d resolving\n",h->If->Name,ndp_t->Id);
    EvrNetNDP_ResolveEntry (h->If->Id, ndp_t->Id);
    ndp_t->State   = NDP_STATE_PENDING;
    ndp_t->Retries = h->MaxRetry;
    ndp_t->Tout    = h->ResendTout;
    ndp_send_request (h, ICMP6_NEIGHB_SOL, ndp_t->IpAddr);
  }
  return (true);
}

/**
  \brief       Run main process of Neighbor cache.
*/
void net_ndp_cache_run (void) {
  NET_NDP_CFG *const *p;

  for (p = &net_ndp_list[0]; *p; p++) {
    ndp_cache_run (*p);
  }
}

/**
  \brief       Run main process of Neighbor cache.
  \param[in]   h  NDP instance handle.
  \description Remove expired temporary and update fixed IP entries.
*/
static void ndp_cache_run (NET_NDP_CFG *h) {
  NET_NDP_INFO *ndp_t;

  if (ctrl->entry == 0) {
    /* Take a snapshot of time tick flags. This prevents missed time ticks,*/
    /* if processing is split to multiple calls to ndp cache run function. */
    ctrl->sys_flags = sys->Flags;
    if (!(ctrl->sys_flags & SYS_FLAG_TICK)) {
      return;
    }
    ndp_slaac_run (h);
    ndp_probe_run (h);
  }
  sys->Busy = true;

  /* Every 100 ms all entries are processed. If there are actions taken     */
  /* (a packet is sent), processing is interrupted and the function returns.*/
  /* Remaining entries are processed in subsequent call to this function.   */
  for (++ctrl->entry; ctrl->entry <= h->TabSize; ctrl->entry++) {
    ndp_t = &h->Table[ctrl->entry-1];
    switch (ndp_t->State) {
      case NDP_STATE_FREE:
        break;

      case NDP_STATE_RESERVED:
        DEBUGF (NDP,"Cache %s, Entry %d resolving\n",h->If->Name,ctrl->entry);
        EvrNetNDP_ResolveEntry (h->If->Id, ctrl->entry);
        ndp_t->State   = NDP_STATE_PENDING;
        ndp_t->Retries = h->MaxRetry;
        goto request;

      case NDP_STATE_PENDING:
      case NDP_STATE_REFRESH:
        /* Counting rate is 1 tick */
        if (ndp_t->Tout != 0) {
          if (--ndp_t->Tout != 0) {
            /* A timeout not yet expired */
            break;
          }
        }
        if (ndp_t->Retries != 0) {
          /* Resend solicitation request */
          DEBUGF (NDP,"Cache %s, Entry %d resend\n",h->If->Name,ctrl->entry);
          EvrNetNDP_ResolveEntry (h->If->Id, ctrl->entry);
          ndp_t->Retries--;
request:  ndp_t->Tout = h->ResendTout;
          ndp_send_request (h, ICMP6_NEIGHB_SOL, ndp_t->IpAddr);
          return;
        }
        DEBUGF (NDP,"Cache %s, Entry %d released\n",h->If->Name,ctrl->entry);
        EvrNetNDP_EntryReleased (h->If->Id, ctrl->entry);
        ndp_t->State = NDP_STATE_FREE;
        ndp_que_free (ndp_t);
        break;

      case NDP_STATE_RESOLVED:
        if (!(ctrl->sys_flags & SYS_FLAG_SEC)) {
          break;
        }
        /* Counting rate is 1 second */
        if (ndp_t->Tout != 0) {
          ndp_t->Tout--;
          break;
        }
        /* This entry has timed out */
        DEBUGF (NDP,"Cache %s, Entry %d timeout\n",h->If->Name,ctrl->entry);
        DEBUGF (NDP," Type %s%s\n",(ndp_t->Flags & NDP_FLAG_ROUTER)?"Router":"Host",
                                   (ndp_t->Flags & NDP_FLAG_HOST_INUSE)?"-Inuse":"");
        DEBUG_INF2 (D_IP, ndp_t->IpAddr);
        DEBUG_INF2 (D_MAC, ndp_t->MacAddr);
        EvrNetNDP_CacheEntryTimeout (h->If->Id, ctrl->entry, ndp_t->IpAddr,
                                                ndp_t->MacAddr, ndp_t->Flags);
        if (ndp_t->Flags & NDP_FLAG_ROUTER) {
          /* This router entry has expired */
          ndp_router_remove (h, ndp_t);
          goto rel;
        }
        if (ndp_t->Flags & NDP_FLAG_HOST_INUSE) {
          /* InUse Host IP is refreshed once */
          DEBUGF (NDP," Refresh entry\n");
          EvrNetNDP_RefreshEntry (h->If->Id, ctrl->entry);
          ndp_t->State   = NDP_STATE_REFRESH;
          ndp_t->Retries = h->MaxRetry;
          ndp_t->Tout    = h->ResendTout;
          ndp_send_request (h, ICMP6_NEIGHB_SOL, ndp_t->IpAddr);
          return;
        }
        /* Host is released from cache */
rel:    DEBUGF (NDP," Entry released\n");
        EvrNetNDP_EntryReleased (h->If->Id, ctrl->entry);
        ndp_t->State = NDP_STATE_FREE;
        ndp_que_free (ndp_t);
        break;
    }
  }
  /* Prepare for the next run */
  ctrl->entry = 0;
}

/**
  \brief       Send all enqueued frames.
  \param[in]   h      NDP instance handle.
  \param[in]   ndp_t  pointer to a cache entry control block.
*/
static void ndp_que_send (NET_NDP_CFG *h, NET_NDP_INFO *ndp_t) {
  NET_FRAME *frame, *next;

  frame = ndp_t->tx_list;
  ndp_t->tx_list = NULL;
  for ( ; frame; frame = next) {
    next = NDP_QUE(frame)->next;
    /* Set source and destination MAC address */
    net_mac_copy (ETH_FRAME(frame)->SrcAddr, h->If->MacAddr);
    net_mac_copy (ETH_FRAME(frame)->DstAddr, ndp_t->MacAddr);
    h->If->output_lan (h->If->Id&0xFF, frame);
    net_mem_free (frame);
  }
}

/**
  \brief       Release all enqueued frames from memory.
  \param[in]   ndp_t  pointer to a cache entry control block.
*/
static void ndp_que_free (NET_NDP_INFO *ndp_t) {
  NET_FRAME *frame, *next;

  frame = ndp_t->tx_list;
  ndp_t->tx_list = NULL;
  for ( ; frame; frame = next) {
    next = NDP_QUE(frame)->next;
    net_mem_free (frame);
  }
}

#ifdef Network_Debug_STDIO
/**
  \brief       Debug print IPv6 or MAC address.
  \param[in]   type  address type.
  \param[in]   addr  IPv6 or MAC address.
*/
static void debug_inf2 (uint32_t type, const uint8_t *addr) {

  switch (type) {
    case D_MAC:
      DEBUGF (NDP," MAC (%s)\n",net_mac_ntoa(addr));
      break;
    case D_IP:
      DEBUGF (NDP," IP6 [%s]\n",net_addr6_ntoa(addr));
      break;
  }
}
#endif
#endif /* Network_IPv6 */
