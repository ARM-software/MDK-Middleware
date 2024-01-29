/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_icmp6.c
 * Purpose: Internet Control Message Protocol Version 6
 *----------------------------------------------------------------------------*/

#include "rl_net_lib.h"
#include <string.h>
#include "net_sys.h"
#include "net_common.h"
#include "net_mem.h"
#include "net_addr.h"
#include "net_ip6.h"
#include "net_icmp.h"
#include "net_icmp6.h"
#include "net_ping.h"
#include "net_ndp.h"
#include "net_dbg.h"

/* Global variables */
NET_ICMP6_CTRL net_eth0_icmp6_control;
NET_ICMP6_CTRL net_eth1_icmp6_control;
NET_ICMP6_CTRL net_wifi0_icmp6_control;
NET_ICMP6_CTRL net_wifi1_icmp6_control;

/* Code shortening macros */
#define ctrl       (h->Ctrl)

/* Local Functions */
#ifdef DEBUG_STDIO
 static void debug_info (const NET_FRAME *frm);
#endif

/**
  \brief       Initialize ICMP6 protocol.
  \return      none.
*/
void net_icmp6_init (void) {
  NET_ICMP6_CFG *const *p;

  /* Set NoEcho control for network interfaces */
  for (p = &net_icmp6_list[0]; *p; p++) {
    (*p)->Ctrl->NoEcho = (*p)->NoEcho;
  }
}

/**
  \brief       Enable or disable ICMPv6 Echo response.
  \param[in]   if_id    interface identifier.
  \param[in]   no_echo  new state of NoEcho attribute:
                        - true=  disable echo response,
                        - false= enable echo response.
  \return      status code as defined with netStatus.
*/
netStatus netICMP6_SetNoEcho (uint32_t if_id, bool no_echo) {
  NET_IF_CFG *net_if = net_if_map_all (if_id);
  NET_ICMP6_CFG *h;

  if (!(net_if && net_if->Ip6Cfg)) {
    return (netInvalidParameter);
  }
  h = net_if->Ip6Cfg->Icmp6Cfg;
  ctrl->NoEcho = no_echo;
  return (netOK);
}

/**
  \brief       Process received ICMP6 message.
  \param[in]   net_if  network interface descriptor.
  \param[in]   frame   network frame.
  \return      status:
               - true  = success,
               - false = error.
*/
bool net_icmp6_process (NET_IF_CFG *net_if, NET_FRAME *frame) {
  NET_FRAME *txfrm;

  DEBUGF (ICMP6,"*** Process_frame %s ***\n",net_if->Name);
  if (frame->length < ICMP_HEADER_LEN) {
    ERRORF (ICMP6,"Process %s, Frame too short\n",net_if->Name);
    EvrNetICMP6_FrameTooShort (net_if->Id, frame->length, ICMP_HEADER_LEN);
    return (false);
  }
  /* Check checksum for received frame */
  if (!(sys->RxOffload & SYS_OFFL_ICMP6_RX) &&
       net_ip6_chksum (IP6_FRAME(frame)->SrcAddr, IP6_FRAME(frame)->DstAddr,
                       ICMP6_FRAME(frame), IP6_PROT_ICMP, frame->length) != 0) {
    ERRORF (ICMP6,"Process %s, Checksum failed\n",net_if->Name);
    EvrNetICMP6_ChecksumFailed (net_if->Id, frame->length);
    return (false);
  }
  DEBUG_INFO (frame);
  EvrNetICMP6_ReceiveFrame (net_if->Id, frame->length);
  EvrNetICMP6_ShowFrameHeader (ICMP6_FRAME(frame));

  switch (ICMP6_FRAME(frame)->Type) {
    case ICMP6_ECHO_REQ:
      /* Echo Request from remote host */
      if (ICMP6_FRAME(frame)->Code != 0) {
        ERRORF (ICMP6,"Process %s, Wrong EchoReq code\n",net_if->Name);
        EvrNetICMP6_EchoRequestWrongCode (net_if->Id, ICMP6_FRAME(frame)->Code, 0);
        return (false);
      }
      DEBUGF (ICMP6," EchoReq received\n");
      EvrNetICMP6_EchoRequestReceived (net_if->Id, frame->length-ICMP_HEADER_LEN-4);

      /* Check if echo reply allowed */
      if (!net_if->Ip6Cfg || net_if->Ip6Cfg->Icmp6Cfg->Ctrl->NoEcho) {
        DEBUGF (ICMP6," Discarded, %s:NoEcho\n",net_if->Name);
        EvrNetICMP6_EchoReplyDisabled (net_if->Id);
        return (false);
      }

      /* Reply to ICMP6 request */
      txfrm = net_mem_alloc (IP6_DATA_OFFS + frame->length);
      ICMP6_FRAME(txfrm)->Type   = ICMP6_ECHO_REPLY;
      ICMP6_FRAME(txfrm)->Code   = 0;
      ICMP6_FRAME(txfrm)->Chksum = 0;

      /* Now copy the received ICMP6 frame data */
      memcpy (&ICMP6_FRAME(txfrm)->Data[0],
              &ICMP6_FRAME(frame)->Data[0], frame->length - ICMP_HEADER_LEN);

      /* Response goes to the same network interface */
      if (!(sys->RxOffload & SYS_OFFL_ICMP6_TX)) {
        /* Calculate checksum for packet to be sent */
        uint16_t cks =
          net_ip6_chksum (IP6_FRAME(frame)->DstAddr, IP6_FRAME(frame)->SrcAddr,
                          ICMP6_FRAME(txfrm), IP6_PROT_ICMP, frame->length);
        ICMP6_FRAME(txfrm)->Chksum = htons(cks);
      }
      DEBUGF (ICMP6,"Send_EchoReply\n");
      /* Reply with the same frame data size */
      txfrm->length = frame->length;
      DEBUG_INFO (txfrm);
      EvrNetICMP6_SendEchoReply (net_if->Id);
      EvrNetICMP6_ShowFrameHeader (ICMP6_FRAME(txfrm));
      net_ip6_send_frame (net_if, txfrm,
                          IP6_FRAME(frame)->DstAddr,
                          IP6_FRAME(frame)->SrcAddr, IP6_PROT_ICMP, 0, 128);
      net_mem_free (txfrm);
      break;

    case ICMP6_ECHO_REPLY:
      /* Echo Reply to our ping */
      DEBUGF (ICMP6," EchoReply received\n");
      EvrNetICMP6_EchoReplyReceived (net_if->Id, frame->length-ICMP_HEADER_LEN-4);
      if (!(ping->Flags & PING_FLAG_BUSY)) {
        ERRORF (ICMP6,"Process %s, EchoReply in wrong state\n",net_if->Name);
        EvrNetICMP6_EchoReplyWrongState (net_if->Id);
        return (false);
      }
      if (ICMP6_FRAME(frame)->Code != 0) {
        ERRORF (ICMP6,"Process %s, EchoReply wrong code\n",net_if->Name);
        EvrNetICMP6_EchoReplyWrongCode (net_if->Id, ICMP6_FRAME(frame)->Code, 0);
        return (false);
      }
      if (!net_addr6_comp (IP6_FRAME(frame)->SrcAddr, ping->Host.addr)) {
        ERRORF (ICMP6,"Process %s, EchoRely wrong host IP\n",net_if->Name);
        EvrNetICMP6_EchoReplyWrongIpAddress (net_if->Id, IP6_FRAME(frame)->SrcAddr);
        return (false);
      }
      if (ntohs(ECHO6_FRAME(frame)->Id) != ping->Id) {
        ERRORF (ICMP6,"Process %s, EchoRely Wrong Id\n",net_if->Name);
        EvrNetICMP6_EchoReplyWrongId (net_if->Id, ntohs(ECHO6_FRAME(frame)->Id), ping->Id);
        return (false);
      }
      if ((frame->length != ICMP_HEADER_LEN + 22) ||
          memcmp (ECHO6_FRAME(frame)->Data, net_ping_payload, 18) != 0) {
        ERRORF (ICMP6,"Process %s, Wrong EchoReply payload\n",net_if->Name);
        EvrNetICMP6_EchoReplyWrongPayload (net_if->Id);
        return (false);
      }
      ping->cb_func (netPing_EventSuccess);
      ping->Flags = 0x00;
      break;

    case ICMP6_ROUTER_SOL:
      /* Router Solicitation message */
      /* Hosts must silently discard RS messages [RFC4861 page 38] */
      DEBUGF (ICMP6," Message discarded\n");
      EvrNetICMP6_MessageDiscarded (net_if->Id, ICMP6_FRAME(frame)->Type);
      break;

    case ICMP6_ROUTER_ADVER:
      /* Router Advertisement message */
    case ICMP6_NEIGHB_SOL:
      /* Neighbor Solicitation message */
    case ICMP6_NEIGHB_ADVER:
      /* Neighbor Advertisement message */
      if (net_if->output_lan == NULL) {
        /* Silently ignore for non-LAN interfaces */
        break;
      }
      if (IP6_FRAME(frame)->HopLim < 255) {
        ERRORF (ICMP6,"Discover %s, Wrong HopLimit\n",net_if->Name);
        EvrNetICMP6_WrongHopLimit (net_if->Id, IP6_FRAME(frame)->HopLim, 255);
        return (false);
      }
      if (ICMP6_FRAME(frame)->Code != 0) {
        ERRORF (ICMP6,"Discover %s, Wrong code\n",net_if->Name);
        EvrNetICMP6_WrongCode (net_if->Id, ICMP6_FRAME(frame)->Code, 0);
        return (false);
      }
      net_ndp_process (net_if, frame);
      break;

    default:
      DEBUGF (ICMP6," Message not recognized\n");
      EvrNetICMP6_MessageTypeUnknown (net_if->Id, ICMP6_FRAME(frame)->Type);
      return (false);
  }
  return (true);
}

/**
  \brief       Construct ICMP6 header and send frame.
  \param[in]   net_if    network interface descriptor.
  \param[in]   frame     network frame.
  \param[in]   src_addr  source address.
  \param[in]   dst_addr  destination address.
  \param[in]   type      message type.
  \return      status:
               - true  = success,
               - false = error.
*/
bool net_icmp6_send (NET_IF_CFG *net_if, NET_FRAME *frame,
       const uint8_t *src_addr, const uint8_t *dst_addr, uint8_t type) {

  DEBUGF (ICMP6,"Send_frame %s\n",net_if->Name);
  EvrNetICMP6_SendFrame (net_if->Id, frame->length);
  /* Construct ICMP6 header */
  ICMP6_FRAME(frame)->Type   = type;
  ICMP6_FRAME(frame)->Code   = 0;
  ICMP6_FRAME(frame)->Chksum = 0;
  if (type == ICMP6_NEIGHB_SOL) {
    dst_addr = net_addr6_get_solicited (dst_addr);
  }
  if (!net_ip6_tx_offl_hl (net_if, SYS_OFFL_ICMP6_TX)) {
    ICMP6_FRAME(frame)->Chksum =
      htons(net_ip6_chksum (src_addr, dst_addr,
                            ICMP6_FRAME(frame), IP6_PROT_ICMP, frame->length));
  }
  DEBUG_INFO (frame);
  EvrNetICMP6_ShowFrameHeader (ICMP6_FRAME(frame));
  return (net_ip6_send_frame (net_if,
                              frame, src_addr, dst_addr, IP6_PROT_ICMP, 0, 255));
}

/**
  \brief       Construct and send echo request.
  \param[in]   net_if  network interface descriptor.
  \return      status:
               - true  = success,
               - false = error.
*/
bool net_icmp6_send_echo (NET_IF_CFG *net_if) {
  NET_FRAME *txfrm;
  bool retv;

  txfrm = net_mem_alloc (IP6_DATA_OFFS + ICMP_HEADER_LEN + 22);
  ECHO6_FRAME(txfrm)->Id  = htons(ping->Id);
  ECHO6_FRAME(txfrm)->Seq = htons(ping->Seq);
  /* Fill in the payload data */
  memcpy (ECHO6_FRAME(txfrm)->Data, net_ping_payload, 18);
  txfrm->length = ICMP_HEADER_LEN + 22;
  retv = net_icmp6_send (net_if,
                         txfrm, ping->LocAddr, ping->Host.addr, ICMP6_ECHO_REQ);
  net_mem_free (txfrm);
  return (retv);
}

#ifdef DEBUG_STDIO
/**
  \brief       Debug print ICMP6 frame information.
  \param[in]   frm  network frame.
  \return      none.
*/
static void debug_info (const NET_FRAME *frm) {
  static const char *const t_asc[] = {
    "Echo-Request",
    "Echo-Reply",
    "McListener-Query",
    "McListener-Report",
    "McListener-Done",
    "Router-Solic.",
    "Router-Adver.",
    "Neighbor-Solic.",
    "Neighbor-Adver.",
    "Inv.Neighbor-Solic.",
    "Inv.Neighbor-Adver."
  };
  static const uint8_t t_bin[] = {
    ICMP6_ECHO_REQ,
    ICMP6_ECHO_REPLY,
    ICMP6_MCAST_QUERY,
    ICMP6_MCAST_REPORT,
    ICMP6_MCAST_DONE,
    ICMP6_ROUTER_SOL,
    ICMP6_ROUTER_ADVER,
    ICMP6_NEIGHB_SOL,
    ICMP6_NEIGHB_ADVER,
    ICMP6_INV_NBR_SOL,
    ICMP6_INV_NBR_ADVER
  };
  uint32_t i;

  /* Print user friendly ICMPv6 types */
  for (i = 0; i < sizeof (t_bin); i++) {
    if (t_bin[i] == ICMP6_FRAME(frm)->Type) {
      DEBUGF (ICMP6," Type %s\n",t_asc[i]);
      goto d;
    }
  }
  DEBUGF (ICMP6," Type (%d)\n",ICMP6_FRAME(frm)->Type);
d:DEBUGF (ICMP6," Code=%d, Cks=0x%04X\n",ICMP6_FRAME(frm)->Code,
                                         ntohs(ICMP6_FRAME(frm)->Chksum));

  /* Print type specific information */
  switch (ICMP6_FRAME(frm)->Type) {
    case ICMP6_ECHO_REQ:
    case ICMP6_ECHO_REPLY:
      DEBUGF (ICMP6," EchoId=0x%04X, Seq=0x%04X\n",
                                         ntohs(ECHO6_FRAME(frm)->Id),
                                         ntohs(ECHO6_FRAME(frm)->Seq));
      DEBUGF (ICMP6," Size %d bytes\n",frm->length-8);
      break;
  }
}
#endif
