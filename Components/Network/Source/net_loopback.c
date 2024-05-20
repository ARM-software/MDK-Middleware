/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_loopback.c
 * Purpose: Loopback Interface
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "net_lib.h"
#include "net_sys.h"
#include "net_mem.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_loopback.h"
#include "net_igmp.h"
#include "net_icmp.h"
#include "net_ip4.h"
#ifdef Network_IPv6
#include "net_icmp6.h"
#include "net_ip6.h"
#endif
#include "net_udp.h"
#include "net_tcp.h"
#include "net_dbg.h"

/* Local variables and constants */
static NET_LOOP_CTRL loop_control;

/* LOOP: Interface configuration */
NET_IF_CFG net_loop_if_config = {
  NULL,
  NET_IF_CLASS_LOOP+0,
  "LOOP0",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  net_loop_send_frame,
  NULL
};

/* LOOP: low layer configuration */
static NET_LOOP_CFG loop_ll_config = {
  &loop_control,
  &net_loop_if_config,
  0
};

/* Code shortening macros */
#define ctrl       (h->Ctrl)

/* Local functions */
#ifdef Network_Debug_STDIO
 static void debug_info (const NET_FRAME *frm);
#endif

/**
  \brief       Initialize loopback network interface.
*/
void net_loop_iface_init (void) {
  NET_LOOP_CFG *h = &loop_ll_config;

  DEBUGF (LOOP,"Init interface\n");
  EvrNetLOOP_InitInterface ();
  memset (ctrl, 0, sizeof(*ctrl));

  /* Initialize IP protocols */
  net_ip4_init ();
  net_icmp_init ();
#ifdef Network_IPv6
  net_ip6_init ();
  net_icmp6_init ();
#endif
}

/**
  \brief       De-initialize loopback network interface.
*/
void net_loop_iface_uninit (void) {
  NET_LOOP_CFG *h = &loop_ll_config;

  DEBUGF (LOOP,"Uninit interface\n");
  EvrNetLOOP_UninitInterface ();
  memset (ctrl, 0, sizeof(*ctrl));

  /* De-initialize IP protocols */
  net_ip4_uninit ();
#ifdef Network_IPv6
  net_ip6_uninit ();
#endif
}

/**
  \brief       Send frame to local loopback queue.
  \param[in]   if_num  interface number.
  \param[in]   frame   network frame.
  \param[in]   ip_ver  IP version.
  \return      send status:
               - true  = success,
               - false = failed.
*/
bool net_loop_send_frame (uint32_t if_num, NET_FRAME *frame, uint8_t ip_ver) {
  NET_LOOP_CFG *h = &loop_ll_config;
  NET_FRAME *txframe,*next;
  (void)if_num;

  DEBUGF (LOOP,"Sending frame\n");
  EvrNetLOOP_SendFrame (frame->length-PHY_HEADER_LEN, ip_ver);
  txframe = net_mem_alloc (frame->length | 0x40000000);
  if (txframe == NULL) {
    ERRORF (LOOP,"Send, Out of memory\n");
    EvrNetLOOP_NoMemoryError ();
    return (false);
  }
  LOOPB(txframe)->IpVer = ip_ver;
  memcpy (LOOPB(txframe)->Data, LOOPB(frame)->Data, frame->length-PHY_HEADER_LEN);
  DEBUG_INFO (frame);

  if (ctrl->rx_list == NULL) {
    /* First frame added to queue */
    ctrl->rx_list = txframe;
  }
  else {
    /* Append the frame to queue tail */
    next = ctrl->rx_list;
    for ( ; LOOPB(next)->next; next = LOOPB(next)->next);
    LOOPB(next)->next = txframe;
  }
  LOOPB(txframe)->next = NULL;
  net_sys_wakeup ();
  return (true);
}

/**
  \brief       Run main process for loopback interface.
*/
void net_loop_iface_run (void) {
  NET_LOOP_CFG *h = &loop_ll_config;
  NET_FRAME *frame;

  /* Check message queue if a frame has been received */
  if (ctrl->rx_list == NULL) {
    return;
  }
  sys->Busy = true;

  /* Global parameters for input frame processing */
  sys->RxIpAddr  = NULL;
  sys->RxOffload = SYS_OFFL_RX_ALL | SYS_OFFL_TX_ALL;

  frame          = ctrl->rx_list;
  ctrl->rx_list  = LOOPB(frame)->next;

  DEBUGF (LOOP,"*** Process_frame ***\n");
  DEBUG_INFO (frame);
  EvrNetLOOP_ReceiveFrame (frame->length-PHY_HEADER_LEN, LOOPB(frame)->IpVer);

  /* Check IP protocol version */
  switch (LOOPB(frame)->IpVer) {
    case IP_VER4:
      if (!net_ip4_chk_frame (h->If, frame)) {
        break;
      }
      /* Check IP4 Protocol type */
      switch (IP4_FRAME(frame)->Prot) {
        case IP4_PROT_ICMP:
          net_icmp_process (h->If, frame);
          break;

        case IP4_PROT_UDP:
          net_udp_process (h->If, frame, IP_VER4);
          break;

        case IP4_PROT_TCP:
          net_tcp_process (h->If, frame, IP_VER4);
          break;
      }
      break;

#ifdef Network_IPv6
    case IP_VER6:
      if (!net_ip6_chk_frame (h->If, frame)) {
        break;
      }
      /* Check IP6 Protocol type */
      switch (IP6_PROT(frame)) {
        case IP6_PROT_ICMP:
          net_icmp6_process (h->If, frame);
          break;

        case IP6_PROT_UDP:
          net_udp_process (h->If, frame, IP_VER6);
          break;

        case IP6_PROT_TCP:
          net_tcp_process (h->If, frame, IP_VER6);
          break;
      }
      break;
#endif
  }

  /* Release allocated memory for frame */
  net_mem_free (frame);
}

#ifdef Network_Debug_STDIO
/**
  \brief       Debug print loopback information.
  \param[in]   frm  loopback network frame.
*/
static void debug_info (const NET_FRAME *frm) {
  DEBUGF (LOOP," Proto %s, %d bytes\n",
                (LOOPB(frm)->IpVer == IP_VER4) ? "IP4" : "IP6",
                 frm->length-PHY_HEADER_LEN);
}
#endif
