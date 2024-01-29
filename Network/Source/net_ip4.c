/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ip4.c
 * Purpose: Internet Protocol Version 4
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_mem.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_loopback.h"
#include "net_igmp.h"
#include "net_icmp.h"
#include "net_ip4.h"
#include "net_ip4_frag.h"
#include "net_udp.h"
#include "net_tcp.h"
#include "net_dbg.h"

/* Global constants */
uint8_t const net_addr4_loopback[NET_ADDR_IP4_LEN] = { 127, 0, 0, 1 };

/* Local variables */
static NET_IP4_CTRL ip4_control;

/* Code shortening macros */
#define ip4        (&ip4_control)

/* Local Functions */
static uint16_t ip4_get_mtu (NET_IF_CFG *net_if);
static bool     ip4_tx_offl_ip (NET_IF_CFG *net_if);
static bool     ip4_is_subcast (NET_IF_CFG *net_if, const uint8_t *ip4_addr);
static uint16_t ip4_get_chksum (NET_IP4_HEADER *ip4_hdr);
#ifdef DEBUG_STDIO
 static void debug_info (const NET_IP4_HEADER *ip4_hdr);
#endif

/**
  \brief       Initialize IPv4.
  \return      none.
*/
void net_ip4_init (void) {
  /* Clear control and set default interface */
  memset (ip4, 0, sizeof(*ip4));
  ip4->DefNetIf = net_if_inet_def[0];
  DEBUGF (IP4,"Init Core, Default %s\n",ip4->DefNetIf->Name);
  EvrNetIP4_InitCore (ip4->DefNetIf->Id);
}

/**
  \brief       De-initialize IPv4.
  \return      none.
*/
void net_ip4_uninit (void) {
  DEBUGF (IP4,"Uninit Core\n");
  EvrNetIP4_UninitCore ();
  memset (ip4, 0, sizeof(*ip4));
}

/**
  \brief       Check frame IPv4 header information.
  \param[in]   net_if  network interface descriptor.
  \param[in]   frame   IPv4 frame.
  \return      check result:
               - true  = frame valid,
               - false = invalid frame.
*/
bool net_ip4_chk_frame (NET_IF_CFG *net_if, NET_FRAME *frame) {
  uint16_t hlen, dlen, flags;

  DEBUGF (IP4,"*** Process_frame %s ***\n",net_if->Name);
  dlen = ntohs(IP4_FRAME(frame)->Len);
  /* Check the frame length:                                         */
  /* - minimum length  >= (IP header length + 1 byte payload)        */
  /* - physical length >= (IP frame length + ethernet header length) */
  if ((dlen < (IP4_HEADER_LEN+1)) || (dlen + PHY_HEADER_LEN) > frame->length) {
    /* Frame too short, at least 1 byte of IP-payload is required */
    ERRORF (IP4,"Process %s, Frame too short\n",net_if->Name);
    EvrNetIP4_FrameTooShort (net_if->Id, frame->length, (PHY_HEADER_LEN + dlen));
    return (false);
  }
  /* Check IP Header Information if it is IPv4 */
  if ((IP4_FRAME(frame)->VerHLen >> 4) != 4) {
    ERRORF (IP4,"Process %s, Not IPv4\n",net_if->Name);
    EvrNetIP4_InvalidIpVersion (net_if->Id, IP4_FRAME(frame)->VerHLen >> 4);
    return (false);
  }

  DEBUG_INFO (IP4_FRAME(frame));
  EvrNetIP4_ReceiveFrame (net_if->Id, frame->length - PHY_HEADER_LEN);
  EvrNetIP4_ShowFrameHeader (IP4_FRAME(frame));

  /* Check Rx interface */
  if (net_if->Id == NET_IF_CLASS_LOOP) {
    /* Loopback interface, skip address checking */
    /*  (SrcAddr=127.0.0.1, DstAddr=127.0.0.1)   */
    sys->RxIpAddr = net_addr4_loopback;
    goto dst_valid;
  }

  /* Check if source IP address is valid               */
  /* The following source addresses are not valid:     */
  /*   a) 224.0.0.0/4     - Multicast range (Class C)  */
  /*   b) 240.0.0.0/4     - Reserved range  (Class D)  */
  /*   c) 255.255.255.255 - Limited broadcast          */
  /*   d) x.x.x.255       - Directed broadcast (subnet)*/
  /*   e) 127.x.x.x       - Internal host loopback     */
  /*                 (RFC1700 page 3, RFC5735 page 5)  */
  /*   f) SrcAddr==DstAddr - used in TCP LAND attack   */
  if (((IP4_FRAME(frame)->SrcAddr[0] & 0xF0) >= 0xE0) ||
       (IP4_FRAME(frame)->SrcAddr[0]         == 0x7F) ||
      net_addr4_comp (IP4_FRAME(frame)->SrcAddr, IP4_FRAME(frame)->DstAddr) ||
      ip4_is_subcast (net_if, IP4_FRAME(frame)->SrcAddr)) {
    ERRORF (IP4,"Process %s, SrcAddr invalid\n",net_if->Name);
    EvrNetIP4_SourceIpAddressInvalid (net_if->Id, IP4_FRAME(frame)->SrcAddr);
    return (false);
  }

  /* Check if destination IP address is valid */
  if (net_addr4_comp (IP4_FRAME(frame)->DstAddr, net_if->localm->IpAddr)) {
    /* Unicast address */
    sys->RxIpAddr = net_if->localm->IpAddr;
    goto dst_valid;
  }
  if (net_addr4_comp (IP4_FRAME(frame)->DstAddr, net_addr_bcast)) {
    /* Broadcast address */
    goto dst_valid;
  }
  if (net_if->output_lan == NULL) {
    /* Non-LAN interface (PPP, SLIP) */
    goto wrong_dst;
  }
  /* Check ethernet multicast address */
  if ((IP4_FRAME(frame)->DstAddr[0] & 0xF0) == 0xE0) {
    /* Multicast address (224.0.0.0 - 239.255.255.255) */
    if (!net_igmp_is_member (net_if, IP4_FRAME(frame)->DstAddr)) {
      /* Not a member of any group */
      goto wrong_dst;
    }
    switch (IP4_FRAME(frame)->Prot) {
      case IP4_PROT_IGMP:
      case IP4_PROT_UDP :
        /* Only IGMP and UDP accepted */
        goto dst_valid;
    }
    ERRORF (IP4,"Process %s, Wrong mcast protocol\n",net_if->Name);
    EvrNetIP4_WrongMulticastProtocol (net_if->Id, IP4_FRAME(frame)->Prot);
    return (false);
  }
  if (ip4_is_subcast (net_if, IP4_FRAME(frame)->DstAddr)) {
    /* Subnet broadcast address */
    if (IP4_FRAME(frame)->Prot == IP4_PROT_UDP) {
      /* Only UDP accepted */
      goto dst_valid;
    }
    ERRORF (IP4,"Process %s, Wrong bcast protocol\n",net_if->Name);
    EvrNetIP4_WrongBroadcastProtocol (net_if->Id, IP4_FRAME(frame)->Prot);
    return (false);
  }
  /* Check if link-layer addressed UDP message */
  if ((sys->Flags & SYS_FLAG_LINKADDR) &&
      (IP4_FRAME(frame)->Prot == IP4_PROT_UDP)) {
    /* Accept the message for DHCP [RFC2131 page 24] */
    sys->Flags |= SYS_FLAG_LLA_UDP;
  }
  else {
wrong_dst:
    DEBUGF (IP4," Discarded, Wrong DstAddr\n");
    EvrNetIP4_WrongDestinationAddress (net_if->Id, IP4_FRAME(frame)->DstAddr);
    return (false);
  }

dst_valid:
  /* Is the IP header checksum OK? */
  if (!(sys->RxOffload & SYS_OFFL_IP4_RX) &&
       (ip4_get_chksum (IP4_FRAME(frame)) != 0)) {
    ERRORF (IP4,"Process %s, Checksum failed\n",net_if->Name);
    EvrNetIP4_ChecksumFailed (net_if->Id, (IP4_FRAME(frame)->VerHLen & 0x0F) * 4);
    return (false);
  }

  /* Is this packet an IP-fragment? */
  flags = ntohs(IP4_FRAME(frame)->FragOffs);
  if (flags & (IP4_MF_FLAG | IP4_FRAG_MASK)) {
    /* This packet is a fragment */
    if (flags & IP4_DF_FLAG) {
      /* Don't-Fragment flag should not be set in fragments */
      ERRORF (IP4,"Process %s, DF-flag set\n",net_if->Name);
      EvrNetIP4_FragmentDfFlagSet (net_if->Id, flags);
      return (false);
    }
    if (ip4_get_mtu(net_if) == 0) {
      /* Fragmented packets are disabled for this interface */
      ERRORF (IP4,"Process %s, Fragmentation disabled\n",net_if->Name);
      EvrNetIP4_FragmentationDisabled (net_if->Id);
      return (false);
    }
  }

  /* Calculate payload data start and length */
  hlen = (IP4_FRAME(frame)->VerHLen & 0x0F) * 4;
  frame->index  = PHY_HEADER_LEN + hlen;
  frame->length = dlen - hlen;
  return (true);
}

/**
  \brief       Reassemble incoming IPv4 fragment.
  \param[in]   frame  IPv4 fragment.
  \return      output frame:
               - NULL        = fragment taken,
               - input frame = frame not fragmented,
               - new frame   = assembled from frame fragments.
*/
NET_FRAME *net_ip4_reass_frame (NET_FRAME *frame) {

  if (!(IP4_FRAME(frame)->FragOffs & HTONS(IP4_MF_FLAG | IP4_FRAG_MASK))) {
    /* This frame is not fragmented, return it for further processing */
    return (frame);
  }
  /* Return new reassembled frame or NULL */
  return (net_ip4_frag_add (frame));
}

/**
  \brief       Construct IPv4 header and send the frame.
  \param[in]   net_if    network interface descriptor.
  \param[in]   frame     network frame.
  \param[in]   dst_addr  destination address.
  \param[in]   prot      protocol type.
  \param[in]   tos       type of service (tos).
  \param[in]   ttl       time to live (ttl).
  \return      status:
               - true  = success,
               - false = error.
  \note        'frame->length' must be properly set to the total length of
               the upper layer payload data, before calling this function.
*/
bool net_ip4_send_frame (NET_IF_CFG *net_if,
                         NET_FRAME *frame, const uint8_t *dst_addr,
                         uint8_t prot, uint8_t tos, uint8_t ttl) {
  uint16_t mtu;

  DEBUGF (IP4,"Send Frame %s\n",net_if->Name);
  EvrNetIP4_SendFrame (net_if->Id, prot, frame->length);
  /* Check IP protocol type */
  switch (prot) {
    case IP4_PROT_ICMP:
    case IP4_PROT_IGMP:
    case IP4_PROT_UDP:
    case IP4_PROT_TCP:
      break;
    default:
      /* Unknown protocol */
      ERRORF (IP4,"Send %s, Unknown Protocol (0x%02X)\n",net_if->Name,prot);
      EvrNetIP4_ProtocolUnknown (net_if->Id, prot);
      return (false);
  }
  if (dst_addr == NULL) {
    /* Destination address not provided */
    ERRORF (IP4,"Send %s, DstAddr NULL\n",net_if->Name);
    EvrNetIP4_DestinationAddressNull (net_if->Id);
    return (false);
  }
  sys->Busy = true;

  /* Construct the IP header */
  if ((prot == IP4_PROT_IGMP) && (frame->index == IP4_DATA_OFFS + 4)) {
    /* Add IP Router Alert option for IGMPv2 */
    IP4_FRAME(frame)->VerHLen    = IP4_DEF_VERHLEN + 1;
    frame->data[IP4_DATA_OFFS+0] = 0x94;
    frame->data[IP4_DATA_OFFS+1] = 0x04;
    frame->data[IP4_DATA_OFFS+2] = 0;
    frame->data[IP4_DATA_OFFS+3] = 0;
    frame->length += IP4_HEADER_LEN + 4;
  }
  else {
    /* No IP options for all other protocols */
    IP4_FRAME(frame)->VerHLen = IP4_DEF_VERHLEN;
    frame->length += IP4_HEADER_LEN;
  }
  IP4_FRAME(frame)->Len     = htons(frame->length);
  IP4_FRAME(frame)->Tos     = tos;
  IP4_FRAME(frame)->Id      = htons(ip4->Id);

  /* Do not use post-increment in htons(ip_id++) */
  ip4->Id++;

  IP4_FRAME(frame)->FragOffs= (prot == IP4_PROT_TCP) ? HTONS(IP4_DF_FLAG) : 0;
  IP4_FRAME(frame)->Ttl     = ttl;
  IP4_FRAME(frame)->Prot    = prot;
  IP4_FRAME(frame)->Chksum  = 0;
  net_addr4_copy (IP4_FRAME(frame)->SrcAddr, net_ip4_get_local (net_if, dst_addr));
  net_addr4_copy (IP4_FRAME(frame)->DstAddr, dst_addr);

  /* Check MTU and possible frame fragmentation */
  mtu = ip4_get_mtu (net_if);
  /* Note: frame->length = IP4_header_length + payload_length */
  for (frame->index = 0; frame->index < (frame->length - IP4_HEADER_LEN); ) {
    NET_FRAME *txfrm = net_ip4_frag_get (frame, mtu);
    if (txfrm == NULL) {
      /* Failed to allocate memory for tx frame */
      return (false);
    }
    DEBUG_INFO (IP4_FRAME(txfrm));
    EvrNetIP4_ShowFrameHeader (IP4_FRAME(txfrm));
    /* Calculate checksum for the IP header */
    if (!ip4_tx_offl_ip (net_if)) {
      IP4_FRAME(txfrm)->Chksum = htons(ip4_get_chksum (IP4_FRAME(txfrm)));
    }
    txfrm->length += PHY_HEADER_LEN;
    if (!net_if->send_frame (net_if->Id & 0xFF, txfrm, IP_VER4)) {
      /* Failed to send, release buffer and return */
      if (txfrm != frame) net_ip4_frag_get (NULL, 0);
      return (false);
    }
    if (txfrm == frame) {
      /* No fragments were generated, we are done */
      return (true);
    }
  }
  net_ip4_frag_get (NULL, 0);
  return (true);
}

/**
  \brief       Get MTU for the network interface.
  \param[in]   net_if  network interface descriptor.
  \return      maximum transmission unit.
*/
static uint16_t ip4_get_mtu (NET_IF_CFG *net_if) {
  switch (net_if->Id & 0xFF00) {
    case NET_IF_CLASS_ETH:
    case NET_IF_CLASS_WIFI:
    case NET_IF_CLASS_PPP:
    case NET_IF_CLASS_SLIP:
      if (net_if->localm) {
        return (net_if->localm->Mtu);
      }
      __FALLTHROUGH;
    default:
      return (0);
  }
}

/**
  \brief       Set default interface for internet access.
  \param[in]   net_if  network interface descriptor.
  \return      status:
               - true  = success,
               - false = error.
*/
bool net_ip4_set_netif (NET_IF_CFG *net_if) {
  if (net_if->Ip4Cfg == NULL) {
    return (false);
  }
  DEBUGF (IP4,"Set Default %s\n",net_if->Name);
  EvrNetIP4_SetDefaultInterface (net_if->Id);
  ip4->DefNetIf = net_if;
  return (true);
}

/**
  \brief       Get default internet access info.
  \return      pointer to local machine info.
*/
NET_LOCALM *net_ip4_def_localm (void) {
  if (ip4->DefNetIf != NULL) {
    return (ip4->DefNetIf->localm);
  }
  return (NULL);
}

/**
  \brief       Check transmit IP checksum offload.
  \param[in]   net_if  network interface descriptor.
  \return      - true  = yes (hw checksum),
               - false = no  (sw checksum).
*/
static bool ip4_tx_offl_ip (NET_IF_CFG *net_if) {
  switch (net_if->Id & 0xFF00) {
    case NET_IF_CLASS_ETH:
      if (net_if->State->Offload & SYS_OFFL_IP4_TX) {
        /* Checksum offload supported */
        return (true);
      }
      break;

    case NET_IF_CLASS_LOOP:
      /* Don't calculate checksum */
      return (true);

    default:
      /* Always calculate checksum */
      return (false);
  }
  return (false);
}

/**
  \brief       Check transmit TCP,UDP,ICMP checksum offload.
  \param[in]   net_if  network interface descriptor.
  \param[in]   flag    transmit offload flag.
  \param[in]   len     length of IP datagram payload.
  \return      - true  = yes (hw checksum),
               - false = no  (sw checksum).
*/
bool net_ip4_tx_offl_hl (NET_IF_CFG *net_if, uint8_t flag, uint32_t len) {
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
    /* Check if IP fragmentation is needed */
    uint16_t mtu = net_if->localm->Mtu;
    if ((mtu == 0) || (mtu >= (IP4_HEADER_LEN + len))) {
      /* No IP fragmentation, checksum offload works */
      return (true);
    }
  }
  return (false);
}

/**
  \brief       Find route to destination IPv4 address.
  \param[in]   net_if    proposed network interface.
  \param[in]   dst_addr  destination IPv4 address.
  \return      assigned network interface for the route.
*/
NET_IF_CFG *net_ip4_find_route (NET_IF_CFG *net_if, const uint8_t *dst_addr) {
  NET_IF_CFG *const *p;

  /* Check loopback address */
  if (dst_addr[0] == 127) {
    /* Range: 127.0.0.0 - 127.255.255.255 */
    if (net_addr4_comp (dst_addr, net_addr4_loopback)) {
      /* Localhost IP 127.0.0.1 */
      return (&net_loop_if_config);
    }
    /* Ignore other loopback addresses */
    return (NULL);
  }
  /* Check broadcast address */
  if (net_addr4_comp (dst_addr, net_addr_bcast)) {
    if (net_if == NULL) {
      /* Use default link interface */
      return (net_if_link_def[0]);
    }
    if ((net_if->Ip4Cfg != NULL) && (net_if->output_lan != NULL)) {
      /* Use proposed LAN interface */
      return (net_if);
    }
    /* Ignore for non-LAN interfaces */
    return (NULL);
  }
  /* Check proposed network interface */
  if ((net_if != NULL) && net_ip4_is_onlink (net_if, dst_addr)) {
    /* Ok, address is on this link */
    return (net_if);
  }
  /* Check remaining network interfaces */
  for (p = net_if_list_all; *p; p++) {
    if ((*p != net_if) && net_ip4_is_onlink (*p, dst_addr)) {
      /* Ok, address is local */
      return (*p);
    }
  }
  /* Address is external */
  return (ip4->DefNetIf);
}

/**
  \brief       Get local IPv4 address for the route.
  \param[in]   net_if    assigned network interface.
  \param[in]   dst_addr  destination address.
  \return      local/source IPv4 address.
*/
const uint8_t *net_ip4_get_local (NET_IF_CFG *net_if, const uint8_t *dst_addr) {
  (void)dst_addr;

  switch (net_if->Id & 0xFF00) {
    case NET_IF_CLASS_ETH:
    case NET_IF_CLASS_WIFI:
    case NET_IF_CLASS_PPP:
    case NET_IF_CLASS_SLIP:
      return (net_if->localm->IpAddr);
    default:
      return (net_addr4_loopback);
  }
}

/**
  \brief       Check if IPv4 address is interface-local.
  \param[in]   net_if    network interface descriptor.
  \param[in]   ip4_addr  IPv4 address to check.
  \return      address status:
               - true  = local/onlink,
               - false = global.
*/
bool net_ip4_is_onlink (NET_IF_CFG *net_if, const uint8_t *ip4_addr) {
  uint32_t ip,ip_lm,net_mask;

  if ((net_if == NULL) || (net_if->localm == NULL)) {
    /* IPv4 not enabled for interface */
    return (false);
  }
  ip_lm    = __ALIGNED_UINT32(net_if->localm->IpAddr);
  net_mask = __ALIGNED_UINT32(net_if->localm->NetMask);
  /* ip4_addr may not be aligned */
  memcpy (&ip, ip4_addr, NET_ADDR_IP4_LEN);
  if (((ip ^ ip_lm) & net_mask) == 0) {
    return (true);
  }
  return (false);
}

/**
  \brief       Check if IP address is a subnet broadcast address.
  \param[in]   net_if    network interface descriptor.
  \param[in]   ip4_addr  IPv4 address to check.
  \return      address status:
               - true  = is subnet bcast,
               - false = not subnet bcast.
*/
static bool ip4_is_subcast (NET_IF_CFG *net_if, const uint8_t *ip4_addr) {
  if (net_if->output_lan == NULL) {
    /* Not possible on non-LAN interface */
    return (false);
  }
  return (net_addr4_is_subcast (ip4_addr, net_if->localm));
}

/**
  \brief       Calculate IPv4 header checksum.
  \param[in]   ip4_hdr  pointer to IPv4 header.
  \return      16-bit checksum.
*/
static uint16_t ip4_get_chksum (NET_IP4_HEADER *ip4_hdr) {
  uint32_t len;

  /* Total Length of the IP header + Options in words */
  len = (ip4_hdr->VerHLen & 0x0f) * 4;
  return (net_chksum_buf (ip4_hdr, len, 0));
}


/**
  \brief       Calculate IPv4 buffer checksum.
  \param[in]   buf  input buffer.
  \param[in]   len  length of data.
  \return      16-bit checksum.
*/
uint16_t net_ip4_chksum_buf (const void *buf, int32_t len) {
  return (net_chksum_buf (buf, (uint32_t)len, 0));
}

/**
  \brief       Calculate IPv4 checksum with pseudo header.
  \param[in]   src_addr  source address.
  \param[in]   dst_addr  destination address.
  \param[in]   payload   pointer to payload data.
  \param[in]   proto     upper layer protocol type.
  \param[in]   len       payload length.
  \return      16-bit checksum.
*/
uint16_t net_ip4_chksum (const uint8_t *src_addr, const uint8_t *dst_addr,
                         const void *payload, uint8_t proto, int32_t len) {
  uint32_t sum;
  int32_t i;

  /* Calculate pseudo header checksum */
  sum = proto + (uint32_t)len;

  for (i = 0; i < NET_ADDR_IP4_LEN; i += 2) {
    sum += ((uint32_t)src_addr[i] << 8 | src_addr[i+1]);
    sum += ((uint32_t)dst_addr[i] << 8 | dst_addr[i+1]);
  }

  /* Calculate payload data checksum */
  return (net_chksum_buf (payload, (uint32_t)len, sum));
}

#ifdef DEBUG_STDIO
/**
  \brief       Debug print IPv4 header information.
  \param[in]   ip4_hdr  IPv4 frame header.
  \return      none.
*/
static void debug_info (const NET_IP4_HEADER *ip4_hdr) {
  static const char p_asc[][5] = {
    "ICMP",
    "IGMP",
    "UDP",
    "TCP"
  };
  static const uint8_t p_bin[] = {
    IP4_PROT_ICMP,
    IP4_PROT_IGMP,
    IP4_PROT_UDP,
    IP4_PROT_TCP
  };
  static const char flg[][3] = {
    "MF",
    "DF",
    "RF"
  };
  char buf[12];
  uint32_t i,n,frag;

  DEBUGF (IP4," SrcAddr %s\n",net_addr4_ntoa(ip4_hdr->SrcAddr));
  DEBUGF (IP4," DstAddr %s\n",net_addr4_ntoa(ip4_hdr->DstAddr));

  /* Print user friendly IP protocols */
  for (i = 0; i < sizeof (p_bin); i++) {
    if (p_bin[i] == ip4_hdr->Prot) {
      DEBUGF (IP4," Proto %s, Id=0x%04X\n",p_asc[i],ntohs(ip4_hdr->Id));
      goto d;
    }
  }
  DEBUGF (IP4," Proto (0x%02X), Id=0x%04X\n",ip4_hdr->Prot,ntohs(ip4_hdr->Id));
d:frag = ntohs(ip4_hdr->FragOffs);

  /* Print user friendly IP flags */
  for (i = n = 0; i < sizeof(flg)/sizeof(flg[0]); i++) {
    if (frag & (IP4_MF_FLAG << i)) {
      /* Append IP flag string */
      if (n) buf[n++] = ',';
      buf[n++] = flg[i][0];
      buf[n++] = flg[i][1];
    }
  }
  if (!n) buf[n++] = '-';
  /* Add null-termination */
  buf[n] = 0;

  frag &= IP4_FRAG_MASK;
  DEBUGF (IP4," Flags [%s] Offs=%d, Ttl=%d\n",buf,frag*8,ip4_hdr->Ttl);
  DEBUGF (IP4," Length %d bytes\n",ntohs(ip4_hdr->Len));
}
#endif
