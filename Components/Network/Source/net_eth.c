/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_eth.c
 * Purpose: Ethernet Interface
 *----------------------------------------------------------------------------*/

#include "net_lib.h"
#include "Driver_ETH_MAC.h"
#include "Driver_ETH_PHY.h"
#include "net_mem.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_eth.h"
#include "net_arp.h"
#include "net_ip4.h"
#include "net_icmp.h"
#ifdef Network_IPv6
#include "net_ndp.h"
#include "net_ip6.h"
#include "net_icmp6.h"
#endif
#include "net_udp.h"
#include "net_tcp.h"
#include "net_dbg.h"

/* Global variables */
NET_ETH_CTRL net_eth0_if_control;
NET_ETH_CTRL net_eth1_if_control;

/* Code shortening macros */
#define ctrl       ( h->Ctrl)
#define LocM       (*h->If->localm)
#define LocM6      (*h->If->localm6)
#define drv_mac    ((ARM_DRIVER_ETH_MAC *)h->DrvMac)
#define drv_phy    ((ARM_DRIVER_ETH_PHY *)h->DrvPhy)

#define N_MCAST6    3

#ifdef ACHILLES_TEST
/* This structure is used to measure the ability of a Network to use  */
/* received ethernet packets in real time. The Achilles Test Software */
/* sends storm packets in the test. Sooner or later, the system starts*/
/* dropping the packets. The counters in this structure show errors   */
/* in the processing of received packets. When the storm is too high, */
/* the err_xxxx counters are incremented. This is an indication, that */
/* the system is overloaded. The test is bound to ETH0 interface.     */
struct {
  uint32_t n_int;           // Receive interrupts
  uint32_t n_fetch;         // Fetched frames
  uint32_t n_proc;          // Processed frames
  uint32_t err_size;        // Frame size errors
  uint32_t err_emac;        // CRC,SYM,Alignment errors
  uint32_t err_queue;       // Queue overflow errors
  uint32_t err_mem;         // Memory allocation errors
} eth_test;
#endif

/* Local Functions */
static void eth_iface_init (NET_ETH_CFG *h);
static void eth_iface_uninit (NET_ETH_CFG *h);
static uint32_t eth_opt_len (NET_ETH_CFG *h, netIF_Option option);
static uint16_t eth_mtu_limit (uint16_t mtu, uint8_t ip_ver);
static void eth_config_mcast (uint32_t if_num);
static void eth_receive (NET_ETH_CFG *h);
static void eth_check_link (NET_ETH_CFG *h);
static void eth_iface_run (NET_ETH_CFG *h);
static bool eth_vlan_accept (NET_ETH_CFG *h, NET_FRAME *frame);
static bool eth_is_ucast4 (const uint8_t *mac_addr);
static NET_ETH_CFG *eth_if_map (uint32_t if_num);
static void eth_lock (NET_ETH_CFG *h);
static void eth_unlock (NET_ETH_CFG *h);
#ifdef Network_Debug_STDIO
 static void debug_info (uint32_t link_info);
 static void debug_inf2 (const void *eth_header, uint32_t len);
#endif

/**
  \brief       Initialize ETH network interfaces.
*/
void net_eth_iface_init (void) {
  NET_ETH_CFG *const *p;

  for (p = &net_eth_list[0]; *p; p++) {
    eth_iface_init ((*p));
  }
}

/**
  \brief       Initialize an ethernet interface.
  \param[in]   h  ethernet interface handle.
*/
static void eth_iface_init (NET_ETH_CFG *h) {
  ARM_ETH_MAC_CAPABILITIES capab;
  uint16_t val;
  int32_t rc;

  DEBUGF (ETH,"Init_interface %d\n",h->IfNum);
  EvrNetETH_InitInterface (h->IfNum);
  memset (ctrl, 0, sizeof (*ctrl));

  /* Initialize Media Access Controller */
  capab = drv_mac->GetCapabilities ();
  drv_mac->Initialize ((capab.event_rx_frame) ? h->cb_event : NULL);
  drv_mac->PowerControl (ARM_POWER_FULL);

  /* Set MAC address */
  if (capab.mac_address) {
    /* Read hardware provided MAC address */
    drv_mac->GetMacAddress ((ARM_ETH_MAC_ADDR *)h->MacAddr);
  }
  else {
    /* Use MAC address from the configuration */
    if (!netMAC_aton (h->MacCfg, h->MacAddr) || (h->MacAddr[0] & 0x01)) {
      /* Invalid MAC address format, or Multicast bit is set in MAC0 */
      ERRORF (ETH,"Init %d, MAC config error\n",h->IfNum);
      EvrNetETH_MacAddressConfigError (h->IfNum);
      net_sys_error (NET_ERROR_CONFIG);
    }
  }
  drv_mac->SetMacAddress ((ARM_ETH_MAC_ADDR *)h->MacAddr);
  DEBUGF (ETH," MAC (%s)\n",net_mac_ntoa(h->MacAddr));
  EvrNetETH_SetMacAddress (h->IfNum, h->MacAddr);

  /* Set VLAN */
  if (h->VlanTag) {
    /* Try to configure VLAN in the driver */
    rc = drv_mac->Control (ARM_ETH_MAC_VLAN_FILTER,
                           ARM_ETH_MAC_VLAN_FILTER_ID_ONLY | h->VlanTag);
    if (rc == ARM_DRIVER_OK) {
      ctrl->VlanId = h->VlanTag;
      DEBUGF (ETH," VLAN (%d)\n",ctrl->VlanId);
      EvrNetETH_SetVlanIdentifier (h->IfNum, ctrl->VlanId);
    }
    else if (rc == ARM_DRIVER_ERROR_UNSUPPORTED) {
      ERRORF (ETH,"Init %d, VLAN config error\n",h->IfNum);
      EvrNetETH_VlanConfigError (h->IfNum);
      net_sys_error (NET_ERROR_CONFIG);
    }
    else {
      ERRORF (ETH,"Init %d, VLAN init failed\n",h->IfNum);
      EvrNetETH_VlanInitError (h->IfNum);
    }
  }

  /* Disable hardware multicast hash filtering */
  if ((h->If->Ip4Cfg && h->If->Ip4Cfg->IgmpCfg) || h->If->Ip6Cfg) {
    /* For IPv4-IGMP enabled or IPv6 enabled only */
    rc = drv_mac->SetAddressFilter (NULL, 0);
    if (rc == ARM_DRIVER_ERROR_UNSUPPORTED) {
      /* Multicast hash filtering not supported in the driver */
      h->Ctrl->Flags |= ETH_FLAG_MCAST_ALL;
    }
  }
  else {
    h->Ctrl->Flags |= ETH_FLAG_MCAST_NONE;
  }

  /* Set MTU */
  ctrl->Mtu = (ctrl->VlanId) ? (ETH_MAX_FRAME_SIZE + 4) : ETH_MAX_FRAME_SIZE;

  /* Initialize Physical Media Interface */
  rc = drv_phy->Initialize (drv_mac->PHY_Read, drv_mac->PHY_Write);
  if (rc == ARM_DRIVER_OK) {
    ctrl->Flags |= ETH_FLAG_PHY_OK;
    drv_phy->PowerControl (ARM_POWER_FULL);
    drv_phy->SetInterface (capab.media_interface);
    drv_phy->SetMode (ARM_ETH_PHY_AUTO_NEGOTIATE);
  }
  else if (rc == ARM_DRIVER_ERROR_UNSUPPORTED) {
    ERRORF (ETH,"Init %d, PHY config error\n",h->IfNum);
    EvrNetETH_PhyDriverConfigError (h->IfNum);
    net_sys_error (NET_ERROR_CONFIG);
  }
  else {
    ERRORF (ETH,"Init %d, PHY init failed\n",h->IfNum);
    EvrNetETH_PhyDriverInitError (h->IfNum);
  }
  /* Init IP4 protocol and local machine info */
  if (h->If->Ip4Cfg) {
    LocM.Mtu = eth_mtu_limit (h->If->Ip4Cfg->Mtu, IP_VER4);
    if (!net_addr4_aton (h->If->Ip4Cfg->IpAddr, LocM.IpAddr)  ||
        !net_addr4_aton (h->If->Ip4Cfg->NetMask,LocM.NetMask) ||
        !net_addr4_aton (h->If->Ip4Cfg->DefGW,  LocM.DefGW)   ||
        !net_addr4_aton (h->If->Ip4Cfg->PriDNS, LocM.PriDNS)  ||
        !net_addr4_aton (h->If->Ip4Cfg->SecDNS, LocM.SecDNS)) {
      ERRORF (ETH,"Init %d, IPv4 config error\n",h->IfNum);
      EvrNetETH_Ip4ConfigError (h->IfNum);
      net_sys_error (NET_ERROR_CONFIG);
    }
  }

#ifdef Network_IPv6
  /* Init IP6 protocol and local machine info */
  if (h->If->Ip6Cfg) {
    LocM6.Mtu = eth_mtu_limit (h->If->Ip6Cfg->Mtu, IP_VER6);
    LocM6.HopLimit  = 128;
    LocM6.PrefixLen = h->If->Ip6Cfg->PrefixLen;
    if (!net_addr6_aton (h->If->Ip6Cfg->IpAddr, LocM6.IpAddr) ||
        !net_addr6_aton (h->If->Ip6Cfg->DefGW,  LocM6.DefGW)  ||
        !net_addr6_aton (h->If->Ip6Cfg->PriDNS, LocM6.PriDNS) ||
        !net_addr6_aton (h->If->Ip6Cfg->SecDNS, LocM6.SecDNS)) {
      ERRORF (ETH,"Init %d, IPv6 config error\n",h->IfNum);
      EvrNetETH_Ip6ConfigError (h->IfNum);
      net_sys_error (NET_ERROR_CONFIG);
    }
  }
#endif

  /* Max. number of multicast addresses */
  ctrl->n_mcast = 0;
  if (h->If->Ip4Cfg && h->If->Ip4Cfg->IgmpCfg) {
    ctrl->n_mcast += h->If->Ip4Cfg->IgmpCfg->TabSize + 1;
  }
#ifdef Network_IPv6
  ctrl->n_mcast += N_MCAST6;
  if (h->If->Ip6Cfg && h->If->Ip6Cfg->MldCfg) {
    ctrl->n_mcast += h->If->Ip6Cfg->MldCfg->TabSize;
  }
#endif

  /* Seed for pseudo-random generator based on MAC address */
  sys->RndState = net_crc_buf (h->MacAddr, NET_ADDR_ETH_LEN, sys->RndState);

  /* Get checksum offload capabilities of the driver */
  val = 0;
  if (capab.checksum_offload_rx_ip4) {
    val |= SYS_OFFL_IP4_RX;
  }
  if (capab.checksum_offload_rx_udp) {
    val |= SYS_OFFL_UDP4_RX;
#ifdef Network_IPv6
    val |= SYS_OFFL_UDP6_RX;
#endif
  }
  if (capab.checksum_offload_rx_tcp) {
    val |= SYS_OFFL_TCP4_RX;
#ifdef Network_IPv6
    val |= SYS_OFFL_TCP6_RX;
#endif
  }
  if (capab.checksum_offload_rx_icmp) {
    val |= SYS_OFFL_ICMP4_RX;
#ifdef Network_IPv6
    val |= SYS_OFFL_ICMP6_RX;
#endif
  }
  if (capab.checksum_offload_tx_ip4) {
    val |= SYS_OFFL_IP4_TX;
  }
  if (capab.checksum_offload_tx_udp) {
    val |= SYS_OFFL_UDP4_TX;
#ifdef Network_IPv6
    val |= SYS_OFFL_UDP6_TX;
#endif
  }
  if (capab.checksum_offload_tx_tcp) {
    val |= SYS_OFFL_TCP4_TX;
#ifdef Network_IPv6
    val |= SYS_OFFL_TCP6_TX;
#endif
  }
  if (capab.checksum_offload_tx_icmp) {
    val |= SYS_OFFL_ICMP4_TX;
#ifdef Network_IPv6
    val |= SYS_OFFL_ICMP6_TX;
#endif
  }

#ifdef Network_IPv6
  if (!capab.checksum_offload_rx_ip6) {
    /* IPv6 cksum check not supported */
    val &= ~SYS_OFFL_RX6;
  }
  if (!capab.checksum_offload_tx_ip6) {
    /* IPv6 cksum generation not supported */
    val &= ~SYS_OFFL_TX6;
  }
#endif
  if (!h->If->Ip4Cfg) {
    /* IPv4 not enabled */
    val &= ~(SYS_OFFL_RX4 | SYS_OFFL_TX4);
  }
#ifdef Network_IPv6
  if (!h->If->Ip6Cfg) {
    /* IPv6 not enabled */
    val &= ~(SYS_OFFL_RX6 | SYS_OFFL_TX6);
  }
#endif
  h->If->State->Offload = val;

  if (!capab.event_rx_frame) {
    /* Use polling mode */
    ctrl->Flags |= ETH_FLAG_POLLING;
  }

  /* Create OS objects for ethernet interface */
  ctrl->thread = netif_create (h->If->Id , &ctrl->semaphore);
  if (ctrl->thread == NULL) {
    ERRORF (ETH,"Init %d, Thread create failed\n",h->IfNum);
    EvrNetETH_ThreadCreateFailed (h->IfNum);
    net_sys_error (NET_ERROR_CONFIG);
  }
  if (ctrl->semaphore == NULL) {
    ERRORF (ETH,"Init %d, Semaphore create failed\n",h->IfNum);
    EvrNetETH_SemaphoreCreateFailed (h->IfNum);
    net_sys_error (NET_ERROR_CONFIG);
  }

  /* Send a notification at the end of initialization */
  if (ctrl->Flags & ETH_FLAG_PHY_OK) {
    netETH_Notify (h->IfNum, netETH_LinkDown, 0);
  }
}

/**
  \brief       De-initialize ETH network interface.
*/
void net_eth_iface_uninit (void) {
  NET_ETH_CFG *const *p;

  for (p = &net_eth_list[0]; *p; p++) {
    eth_iface_uninit ((*p));
  }
}

/**
  \brief       De-initialize ethernet network interface.
  \param[in]   h  ethernet interface handle.
*/
static void eth_iface_uninit (NET_ETH_CFG *h) {

  DEBUGF (ETH,"Uninit_interface %d\n",h->IfNum);
  EvrNetETH_UninitInterface (h->IfNum);

  /* De-initialize Physical Media Interface */
  drv_phy->PowerControl (ARM_POWER_OFF);
  drv_phy->Uninitialize ();
  netETH_Notify (h->IfNum, netETH_LinkDown, 0);

  /* De-initialize Media Access Controller */
  memset (h->MacAddr, 0, NET_ADDR_ETH_LEN);
  drv_mac->SetMacAddress ((ARM_ETH_MAC_ADDR *)h->MacAddr);
  drv_mac->PowerControl (ARM_POWER_OFF);
  drv_mac->Uninitialize ();

  /* De-init IP4 protocol and local machine info */
  if (h->If->Ip4Cfg) {
    memset (&LocM, 0, sizeof (LocM));
  }

#ifdef Network_IPv6
  /* De-init IP6 protocol and local machine info */
  if (h->If->Ip6Cfg) {
    memset (&LocM6, 0, sizeof (LocM6));
  }
#endif

  /* Delete OS objects for ethernet interface */
  netif_delete (ctrl->thread, ctrl->semaphore);
  memset (ctrl, 0, sizeof(*ctrl));
}

/**
  \brief       Ethernet interface thread.
  \param[in]   arg  ethernet interface handle.
*/
__NO_RETURN void netETH_Thread (void *arg) {
  NET_ETH_CFG *h = (NET_ETH_CFG *)arg;

  while (1) {
    /* Wait for link_up status */
    netos_delay (300);
    if (ctrl->Flags & ETH_FLAG_PHY_OK) {
      /* Protect driver access */
      eth_lock (h);
      eth_check_link (h);
      eth_unlock (h);
      if (ctrl->th.LinkState == ARM_ETH_LINK_UP) {
        break;
      }
    }
  }

  ctrl->th.SecTick = false;
  ctrl->th.osDelay = 25;
  while (1) {
    /* Adaptive delays based on RX activity */
    netos_flag_wait (0x0001, ctrl->th.osDelay);
    if (ctrl->th.osDelay < 25) {
      ctrl->th.osDelay++;
    }
    /* Protect driver access */
    eth_lock (h);
    eth_receive (h);
    if (ctrl->th.SecTick) {
      ctrl->th.SecTick = false;
      eth_check_link (h);
    }
    eth_unlock (h);
  }
}

/**
  \brief       Event notification on ethernet receive.
  \param[in]   event  receive event.
  \param[in]   ctx    interface context.
  \note        This function is called from eth interrupt.
*/
void net_eth_callback (uint32_t event, NET_ETH_CTRL *ctx) {
  if (event & ARM_ETH_MAC_EVENT_RX_FRAME) {
#ifdef ACHILLES_TEST
    if (ctx == &net_eth0_if_control) eth_test.n_int++;
#endif
    netos_flag_set (ctx->thread, 0x0001);
  }
}

/**
  \brief       Get current value of ETH Interface option.
  \param[in]   if_num   interface number.
  \param[in]   option   option as specified by netIF_Option.
  \param[out]  buf      buffer to store the option value to
  \param[in]   buf_len  length of buffer
  \return      status code as defined with netStatus.
  \note        This function is called from a protected function.
*/
netStatus net_eth_get_option (uint32_t if_num, netIF_Option option,
                                               uint8_t *buf, uint32_t buf_len) {
  NET_ETH_CFG *h = eth_if_map (if_num);
  uint32_t opt_len;

  if (h == NULL || buf == NULL || buf_len == 0) {
    goto invalid_param;
  }
  opt_len = eth_opt_len (h, option);
  if (opt_len == 0 || opt_len > buf_len) {
    /* Return opt_len=0, when IP4 or IP6 disabled */
    goto invalid_param;
  }
  /* Read option value */
  switch ((int32_t)option) {
    case netIF_OptionMAC_Address:
      net_mac_copy (buf, h->MacAddr);
      return (netOK);

    case netIF_OptionVLAN_Identifier:
      /* Possibly non-aligned buffer */
      memcpy (buf, &ctrl->VlanId, 2);
      return (netOK);

    case netIF_OptionIP4_Address:
      net_addr4_copy (buf, LocM.IpAddr);
      return (netOK);

    case netIF_OptionIP4_SubnetMask:
      net_addr4_copy (buf, LocM.NetMask);
      return (netOK);

    case netIF_OptionIP4_DefaultGateway:
      net_addr4_copy (buf, LocM.DefGW);
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

#ifdef Network_IPv6
    case netIF_OptionIP6_LinkLocalAddress:
      net_addr6_copy (buf, LocM6.LLAddr);
      return (netOK);

    case netIF_OptionIP6_StaticAddress:
      net_addr6_copy (buf, LocM6.IpAddr);
      return (netOK);

    case netIF_OptionIP6_DynamicAddress:
      net_addr6_copy (buf, LocM6.TempAddr);
      return (netOK);

    case netIF_OptionIP6_SubnetPrefixLength:
      buf[0] = LocM6.PrefixLen;
      return (netOK);

    case netIF_OptionIP6_DefaultGateway:
      net_addr6_copy (buf, LocM6.DefGW);
      return (netOK);

    case netIF_OptionIP6_PrimaryDNS:
      net_addr6_copy (buf, LocM6.PriDNS);
      return (netOK);

    case netIF_OptionIP6_SecondaryDNS:
      net_addr6_copy (buf, LocM6.SecDNS);
      return (netOK);

    case netIF_OptionIP6_MTU:
      /* Possibly non-aligned buffer */
      memcpy (buf, &LocM6.Mtu, 2);
      return (netOK);
#endif
    default:
      break;
  }
invalid_param:
  ERRORF (ETH,"GetOption %d, Invalid parameter\n",h->IfNum);
  EvrNetETH_GetOptionInvalidParameter (h->IfNum);
  return (netInvalidParameter);
}

/**
  \brief       Set the value of ETH Interface option.
  \param[in]   if_num   interface number.
  \param[in]   option   option as specified by netIF_Option.
  \param[out]  buf      buffer containing the option value
  \param[in]   buf_len  length of buffer
  \return      status code as defined with netStatus.
  \note        This function is called from a protected function.
*/
netStatus net_eth_set_option (uint32_t if_num, netIF_Option option,
                                               const uint8_t *buf, uint32_t buf_len) {
  NET_ETH_CFG *h = eth_if_map (if_num);
  uint16_t bval16;
  int32_t rc;

  if (h == NULL || buf == NULL || buf_len == 0) {
    goto invalid_param;
  }
  if (buf_len != eth_opt_len (h, option)) {
    /* Return opt_len=0, when IP4 or IP6 disabled */
    goto invalid_param;
  }
  /* Write option value */
  switch ((int32_t)option) {
    case netIF_OptionMAC_Address:
      /* Error if Multicast bit is set */
      if (buf[0] & 0x01) break;
      DEBUGF (ETH," MAC_Addr=%s\n",net_mac_ntoa(buf));
      EvrNetETH_SetMacAddress (h->IfNum, buf);
      if (!net_mac_comp (buf, h->MacAddr)) {
        net_mac_copy (h->MacAddr, buf);
        /* Configure MAC address in the driver when MAC changed */
        eth_lock (h);
        drv_mac->SetMacAddress ((ARM_ETH_MAC_ADDR *)h->MacAddr);
        eth_unlock (h);
        /* Update the pseudo-random generator from the new MAC address */
        sys->RndState = net_crc_buf (h->MacAddr, NET_ADDR_ETH_LEN, sys->RndState);
        h->If->State->MacNew = true;
      }
      return (netOK);

    case netIF_OptionVLAN_Identifier:
      /* Possibly non-aligned buffer */
      memcpy (&bval16, buf, 2);
      if (bval16 > 4093) bval16 = 4093;
      /* Try to configure VLAN in the driver */
      eth_lock (h);
      rc = drv_mac->Control (ARM_ETH_MAC_VLAN_FILTER, (bval16 == 0) ? 0 :
                             bval16 | ARM_ETH_MAC_VLAN_FILTER_ID_ONLY);
      eth_unlock (h);
      if (rc != ARM_DRIVER_OK) {
        /* Not supported by the driver */
        goto invalid_param;
      }
      ctrl->VlanId = bval16;
      ctrl->Mtu    = (bval16) ? (ETH_MAX_FRAME_SIZE + 4) : ETH_MAX_FRAME_SIZE;
      DEBUGF (ETH," VLAN_Id=%d\n",ctrl->VlanId);
      EvrNetETH_SetVlanIdentifier (h->IfNum, ctrl->VlanId);
      return (netOK);

    case netIF_OptionIP4_Address:
      DEBUGF (ETH," IP4_Addr=%s\n",net_addr4_ntoa(buf));
      EvrNetETH_SetIp4Address (h->IfNum, buf);
      if (!net_addr4_comp (LocM.IpAddr, buf)) {
        net_addr4_copy (LocM.IpAddr, buf);
        /* Send Gratuitous ARP when IP address changed */
        net_arp_notify (h->If);
      }
      return (netOK);

    case netIF_OptionIP4_SubnetMask:
      DEBUGF (ETH," NetMask=%s\n",net_addr4_ntoa(buf));
      EvrNetETH_SetIp4SubnetMask (h->IfNum, buf);
      net_addr4_copy (LocM.NetMask, buf);
      return (netOK);

    case netIF_OptionIP4_DefaultGateway:
      DEBUGF (ETH," Def_GW=%s\n",net_addr4_ntoa(buf));
      EvrNetETH_SetIp4DefaultGateway (h->IfNum, buf);
      net_addr4_copy (LocM.DefGW, buf);
      return (netOK);

    case netIF_OptionIP4_PrimaryDNS:
      DEBUGF (ETH," Pri_DNS=%s\n",net_addr4_ntoa(buf));
      EvrNetETH_SetIp4PrimaryDNS (h->IfNum, buf);
      net_addr4_copy (LocM.PriDNS, buf);
      return (netOK);

    case netIF_OptionIP4_SecondaryDNS:
      DEBUGF (ETH," Sec_DNS=%s\n",net_addr4_ntoa(buf));
      EvrNetETH_SetIp4SecondaryDNS (h->IfNum, buf);
      net_addr4_copy (LocM.SecDNS, buf);
      return (netOK);

    case netIF_OptionIP4_MTU:
      /* Possibly non-aligned buffer */
      memcpy (&bval16, buf, 2);
      LocM.Mtu = eth_mtu_limit (bval16, IP_VER4);
      DEBUGF (ETH," IP4_MTU=%d\n",LocM.Mtu);
      EvrNetETH_SetIp4Mtu (h->IfNum, LocM.Mtu);
      return (netOK);

#ifdef Network_IPv6
    case netIF_OptionIP6_StaticAddress:
      DEBUGF (ETH," IP6_Addr=%s\n",net_addr6_ntoa(buf));
      EvrNetETH_SetIp6Address (h->IfNum, buf);
      net_addr6_copy (LocM6.IpAddr, buf);
      eth_config_mcast (h->IfNum);
      return (netOK);

    case netIF_OptionIP6_DefaultGateway:
      DEBUGF (ETH," Def_GW6=%s\n",net_addr6_ntoa(buf));
      EvrNetETH_SetIp6DefaultGateway (h->IfNum, buf);
      net_addr6_copy (LocM6.DefGW, buf);
      return (netOK);

    case netIF_OptionIP6_PrimaryDNS:
      DEBUGF (ETH," Pri_DNS6=%s\n",net_addr6_ntoa(buf));
      EvrNetETH_SetIp6PrimaryDNS (h->IfNum, buf);
      net_addr6_copy (LocM6.PriDNS, buf);
      return (netOK);

    case netIF_OptionIP6_SecondaryDNS:
      DEBUGF (ETH," Sec_DNS6=%s\n",net_addr6_ntoa(buf));
      EvrNetETH_SetIp6SecondaryDNS (h->IfNum, buf);
      net_addr6_copy (LocM6.SecDNS, buf);
      return (netOK);

    case netIF_OptionIP6_SubnetPrefixLength:
      /* Error if prefix length out of range */
      if ((buf[0] == 0) || (buf[0] > 127)) break;
      DEBUGF (ETH," PrefixLen=%d\n",buf[0]);
      EvrNetETH_SetIp6PrefixLength (h->IfNum, buf[0]);
      LocM6.PrefixLen = buf[0];
      return (netOK);

    case netIF_OptionIP6_MTU:
      /* Possibly non-aligned buffer */
      memcpy (&bval16, buf, 2);
      LocM6.Mtu = eth_mtu_limit (bval16, IP_VER6);
      DEBUGF (ETH," IP6_MTU=%d\n",LocM6.Mtu);
      EvrNetETH_SetIp6Mtu (h->IfNum, LocM6.Mtu);
      return (netOK);
#endif
    default:
      /* Read only options */
      break;
  }
invalid_param:
  ERRORF (ETH,"SetOption %d, Invalid parameter\n",h->IfNum);
  EvrNetETH_SetOptionInvalidParameter (h->IfNum);
  return (netInvalidParameter);
}

/**
  \brief       Get requested interface option length.
  \param[in]   h       ethernet interface handle.
  \param[in]   option  option as specified by netIF_Option.
  \return      length in bytes, =0 when IPv4 or IPv6 not enabled.
*/
static uint32_t eth_opt_len (NET_ETH_CFG *h, netIF_Option option) {
  switch ((int32_t)option) {
    /* MAC address option */
    case netIF_OptionMAC_Address:
      return (NET_ADDR_ETH_LEN);

    /* VLAN identifier option */
    case netIF_OptionVLAN_Identifier:
      return (2);

    /* IPv4 options */
    case netIF_OptionIP4_Address:
    case netIF_OptionIP4_SubnetMask:
    case netIF_OptionIP4_DefaultGateway:
    case netIF_OptionIP4_PrimaryDNS:
    case netIF_OptionIP4_SecondaryDNS:
      if (!h->If->Ip4Cfg) break;
      return (NET_ADDR_IP4_LEN);

    case netIF_OptionIP4_MTU:
      if (!h->If->Ip4Cfg) break;
      return (2);

#ifdef Network_IPv6
    /* IPv6 options */
    case netIF_OptionIP6_LinkLocalAddress:
    case netIF_OptionIP6_StaticAddress:
    case netIF_OptionIP6_DynamicAddress:
    case netIF_OptionIP6_DefaultGateway:
    case netIF_OptionIP6_PrimaryDNS:
    case netIF_OptionIP6_SecondaryDNS:
      if (!h->If->Ip6Cfg) break;
      return (NET_ADDR_IP6_LEN);

    case netIF_OptionIP6_SubnetPrefixLength:
      if (!h->If->Ip6Cfg) break;
      return (1);

    case netIF_OptionIP6_MTU:
      if (!h->If->Ip6Cfg) break;
      return (2);
#endif
    default:
      break;
  }
  /* Invalid option */
  return (0);
}

/**
  \brief       Limit the range of MTU.
  \param[in]   mtu     input MTU value.
  \param[in]   ip_ver  IP version.
  \return      limited value of input MTU.
  \note        IPv4  576..1500 bytes
               IPv6 1280..1500 bytes
*/
static uint16_t eth_mtu_limit (uint16_t mtu, uint8_t ip_ver) {
  uint16_t min_mtu;

  if (mtu != 0) {
    min_mtu = (ip_ver == IP_VER4) ? 576 : 1280;
    if (mtu <  min_mtu) mtu = min_mtu;
    if (mtu > 1500)     mtu = 1500;
  }
  return (mtu);
}

/**
  \brief       Configure hardware multicast address filtering.
  \param[in]   if_num  interface number.
*/
static void eth_config_mcast (uint32_t if_num) {
  NET_ETH_CFG *h = eth_if_map (if_num);
  uint8_t *buf;
  uint32_t n;

  if ((h == NULL) || (ctrl->th.LinkState == ARM_ETH_LINK_DOWN)) {
    return;
  }
  if (ctrl->Flags & (ETH_FLAG_MCAST_ALL | ETH_FLAG_MCAST_NONE)) {
    /* Multicasts: all accepted or not used, no filtering */
    return;
  }
  /* Provide buffer big enough (add one more for safety) */
  buf = (uint8_t *)net_mem_alloc ((ctrl->n_mcast + 1) * NET_ADDR_ETH_LEN);
  n   = 0;

  /* Collect IPv4 multicast MAC addresses */
  if (h->If->Ip4Cfg) {
    if (h->If->Ip4Cfg->IgmpCfg) {
      n += h->If->Ip4Cfg->IgmpCfg->collect_mcast (h->If, &buf[0]);
    }
  }
#ifdef Network_IPv6
  /* Collect IPv6 multicast MAC addresses */
  if (h->If->Ip6Cfg) {
    n += h->If->Ip6Cfg->collect_mcast (h->If, &buf[n * NET_ADDR_ETH_LEN]);
    if (h->If->Ip6Cfg->MldCfg) {
      n += h->If->Ip6Cfg->MldCfg->collect_mcast (h->If, &buf[n * NET_ADDR_ETH_LEN]);
    }
  }
#endif
  if (n != 0) {
    eth_lock (h);
    drv_mac->SetAddressFilter ((ARM_ETH_MAC_ADDR *)buf, n);
    eth_unlock (h);
  }
  net_mem_free (__FRAME(buf));
}

/**
  \brief       Receive ethernet frame.
  \param[in]   h  ethernet interface handle.
  \details     Called from ETH thread!
*/
static void eth_receive (NET_ETH_CFG *h) {
  NET_FRAME *frame;
  uint32_t size;

  /* Fetch all available frames */
  while ((size = drv_mac->GetRxFrameSize ()) != 0) {
#ifdef ACHILLES_TEST
    if (h->IfNum == 0) eth_test.n_fetch++;
#endif
    if ((size < PHY_HEADER_LEN) || (size > ctrl->Mtu)) {
      /* Frame error, release it */
#ifdef ACHILLES_TEST
      if (h->IfNum == 0) {
        if (size < 0x10000) eth_test.err_size++;
        else                eth_test.err_emac++;
      }
#endif
      drv_mac->ReadFrame (NULL, 0);
      continue;
    }
    if (((ctrl->q_head - ctrl->q_tail) & 0xFF) >= ETH_QSIZE) {
      /* Queue overflow, dump this frame */
#ifdef ACHILLES_TEST
      if (h->IfNum == 0) eth_test.err_queue++;
#endif
      drv_mac->ReadFrame (NULL, 0);
      continue;
    }
    /* Do not call sys_error() if out of memory */
    frame = net_mem_alloc (size | 0x80000000);
    if (frame == NULL) {
      /* Out of memory, dump this frame */
#ifdef ACHILLES_TEST
      if (h->IfNum == 0) eth_test.err_mem++;
#endif
      drv_mac->ReadFrame (NULL, 0);
      continue;
    }
    /* Valid frame, read it and release it */
    drv_mac->ReadFrame (&frame->data[0], size);
    ctrl->rx_q[ctrl->q_head & (ETH_QSIZE-1)] = frame;
    ctrl->RxCount += size;
    ctrl->q_head++;
  }
  if (ctrl->q_head == ctrl->q_tail) {
    return;
  }
  if (ctrl->Flags & ETH_FLAG_POLLING) {
    ctrl->th.osDelay = 2;
  }
  net_sys_wakeup ();
}

/**
  \brief       Check ethernet link state.
  \param[in]   h  ethernet interface handle.
  \details     Called from ETH thread!
*/
static void eth_check_link (NET_ETH_CFG *h) {
  ARM_ETH_LINK_STATE link;
  uint32_t arg;

  link = drv_phy->GetLinkState ();
  if (link == ctrl->th.LinkState) {
    /* No change */
    return;
  }
  /* Link state has changed */
  ctrl->th.LinkState = link;
  if (ctrl->th.LinkState == ARM_ETH_LINK_UP) {
    /* Start EMAC DMA */
    ARM_ETH_LINK_INFO info = drv_phy->GetLinkInfo ();
    /* Info structure is compatible with NET_ETH_LINK_INFO */
    ctrl->th.LinkInfo = (info.duplex << 2 | info.speed) & 0xFF;
    /* Enable checksum offload, based on capabilities */
    arg = (uint32_t)info.speed  << ARM_ETH_MAC_SPEED_Pos |
          (uint32_t)info.duplex << ARM_ETH_MAC_DUPLEX_Pos;
    if (h->If->Ip4Cfg) {
      arg |= ARM_ETH_MAC_ADDRESS_BROADCAST;
    }
    if (ctrl->Flags & ETH_FLAG_MCAST_ALL) {
      arg |= ARM_ETH_MAC_ADDRESS_MULTICAST;
    }
    if (h->If->State->Offload & SYS_OFFL_RX_ALL) {
      arg |= ARM_ETH_MAC_CHECKSUM_OFFLOAD_RX;
    }
    if (h->If->State->Offload & SYS_OFFL_TX_ALL) {
      arg |= ARM_ETH_MAC_CHECKSUM_OFFLOAD_TX;
    }
    drv_mac->Control (ARM_ETH_MAC_CONFIGURE, arg);
    drv_mac->Control (ARM_ETH_MAC_CONTROL_RX, 1);
    drv_mac->Control (ARM_ETH_MAC_CONTROL_TX, 1);
  }
  else {
    /* Stop EMAC DMA */
    drv_mac->Control (ARM_ETH_MAC_CONTROL_RX, 0);
    drv_mac->Control (ARM_ETH_MAC_CONTROL_TX, 0);
    drv_mac->Control (ARM_ETH_MAC_FLUSH,
                      ARM_ETH_MAC_FLUSH_RX | ARM_ETH_MAC_FLUSH_TX);
  }
  ctrl->th.ChangeSt = true;
}

/**
  \brief       Construct ethernet header and send frame.
  \param[in]   if_num  interface number.
  \param[in]   frame   network frame.
  \param[in]   ip_ver  IP version.
  \return      send status:
               - true  = success,
               - false = failed.
*/
bool net_eth_send_frame (uint32_t if_num, NET_FRAME *frame, uint8_t ip_ver) {
  NET_ETH_CFG  *h = eth_if_map (if_num);
  NET_ARP_INFO *arp_t;
#ifdef Network_IPv6
  NET_NDP_INFO *ndp_t;
#endif

  if (h == NULL) {
    return (false);
  }
  DEBUGF (ETH,"Send_frame %d\n",h->IfNum);
  EvrNetETH_SendFrame (h->IfNum, frame->length-PHY_HEADER_LEN, ip_ver);
  if (ctrl->th.LinkState == ARM_ETH_LINK_DOWN) {
    /* Discard frame, the link is down */
    ERRORF (ETH,"Send %d, Link down\n",h->IfNum);
    EvrNetETH_LinkDownError(h->IfNum);
    return (false);
  }
  if (frame->length > (PHY_HEADER_LEN + ETH_MTU)) {
    /* Discard frame, payload is too large */
    ERRORF (ETH,"Send %d, Data too large\n",h->IfNum);
    EvrNetETH_SendDataTooLarge(h->IfNum, frame->length-PHY_HEADER_LEN, ETH_MTU);
    return (false);
  }

  /* Set destination MAC address */
  switch (ip_ver) {
    case IP_VER4:
      if (!h->If->Ip4Cfg) {
        ERRORF (ETH,"Send %d, IP4 disabled\n",h->IfNum);
        EvrNetETH_SendIp4Disabled (h->IfNum);
        return (false);
      }
      /* Set protocol type IP4*/
      ETH_FRAME(frame)->Prot = HTONS(ETH_PROT_IP4);
      if (net_addr4_comp (IP4_FRAME(frame)->DstAddr, net_addr_bcast)) {
        /* Broadcast IPv4 destination address */
        net_mac_copy (ETH_FRAME(frame)->DstAddr, net_addr_bcast);
        break;
      }
      if (get_u32 (IP4_FRAME(frame)->SrcAddr) == 0) {
        /* Local IP address invalid or not set by DHCP */
        ERRORF (ETH,"Send %d, Local-IP4 undefined\n",h->IfNum);
        EvrNetETH_Ip4LocalAddressUndefined (h->IfNum);
        return (false);
      }
      /* Check if IP Multicast address */
      if ((IP4_FRAME(frame)->DstAddr[0] & 0xF0) == 0xE0) {
        /* Multicast IP range: 224.0.0.0 - 239.255.255.255 */
        /* Map IPv4 address to multicast MAC address       */
        net_addr4_to_mac (IP4_FRAME(frame)->DstAddr, ETH_FRAME(frame)->DstAddr);
        break;
      }
      /* Check if it is a subnet broadcast */
      if (net_addr4_is_subcast (IP4_FRAME(frame)->DstAddr, h->If->localm)) {
        net_mac_copy (ETH_FRAME(frame)->DstAddr, net_addr_bcast);
        break;
      }
      /* Try to get MAC address from ARP cache */
      arp_t = net_arp_cache_find (h->If, IP4_FRAME(frame)->DstAddr);
      if (arp_t == NULL) {
        /* Requested IP is external but gateway unspecified */
        ERRORF (ETH,"Send %d, Unresolved MAC for %s\n",h->IfNum,
                     net_addr4_ntoa(IP4_FRAME(frame)->DstAddr));
        EvrNetETH_Ip4MacAddressUnresolved (h->IfNum, IP4_FRAME(frame)->DstAddr);
        return (false);
      }
      if (arp_t->State < ARP_STATE_RESOLVED) {
        /* MAC resolving in progress, add frame to the queue */
        DEBUGF (ETH," Add Que=%d, len=%d\n",arp_t->Id,frame->length);
        EvrNetETH_EnqueueFrame (h->IfNum, arp_t->Id, frame->length);
        return (net_arp_enqueue (h->If, arp_t, frame));
      }
      net_mac_copy (ETH_FRAME(frame)->DstAddr, arp_t->MacAddr);
      break;
#ifdef Network_IPv6
    case IP_VER6:
      if (!h->If->Ip6Cfg) {
        ERRORF (ETH,"Send %d, IP6 disabled\n",h->IfNum);
        EvrNetETH_SendIp6Disabled (h->IfNum);
        return (false);
      }
      /* Set protocol type IP6 */
      ETH_FRAME(frame)->Prot = HTONS(ETH_PROT_IP6);
      if (IP6_FRAME(frame)->DstAddr[0] == 0xFF) {
        /* Multicast destination, prefix "ff00::/8"  */
        /* Map IPv6 address to multicast MAC address */
        net_addr6_to_mac (IP6_FRAME(frame)->DstAddr, ETH_FRAME(frame)->DstAddr);
        break;
      }
      /* Unicast destination */
      if (net_addr6_is_unspec (IP6_FRAME(frame)->SrcAddr)) {
        /* Local IPv6 address invalid or not set by DHCP */
        ERRORF (ETH,"Send %d, Local-IP6 undefined\n",h->IfNum);
        EvrNetETH_Ip6LocalAddressUndefined (h->IfNum);
        return (false);
      }
      /* Try to get MAC address from Neigbor cache */
      ndp_t = net_ndp_cache_find (h->If, IP6_FRAME(frame)->DstAddr);
      if (ndp_t == NULL) {
        /* This entry was not found */
        ERRORF (ETH,"Send %d, Unresolved MAC for [%s]\n",h->IfNum,
                     net_addr6_ntoa(IP6_FRAME(frame)->DstAddr));
        EvrNetETH_Ip6MacAddressUnresolved (h->IfNum, IP6_FRAME(frame)->DstAddr);
        return (false);
      }
      if (ndp_t->State < NDP_STATE_RESOLVED) {
        /* MAC resolving in progress, add frame to the queue */
        DEBUGF (ETH," Add Que=%d, len=%d\n",ndp_t->Id,frame->length);
        EvrNetETH_EnqueueFrame (h->IfNum, ndp_t->Id, frame->length);
        return (net_ndp_enqueue (h->If, ndp_t, frame));
      }
      net_mac_copy (ETH_FRAME(frame)->DstAddr, ndp_t->MacAddr);
      break;
#endif
    default:
      ERRORF (ETH,"Send %d, Invalid IP version\n",h->IfNum);
      EvrNetETH_InvalidIpVersion (h->IfNum, ip_ver);
      return (false);
  }

  /* Set source MAC address */
  net_mac_copy (ETH_FRAME(frame)->SrcAddr, h->MacAddr);

  DEBUG_INF2 (frame->data, frame->length);
  EvrNetETH_ShowFrameHeader (frame->data);
  return (net_eth_output (h->IfNum, frame));
}

/**
  \brief       User send raw ethernet data.
  \param[in]   if_num  interface number.
  \param[in]   buf     buffer containing the data.
  \param[in]   len     length of data in bytes.
  \return      status code as defined with netStatus.
*/
netStatus netETH_SendRaw (uint32_t if_num, const uint8_t *buf, uint32_t len) {
  NET_ETH_CFG *h = eth_if_map (if_num);
  int32_t i,rc;

  START_LOCK (netStatus);

  DEBUGF (ETH,"SendRaw %d\n",h->IfNum);
  EvrNetETH_SendRawFrame (h->IfNum, len);
  if (h == NULL || buf == NULL || len < PHY_HEADER_LEN || len > ctrl->Mtu) {
    ERRORF (ETH,"SendRaw %d, Invalid parameter\n",h->IfNum);
    EvrNetETH_SendRawInvalidParameter (h->IfNum);
    RETURN (netInvalidParameter);
  }
  if (ctrl->th.LinkState == ARM_ETH_LINK_DOWN) {
    ERRORF (ETH,"SendRaw %d, Link down\n",h->IfNum);
    EvrNetETH_LinkDownError(h->IfNum);
    RETURN (netWrongState);
  }

  DEBUG_INF2 (buf, len);
  EvrNetETH_ShowFrameHeader (buf);

  eth_lock (h);
  for (i = 0; i < 16; i++) {
    rc = drv_mac->SendFrame (buf, len, 0);
    if (rc == ARM_DRIVER_OK) {
      /* Success, frame transmit started */
      ctrl->TxCount += len;
      eth_unlock (h);
      RETURN (netOK);
    }
    /* Retry if the driver is busy */
    if (rc != ARM_DRIVER_ERROR_BUSY) {
      eth_unlock (h);
      RETURN (netDriverError);
    }
  }
  eth_unlock (h);
  RETURN (netBusy);

  END_LOCK;
}

/**
  \brief       Protected output of ethernet frame.
  \param[in]   if_num  interface number.
  \param[in]   frame   network frame.
  \return      output status:
               - true  = success,
               - false = failed.
  \note Adds VLAN tag if virtual LAN is enabled.
*/
bool net_eth_output (uint32_t if_num, NET_FRAME *frame) {
  NET_ETH_CFG *h = eth_if_map (if_num);
  uint8_t save_buf[4];
  uint32_t i;
  int32_t rc;

  if (h == NULL) {
    return (false);
  }
  DEBUGF (ETH,"Output %d, %d bytes\n",h->IfNum,frame->length);
  EvrNetETH_OutputLowLevel (h->IfNum, frame->length);
  eth_lock (h);
  for (i = 0; i < 16; i++) {
    if (ctrl->VlanId == 0) {
      rc = drv_mac->SendFrame (&frame->data[0], frame->length, 0);
      /* Retry if the driver is busy */
      if (rc == ARM_DRIVER_ERROR_BUSY) {
        /* After 12 retries start using os delays */
        /* Max. delay is 6ms (1ms + 2ms + 3ms)    */
        if (i > 12) netos_delay (i-12);
        continue;
      }
      if (rc != ARM_DRIVER_OK) {
        break;
      }
    }
    else {
      /* Send DstAddr, SrcAddr and Q-tag */
      /* Data is overlaid. First save the original 4 bytes to  */
      /* to 'save_buf', generate VLAN Q-tag and send 16 bytes. */
      __ALIGNED_UINT32(&save_buf[0]) = __ALIGNED_UINT32(&frame->data[12]);
      VLAN_TAG(frame)->Prot = HTONS(ETH_PROT_VLAN);
      VLAN_TAG(frame)->Ctrl = htons(ctrl->VlanId);
      rc = drv_mac->SendFrame (&frame->data[0], 16, ARM_ETH_MAC_TX_FRAME_FRAGMENT);
      /* Restore the original 4 bytes from 'save_buf' */
      __ALIGNED_UINT32(&frame->data[12]) = __ALIGNED_UINT32(&save_buf[0]);
      /* Retry if the driver is busy */
      if (rc == ARM_DRIVER_ERROR_BUSY) {
        /* After 12 retries start using os delays */
        /* Max. delay is 6ms (1ms + 2ms + 3ms)    */
        if (i > 12) netos_delay (i-12);
        continue;
      }
      if (rc != ARM_DRIVER_OK) {
        break;
      }
      /* Send the remaining bytes */
      rc = drv_mac->SendFrame (&frame->data[12], frame->length-12, 0);
      if (rc != ARM_DRIVER_OK) {
        break;
      }
    }
    /* Success, frame transmit started */
    ctrl->TxCount += frame->length;
    eth_unlock (h);
    return (true);
  }
  eth_unlock (h);
  return (false);
}

/**
  \brief       Run main process for ethernet interface.
*/
void net_eth_iface_run (void) {
  NET_ETH_CFG *const *p;

  for (p = &net_eth_list[0]; *p; p++) {
    eth_iface_run (*p);
  }
}

/**
  \brief       Run main process for ethernet interface.
  \param[in]   h  ethernet interface handle.
*/
static void eth_iface_run (NET_ETH_CFG *h) {
  NET_FRAME *frame;

  if (sys->Flags & SYS_FLAG_SEC2) {
    /* Sync timings for ETH thread */
    ctrl->th.SecTick = true;
    if (h->If->State->ConfigMcast) {
      /* Multicast addresses changed */
      h->If->State->ConfigMcast = false;
      eth_config_mcast (h->IfNum);
    }
  }
  else if (ctrl->th.ChangeSt) {
    if (ctrl->th.LinkState == ARM_ETH_LINK_UP) {
      DEBUGF (ETH,"Link %d up\n",h->IfNum);
      DEBUG_INFO (ctrl->th.LinkInfo);
      EvrNetETH_LinkUpStatus (h->IfNum, ctrl->th.LinkInfo);
      eth_config_mcast (h->IfNum);
      netETH_Notify (h->IfNum, netETH_LinkUp, ctrl->th.LinkInfo);
      if (h->If->Ip4Cfg && !h->If->Ip4Cfg->DhcpCfg) {
        /* Send Gratuitous ARP here if DHCP is disabled */
        net_arp_notify (h->If);
      }
      h->If->State->LinkUp = true;
    }
    else {
      DEBUGF (ETH,"Link %d down\n",h->IfNum);
      EvrNetETH_LinkDownStatus (h->IfNum);
      netETH_Notify (h->IfNum, netETH_LinkDown, 0);
      h->If->State->LinkUp = false;
    }
    ctrl->th.ChangeSt = false;
  }
  /* Check if a frame has been received */
  if (ctrl->q_head == ctrl->q_tail) {
    return;
  }
  sys->Busy = true;
#ifdef ACHILLES_TEST
  if (h->IfNum == 0) eth_test.n_proc++;
#endif

  /* Global parameters for input frame processing */
  sys->RxIpAddr  = NULL;
  sys->RxOffload = h->If->State->Offload;

  DEBUGF (ETH,"*** Process_frame %d ***\n",h->IfNum);
  frame = ctrl->rx_q[ctrl->q_tail & (ETH_QSIZE-1)];
  __COMPILER_BARRIER();
  ctrl->q_tail++;

  EvrNetETH_ReceiveFrame (h->IfNum, frame->length);
  if (ctrl->VlanId != 0) {
    /* VLAN enabled on interface */
    if (!eth_vlan_accept (h, frame)) {
      /* VLAN ID is not ours, dump the frame */
      DEBUGF (ETH," Discarded, VLAN invalid\n");
      EvrNetETH_VlanInvalid (h->IfNum);
      net_mem_free (frame);
      return;
    }
  }
  DEBUG_INF2 (frame->data, frame->length);
  EvrNetETH_ShowFrameHeader (frame->data);

  switch (ntohs(ETH_FRAME(frame)->Prot)) {
    case ETH_PROT_ARP:
      if (!h->If->Ip4Cfg) {
        DEBUGF (ETH," Discarded, %s:IPv4 disabled\n",h->If->Name);
        EvrNetETH_Ip4Disabled (h->IfNum);
        break;
      }
      net_arp_process (h->If, frame);
      break;

    case ETH_PROT_IP4:
      if (!h->If->Ip4Cfg) {
        DEBUGF (ETH," Discarded, %s:IPv4 disabled\n",h->If->Name);
        EvrNetETH_Ip4Disabled (h->IfNum);
        break;
      }
      if (net_mac_comp (ETH_FRAME(frame)->DstAddr, h->MacAddr)) {
        /* Link-layer addressed, destination address our local MAC */
        sys->Flags |= SYS_FLAG_LINKADDR;
      }
      /* Check IPv4 header and update ARP cache */
      if (!net_ip4_chk_frame (h->If, frame)) {
        break;
      }
      /* In NIC teaming mode, which is used for load balancing, all */
      /* broadcasts and multicasts are sent from the Primary NIC.   */
      if (!eth_is_ucast4 (ETH_FRAME(frame)->DstAddr)) {
        /* Add the sender's IP of bc/mc packet to the ARP cache */
        net_arp_cache_add (h->If, IP4_FRAME(frame)->SrcAddr,
                                  ETH_FRAME(frame)->SrcAddr);
      }
      else {
        /* Early cache & resolve sender's IP of unicast packet */
        net_arp_cache_early (h->If, IP4_FRAME(frame)->SrcAddr,
                                    ETH_FRAME(frame)->SrcAddr);
      }
      /* Process fragmented frame reassembly */
      frame = net_ip4_reass_frame (frame);
      if (frame == NULL) {
        /* Reassembly has taken over this frame */
        return;
      }
      /* Now check IPv4 frame protocol type */
      switch (IP4_FRAME(frame)->Prot) {
        case IP4_PROT_ICMP:
          net_icmp_process (h->If, frame);
          break;

        case IP4_PROT_IGMP:
          if (!h->If->Ip4Cfg->IgmpCfg) {
            /* Silently ignore if IGMP not enabled */
            break;
          }
          h->If->Ip4Cfg->IgmpCfg->process (h->If, frame);
          break;

        case IP4_PROT_UDP:
          net_udp_process (h->If, frame, IP_VER4);
          break;

        case IP4_PROT_TCP:
          net_tcp_process (h->If, frame, IP_VER4);
          break;
      }
      break;

    case ETH_PROT_IP6:
#ifdef Network_IPv6
      if (!h->If->Ip6Cfg) {
        DEBUGF (ETH," Discarded, %s:IPv6 disabled\n",h->If->Name);
        EvrNetETH_Ip6Disabled (h->IfNum);
        break;
      }
      /* Check IPv6 header and update Neighbor cache */
      if (!net_ip6_chk_frame (h->If, frame)) {
        break;
      }
      /* Frame is accepted */
      if (!(ETH_FRAME(frame)->DstAddr[0] & 0x01)) {
        /* Refresh unicast sender in neighbor cache */
        if ((IP6_FRAME(frame)->NextHdr == IP6_PROT_UDP) ||
            (IP6_FRAME(frame)->NextHdr == IP6_PROT_TCP)) {
          net_ndp_cache_refresh (h->If, IP6_FRAME(frame)->SrcAddr,
                                        ETH_FRAME(frame)->SrcAddr);
        }
      }
      /* Process fragmented frame reassembly */
      frame = net_ip6_reass_frame (frame);
      if (frame == NULL) {
        /* Reassembly has taken over this frame */
        return;
      }
      /* Now check IPv6 frame protocol type */
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
#endif
      break;

    default:
      /* Not supported, unknown Ethernet protocol */
      DEBUGF (ETH," Discarded, Unknown protocol\n");
      EvrNetETH_ProtocolUnknown(h->IfNum, ntohs(ETH_FRAME(frame)->Prot));
      netETH_ReceiveRaw (h->IfNum, frame->data, frame->length);
      break;
  }
  /* Release memory allocated for frame */
  net_mem_free (frame);
}

/**
  \brief       Process VLAN frame and check if accepted.
  \param[in]   h      ethernet interface handle.
  \param[in]   frame  ethernet VLAN frame.
  \return      accept status:
               - true  = accepted,
               - false = not accepted.
*/
static bool eth_vlan_accept (NET_ETH_CFG *h, NET_FRAME *frame) {
  uint32_t i,len;

  /* Check frame length */
  len = frame->length - 4;
  if (len < PHY_HEADER_LEN) {
    /* Frame too small */
    return (false);
  }
  if (VLAN_TAG(frame)->Prot != HTONS(ETH_PROT_VLAN)) {
    /* Frame not VLAN tagged */
    return (false);
  }
  if ((ntohs(VLAN_TAG(frame)->Ctrl) & 0xFFF) != ctrl->VlanId) {
    /* Incorrect VID, not our VLAN */
    return (false);
  }

  /* Remove VLAN tag from the frame */
  for (i = 12; i < len-4; i += 8) {
    /* Copy overlapped memory, optimized for speed */
    __ALIGNED_UINT64(&frame->data[i]) = __ALIGNED_UINT64(&frame->data[i+4]);
  }
  if (i < len) {
    /* Copy remaining 1 to 4 bytes */
    __ALIGNED_UINT32(&frame->data[i]) = __ALIGNED_UINT32(&frame->data[i+4]);
  }
  frame->length = len & 0xFFFF;
  return (true);
}

/**
  \brief       Check if MAC address is IP4 unicast address.
  \param[in]   mac_addr  MAC address.
  \return      check status:
               - true  = is unicast,
               - false = not unicast.
*/
static bool eth_is_ucast4 (const uint8_t *mac_addr) {
  if (net_mac_comp (mac_addr, net_addr_bcast) ||
      (get_u32 (mac_addr) >> 8) == 0x01005E) {
    /* Broadcast or multicast MAC address */
    return (false);
  }
  return (true);
}

/**
  \brief       Map interface number to ETH interface.
  \param[in]   if_num  ETH interface number.
  \return      pointer to interface object.
*/
static NET_ETH_CFG *eth_if_map (uint32_t if_num) {
  NET_ETH_CFG *const *p;

  for (p = &net_eth_list[0]; *p; p++) {
    if ((*p)->IfNum == if_num) {
      return (*p);
    }
  }
  return (NULL);
}

/**
  \brief       Lock ethernet interface.
  \param[in]   h  ethernet interface handle.
*/
static void eth_lock (NET_ETH_CFG *h) {
  netif_lock (ctrl->semaphore);
}

/**
  \brief       Unlock ethernet interface.
  \param[in]   h  ethernet interface handle.
*/
static void eth_unlock (NET_ETH_CFG *h) {
  netif_unlock (ctrl->semaphore);
}

#ifdef Network_Debug_STDIO
/**
  \brief       Debug print Link information.
  \param[in]   link_info  link speed and duplex mode.
*/
static void debug_info (uint32_t link_info) {
  char *sp, *dp;

  switch (link_info & 0x03) {
    case ARM_ETH_SPEED_10M:
      sp = "10M";
      break;
    case ARM_ETH_SPEED_100M:
      sp = "100M";
      break;
    default:
      sp = "1G";
      break;
  }
  switch (link_info >> 2) {
    case ARM_ETH_DUPLEX_HALF:
      dp = "Half";
      break;
    default:
      dp = "Full";
      break;
  }
  DEBUGF (ETH," %s, %s duplex\n",sp,dp);
}
/**
  \brief       Debug print ethernet header information.
  \param[in]   eth_header  ethernet frame header.
  \param[in]   len         length of frame in bytes.
*/
static void debug_inf2 (const void *eth_header, uint32_t len) {
  const NET_ETH_HEADER *eth_hdr = (const NET_ETH_HEADER *)eth_header;
  static const char p_asc[][4] = {
    "ARP",
    "IP4",
    "IP6"
  };
  static const uint16_t p_bin[] = {
    ETH_PROT_ARP,
    ETH_PROT_IP4,
    ETH_PROT_IP6
  };
  uint16_t prot;
  uint32_t i;

  DEBUGF (ETH," DstMAC %s\n",net_mac_ntoa(eth_hdr->DstAddr));
  DEBUGF (ETH," SrcMAC %s\n",net_mac_ntoa(eth_hdr->SrcAddr));
  /* Print user friendly ETH protocols */
  prot = ntohs(eth_hdr->Prot);
  for (i = 0; i < sizeof (p_bin)/sizeof(uint16_t); i++) {
    if (p_bin[i] == prot) {
      DEBUGF (ETH," Proto %s, %d bytes\n",p_asc[i],len);
      return;
    }
  }
  DEBUGF (ETH," Proto (0x%04X), %d bytes\n",prot,len);
}
#endif

/* Library default function(s) */
__WEAK void netETH_Notify (uint32_t if_num, netETH_Event event, uint32_t val) {
  (void)if_num; (void)event; (void)val; }
__WEAK void netETH_ReceiveRaw (uint32_t if_num, const uint8_t *buf, uint32_t len) {
  (void)if_num; (void)buf; (void)len; }
