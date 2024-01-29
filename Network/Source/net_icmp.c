/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_icmp.c
 * Purpose: Internet Control Message Protocol
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_mem.h"
#include "net_ip4.h"
#include "net_icmp.h"
#include "net_ping.h"
#include "net_dbg.h"

/* Global variables */
NET_ICMP_CTRL net_eth0_icmp_control;
NET_ICMP_CTRL net_eth1_icmp_control;
NET_ICMP_CTRL net_wifi0_icmp_control;
NET_ICMP_CTRL net_wifi1_icmp_control;
NET_ICMP_CTRL net_ppp0_icmp_control;
NET_ICMP_CTRL net_slip0_icmp_control;

/* Code shortening macros */
#define ctrl       (h->Ctrl)

/* Local Functions */
#ifdef DEBUG_STDIO
 static void debug_info (const NET_FRAME *frm);
#endif

/**
  \brief       Initialize ICMP protocol.
  \return      none.
*/
void net_icmp_init (void) {
  NET_ICMP_CFG *const *p;

  /* Set NoEcho control for network interfaces */
  for (p = &net_icmp_list[0]; *p; p++) {
    (*p)->Ctrl->NoEcho = (*p)->NoEcho;
  }
}

/**
  \brief       Enable or disable ICMP Echo response.
  \param[in]   if_id    interface identifier.
  \param[in]   no_echo  new state of NoEcho attribute:
                       - true=  disable echo response,
                       - false= enable echo response.
  \return      status code as defined with netStatus.
*/
netStatus netICMP_SetNoEcho (uint32_t if_id, bool no_echo) {
  NET_IF_CFG *net_if = net_if_map_all (if_id);
  NET_ICMP_CFG *h;

  if (!(net_if && net_if->Ip4Cfg)) {
    return (netInvalidParameter);
  }
  h = net_if->Ip4Cfg->IcmpCfg;
  ctrl->NoEcho = no_echo;
  return (netOK);
}

/**
  \brief       Process received ICMP message.
  \param[in]   net_if  network interface descriptor.
  \param[in]   frame   network frame.
  \return      status:
               - true  = success,
               - false = error.
*/
bool net_icmp_process (NET_IF_CFG *net_if, NET_FRAME *frame) {
  NET_FRAME *txfrm;

  DEBUGF (ICMP,"*** Process_frame %s ***\n",net_if->Name);
  if (frame->length < ICMP_HEADER_LEN) {
    ERRORF (ICMP,"Process %s, Frame too short\n",net_if->Name);
    EvrNetICMP_FrameTooShort (net_if->Id, frame->length, ICMP_HEADER_LEN);
    return (false);
  }
  /* Check checksum for received frame */
  if (!(sys->RxOffload & SYS_OFFL_ICMP4_RX) &&
       net_ip4_chksum_buf (ICMP_FRAME(frame), frame->length) != 0) {
    ERRORF (ICMP,"Process %s, Checksum failed\n",net_if->Name);
    EvrNetICMP_ChecksumFailed (net_if->Id, frame->length);
    return (false);
  }
  DEBUG_INFO (frame);
  EvrNetICMP_ReceiveFrame (net_if->Id, frame->length);
  EvrNetICMP_ShowFrameHeader (ICMP_FRAME(frame));

  switch (ICMP_FRAME(frame)->Type) {
    case ICMP_ECHO_REQ:
      /* Echo Request from remote host */
      if (ICMP_FRAME(frame)->Code != 0) {
        ERRORF (ICMP,"Process %s, Wrong EchoReq code\n",net_if->Name);
        EvrNetICMP_EchoRequestWrongCode (net_if->Id, ICMP_FRAME(frame)->Code, 0);
        return (false);
      }
      DEBUGF (ICMP," EchoReq received\n");
      EvrNetICMP_EchoRequestReceived (net_if->Id, frame->length-ICMP_HEADER_LEN-4);

      /* Check if echo reply allowed */
      if (!net_if->Ip4Cfg || net_if->Ip4Cfg->IcmpCfg->Ctrl->NoEcho) {
        DEBUGF (ICMP," Discarded, %s:NoEcho\n",net_if->Name);
        EvrNetICMP_EchoReplyDisabled (net_if->Id);
        return (false);
      }

      /* Reply to ICMP request */
      txfrm = net_mem_alloc (IP4_DATA_OFFS + frame->length);
      ICMP_FRAME(txfrm)->Type   = ICMP_ECHO_REPLY;
      ICMP_FRAME(txfrm)->Code   = 0;
      ICMP_FRAME(txfrm)->Chksum = 0;

      /* Now copy the received ICMP frame data */
      memcpy (&ICMP_FRAME(txfrm)->Data[0],
              &ICMP_FRAME(frame)->Data[0], frame->length - ICMP_HEADER_LEN);

      /* Response goes to the same network interface */
      if (!(sys->RxOffload & SYS_OFFL_ICMP4_TX) ||
          (net_if->localm->Mtu != 0 && net_if->localm->Mtu < frame->length)) {
        /* Calculate checksum for packet to be sent */
        ICMP_FRAME(txfrm)->Chksum =
          htons(net_ip4_chksum_buf (ICMP_FRAME(txfrm), frame->length));
      }
      DEBUGF (ICMP,"Send_EchoReply\n");
      /* Reply with the same frame data size */
      txfrm->length = frame->length;
      DEBUG_INFO (txfrm);
      EvrNetICMP_SendEchoReply (net_if->Id);
      EvrNetICMP_ShowFrameHeader (ICMP_FRAME(txfrm));
      net_ip4_send_frame (net_if, txfrm,
                          IP4_FRAME(frame)->SrcAddr, IP4_PROT_ICMP, 0, 128);
      net_mem_free (txfrm);
      break;

    case ICMP_ECHO_REPLY:
      /* Echo Reply to our ping received */
      DEBUGF (ICMP," EchoReply received\n");
      EvrNetICMP_EchoReplyReceived (net_if->Id, frame->length-ICMP_HEADER_LEN-4);
      if (!(ping->Flags & PING_FLAG_BUSY)) {
        ERRORF (ICMP,"Process %s, EchoReply in wrong state\n",net_if->Name);
        EvrNetICMP_EchoReplyWrongState (net_if->Id);
        return (false);
      }
      if (ICMP_FRAME(frame)->Code != 0) {
        ERRORF (ICMP,"Process %s, EchoReply wrong code\n",net_if->Name);
        EvrNetICMP_EchoReplyWrongCode (net_if->Id, ICMP_FRAME(frame)->Code, 0);
        return (false);
      }
      if (!net_addr4_comp (IP4_FRAME(frame)->SrcAddr, ping->Host.addr)) {
        ERRORF (ICMP,"Process %s, EchoRely wrong host IP\n",net_if->Name);
        EvrNetICMP_EchoReplyWrongIpAddress (net_if->Id, IP4_FRAME(frame)->SrcAddr);
        return (false);
      }
      if (ntohs(ECHO_FRAME(frame)->Id) != ping->Id) {
        ERRORF (ICMP,"Process %s, EchoRely Wrong Id\n",net_if->Name);
        EvrNetICMP_EchoReplyWrongId (net_if->Id, ntohs(ECHO_FRAME(frame)->Id), ping->Id);
        return (false);
      }
      if ((frame->length != ICMP_HEADER_LEN + 22) ||
          memcmp (ECHO_FRAME(frame)->Data, net_ping_payload, 18) != 0) {
        ERRORF (ICMP,"Process %s, Wrong EchoReply payload\n",net_if->Name);
        EvrNetICMP_EchoReplyWrongPayload (net_if->Id);
        return (false);
      }
      ping->cb_func (netPing_EventSuccess);
      ping->Flags = 0x00;
      break;

    default:
      DEBUGF (ICMP," Message not recognized\n");
      EvrNetICMP_MessageTypeUnknown (net_if->Id, ICMP_FRAME(frame)->Type);
      return (false);
  }
  return (true);
}

/**
  \brief       Construct and send echo request.
  \param[in]   net_if  network interface descriptor.
  \return      status:
               - true  = success,
               - false = error.
*/
bool net_icmp_send_echo (NET_IF_CFG *net_if) {
  NET_FRAME *txfrm;
  bool retv;

  txfrm = net_mem_alloc (IP4_DATA_OFFS + ICMP_HEADER_LEN + 22);
  ICMP_FRAME(txfrm)->Type   = ICMP_ECHO_REQ;
  ICMP_FRAME(txfrm)->Code   = 0;
  ICMP_FRAME(txfrm)->Chksum = 0;
  ECHO_FRAME(txfrm)->Id     = htons(ping->Id);
  ECHO_FRAME(txfrm)->Seq    = htons(ping->Seq);
  /* Fill in the payload data */
  memcpy (ECHO_FRAME(txfrm)->Data, net_ping_payload, 18);
  if (!net_ip4_tx_offl_hl (net_if, SYS_OFFL_ICMP4_TX, ICMP_HEADER_LEN + 22)) {
    ICMP_FRAME(txfrm)->Chksum =
      htons(net_ip4_chksum_buf (ICMP_FRAME(txfrm), ICMP_HEADER_LEN + 22));
  }
  txfrm->length = ICMP_HEADER_LEN + 22;
  DEBUG_INFO (txfrm);
  EvrNetICMP_SendEchoRequest (net_if->Id);
  EvrNetICMP_ShowFrameHeader (ICMP_FRAME(txfrm));
  retv = net_ip4_send_frame (net_if, txfrm, ping->Host.addr, IP4_PROT_ICMP, 0, 128);
  net_mem_free (txfrm);
  return (retv);
}

#ifdef DEBUG_STDIO
/**
  \brief       Debug print ICMP frame information.
  \param[in]   frm  network frame.
  \return      none.
*/
static void debug_info (const NET_FRAME *frm) {
  static const char *const t_asc[] = {
    "Echo-Reply",
    "Dest-Unreach.",
    "Echo-Request",
    "Time-Exceeded",
    "Traceroute"
  };
  static const uint8_t t_bin[] = {
    ICMP_ECHO_REPLY,
    ICMP_DST_UNREACH,
    ICMP_ECHO_REQ,
    ICMP_TIME_EXC,
    ICMP_TRACERT
  };
  uint32_t i;

  /* Print user friendly ICMP types */
  for (i = 0; i < sizeof (t_bin); i++) {
    if (t_bin[i] == ICMP_FRAME(frm)->Type) {
      DEBUGF (ICMP," Type %s\n",t_asc[i]);
      goto d;
    }
  }
  DEBUGF (ICMP," Type (%d)\n",ICMP_FRAME(frm)->Type);
d:DEBUGF (ICMP," Code=%d, Cks=0x%04X\n",ICMP_FRAME(frm)->Code,
                                        ntohs(ICMP_FRAME(frm)->Chksum));

  /* Print type specific information */
  switch (ICMP_FRAME(frm)->Type) {
    case ICMP_ECHO_REQ:
    case ICMP_ECHO_REPLY:
      DEBUGF (ICMP," EchoId=0x%04X, Seq=0x%04X\n",
                                        ntohs(ECHO_FRAME(frm)->Id),
                                        ntohs(ECHO_FRAME(frm)->Seq));
      DEBUGF (ICMP," Size %d bytes\n",frm->length-8);
      break;
  }
}
#endif
