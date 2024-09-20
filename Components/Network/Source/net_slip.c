/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_slip.c
 * Purpose: IP over Serial Line Interface
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_mem.h"
#include "net_addr.h"
#include "net_ip4.h"
#include "net_icmp.h"
#include "net_udp.h"
#include "net_tcp.h"
#include "net_slip.h"
#include "net_modem_drv.h"
#include "net_serial.h"
#include "net_dbg.h"

/* Global variables */
NET_SLIP_CTRL net_slip0_if_control;

/* Code shortening macros */
#define ctrl        ( h->Ctrl)
#define LocM        (*h->If->localm)

/* Local Functions */
static uint32_t slip_opt_len (netIF_Option option);
static uint16_t slip_mtu_limit (uint16_t mtu);
static void slip_receive (NET_SLIP_CFG *h);
static void slip_transmit (NET_SLIP_CFG *h);
static void que_add_tail (NET_FRAME **list, NET_FRAME *frame);
static NET_FRAME *que_get_first (NET_FRAME **list);
static void slip_lock (NET_SLIP_CFG *h);
static void slip_unlock (NET_SLIP_CFG *h);

/**
  \brief       Initialize SLIP network interface.
*/
void net_slip_iface_init (void) {
  NET_SLIP_CFG *h = net_slip_list[0];

  DEBUGF (SLIP,"Init %s interface\n",h->If->Name);
  EvrNetSLIP_InitInterface ();
  memset (ctrl, 0, sizeof (*ctrl));

  /* Init IPv4 local machine info */
  if (!net_addr4_aton (h->If->Ip4Cfg->IpAddr, LocM.IpAddr) ||
      !net_addr4_aton (h->If->Ip4Cfg->PriDNS, LocM.PriDNS) ||
      !net_addr4_aton (h->If->Ip4Cfg->SecDNS, LocM.SecDNS)) {
    ERRORF (SLIP,"Init, IPv4 config error\n");
    EvrNetSLIP_Ip4ConfigError ();
    net_sys_error (NET_ERROR_CONFIG);
  }
  __ALIGNED_UINT32(LocM.NetMask) = HTONL(SLIP_NET_MASK);
  LocM.Mtu = slip_mtu_limit (h->If->Ip4Cfg->Mtu);

  /* Initialize modem and UART devices */
  net_com_init (h->ComCfg);
  net_modem_init (h->ComCfg);

  /* Create OS objects for SLIP interface */
  ctrl->thread = netif_create (h->If->Id, &ctrl->semaphore);
  if (ctrl->thread == NULL) {
    ERRORF (SLIP,"Init, Thread create failed\n");
    EvrNetSLIP_ThreadCreateFailed (); 
    net_sys_error (NET_ERROR_CONFIG);
  }
  if (ctrl->semaphore == NULL) {
    ERRORF (SLIP,"Init, Semaphore create failed\n");
    EvrNetSLIP_SemaphoreCreateFailed (); 
    net_sys_error (NET_ERROR_CONFIG);
  }
}

/**
  \brief       De-initialize SLIP network interface.
*/
void net_slip_iface_uninit (void) {
  NET_SLIP_CFG *h = net_slip_list[0];

  DEBUGF (SLIP,"Uninit interface\n");
  EvrNetSLIP_UninitInterface ();

  /* De-initialize modem and UART devices */
  net_modem_uninit (h->ComCfg);
  net_com_uninit (h->ComCfg);

  /* Delete OS objects for SLIP interface */
  netif_delete (ctrl->thread, ctrl->semaphore);

  /* Clear IPv4 local machine info */
  memset (&LocM, 0, sizeof (LocM));
  memset (ctrl, 0, sizeof (*ctrl));
}

/**
  \brief       SLIP interface thread.
  \param[in]   arg  slip interface handle.
*/
__NO_RETURN void netSLIP_Thread (void *arg) {
  NET_SLIP_CFG *h = (NET_SLIP_CFG *)arg;

  ctrl->th.osDelay = h->ComCfg->MaxDelay;
  while (1) {
    /* Adaptive delays based on UART activity */
    netos_delay (ctrl->th.osDelay);
    if (ctrl->th.osDelay < h->ComCfg->MaxDelay) {
      ctrl->th.osDelay++;
    }
    /* Protect driver access */
    slip_lock (h);
    slip_receive (h);
    slip_transmit (h);
    if (ctrl->th.SysTick) {
      ctrl->th.SysTick = false;
      if (ctrl->th.RecTimer) {
        ctrl->th.RecTimer--;
      }
      net_modem_run (h->ComCfg);
      if (ctrl->th.Flags & SLIP_TFLAG_ONLINE) {
        /* Check if modem still online */
        if (net_modem_online (h->ComCfg) == false) {
          ctrl->th.Flags &= ~SLIP_TFLAG_ONLINE;
          ctrl->th.DisconnRq = true;
          ctrl->th.OfflineSt = true;
        }
      }
    }
    slip_unlock (h);
  }
}

/**
  \brief       Get current value of SLIP Interface option.
  \param[in]   option   option as specified by netIF_Option.
  \param[out]  buf      buffer to store the option value to
  \param[in]   buf_len  length of buffer
  \return      status code as defined with netStatus.
  \note        This function is called from a protected function.
*/
netStatus net_slip_get_option (netIF_Option option, uint8_t *buf, uint32_t buf_len) {
  NET_SLIP_CFG *h = net_slip_list[0];
  uint32_t opt_len;

  if (buf == NULL || buf_len == 0) {
    goto invalid_param;
  }
  opt_len = slip_opt_len (option);
  if (opt_len == 0 || opt_len > buf_len) {
    goto invalid_param;
  }
  /* Read option value */
  switch ((int32_t)option) {
    case netIF_OptionIP4_Address:
      net_addr4_copy (buf, LocM.IpAddr);
      return (netOK);

    case netIF_OptionIP4_PrimaryDNS:
      net_addr4_copy (buf, LocM.PriDNS);
      return (netOK);

    case netIF_OptionIP4_SecondaryDNS:
      net_addr4_copy (buf, LocM.SecDNS);
      return (netOK);

    case netIF_OptionIP4_MTU:
      /* Possibly non-aligned buffer */
      memcpy (buf, &LocM.Mtu, 2);
      return (netOK);

    default:
      /* Not supported options */
      break;
  }
invalid_param:
  ERRORF (SLIP,"GetOption, Invalid parameter\n");
  EvrNetSLIP_GetOptionInvalidParameter ();
  return (netInvalidParameter);
}

/**
  \brief       Set the value of SLIP Interface option.
  \param[in]   option   option as specified by netIF_Option.
  \param[out]  buf      buffer containing the option value
  \param[in]   buf_len  length of buffer
  \return      status code as defined with netStatus.
  \note        This function is called from a protected function.
*/
netStatus net_slip_set_option (netIF_Option option, const uint8_t *buf, uint32_t buf_len) {
  NET_SLIP_CFG *h = net_slip_list[0];
  uint16_t bval16;

  if (buf == NULL || buf_len == 0) {
    goto invalid_param;
  }
  if (buf_len != slip_opt_len (option)) {
    goto invalid_param;
  }
  /* Write option value */
  switch ((int32_t)option) {
    case netIF_OptionIP4_Address:
      DEBUGF (SLIP," IP4_Addr=%s\n",net_addr4_ntoa(buf));
      EvrNetSLIP_SetIp4Address (buf);
      net_addr4_copy (LocM.IpAddr, buf);
      return (netOK);

    case netIF_OptionIP4_PrimaryDNS:
      DEBUGF (SLIP," Pri_DNS=%s\n",net_addr4_ntoa(buf));
      EvrNetSLIP_SetIp4PrimaryDNS (buf);
      net_addr4_copy (LocM.PriDNS, buf);
      return (netOK);

    case netIF_OptionIP4_SecondaryDNS:
      DEBUGF (SLIP," Sec_DNS=%s\n",net_addr4_ntoa(buf));
      EvrNetSLIP_SetIp4SecondaryDNS (buf);
      net_addr4_copy (LocM.SecDNS, buf);
      return (netOK);

    case netIF_OptionIP4_MTU:
      /* Possibly non-aligned buffer */
      memcpy (&bval16, buf, 2);
      LocM.Mtu = slip_mtu_limit (bval16);
      DEBUGF (SLIP," IP4_MTU=%d\n",LocM.Mtu);
      EvrNetSLIP_SetIp4Mtu (LocM.Mtu);
      return (netOK);

    default:
      /* Not supported options */
      break;
  }
invalid_param:
  ERRORF (SLIP,"SetOption, Invalid parameter\n");
  EvrNetSLIP_SetOptionInvalidParameter ();
  return (netInvalidParameter);
}

/**
  \brief       Get requested interface option length.
  \param[in]   option   option as specified by netIF_Option.
  \return      length in bytes.
*/
static uint32_t slip_opt_len (netIF_Option option) {
  switch ((int32_t)option) {
    /* IPv4 options */
    case netIF_OptionIP4_Address:
    case netIF_OptionIP4_PrimaryDNS:
    case netIF_OptionIP4_SecondaryDNS:
      return (NET_ADDR_IP4_LEN);

    case netIF_OptionIP4_MTU:
      return (2);

    default:
      break;
  }
  /* Invalid option */
  return (0);
}

/**
  \brief       Limit the range of MTU.
  \param[in]   mtu  input MTU value.
  \return      limited value of input MTU.
*/
static uint16_t slip_mtu_limit (uint16_t mtu) {
  if (mtu != 0) {
    /* MTU range from 296 to 1500 bytes */
    if (mtu <  296) mtu =  296;
    if (mtu > 1500) mtu = 1500;
  }
  return (mtu);
}

/**
  \brief       Start a dial-up connection to remote SLIP server.
  \param[in]   dial_num  phone number of remote SLIP server.
  \return      status code as defined with netStatus.
  \note        Parameter is null-terminated string.
*/
netStatus netSLIP_Connect (const char *dial_num) {
  NET_SLIP_CFG *h = net_slip_list[0];

  START_LOCK (netStatus);

  if (dial_num == NULL) {
    ERRORF (SLIP,"Connect, Invalid parameter\n");
    EvrNetSLIP_ConnectInvalidParameter ();
    RETURN (netInvalidParameter);
  }
  if (ctrl->th.Flags & SLIP_TFLAG_ONLINE) {
    ERRORF (SLIP,"Connect, Wrong state\n");
    EvrNetSLIP_ConnectWrongState ();
    RETURN (netWrongState);
  }
  DEBUGF (SLIP,"Connect to server\n");
  DEBUGF (SLIP,"Dial number: %s\n",dial_num);
  EvrNetSLIP_Connect (dial_num, strlen(dial_num));
  slip_lock (h);
  net_modem_dial (h->ComCfg, dial_num);
  slip_unlock (h);
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Start SLIP interface to accept incoming SLIP connections.
  \return      status code as defined with netStatus.
*/
netStatus netSLIP_Listen (void) {
  NET_SLIP_CFG *h = net_slip_list[0];

  START_LOCK (netStatus);

  if (ctrl->th.Flags & SLIP_TFLAG_ONLINE) {
    ERRORF (SLIP,"Listen, Wrong state\n");
    EvrNetSLIP_ListenWrongState ();
    RETURN (netWrongState);
  }
  DEBUGF (SLIP,"Start listen mode\n");
  EvrNetSLIP_Listen ();
  slip_lock (h);
  net_modem_listen (h->ComCfg);
  slip_unlock (h);
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Disconnect SLIP link between two modems.
  \return      status code as defined with netStatus.
*/
netStatus netSLIP_Close (void) {
  NET_SLIP_CFG *h = net_slip_list[0];

  START_LOCK (netStatus);

  DEBUGF (SLIP,"Close connection\n");
  EvrNetSLIP_Close ();
  /* Start delayed modem shutdown */
  ctrl->th.DisconnRq = true;
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Determine the state of SLIP link.
  \return      link state:
               - true  = Link is up, IP frames can be exchanged,
               - false = Link is down.
*/
bool netSLIP_LinkUp (void) {
  NET_SLIP_CFG *h = net_slip_list[0];

  if (ctrl->th.Flags & SLIP_TFLAG_ONLINE) {
    return (true);
  }
  return (false);
}

/**
  \brief       Receive slip frame.
  \param[in]   h  slip interface handle.
  \note        Called from SLIP thread!
  \details     This function decodes byte stuffing and packet framing.
               SYNC flag starts receive timeout timer.
*/
static void slip_receive (NET_SLIP_CFG *h) {
  bool rec_flag = false;
  int32_t ch;

  while (1) {
    ch = net_com_recv_char (h->ComCfg);
    if (ch < 0) {
      if ((ctrl->th.Flags & SLIP_TFLAG_SYNC) == 0) {
        /* Receive buffer is empty */
        return;
      }
      if (rec_flag == true) {
        /* Frame receive in progress */
        ctrl->th.RecTimer = h->ComCfg->RecTout;
        ctrl->th.osDelay  = 2;
        return;
      }
      if (ctrl->th.RecTimer) {
        /* Nothing received this time */
        return;
      }
      /* Timeout on receive */
      ctrl->th.Flags &= ~SLIP_TFLAG_ESC;
      ch = SLIP_CH_END;
    }
    rec_flag = true;
    if ((ctrl->th.Flags & SLIP_TFLAG_SYNC) == 0) {
      /* ReSynchronization is required, wait for SYNC character */
      if ((ctrl->th.Flags & SLIP_TFLAG_ONLINE) && ch == SLIP_CH_END) {
        ctrl->th.Flags |= SLIP_TFLAG_SYNC;
        continue;
      }
      if (net_modem_process (h->ComCfg, ch) == true) {
        ctrl->th.Flags |= SLIP_TFLAG_ONLINE;
      }
      continue;
    }

    if (ctrl->th.Frame == NULL) {
      ctrl->th.Frame = net_mem_alloc (SLIP_FRM_OFFS + SLIP_MTU);
      /* Add frame offset for compatibility with Ethernet frames */
      ctrl->th.Frame->index = SLIP_FRM_OFFS;
    }

    if ((ctrl->th.Flags & SLIP_TFLAG_ESC)) {
      ctrl->th.Flags &= ~SLIP_TFLAG_ESC;
      switch (ch) {
        case SLIP_CH_ESC_END:
          ch = SLIP_CH_END;
          goto store;

        case SLIP_CH_ESC_ESC:
          ch = SLIP_CH_ESC;
          goto store;
      }
    }

    switch (ch) {
      case SLIP_CH_ESC:
        /* Special ESC character received */
        ctrl->th.Flags |= SLIP_TFLAG_ESC;
        break;

      case SLIP_CH_END:
        /* Special END Flag character received */
        if (ctrl->th.Frame->index == SLIP_FRM_OFFS) {
          break;
        }
        ctrl->th.Flags &= ~(SLIP_TFLAG_ESC | SLIP_TFLAG_SYNC);
        /* Resize the frame and store it to rx queue */
        net_mem_shrink (ctrl->th.Frame, ctrl->th.Frame->index);
        ctrl->RxCount += ctrl->th.Frame->length;
        que_add_tail (&ctrl->rx_list, ctrl->th.Frame);
        ctrl->th.Frame = NULL;
        net_sys_wakeup ();
        return;

      default:
store:  if (ctrl->th.Frame->index < (SLIP_FRM_OFFS + SLIP_MTU)) {
          ctrl->th.Frame->data [ctrl->th.Frame->index++] = ch & 0xFF;
        }
        break;
    }
  }
}

/**
  \brief       Transmit frame from tx-queue.
  \param[in]   h  slip interface handle.
  \note        Called from SLIP thread!
  \details     This function performs packet framing and byte stuffing.
*/
static void slip_transmit (NET_SLIP_CFG *h) {
  NET_FRAME *frame;
  uint32_t idx;
  uint8_t ch;

  if (ctrl->tx_list == NULL) {
    /* Transmit queue is empty */
    if (ctrl->th.DisconnRq) {
      /* Link disconnect requested */
      if (net_com_tx_active (h->ComCfg)) {
        /* Wait, serial data not sent yet */
        return;
      }
      net_modem_hangup (h->ComCfg);
      if (ctrl->th.Frame != NULL) {
        net_mem_free (ctrl->th.Frame);
        ctrl->th.Frame   = NULL;
      }
      /* Reset the receive process status flags */
      ctrl->th.Flags     = 0x00;
      ctrl->th.DisconnRq = false;
    }
    return;
  }
  frame = ctrl->tx_list;
  if ((ctrl->th.Flags & SLIP_TFLAG_ONLINE) == 0) {
    /* When modem is offline, discard TX frames */
    goto exit;
  }
  /* Short osDelay while transmitting */
  ctrl->th.osDelay = 2;
  idx = frame->index;
  if (idx == (SLIP_FRM_OFFS-1)) {
    /* Start transmitting, send opening FLAG */
    ctrl->th.Flags &= ~SLIP_TFLAG_TX_ESC;
    goto flag;
  }
  ch = SLIP_FRAME(frame)->Ch;
  while (1) {
tx: if (net_com_send_char (h->ComCfg, ch) == false) {
      /* Transmit buffer is full, can not send */
      frame->index = idx & 0xFFFF;
      SLIP_FRAME(frame)->Ch = ch;
      return;
    }
    if (idx == 0) {
      /* Frame has been sent, append END character */
flag: ch = SLIP_CH_END;
      idx++;
      goto tx;
    }
    if (idx == 1) {
      net_com_flush_buf (h->ComCfg);
      ctrl->TxCount += frame->length;
      /* Remove frame from top of the list */
exit: que_get_first (&ctrl->tx_list);
      net_mem_free (frame);
      return;
    }
    ch = frame->data[idx];
    switch (ch) {
      case SLIP_CH_ESC:
        if ((ctrl->th.Flags & SLIP_TFLAG_TX_ESC) == 0) {
          ctrl->th.Flags |= SLIP_TFLAG_TX_ESC;
          goto tx;
        }
        ctrl->th.Flags &= ~SLIP_TFLAG_TX_ESC;
        ch = SLIP_CH_ESC_ESC;
        break;

      case SLIP_CH_END:
        if ((ctrl->th.Flags & SLIP_TFLAG_TX_ESC) == 0) {
          ctrl->th.Flags |= SLIP_TFLAG_TX_ESC;
          ch = SLIP_CH_ESC;
          goto tx;
        }
        ctrl->th.Flags &= ~SLIP_TFLAG_TX_ESC;
        ch = SLIP_CH_ESC_END;
        break;
    }
    if (++idx >= frame->length) {
      /* Send also the END character */
      idx = 0;
    }
  }
}

/**
  \brief       Send frame to SLIP transmit queue.
  \param[in]   if_num  interface number.
  \param[in]   frame   network frame.
  \param[in]   ip_ver  IP version.
  \return      send status:
               - true  = success,
               - false = failed.
*/
bool net_slip_send_frame (uint32_t if_num, NET_FRAME *frame, uint8_t ip_ver) {
  NET_SLIP_CFG *h = net_slip_list[0];
  NET_FRAME *txframe;
  (void)if_num;

  DEBUGF (SLIP,"Sending frame\n");
  EvrNetSLIP_SendFrame (frame->length-SLIP_FRM_OFFS, ip_ver);
  if (ip_ver != IP_VER4) {
    ERRORF (SLIP,"Send, Invalid IP version\n");
    EvrNetSLIP_InvalidIpVersion (ip_ver);
    return (false);
  }
  if (frame->length > (PHY_HEADER_LEN + SLIP_MTU)) {
    /* Discard frame, payload is too large */
    ERRORF (SLIP,"Send, Data too large\n");
    EvrNetSLIP_SendDataTooLarge(frame->length-PHY_HEADER_LEN, SLIP_MTU);
    return (false);
  }
  if (!(net_mem_avail (0))) {
    /* System out of memory, ignore packet */
    ERRORF (SLIP,"Send, Out of memory\n");
    EvrNetSLIP_NoMemoryError ();
    return (false);
  }

  txframe = net_mem_alloc (frame->length);
  memcpy (txframe->data, frame->data, frame->length);
  DEBUGF (SLIP," Length %d bytes\n",txframe->length-SLIP_FRM_OFFS);
  /* Set start index for first END character */
  txframe->index = SLIP_FRM_OFFS - 1;

  /* Add the frame to transmit queue tail */
  DEBUGF (SLIP," Que_add xmit\n");
  EvrNetSLIP_QueueAddTransmit ();
  slip_lock (h);
  que_add_tail (&ctrl->tx_list, txframe);
  slip_unlock (h);
  return (true);
}

/**
  \brief       Run main process for SLIP interface.
*/
void net_slip_iface_run (void) {
  NET_SLIP_CFG *h = net_slip_list[0];
  NET_FRAME *frame;

  if (sys->Flags & SYS_FLAG_TICK) {
    /* Sync timings for SLIP thread */
    ctrl->th.SysTick = true;
    if (ctrl->th.OfflineSt) {
      DEBUGF (SLIP,"Dropping, Modem offline\n");
      EvrNetSLIP_ModemOffline ();
      ctrl->th.OfflineSt = false;
    }
  }
  /* Check if a frame has been received */
  if (ctrl->rx_list == NULL) {
    return;
  }
  sys->Busy = true;

  /* Global parameters for input frame processing */
  sys->RxIpAddr  = NULL;
  sys->RxOffload = SYS_OFFL_NONE;

  slip_lock (h);
  frame = que_get_first (&ctrl->rx_list);
  slip_unlock (h);

  DEBUGF (SLIP,"*** Process_frame ***\n");
  DEBUGF (SLIP," Length %d bytes\n",frame->length-SLIP_FRM_OFFS);
  EvrNetSLIP_ReceiveFrame (frame->length-SLIP_FRM_OFFS);

  /* Check IP protocol header */
  if (net_ip4_chk_frame (h->If, frame)) {
    /* Process fragmented frame reassembly */
    frame = net_ip4_reass_frame (frame);
    if (frame == NULL) {
      /* Reassembly has taken over this frame */
      return;
    }
    /* Now check IP frame Protocol type */
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
  }
  /* Release memory allocated for frame */
  net_mem_free (frame);
}

/**
  \brief       Add frame to queue tail.
  \param[in]   list   queue list head.
  \param[in]   frame  frame to add.
*/
static void que_add_tail (NET_FRAME **list, NET_FRAME *frame) {
  NET_FRAME *next;

  if (*list == NULL) {
    /* First frame added to queue */
    *list = frame;
  }
  else {
    /* Append the frame to queue tail */
    next = *list;
    for ( ; SLIP_FRAME(next)->next; next = SLIP_FRAME(next)->next);
    SLIP_FRAME(next)->next = frame;
  }
  SLIP_FRAME(frame)->next = NULL;
}

/**
  \brief       Get frame from the top of queue.
  \param[in]   list   queue list head.
  \return      pointer to frame.
*/
static NET_FRAME *que_get_first (NET_FRAME **list) {
  NET_FRAME *frame;

  frame = *list;
  *list = SLIP_FRAME(*list)->next;
  return (frame);
}

/**
  \brief       Lock slip interface.
  \param[in]   h  slip interface handle.
*/
static void slip_lock (NET_SLIP_CFG *h) {
  netif_lock (ctrl->semaphore);
}

/**
  \brief       Unlock slip interface.
  \param[in]   h  slip interface handle.
*/
static void slip_unlock (NET_SLIP_CFG *h) {
  netif_unlock (ctrl->semaphore);
}

