/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ip6.c
 * Purpose: Internet Protocol Version 6
 *----------------------------------------------------------------------------*/

#include "rl_net_lib.h"
#include <string.h>
#include "net_sys.h"
#include "net_mem.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_loopback.h"
#include "net_icmp6.h"
#include "net_ip6.h"
#include "net_udp.h"
#include "net_tcp.h"
#include "net_dbg.h"

/* Global constants */
uint8_t const net_addr6_all_nodes[NET_ADDR_IP6_LEN] = {
  0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };

uint8_t const net_addr6_all_routers[NET_ADDR_IP6_LEN] = {
  0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 };

uint8_t const net_addr6_loopback[NET_ADDR_IP6_LEN] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };

/* Local variables */
static NET_IP6_CTRL ip6_control;

/* Code shortening macros */
#define ip6        (&ip6_control)

/* Local Functions */
static uint16_t ip6_get_mtu (NET_IF_CFG *net_if);
static bool     ip6_is_mcast (NET_IF_CFG *net_if, const uint8_t *ip6_addr);
#ifdef DEBUG_STDIO
 static void debug_info (const NET_IP6_HEADER *ip6_hdr);
#endif

/**
  \brief       Initialize IPv6.
  \return      none.
*/
void net_ip6_init (void) {
  /* Clear control and set default interface */
  memset (ip6, 0, sizeof(*ip6));
  ip6->DefNetIf = net_if_inet_def[1];
  DEBUGF (IP6,"Init Core, Default %s\n",ip6->DefNetIf->Name);
  EvrNetIP6_InitCore (ip6->DefNetIf->Id);
}

/**
  \brief       De-initialize IPv6.
  \return      none.
*/
void net_ip6_uninit (void) {
  DEBUGF (IP6,"Uninit Core\n");
  EvrNetIP6_UninitCore ();
  memset (ip6, 0, sizeof(*ip6));
}

/**
  \brief       Check frame IPv6 header information.
  \param[in]   net_if  network interface descriptor.
  \param[in]   frame   IPv6 frame.
  \return      check result:
               - true  = frame valid,
               - false = corrupted frame.
*/
bool net_ip6_chk_frame (NET_IF_CFG *net_if, NET_FRAME *frame) {
  uint16_t ip_len;

  DEBUGF (IP6,"*** Process_frame %s ***\n",net_if->Name);
  ip_len = ntohs(IP6_FRAME(frame)->Len);
  /* Check the frame length:                          */
  /* - minimum  >=  1 byte payload                    */
  /* - physical >= (dlen + IP6_hdr_len + eth_hdr_len) */
  if ((ip_len < 1) || (ip_len + IP6_HEADER_LEN + PHY_HEADER_LEN) > frame->length) {
    ERRORF (IP6,"Process %s, Frame too short\n",net_if->Name);
    EvrNetIP6_FrameTooShort (net_if->Id, frame->length,
                             (PHY_HEADER_LEN + IP6_HEADER_LEN + ip_len));
    return (false);
  }
  /* Check IP Header Information */
  if ((IP6_FRAME(frame)->VerClass >> 4) != 6) {
    ERRORF (IP6,"Process %s, Not IPv6\n",net_if->Name);
    EvrNetIP6_InvalidIpVersion (net_if->Id, IP6_FRAME(frame)->VerClass >> 4);
    return (false);
  }
  DEBUG_INFO (IP6_FRAME(frame));
  EvrNetIP6_ReceiveFrame (net_if->Id, frame->length - PHY_HEADER_LEN);
  EvrNetIP6_ShowFrameHeader (IP6_FRAME(frame));

  /* Check if this frame is for us */
  switch (net_if->Id & 0xFF00) {
    case NET_IF_CLASS_LOOP:
      /* Loopback address */
      sys->RxIpAddr = net_addr6_loopback;
      break;
    case NET_IF_CLASS_ETH:
    case NET_IF_CLASS_WIFI:
      if (net_addr6_is_solicited (IP6_FRAME(frame)->DstAddr, net_if->localm6)) {
        /* Solicited-node multicast address */
        break;
      }
      if (net_addr6_comp (IP6_FRAME(frame)->DstAddr, net_if->localm6->IpAddr)) {
        /* Global static unicast address */
        sys->RxIpAddr = net_if->localm6->IpAddr;
        break;
      }
      if (net_addr6_comp (IP6_FRAME(frame)->DstAddr, net_if->localm6->TempAddr)) {
        /* Global dynamic unicast address */
        sys->RxIpAddr = net_if->localm6->TempAddr;
        break;
      }
      if (net_addr6_comp (IP6_FRAME(frame)->DstAddr, net_if->localm6->LLAddr)) {
        /* Link-local unicast address */
        sys->RxIpAddr = net_if->localm6->LLAddr;
        break;
      }
      if (net_addr6_comp (IP6_FRAME(frame)->DstAddr, net_addr6_all_nodes)) {
        /* All-nodes multicast address */
        break;
      }
      /* It is not for us */
      DEBUGF (IP6," Discarded, Wrong DstAddr\n");
      EvrNetIP6_WrongDestinationAddress (net_if->Id, IP6_FRAME(frame)->DstAddr);
      return (false);
    default:
      ERRORF (IP6,"Process %s, Interface not supported\n",net_if->Name);
      EvrNetIP6_InterfaceNotSupported (net_if->Id);
      return (false);
  }

  /* Check if this is an ethernet IPv6 multicast frame */
  if (ip6_is_mcast (net_if, IP6_FRAME(frame)->DstAddr)) {
    /* Accept only UDP multicast frames */
    switch (IP6_FRAME(frame)->NextHdr) {
      case IP6_PROT_UDP:
      case IP6_PROT_FRAG:
        break;
      default:
        ERRORF (IP6,"Process %s, Wrong mcast protocol\n",net_if->Name);
        EvrNetIP6_WrongMulticastProtocol (net_if->Id, IP6_FRAME(frame)->NextHdr);
        return (false);
    }
  }

  /* Check if this packet is an IPv6 fragment */
  if ((IP6_FRAME(frame)->NextHdr == IP6_PROT_FRAG) && (ip6_get_mtu(net_if) == 0)) {
    /* Fragmented packets are disabled for this interface */
    ERRORF (IP6,"Process %s, Fragmentation disabled\n",net_if->Name);
    EvrNetIP6_FragmentationDisabled (net_if->Id);
    return (false);
  }

  /* Calculate IPv6 payload start and length */
  frame->index  = IP6_DATA_OFFS;
  frame->length = ip_len;
  return (true);
}

/**
  \brief       Reassemble incoming IPv6 fragment.
  \param[in]   frame  IPv6 fragment.
  \return      output frame:
               - NULL        = fragment taken,
               - input frame = frame not fragmented,
               - new frame   = assembled from frame fragments.
*/
NET_FRAME *net_ip6_reass_frame (NET_FRAME *frame) {

  if (IP6_FRAME(frame)->NextHdr != IP6_PROT_FRAG) {
    /* This frame is not fragmented, return it for further processing */
    return (frame);
  }
  /* Return new reassembled frame or NULL */
  return (net_ip6_frag_add (frame));
}

/**
  \brief       Construct IPv6 header and send the frame.
  \param[in]   net_if     network interface descriptor.
  \param[in]   frame      network frame.
  \param[in]   src_addr   source address.
  \param[in]   dst_addr   destination address.
  \param[in]   prot       protocol type.
  \param[in]   t_class    traffic class/packet priority.
  \param[in]   hop_limit  hop limit (0= interface default).
  \return      status:
               - true  = success,
               - false = error.
  \note        'frame->length' must be properly set to the total length of
               the upper layer payload data, before calling this function.
  \note        'TxNetif' must be selected.
*/
bool net_ip6_send_frame (NET_IF_CFG *net_if, NET_FRAME *frame, 
                         const uint8_t *src_addr, const uint8_t *dst_addr,
                         uint8_t prot, uint8_t t_class, uint8_t hop_limit) {
  uint16_t mtu;

  DEBUGF (IP6,"Send Frame %s\n",net_if->Name);
  EvrNetIP6_SendFrame (net_if->Id, prot, frame->length + IP6_HEADER_LEN);
  /* Check IP protocol type */
  switch (prot) {
    case IP6_PROT_ICMP:
    case IP6_PROT_UDP:
    case IP6_PROT_TCP:
      break;
    default:
      /* Unknown protocol */
      ERRORF (IP6,"Send %s, Unknown protocol (0x%02X)\n",net_if->Name,prot);
      EvrNetIP6_ProtocolUnknown (net_if->Id, prot);
      return (false);
  }
  if (src_addr == NULL) {
    /* Local address not provided */
    ERRORF (IP6,"Send %s, SrcAddr NULL\n",net_if->Name);
    EvrNetIP6_SourceAddressNull (net_if->Id);
    return (false);
  }
  if (dst_addr == NULL) {
    /* Destination address not provided */
    ERRORF (IP6,"Send %s, DstAddr NULL\n",net_if->Name);
    EvrNetIP6_DestinationAddressNull (net_if->Id);
    return (false);
  }
  sys->Busy = true;

  if (hop_limit == 0) {
    /* Select hop limit of the interface */
    hop_limit = net_if->localm6->HopLimit;
  }

  /* Construct the IP header */
  IP6_FRAME(frame)->VerClass  = IP6_DEF_VERCLASS | (t_class >> 4);
  IP6_FRAME(frame)->ClassFlow = (t_class << 4) & 0xFF;
  IP6_FRAME(frame)->Flow      = 0;
  IP6_FRAME(frame)->Len       = htons(frame->length);
  IP6_FRAME(frame)->NextHdr   = prot;
  IP6_FRAME(frame)->HopLim    = hop_limit;
  net_addr6_copy (IP6_FRAME(frame)->DstAddr, dst_addr);
  net_addr6_copy (IP6_FRAME(frame)->SrcAddr, src_addr);

  /* Check MTU and possible frame fragmentation */
  mtu = ip6_get_mtu (net_if);
  /* Note: frame->length = payload_length */
  for (frame->index = 0; frame->index < frame->length; ) {
    NET_FRAME *txfrm = net_ip6_frag_get (frame, mtu);
    if (txfrm == NULL) {
      /* Failed to allocate memory for tx frame */
      return (false);
    }
    DEBUG_INFO (IP6_FRAME(txfrm));
    EvrNetIP6_ShowFrameHeader (IP6_FRAME(txfrm));
    txfrm->length += (PHY_HEADER_LEN + IP6_HEADER_LEN);
    if (!net_if->send_frame (net_if->Id & 0xFF, txfrm, IP_VER6)) {
      /* Failed to send, release buffer and return */
      if (txfrm != frame) net_ip4_frag_get (NULL, 0);
      return (false);
    }
    if (txfrm == frame) {
      /* No fragments were generated, we are done */
      return (true);
    }
  }
  net_ip6_frag_get (NULL, 0);
  return (true);
}

/**
  \brief       Get MTU for the network interface.
  \param[in]   net_if  network interface descriptor.
  \return      maximum transmission unit.
*/
static uint16_t ip6_get_mtu (NET_IF_CFG *net_if) {
  switch (net_if->Id & 0xFF00) {
    case NET_IF_CLASS_ETH:
    case NET_IF_CLASS_WIFI:
      if (net_if->localm6) {
        return (net_if->localm6->Mtu);
      }
      __FALLTHROUGH;
    default:
      return (0);
  }
}

/**
  \brief       Set default network interface for internet access.
  \param[in]   net_if  network interface descriptor.
  \return      status:
               - true  = success,
               - false = error.
*/
bool net_ip6_set_netif (NET_IF_CFG *net_if) {
  if (net_if->Ip6Cfg == NULL) {
    return (false);
  }
  DEBUGF (IP6,"Set Default %s\n",net_if->Name);
  EvrNetIP6_SetDefaultInterface (net_if->Id);
  ip6->DefNetIf = net_if;
  return (true);
}

/**
  \brief       Get default internet access info.
  \return      pointer to local machine info.
*/
NET_LOCALM6 *net_ip6_def_localm (void) {
  if (ip6->DefNetIf != NULL) {
    return (ip6->DefNetIf->localm6);
  }
  return (NULL);
}

/**
  \brief       Get local IPv4 address for the route.
  \param[in]   net_if    assigned network interface.
  \param[in]   dst_addr  destination address.
  \return      local/source IPv6 address.
*/
const uint8_t *net_ip6_get_local (NET_IF_CFG *net_if, const uint8_t *dst_addr) {
  NET_LOCALM6 *lm = net_if->localm6;

  switch (net_if->Id & 0xFF00) {
    case NET_IF_CLASS_ETH:
    case NET_IF_CLASS_WIFI:
      break;
    case NET_IF_CLASS_PPP:
    case NET_IF_CLASS_SLIP:
      return (net_addr_unspec);
    default:
      return (net_addr6_loopback);
  }
  if (dst_addr[0] >= 0xFE) {
    /* Destination on-link, use link-local source address */
    return (lm->LLAddr);
  }
  /* For LAN prefer global temp IP address */
  if (!net_addr6_is_unspec (lm->TempAddr)) {
    return (lm->TempAddr);
  }
  return (lm->IpAddr);
}

/**
  \brief       Check transmit TCP,UDP,ICMP checksum offload.
  \param[in]   net_if  network interface descriptor.
  \param[in]   flag    transmit offload flag.
  \return      - true  = yes (hw checksum),
               - false = no  (sw checksum).
*/
bool net_ip6_tx_offl_hl (NET_IF_CFG *net_if, uint16_t flag) {
  switch (net_if->Id & 0xFF00) {
    case NET_IF_CLASS_ETH:
      /* Driver offload capabilities */
      break;

    case NET_IF_CLASS_LOOP:
      /* Don't calculate checksum */
      return (true);

    default:
      /* Always calculate checksum */
      return (false);
  }

  /* Check if checksum offload is supported */
  if (net_if->State->Offload & flag) {
    /* Checksum offload supported */
    return (true);
  }
  return (false);
}

/**
  \brief       Collect multicast MAC addresses for address filtering.
  \param[in]   net_if  network interface descriptor.
  \param[out]  buf     buffer to write multicast MAC array to.
  \return      number of MAC addresses written.
  \note        Return value is at least 2.
*/
uint32_t net_ip6_collect_mcast (NET_IF_CFG *net_if, uint8_t *buf) {
  NET_LOCALM6 *lm = net_if->localm6;
  uint32_t n,j;

  /* All nodes multicast (like bcast in IPv4) */
  net_addr6_to_mac (net_addr6_all_nodes, &buf[0]);
  n = 1;

  /* Solicited-node Link-local and Temp IPv6 address  */
  /* Both addresses have same (EUI-64) address suffix */
  net_addr6_to_mac (net_addr6_get_solicited (lm->LLAddr),
                    &buf[NET_ADDR_ETH_LEN]);
  if (!net_mac_comp (&buf[0], &buf[NET_ADDR_ETH_LEN])) {
    n++;
  }
  if (net_addr6_is_unspec (lm->IpAddr)) {
    /* Global IPv6 not assigned */
    return (n);
  }
  /* Solicited-node Global IPv6 address */
  net_addr6_to_mac (net_addr6_get_solicited (lm->IpAddr),
                    &buf[n * NET_ADDR_ETH_LEN]);
  for (j = 0; j < n; j++) {
    if (net_mac_comp (&buf[j * NET_ADDR_ETH_LEN], &buf[n * NET_ADDR_ETH_LEN])) {
      /* This MAC address already exists */
      return (n);
    }
  }
  n++;
  return (n);
}

/**
  \brief       Find route to destination IPv6 address.
  \param[in]   net_if    proposed network interface.
  \param[in]   dst_addr  destination IPv6 address.
  \return      assigned network interface for the route.
*/
NET_IF_CFG *net_ip6_find_route (NET_IF_CFG *net_if, const uint8_t *dst_addr) {
  NET_IF_CFG *const *p;

  (void)p;
  /* Check loopback address */
  if (dst_addr[0] == 0x00) {
    /* IPv6 prefix "0000::/8" */
    if (net_addr6_comp (dst_addr, net_addr6_loopback)) {
      /* Localhost IP "::1" */
      return (&net_loop_if_config);
    }
    /* Ignore other loopback addresses */
    return (NULL);
  }
  /* Check link-local or multicast address */
  if (dst_addr[0] >= 0xFC) {
    if (net_if == NULL) {
      /* Use default link interface */
      return (net_if_link_def[1]);
    }
    if ((net_if->Ip6Cfg != NULL) && (net_if->output_lan != NULL)) {
      /* Use proposed LAN interface */
      return (net_if);
    }
    /* Ignore for non-LAN interfaces */
    return (NULL);
  }
  /* Address is external */
  return (ip6->DefNetIf);
}

/**
  \brief       Check if IP address is multicast address.
  \param[in]   net_if    network interface descriptor.
  \param[in]   ip6_addr  IPv6 address.
  \return      address status:
               - true  = is IPv6 multicast,
               - false = not multicast.
*/
static bool ip6_is_mcast (NET_IF_CFG *net_if, const uint8_t *ip6_addr) {
  if (net_if->output_lan == NULL) {
    return (false);
  }
  // Todo: Add MLD support
  (void)ip6_addr;
  return (false);
}

/**
  \brief       Calculate IPv6 checksum with pseudo header.
  \param[in]   src_addr  source address.
  \param[in]   dst_addr  destination address.
  \param[in]   payload   pointer to payload data.
  \param[in]   next_hdr  upper layer protocol type.
  \param[in]   len       payload length.
  \return      16-bit checksum.
*/
uint16_t net_ip6_chksum (const uint8_t *src_addr, const uint8_t *dst_addr,
                         const void *payload, uint8_t next_hdr, int32_t len) {
  uint32_t sum;
  int32_t i;

  /* Calculate pseudo header checksum */
  sum  = next_hdr;
  sum += (uint16_t)(len >> 16);
  sum += (uint16_t)(len & 0xFFFF);

  for (i = 0; i < NET_ADDR_IP6_LEN; i += 2) {
    sum += ((uint32_t)src_addr[i] << 8 | src_addr[i+1]);
    sum += ((uint32_t)dst_addr[i] << 8 | dst_addr[i+1]);
  }

  /* Calculate payload data checksum */
  return (net_chksum_buf (payload, (uint32_t)len, sum));
}

#ifdef DEBUG_STDIO
/**
  \brief       Debug print IPv6 header information.
  \param[in]   ip6_hdr  IPv6 frame header.
  \return      none.
*/
static void debug_info (const NET_IP6_HEADER *ip6_hdr) {
  static const char p_asc[][5] = {
    "ICMP",
    "UDP",
    "TCP",
    "FRAG"
  };
  static const uint8_t p_bin[] = {
    IP6_PROT_ICMP,
    IP6_PROT_UDP,
    IP6_PROT_TCP,
    IP6_PROT_FRAG
  };
  uint32_t i;

  DEBUGF (IP6," SrcAddr [%s]\n",net_addr6_ntoa(ip6_hdr->SrcAddr));
  DEBUGF (IP6," DstAddr [%s]\n",net_addr6_ntoa(ip6_hdr->DstAddr));
  /* Print user friendly IP protocols */
  for (i = 0; i < sizeof (p_bin); i++) {
    if (p_bin[i] == ip6_hdr->NextHdr) {
      DEBUGF (IP6," NextHdr %s, HopLimit=%d\n",p_asc[i],ip6_hdr->HopLim);
      goto d;
    }
  }
  DEBUGF (IP6," NextHdr (0x%02X), HopLimit=%d\n",ip6_hdr->NextHdr,ip6_hdr->HopLim);
d:DEBUGF (IP6," Length %d bytes\n",ntohs(ip6_hdr->Len));
}
#endif
