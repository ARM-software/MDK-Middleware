/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ppp.c
 * Purpose: Point to Point Interface
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
#include "net_ppp.h"
#include "net_ppp_lcp.h"
#include "net_ppp_ipcp.h"
#include "net_modem_drv.h"
#include "net_serial.h"
#include "net_dbg.h"

/* Global variables */
NET_PPP_CTRL net_ppp0_if_control;

/* Code shortening macros */
#define ctrl       ( h->Ctrl)
#define LocM       (*h->If->localm)

/* Local Functions */
static uint32_t ppp_opt_len (netIF_Option option);
static uint16_t ppp_mtu_limit (uint16_t mtu);
static uint16_t ppp_get_fcs (const void *buf, int32_t len);
static bool ppp_process (NET_FRAME *frame);
static void ppp_receive (NET_PPP_CFG *h);
static void ppp_transmit (NET_PPP_CFG *h);
static void ppp_prot_reject (NET_PPP_CFG *h, NET_FRAME *frame);
static void que_add_tail (NET_FRAME **list, NET_FRAME *frame);
static NET_FRAME *que_get_first (NET_FRAME **list);
static void ppp_lock (NET_PPP_CFG *h);
static void ppp_unlock (NET_PPP_CFG *h);
#ifdef Network_Debug_STDIO
 static void debug_info (uint16_t prot);
 static void debug_inf2 (const char *msg, const char *val);
#endif

/**
  \brief       Initialize PPP network interface.
*/
void net_ppp_iface_init (void) {
  NET_PPP_CFG *h = net_ppp_list[0];

  DEBUGF (PPP,"Init %s interface\n",h->If->Name);
  EvrNetPPP_InitInterface ();
  memset (ctrl, 0, sizeof (*ctrl));

  /* Init PPP protocols */
  net_ppp_proto_init (h, false);
  LocM.Mtu = ppp_mtu_limit (h->If->Ip4Cfg->Mtu);

  /* Init modem and UART device */
  net_com_init (h->ComCfg);
  net_modem_init (h->ComCfg);

  /* Create OS objects for PPP interface */
  ctrl->thread = netif_create (h->If->Id, &ctrl->semaphore);
  if (ctrl->thread == NULL) {
    ERRORF (PPP,"Init, Thread create failed\n");
    EvrNetPPP_ThreadCreateFailed ();
    netHandleError (netErrorRtosCreate);
  }
  if (ctrl->semaphore == NULL) {
    ERRORF (PPP,"Init, Semaphore create failed\n");
    EvrNetPPP_SemaphoreCreateFailed ();
    netHandleError (netErrorRtosCreate);
  }
}

/**
  \brief       De-initialize PPP network interface.
*/
void net_ppp_iface_uninit (void) {
  NET_PPP_CFG *h = net_ppp_list[0];

  DEBUGF (PPP,"Uninit interface\n");
  EvrNetPPP_UninitInterface ();

  /* De-initialize modem and UART devices */
  net_modem_uninit (h->ComCfg);
  net_com_uninit (h->ComCfg);

  /* De-initialize PPP protocols */
  net_lcp_uninit (h);
  if (h->PapAuth)  h->PapAuth->uninit (h);
  if (h->ChapAuth) h->ChapAuth->uninit (h);
  net_ipcp_uninit (h);

  /* Delete OS objects for PPP interface */
  netif_delete (ctrl->thread, ctrl->semaphore);
  memset (ctrl, 0, sizeof (*ctrl));
}

/**
  \brief       PPP interface thread.
  \param[in]   arg  PPP interface handle.
*/
__NO_RETURN void netPPP_Thread (void *arg) {
  NET_PPP_CFG *h = (NET_PPP_CFG *)arg;

  ctrl->th.osDelay = h->ComCfg->MaxDelay;
  while (1) {
    /* Adaptive delays based on UART activity */
    netos_delay (ctrl->th.osDelay);
    if (ctrl->th.osDelay < h->ComCfg->MaxDelay) {
      ctrl->th.osDelay++;
    }
    /* Protect driver access */
    ppp_lock (h);
    ppp_receive (h);
    ppp_transmit (h);
    if (ctrl->th.SysTick) {
      ctrl->th.SysTick = false;
      if (ctrl->th.RecTimer) {
        ctrl->th.RecTimer--;
      }
      net_modem_run (h->ComCfg);
      if (ctrl->th.Flags & PPP_TFLAG_ONLINE) {
        /* Check if modem still online */
        if (net_modem_online (h->ComCfg) == false) {
          ctrl->th.Flags &= ~PPP_TFLAG_ONLINE;
          ctrl->th.DisconnRq = true;
          ctrl->th.OfflineSt = true;
        }
      }
    }
    ppp_unlock (h);
  }
}

/**
  \brief       Get current value of PPP Interface option.
  \param[in]   option   option as specified by netIF_Option.
  \param[out]  buf      buffer to store the option value to
  \param[in]   buf_len  length of buffer
  \return      status code as defined with netStatus.
  \note        This function is called from a protected function.
*/
netStatus net_ppp_get_option (netIF_Option option, uint8_t *buf, uint32_t buf_len) {
  NET_PPP_CFG *h = net_ppp_list[0];
  uint32_t opt_len;

  if (buf == NULL || buf_len == 0) {
    goto invalid_param;
  }
  opt_len = ppp_opt_len (option);
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
  ERRORF (PPP,"GetOption, Invalid parameter\n");
  EvrNetPPP_GetOptionInvalidParameter ();
  return (netInvalidParameter);
}

/**
  \brief       Set the value of PPP Interface option.
  \param[in]   option   option as specified by netIF_Option.
  \param[out]  buf      buffer containing the option value
  \param[in]   buf_len  length of buffer
  \return      status code as defined with netStatus.
  \note        This function is called from a protected function.
*/
netStatus net_ppp_set_option (netIF_Option option, const uint8_t *buf, uint32_t buf_len) {
  NET_PPP_CFG *h = net_ppp_list[0];
  uint16_t bval16;

  if (buf == NULL || buf_len == 0) {
    goto invalid_param;
  }
  if (buf_len != ppp_opt_len (option)) {
    goto invalid_param;
  }
  /* Write option value */
  switch ((int32_t)option) {
    case netIF_OptionIP4_Address:
      DEBUGF (PPP," IP4_Addr=%s\n",net_addr4_ntoa(buf));
      EvrNetPPP_SetIp4Address (buf);
      net_addr4_copy (LocM.IpAddr, buf);
      return (netOK);

    case netIF_OptionIP4_PrimaryDNS:
      DEBUGF (PPP," Pri_DNS=%s\n",net_addr4_ntoa(buf));
      EvrNetPPP_SetIp4PrimaryDNS (buf);
      net_addr4_copy (LocM.PriDNS, buf);
      return (netOK);

    case netIF_OptionIP4_SecondaryDNS:
      DEBUGF (PPP," Sec_DNS=%s\n",net_addr4_ntoa(buf));
      EvrNetPPP_SetIp4SecondaryDNS (buf);
      net_addr4_copy (LocM.SecDNS, buf);
      return (netOK);

    case netIF_OptionIP4_MTU:
      /* Possibly non-aligned buffer */
      memcpy (&bval16, buf, 2);
      LocM.Mtu = ppp_mtu_limit (bval16);
      DEBUGF (PPP," IP4_MTU=%d\n",LocM.Mtu);
      EvrNetPPP_SetIp4Mtu (LocM.Mtu);
      return (netOK);

    default:
      /* Not supported options */
      break;
  }
invalid_param:
  ERRORF (PPP,"SetOption, Invalid parameter\n");
  EvrNetPPP_SetOptionInvalidParameter ();
  return (netInvalidParameter);
}

/**
  \brief       Get requested interface option length.
  \param[in]   option  option as specified by netIF_Option.
  \return      length in bytes.
*/
static uint32_t ppp_opt_len (netIF_Option option) {
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
static uint16_t ppp_mtu_limit (uint16_t mtu) {
  if (mtu != 0) {
    /* MTU range from 576 to 1500 bytes */
    if (mtu <  576) mtu =  576;
    if (mtu > 1500) mtu = 1500;
  }
  return (mtu);
}

/**
  \brief       Calculate frame check sequence for PPP frame.
  \param[in]   buf  buffer containing PPP frame data.
  \param[in]   len  length of buffer data.
  \return      frame check sequence.
*/
static uint16_t ppp_get_fcs (const void *buf, int32_t len) {
  static const uint16_t fcstab[256] = {
    /* Table for fast 16-bit checksum calculation */
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78 };
  uint16_t fcs = 0xFFFF;
  const uint8_t *sp = buf;

  while (len--) {
    fcs = (fcs >> 8) ^ fcstab[(fcs ^ *sp++) & 0xff];
  }
  return (fcs);
}

/**
  \brief       Process PPP header and expand frame.
  \param[in]   frame  network frame.
  \return      check result:
               - true  = frame valid,
               - false = corrupted frame.
*/
static bool ppp_process (NET_FRAME *frame) {
  uint16_t fcs;

  DEBUGF (PPP,"*** Process_frame ***\n");
  /* Check the received frame length */
  if (frame->length < (PPP_FRM_OFFS + PPP_HEADER_LEN)) {
    DEBUGF (PPP," Discarded, Frame too short\n");
    EvrNetPPP_FrameTooShort (frame->length-PPP_FRM_OFFS, PPP_HEADER_LEN);
    return (false);
  }
  /* Check the Frame Check Sequence */
  fcs = ppp_get_fcs (&PPP_FRAME(frame)->Adr, frame->length - PPP_FRM_OFFS);
  if (fcs != 0xF0B8) {
    DEBUGF (PPP," Length %d bytes\n",frame->length-PPP_FRM_OFFS);
    ERRORF (PPP,"Process, FCS check failed\n");
    EvrNetPPP_ChecksumFailed (frame->length-PPP_FRM_OFFS);
    return (false);
  }

  /* Check the PF and ACF Compression, and expand the frame */
  if (PPP_FRAME(frame)->Adr != 0xFF) {
    if (PPP_FRAME(frame)->Adr & 0x01) {
      /* Expand PF and ACF of the frame, additional 3 bytes are available */
      memmove (&frame->data[PPP_FRM_OFFS+3], &frame->data[PPP_FRM_OFFS],
               frame->length - 2 - PPP_FRM_OFFS);
      *(uint8_t *)&PPP_FRAME(frame)->Prot = 0x00;
      frame->length++;
    }
    else {
      /* Expand PF only, additional 2 bytes are reserved already */
      memmove (&frame->data[PPP_FRM_OFFS+2], &frame->data[PPP_FRM_OFFS],
               frame->length - 2 - PPP_FRM_OFFS);
    }
    PPP_FRAME(frame)->Adr  = 0xFF;
    PPP_FRAME(frame)->Ctrl = 0x03;
  }
  else {
    /* Frame not compressed, data is in place */
    frame->length -= 2;
    if (PPP_FRAME(frame)->Ctrl != 0x03) {
      ERRORF (PPP,"Process, Wrong Ctrl-byte (0x%02X)\n",PPP_FRAME(frame)->Ctrl);
      EvrNetPPP_CtrlByteInvalid (PPP_FRAME(frame)->Ctrl, 0x03);
      return (false);
    }
  }
  DEBUG_INFO (ntohs(PPP_FRAME(frame)->Prot));
  DEBUGF (PPP," Length %d bytes\n",frame->length-PPP_FRM_OFFS);
  EvrNetPPP_ReceiveFrame (ntohs(PPP_FRAME(frame)->Prot), frame->length-PPP_FRM_OFFS);
  return (true);
}

/**
  \brief       Receive ppp frame.
  \param[in]   h  PPP interface handle.
  \note        Called from PPP thread!
  \details     This function decodes byte stuffing and packet framing.
               HDLC open flag starts receive timeout timer.
*/
static void ppp_receive (NET_PPP_CFG *h) {
  bool rec_flag = false;
  int32_t ch;

  while (1) {
    ch = net_com_recv_char (h->ComCfg);
    if (ch < 0) {
      if ((ctrl->th.Flags & PPP_TFLAG_SYNC) == 0) {
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
      ctrl->th.Flags &= ~PPP_TFLAG_ESC;
      ch = PPP_CH_FLAG;
    }
    rec_flag = true;
    if ((ctrl->th.Flags & PPP_TFLAG_SYNC) == 0) {
      /* ReSynchronization is required, wait for opening Flag */
      if ((ctrl->th.Flags & PPP_TFLAG_ONLINE) && ch == PPP_CH_FLAG) {
        ctrl->th.Flags |= PPP_TFLAG_SYNC;
        continue;
      }
      if (net_modem_process (h->ComCfg, ch) == true) {
        ctrl->th.Flags |= PPP_TFLAG_ONLINE;
      }
      continue;
    }

    if (ctrl->th.Frame == NULL) {
      ctrl->th.Frame = net_mem_alloc (PPP_FRM_OFFS + PPP_FRM_SZ + 2);
      /* Add frame offset for compatibility with Ethernet frames */
      ctrl->th.Frame->index = PPP_FRM_OFFS;
    }

    if ((ctrl->th.Flags & PPP_TFLAG_ESC)) {
      ctrl->th.Flags &= ~PPP_TFLAG_ESC;
      if (ch == PPP_CH_FLAG) {
        /* ESC followed by SYNC, ignore packet */
        ctrl->th.Frame->index = PPP_FRM_OFFS;
        continue;
      }
      ch ^= 0x20;
      goto store;
    }
    switch (ch) {
      case PPP_CH_ESC:
        /* Special ESC Character received */
        ctrl->th.Flags |= PPP_TFLAG_ESC;
        break;

      case PPP_CH_FLAG:
        /* Special SYNC Character received */
        if (ctrl->th.Frame->index == PPP_FRM_OFFS) {
          /* Two SYNC flags in sequence */
          break;
        }
        ctrl->th.Flags &= ~(PPP_TFLAG_ESC | PPP_TFLAG_SYNC);
        /* Add 2 additional bytes for decompression later */
        net_mem_shrink (ctrl->th.Frame, ctrl->th.Frame->index + 2);
        ctrl->th.Frame->length = ctrl->th.Frame->index;
        ctrl->RxCount += ctrl->th.Frame->length;
        que_add_tail (&ctrl->rx_list, ctrl->th.Frame);
        ctrl->th.Frame = NULL;
        net_sys_wakeup ();
        return;

      default:
store:  if (ctrl->th.Frame->index < (PPP_FRM_OFFS + PPP_FRM_SZ)) {
          ctrl->th.Frame->data [ctrl->th.Frame->index++] = ch & 0xFF;
        }
        break;
    }
  }
}

/**
  \brief       Transmit frame from tx-queue.
  \param[in]   h  PPP interface handle.
  \note        Called from PPP thread!
  \details     This function performs packet framing and byte stuffing.
*/
static void ppp_transmit (NET_PPP_CFG *h) {
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
  if ((ctrl->th.Flags & PPP_TFLAG_ONLINE) == 0) {
    /* When modem is offline, discard TX frames */
    goto exit;
  }
  /* Short osDelay while transmitting */
  ctrl->th.osDelay = 2;
  idx = frame->index;
  if (idx == (PPP_FRM_OFFS-1)) {
    /* Start transmitting, send opening FLAG */
    ctrl->th.Flags &= ~PPP_TFLAG_TX_ESC;
    goto flag;
  }
  ch = PPP_FRAME(frame)->Ch;
  while (1) {
tx: if (net_com_send_char (h->ComCfg, ch) == false) {
      /* Transmit buffer is full, can not send */
      frame->index = idx & 0xFFFF;
      PPP_FRAME(frame)->Ch = ch;
      return;
    }
    if (idx == (PPP_FCS_OFFS+2)) {
      /* Frame has been sent, append closing FLAG */
flag: ch = PPP_CH_FLAG;
      idx++;
      goto tx;
    }
    if (idx == (PPP_FCS_OFFS+3)) {
      net_com_flush_buf (h->ComCfg);
      ctrl->TxCount += frame->length;
      /* Remove frame from top of the list */
exit: que_get_first (&ctrl->tx_list);
      net_mem_free (frame);
      return;
    }
    ch = frame->data[idx];
    switch (ch) {
      default:
        if (ch >= 32) {
          break;
        }
        /* Process Character Control Map and escape all characters  */
        /* which have their mask bit set in 'CharMap' map register. */
        if ((ctrl->CharMap & (1 << ch)) == 0) {
          break;
        }
        __FALLTHROUGH;
        /* Send Mapped characters with ESC prefix */
      case PPP_CH_ESC:
      case PPP_CH_FLAG:
        if ((ctrl->th.Flags & PPP_TFLAG_TX_ESC) == 0) {
          ctrl->th.Flags |= PPP_TFLAG_TX_ESC;
          ch = PPP_CH_ESC;
          goto tx;
        }
        ctrl->th.Flags &= ~PPP_TFLAG_TX_ESC;
        ch ^= 0x20;
        break;
    }
    if (++idx >= frame->length) {
      /* Send also the FCS */
      idx = PPP_FCS_OFFS;
    }
  }
}

/**
  \brief       Send frame to PPP transmit queue.
  \param[in]   if_num  interface number.
  \param[in]   frame   network frame.
  \param[in]   ip_ver  IP version.
  \return      send status:
               - true  = success,
               - false = failed.
*/
bool net_ppp_send_frame (uint32_t if_num, NET_FRAME *frame, uint8_t ip_ver) {
  NET_PPP_CFG *h = net_ppp_list[0];
  (void)if_num;

  if (ip_ver != IP_VER4) {
    ERRORF (PPP,"Send, Invalid IP version\n");
    EvrNetPPP_InvalidIpVersion (ip_ver);
    return (false);
  }
  return (net_ppp_send_proto (h, frame, PPP_PROT_IP));
}

/**
  \brief       Construct PPP header and send frame internal.
  \param[in]   h      PPP interface handle.
  \param[in]   frame  network frame.
  \param[in]   prot   PPP protocol type.
  \return      send status:
               - true  = success,
               - false = failed.
*/
bool net_ppp_send_proto (NET_PPP_CFG *h, NET_FRAME *frame, uint16_t prot) {
  NET_FRAME *txframe;
  uint16_t fcs, len;

  DEBUGF (PPP,"Sending frame\n");
  if (!(ctrl->Flags & PPP_FLAG_NETWORK) && prot == PPP_PROT_IP) {
    /* Discard IP frames when IPCP is not up */
    ERRORF (PPP,"Send, Network-layer down\n");
    EvrNetPPP_NetworkLayerDown ();
    return (false);
  }
  if (frame->length > (PHY_HEADER_LEN + PPP_MTU)) {
    /* Discard frame, payload is too large */
    ERRORF (PPP,"Send, Data too large\n");
    EvrNetPPP_SendDataTooLarge(frame->length-PHY_HEADER_LEN, PPP_MTU);
    return (false);
  }
  if (!(net_mem_avail (0))) {
    /* System out of memory, ignore packet */
    ERRORF (PPP,"Send, Out of memory\n");
    EvrNetPPP_NoMemoryError ();
    return (false);
  }

  len = 0;
  if ((ctrl->Flags & PPP_FLAG_PFC) && (prot < 0x100)) {
    /* Protocol Field Compression */
    len++;
  }
  /* LCP frames must not have Address/Control fields compressed */
  if ((ctrl->Flags & PPP_FLAG_ACFC) && (prot != PPP_PROT_LCP)) {
    /* Address and Control Field Compression */
    len += 2;
  }
  txframe = net_mem_alloc (frame->length - len);
  memcpy (txframe->data, &frame->data[len], frame->length - len);
  switch (len) {
    case 0:
      PPP_FRAME(txframe)->Prot = htons(prot);
pf:   PPP_FRAME(txframe)->Adr  = 0xFF;
      PPP_FRAME(txframe)->Ctrl = 0x03;
      break;
    case 1:
      *(uint8_t *)&PPP_FRAME(txframe)->Prot = LO_BYTE(prot);
      goto pf;
    case 2:
      PPP_FRAME(txframe)->Adr  = HI_BYTE(prot);
      PPP_FRAME(txframe)->Ctrl = LO_BYTE(prot);
      break;
    case 3:
      PPP_FRAME(txframe)->Adr  = LO_BYTE(prot);
      break;
  }
  DEBUG_INFO (prot);
  DEBUGF (PPP," Length %d bytes\n",txframe->length-PPP_FRM_OFFS);
  EvrNetPPP_SendFrame (prot, txframe->length-PPP_FRM_OFFS);

  fcs = ppp_get_fcs (&PPP_FRAME(txframe)->Adr, txframe->length - PPP_FRM_OFFS);
  fcs ^= 0xFFFF;
  /* We put fcs in frame header (LSB first) */
  PPP_FRAME(txframe)->Fcs  = U16_LE(fcs);
  /* Set start index for opening HDLC flag */
  txframe->index = PPP_FRM_OFFS - 1;

  /* Add the frame to transmit queue tail */
  DEBUGF (PPP," Que_add xmit\n");
  EvrNetPPP_QueueAddTransmit ();
  ppp_lock (h);
  que_add_tail (&ctrl->tx_list, txframe);
  ppp_unlock (h);
  return (true);
}

/**
  \brief       Reject unsupported PPP protocol.
  \param[in]   ppp_if  PPP interface handle.
  \param[in]   frame   received network frame.
*/
static void ppp_prot_reject (NET_PPP_CFG *h, NET_FRAME *frame) {
  NET_FRAME *txfrm;
  uint16_t len;

  DEBUGF (PPP,"Reject unsupported protocol\n");
  EvrNetPPP_RejectProtocol (ntohs(PPP_FRAME(frame)->Prot));
  txfrm = net_mem_alloc (frame->length + 6);
  PPP_CP_FRAME(txfrm)->Code = PPP_CP_PROT_REJ;
  PPP_CP_FRAME(txfrm)->Id   = PPP_CP_FRAME(frame)->Id;
  len = ntohs(PPP_CP_FRAME(frame)->Len);
  PPP_CP_FRAME(txfrm)->Len  = htons(len + 6);
  memcpy (&PPP_FRAME(txfrm)->Data[PPP_CP_HEADER_LEN],
          &PPP_FRAME(frame)->Prot, len+2);
  net_ppp_send_proto (h, txfrm, PPP_PROT_LCP);
  net_mem_free (txfrm);
}

/**
  \brief       Reject unsupported cp code for LCP and IPCP protocols.
  \param[in]   ppp_if  PPP interface handle.
  \param[in]   frame   received network frame.
  \param[in]   prot    PPP protocol type.
  \note        Format of both protocol headers is the same.
*/
void net_ppp_cp_code_reject (NET_PPP_CFG *h, NET_FRAME *frame, uint16_t prot) {
  NET_FRAME *txfrm;
  uint16_t len;

  DEBUGF (PPP,"Reject unsupported code\n");
  EvrNetPPP_RejectCode (prot, PPP_FRAME(frame)->Data[0]);
  txfrm = net_mem_alloc (frame->length + 4);
  len = ntohs(PPP_CP_FRAME(frame)->Len);
  memcpy (&PPP_FRAME(txfrm)->Data[4], PPP_FRAME(frame)->Data, len);
  PPP_CP_FRAME(txfrm)->Code = PPP_CP_CODE_REJ;
  PPP_CP_FRAME(txfrm)->Id   = PPP_CP_FRAME(frame)->Id;
  PPP_CP_FRAME(txfrm)->Len  = htons(len + 4);
  net_ppp_send_proto (h, txfrm, prot);
  net_mem_free (txfrm);
}

/**
  \brief       Start a dial-up connection to remote PPP server.
  \param[in]   dial_num  phone number of remote PPP server.
  \param[in]   username  username for authentication.
  \param[in]   password  password for authentication.
  \return      status code as defined with netStatus.
  \note        All parameters are null-terminated strings.
*/
netStatus netPPP_Connect (const char *dial_num, const char *username, const char *password) {
  NET_PPP_CFG *h = net_ppp_list[0];

  START_LOCK (netStatus);

  if ((dial_num == NULL) || (username == NULL) || (password == NULL)) {
    ERRORF (PPP,"Connect, Invalid parameter\n");
    EvrNetPPP_ConnectInvalidParameter ();
    RETURN (netInvalidParameter);
  }
  if (ctrl->th.Flags & PPP_TFLAG_ONLINE) {
    ERRORF (PPP,"Connect, Wrong state\n");
    EvrNetPPP_ConnectWrongState ();
    RETURN (netWrongState);
  }
  DEBUGF (PPP,"Connect to server\n");
  DEBUG_INF2 ("Dial number",dial_num);
  EvrNetPPP_Connect (dial_num, strlen(dial_num));
  DEBUG_INF2 ("Username",username);
  EvrNetPPP_ShowUsername (username, strlen(username));
  DEBUG_INF2 ("Password",password);
  EvrNetPPP_ShowPassword (password, strlen(password));
  ctrl->User   = username;
  ctrl->Passw  = password;
  /* Client, initiate LCP and IPCP configuration */
  ctrl->Flags |= (PPP_FLAG_CLIENT | PPP_FLAG_LCP_CONF | PPP_FLAG_IPCP_CONF);
  ppp_lock (h);
  net_modem_dial (h->ComCfg, dial_num);
  ppp_unlock (h);
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Start PPP interface to accept incoming PPP connection.
  \param[in]   username  remote username for authentication.
  \param[in]   password  remote password for authentication.
  \return      status code as defined with netStatus.
  \note        Both parameters are null-terminated strings.
*/
netStatus netPPP_Listen (const char *username, const char *password) {
  NET_PPP_CFG *h = net_ppp_list[0];

  START_LOCK (netStatus);

  if ((username == NULL) || (password == NULL)) {
    ERRORF (PPP,"Listen, Invalid parameter\n");
    EvrNetPPP_ListenInvalidParameter ();
    RETURN (netInvalidParameter);
  }
  if (ctrl->th.Flags & PPP_TFLAG_ONLINE) {
    ERRORF (PPP,"Listen, Wrong state\n");
    EvrNetPPP_ListenWrongState ();
    RETURN (netWrongState);
  }
  DEBUGF (PPP,"Start listen mode\n");
  EvrNetPPP_Listen ();
  DEBUG_INF2 ("Username",username);
  EvrNetPPP_ShowUsername (username, strlen(username));
  DEBUG_INF2 ("Password",password);
  EvrNetPPP_ShowPassword (password, strlen(password));
  ctrl->User   = username;
  ctrl->Passw  = password;
  ctrl->Flags &= ~PPP_FLAG_CLIENT;
  ppp_lock (h);
  net_modem_listen (h->ComCfg);
  ppp_unlock (h);
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Disconnect PPP link between two modems.
  \return      status code as defined with netStatus.
*/
netStatus netPPP_Close (void) {
  NET_PPP_CFG *h = net_ppp_list[0];

  START_LOCK (netStatus);

  DEBUGF (PPP,"Close connection\n");
  EvrNetPPP_Close ();
  if (ctrl->th.Flags & PPP_TFLAG_ONLINE) {
    /* Modem is online, send Terminate request first */
    net_lcp_terminate (h);
  }
  else {
    /* Start delayed modem shutdown */
    ctrl->th.DisconnRq = true;
  }
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Initialize PPP protocols.
  \param[in]   h         PPP interface handle.
  \param[in]   shutdown  start delayed modem shutdown.
*/
void net_ppp_proto_init (NET_PPP_CFG *h, bool shutdown) {
  if (shutdown) {
    ctrl->th.DisconnRq = true;
  }
  net_lcp_init (h);
  if (h->PapAuth)  h->PapAuth->init (h);
  if (h->ChapAuth) h->ChapAuth->init (h);
  net_ipcp_init (h);
}

/**
  \brief       Determine the state of PPP link.
  \return      link state:
               - true  = Link is up, IP frames can be exchanged,
               - false = Link is down.
*/
bool netPPP_LinkUp (void) {
  NET_PPP_CFG *h = net_ppp_list[0];

  if (ctrl->Flags & PPP_FLAG_NETWORK) {
    return (true);
  }
  return (false);
}

/**
  \brief       Run main process for PPP interface.
*/
void net_ppp_iface_run (void) {
  NET_PPP_CFG *h = net_ppp_list[0];
  NET_FRAME *frame;

  if (sys->Flags & SYS_FLAG_TICK) {
    /* Sync timings for PPP thread */
    ctrl->th.SysTick = true;
    if (ctrl->th.OfflineSt) {
      /* Modem is offline, re-init PPP protocols */
      DEBUGF (PPP,"Dropping, Modem offline\n");
      EvrNetPPP_ModemOffline ();
      net_ppp_proto_init (h, false);
      ctrl->Id           = 0x00;
      ctrl->Flags        = 0x0000;
      ctrl->th.OfflineSt = false;
    }
    else {
      /* Run PPP protocols */
      net_lcp_run (h);
      if (h->PapAuth)  h->PapAuth->run (h);
      if (h->ChapAuth) h->ChapAuth->run (h);
      net_ipcp_run (h);
    }
    if ((sys->Flags & SYS_FLAG_SEC) && ctrl->EchoTimer) {
      if (--ctrl->EchoTimer == 0) {
        net_lcp_echo (h);
      }
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

  ppp_lock (h);
  frame = que_get_first (&ctrl->rx_list);
  ppp_unlock (h);

  ctrl->EchoTimer = h->EchoTout;
  if (ppp_process (frame) == true) {
    switch (ntohs(PPP_FRAME(frame)->Prot)) {
      case PPP_PROT_LCP:
        net_lcp_process (h, frame);
        break;

      case PPP_PROT_PAP:
        if (h->PapAuth)  h->PapAuth->process (h, frame);
        break;

      case PPP_PROT_CHAP:
        if (h->ChapAuth) h->ChapAuth->process (h, frame);
        break;

      case PPP_PROT_IPCP:
        if (!(ctrl->Flags & PPP_FLAG_OPENED)) {
          /* Silently ignore IPCP if LCP is not up */
          DEBUGF (PPP," Discarded, LCP down");
          EvrNetPPP_DataLinkDown ();
          break;
        }
        net_ipcp_process (h, frame);
        break;

      case PPP_PROT_IP:
        if (!(ctrl->Flags & PPP_FLAG_NETWORK)) {
          /* Reject IPv4 packets if network-layer not established */
          goto rej;
        }
        /* Check IPv4 protocol header */
        if (!net_ip4_chk_frame (h->If, frame)) {
          break;
        }
        /* Process fragmented frame reassembly */
        frame = net_ip4_reass_frame (frame);
        if (frame == NULL) {
          /* Reassembly has taken over this frame */
          return;
        }
        /* Now check IPv4 frame Protocol type */
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

       default:
         /* Can not handle this protocol type */
rej:     ppp_prot_reject (h, frame);
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
    for ( ; PPP_FRAME(next)->next; next = PPP_FRAME(next)->next);
    PPP_FRAME(next)->next = frame;
  }
  PPP_FRAME(frame)->next = NULL;
}

/**
  \brief       Get frame from the top of queue.
  \param[in]   list   queue list head.
  \return      pointer to frame.
*/
static NET_FRAME *que_get_first (NET_FRAME **list) {
  NET_FRAME *frame;

  frame = *list;
  *list = PPP_FRAME(*list)->next;
  return (frame);
}

/**
  \brief       Lock ppp interface.
  \param[in]   ppp_if  PPP interface handle.
*/
static void ppp_lock (NET_PPP_CFG *h) {
  netif_lock (ctrl->semaphore);
}

/**
  \brief       Unlock ppp interface.
  \param[in]   ppp_if  PPP interface handle.
*/
static void ppp_unlock (NET_PPP_CFG *h) {
  netif_unlock (ctrl->semaphore);
}

#ifdef Network_Debug_STDIO
/**
  \brief       Debug print user friendly PPP protocol.
  \param[in]   prot  protocol type for PPP.
*/
static void debug_info (uint16_t prot) {
  static const char p_asc[][5] = {
    "LCP",
    "PAP",
    "CHAP",
    "IPCP",
    "IP4"
  };
  static const uint16_t p_bin[] = {
    PPP_PROT_LCP,
    PPP_PROT_PAP,
    PPP_PROT_CHAP,
    PPP_PROT_IPCP,
    PPP_PROT_IP
  };
  uint32_t i;

  /* Print user friendly PPP protocol */
  for (i = 0; i < sizeof (p_bin)/sizeof(uint16_t); i++) {
    if (p_bin[i] == prot) {
      DEBUGF (PPP," Proto %s\n",p_asc[i]);
      return;
    }
  }
  DEBUGF (PPP," Unknown Proto (0x%04X)\n",prot);
}

/**
  \brief       Debug print PPP dialing information.
  \param[in]   msg  explanation message to print.
  \param[in]   val  pointer to dialing number.
*/
static void debug_inf2 (const char *msg, const char *val) {
  if (val != NULL) {
    DEBUGF (PPP," %s: %s\n",msg,val);
  }
}
#endif
