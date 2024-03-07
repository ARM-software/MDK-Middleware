/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_wifi.c
 * Purpose: WiFi Interface
 *----------------------------------------------------------------------------*/

#include "rl_net_lib.h"
#include "Driver_WiFi.h"
#include "net_mem.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_eth.h"
#include "net_wifi.h"
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
NET_WIFI_CTRL net_wifi0_if_control;
NET_WIFI_CTRL net_wifi1_if_control;

/* Code shortening macros */
#define ctrl        ( h->Ctrl)
#define LocM        (*h->If->localm)
#define LocM6       (*h->If->localm6)
#define drv_wifi    ((ARM_DRIVER_WIFI *)h->DrvWifi)

/* Local Functions */
static void wifi_iface_init (NET_WIFI_CFG *h);
static void wifi_iface_uninit (NET_WIFI_CFG *h);
static uint32_t wifi_opt_len (NET_WIFI_CFG *h, netIF_Option option);
static uint16_t wifi_mtu_limit (uint16_t mtu, uint8_t ip_ver);
static netStatus rc_to_net (int32_t rc_drv);
static uint32_t opt_check (netWiFi_Option opt, uint32_t len);
static void wifi_receive (NET_WIFI_CFG *h);
static void wifi_transmit (NET_WIFI_CFG *h);
static void wifi_check_link (NET_WIFI_CFG *h);
static void wifi_iface_run (NET_WIFI_CFG *h);
static NET_WIFI_CFG *wifi_if_map (uint32_t if_num);
static void wifi_lock (NET_WIFI_CFG *h);
static void wifi_unlock (NET_WIFI_CFG *h);
#ifdef DEBUG_STDIO
 static const char *mode_ascii (uint32_t mode);
 static const char *opt_ascii (uint32_t opt);
 static void debug_info (NET_FRAME *frame);
#endif

/**
  \brief       Initialize WiFi network interfaces.
  \return      none.
*/
void net_wifi_iface_init (void) {
  NET_WIFI_CFG *const *p;

  for (p = &net_wifi_list[0]; *p; p++) {
    wifi_iface_init ((*p));
  }
}

/**
  \brief       Initialize a WiFi interface.
  \param[in]   h  wifi interface handle.
  \return      none.
*/
static void wifi_iface_init (NET_WIFI_CFG *h) {
  ARM_WIFI_CAPABILITIES capab;
  int32_t rc;

  DEBUGF (WIFI,"Init_interface %d\n",h->IfNum);
  EvrNetWiFi_InitInterface (h->IfNum);
  memset (ctrl, 0, sizeof (*ctrl));

  /* Initialize WiFi Controller */
  capab = drv_wifi->GetCapabilities ();
  if (!capab.bypass_mode) {
    goto bypass_err;
  }
  /* Check the functions used in bypass mode */
  if (drv_wifi->BypassControl     == NULL ||
      drv_wifi->EthSendFrame      == NULL ||
      drv_wifi->EthReadFrame      == NULL ||
      drv_wifi->EthGetRxFrameSize == NULL) {
    goto bypass_err;
  }

  /* Initialize the driver */
  rc = drv_wifi->Initialize ((capab.event_eth_rx_frame) ? h->cb_event : NULL);
  if (rc != ARM_DRIVER_OK) {
    goto init_err;
  }
  rc = drv_wifi->PowerControl (ARM_POWER_FULL);
  if (rc != ARM_DRIVER_OK) {
init_err:
    /* Initialize driver error */
    ERRORF (WIFI,"Init %d, Driver init failed\n",h->IfNum);
    EvrNetWiFi_DriverInitFailed (h->IfNum);
    net_sys_error (NET_ERROR_CONFIG);
  }

  /* Enable bypass mode */
  rc = drv_wifi->BypassControl (WIFI_IF_STA, 1);
  if (rc != ARM_DRIVER_OK) {
bypass_err:
    /* Initialize bypass mode error */
    ERRORF (WIFI,"Init %d, Bypass mode error\n",h->IfNum);
    EvrNetWiFi_SetBypassModeFailed (h->IfNum);
    net_sys_error (NET_ERROR_CONFIG);
  }

  /* Manage MAC address */
  if (h->MacCfg == NULL) {
    /* Get MAC address from the WiFi module */
    uint32_t mac_len = NET_ADDR_ETH_LEN;
    rc = drv_wifi->GetOption (WIFI_IF_STA, ARM_WIFI_MAC, h->MacAddr, &mac_len);
    if ((rc != ARM_DRIVER_OK) || (mac_len != NET_ADDR_ETH_LEN)) {
      ERRORF (WIFI,"Init %d, Get MAC failed\n",h->IfNum);
      EvrNetWiFi_GetMacAddressFailed (h->IfNum);
      net_sys_error (NET_ERROR_CONFIG);
    }
    /* Driver provides MAC address */
    DEBUGF (WIFI," Driver MAC (%s)\n",net_mac_ntoa(h->MacAddr));
    EvrNetWiFi_DriverMacAddress (h->IfNum, h->MacAddr);
    ctrl->Flags |= WIFI_FLAG_MAC_ADDR;
  }
  else {
    /* Set MAC address from the configuration */
    if (!netMAC_aton (h->MacCfg, h->MacAddr) || (h->MacAddr[0] & 0x01)) {
      /* Invalid MAC address format, or Multicast bit is set in MAC0 */
      ERRORF (WIFI,"Init %d, MAC config error\n",h->IfNum);
      EvrNetWiFi_MacAddressConfigError (h->IfNum);
      net_sys_error (NET_ERROR_CONFIG);
    }
    rc = drv_wifi->SetOption (WIFI_IF_STA,
                              ARM_WIFI_MAC, h->MacAddr, NET_ADDR_ETH_LEN);
    if (rc != ARM_DRIVER_OK) {
      ERRORF (WIFI,"Init %d, Set MAC failed\n",h->IfNum);
      EvrNetWiFi_SetMacAddressFailed (h->IfNum);
      net_sys_error (NET_ERROR_CONFIG);
    }
    /* Set MAC successful */
    DEBUGF (WIFI," MAC (%s)\n",net_mac_ntoa(h->MacAddr));
    EvrNetWiFi_SetMacAddress (h->IfNum, h->MacAddr);
  }

  /* Set MTU */
  ctrl->Mtu = PHY_HEADER_LEN + WIFI_MTU;

  /* Init IP4 protocol and local machine info */
  if (h->If->Ip4Cfg) {
    LocM.Mtu = wifi_mtu_limit (h->If->Ip4Cfg->Mtu, IP_VER4);
    if (!net_addr4_aton (h->If->Ip4Cfg->IpAddr, LocM.IpAddr)  ||
        !net_addr4_aton (h->If->Ip4Cfg->NetMask,LocM.NetMask) ||
        !net_addr4_aton (h->If->Ip4Cfg->DefGW,  LocM.DefGW)   ||
        !net_addr4_aton (h->If->Ip4Cfg->PriDNS, LocM.PriDNS)  ||
        !net_addr4_aton (h->If->Ip4Cfg->SecDNS, LocM.SecDNS)) {
      ERRORF (WIFI,"Init %d, IPv4 config error\n",h->IfNum);
      EvrNetWiFi_Ip4ConfigError (h->IfNum);
      net_sys_error (NET_ERROR_CONFIG);
    }
  }

#ifdef Network_IPv6
  /* Init IP6 protocol and local machine info */
  if (h->If->Ip6Cfg) {
    LocM6.Mtu = wifi_mtu_limit (h->If->Ip6Cfg->Mtu, IP_VER6);
    LocM6.HopLimit  = 128;
    LocM6.PrefixLen = h->If->Ip6Cfg->PrefixLen;
    if (!net_addr6_aton (h->If->Ip6Cfg->IpAddr, LocM6.IpAddr) ||
        !net_addr6_aton (h->If->Ip6Cfg->DefGW,  LocM6.DefGW)  ||
        !net_addr6_aton (h->If->Ip6Cfg->PriDNS, LocM6.PriDNS) ||
        !net_addr6_aton (h->If->Ip6Cfg->SecDNS, LocM6.SecDNS)) {
      ERRORF (WIFI,"Init %d, IPv6 config error\n",h->IfNum);
      EvrNetWiFi_Ip6ConfigError (h->IfNum);
      net_sys_error (NET_ERROR_CONFIG);
    }
  }
#endif

  /* Seed for pseudo-random generator based on MAC address */
  sys->RndState = net_crc_buf (h->MacAddr, NET_ADDR_ETH_LEN, sys->RndState);

  /* No checksum offload capabilities */
  h->If->State->Offload = SYS_OFFL_NONE;

  if (!capab.event_eth_rx_frame) {
    /* Use polling mode */
    ctrl->Flags |= WIFI_FLAG_POLLING;
  }

  /* Create OS objects for wireless interface */
  ctrl->thread = netif_create (h->If->Id, &ctrl->semaphore);
  if (ctrl->thread == NULL) {
    ERRORF (WIFI,"Init %d, Thread create failed\n",h->IfNum);
    EvrNetWiFi_ThreadCreateFailed (h->IfNum);
    net_sys_error (NET_ERROR_CONFIG);
  }
  if (ctrl->semaphore == NULL) {
    ERRORF (WIFI,"Init %d, Semaphore create failed\n",h->IfNum);
    EvrNetWiFi_SemaphoreCreateFailed (h->IfNum);
    net_sys_error (NET_ERROR_CONFIG);
  }
}

/**
  \brief       De-initialize WiFi network interface.
  \return      none.
*/
void net_wifi_iface_uninit (void) {
  NET_WIFI_CFG *const *p;

  for (p = &net_wifi_list[0]; *p; p++) {
    wifi_iface_uninit ((*p));
  }
}

/**
  \brief       De-initialize WiFi network interface.
  \param[in]   h  wifi interface handle.
  \return      none.
*/
static void wifi_iface_uninit (NET_WIFI_CFG *h) {

  DEBUGF (WIFI,"Uninit_interface %d\n",h->IfNum);
  EvrNetWiFi_UninitInterface (h->IfNum);

  /* De-initialize WiFi Controller */
  memset (h->MacAddr, 0, NET_ADDR_ETH_LEN);
  drv_wifi->SetOption (WIFI_IF_STA,
                       ARM_WIFI_MAC, h->MacAddr, NET_ADDR_ETH_LEN);
  drv_wifi->PowerControl (ARM_POWER_OFF);
  drv_wifi->Uninitialize ();

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

  /* Delete OS objects for WiFi interface */
  netif_delete (ctrl->thread, ctrl->semaphore);
  memset (ctrl, 0, sizeof(*ctrl));
}

/**
  \brief       WiFi interface thread.
  \param[in]   arg  wifi interface handle.
  \return      none.
*/
__NO_RETURN void netWiFi_Thread (void *arg) {
  NET_WIFI_CFG *h = (NET_WIFI_CFG *)arg;

  ctrl->th.osDelay = 25;
  while (1) {
    /* Adaptive delays based on Rx activity */
    netos_flag_wait (0x0001, ctrl->th.osDelay);
    if (ctrl->th.osDelay < 25) {
      ctrl->th.osDelay++;
    }
    /* Protect driver access */
    wifi_lock (h);
    wifi_receive (h);
    wifi_transmit (h);
    if (ctrl->th.SecTick) {
      ctrl->th.SecTick = false;
      wifi_check_link (h);
    }
    wifi_unlock (h);
  }
}

/**
  \brief       Event notification on wifi receive.
  \param[in]   event  receive event.
  \param[in]   ctx    interface context.
  \return      none.
  \note        This function is called from wifi1 thread.
*/
void net_wifi_callback (uint32_t event, NET_WIFI_CTRL *ctx) {
  if (event & ARM_WIFI_EVENT_ETH_RX_FRAME) {
    netos_flag_set (ctx->thread, 0x0001);
  }
}

/**
  \brief       Get current value of WiFi Interface option.
  \param[in]   if_num   interface number.
  \param[in]   option   option as specified by netIF_Option.
  \param[out]  buf      buffer to store the option value to
  \param[in]   buf_len  length of buffer
  \return      status code as defined with netStatus.
  \note        This function is called from a protected function.
*/
netStatus net_wifi_get_option (uint32_t if_num, netIF_Option option,
                                                uint8_t *buf, uint32_t buf_len) {
  NET_WIFI_CFG *h = wifi_if_map (if_num);
  uint32_t opt_len;

  if (h == NULL || buf == NULL || buf_len == 0) {
    goto invalid_param;
  }
  opt_len = wifi_opt_len (h, option);
  if (opt_len == 0 || opt_len > buf_len) {
    /* Return opt_len=0, when IP4 or IP6 disabled */
    goto invalid_param;
  }
  /* Read option value */
  switch ((int32_t)option) {
    case netIF_OptionMAC_Address:
      net_mac_copy (buf, h->MacAddr);
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
  ERRORF (WIFI,"GetOption %d, Invalid parameter\n",h->IfNum);
  EvrNetWiFi_GetOptionInvalidParameter (h->IfNum);
  return (netInvalidParameter);
}

/**
  \brief       Set the value of WiFi Interface option.
  \param[in]   if_num   interface number.
  \param[in]   option   option as specified by netIF_Option.
  \param[out]  buf      buffer containing the option value
  \param[in]   buf_len  length of buffer
  \return      status code as defined with netStatus.
  \note        This function is called from a protected function.
*/
netStatus net_wifi_set_option (uint32_t if_num, netIF_Option option,
                                                const uint8_t *buf, uint32_t buf_len) {
  NET_WIFI_CFG *h = wifi_if_map (if_num);
  uint16_t bval16;

  if (h == NULL || buf == NULL || buf_len == 0) {
    goto invalid_param;
  }
  if (buf_len != wifi_opt_len (h, option)) {
    /* Return opt_len=0, when IP4 or IP6 disabled */
    goto invalid_param;
  }
  /* Write option value */
  switch ((int32_t)option) {
    case netIF_OptionMAC_Address:
      /* Error if Multicast bit is set */
      if (buf[0] & 0x01) break;
      /* Error if driver provides MAC address */
      if (ctrl->Flags & WIFI_FLAG_MAC_ADDR) break;
      DEBUGF (WIFI," MAC_Addr=%s\n",net_mac_ntoa(buf));
      EvrNetWiFi_SetMacAddress (h->IfNum, buf);
      if (!net_mac_comp (buf, h->MacAddr)) {
        net_mac_copy (h->MacAddr, buf);
        /* Configure MAC address in the driver when MAC changed */
        wifi_lock (h);
        /* Required for blocking call to the driver */
        net_sys_unlock ();
        drv_wifi->SetOption (WIFI_IF_STA,
                             ARM_WIFI_MAC, h->MacAddr, NET_ADDR_ETH_LEN);
        net_sys_lock ();
        wifi_unlock (h);
        /* Update the pseudo-random generator from the new MAC address */
        sys->RndState = net_crc_buf (h->MacAddr, NET_ADDR_ETH_LEN, sys->RndState);
        h->If->State->MacNew = true;
      }
      return (netOK);

    case netIF_OptionIP4_Address:
      DEBUGF (WIFI," IP4_Addr=%s\n",net_addr4_ntoa(buf));
      EvrNetWiFi_SetIp4Address (h->IfNum, buf);
      if (!net_addr4_comp (LocM.IpAddr, buf)) {
        net_addr4_copy (LocM.IpAddr, buf);
        /* Send Gratuitous ARP when IP address changed */
        net_arp_notify (h->If);
      }
      return (netOK);

    case netIF_OptionIP4_SubnetMask:
      DEBUGF (WIFI," NetMask=%s\n",net_addr4_ntoa(buf));
      EvrNetWiFi_SetIp4SubnetMask (h->IfNum, buf);
      net_addr4_copy (LocM.NetMask, buf);
      return (netOK);

    case netIF_OptionIP4_DefaultGateway:
      DEBUGF (WIFI," Def_GW=%s\n",net_addr4_ntoa(buf));
      EvrNetWiFi_SetIp4DefaultGateway (h->IfNum, buf);
      net_addr4_copy (LocM.DefGW, buf);
      return (netOK);

    case netIF_OptionIP4_PrimaryDNS:
      DEBUGF (WIFI," Pri_DNS=%s\n",net_addr4_ntoa(buf));
      EvrNetWiFi_SetIp4PrimaryDNS (h->IfNum, buf);
      net_addr4_copy (LocM.PriDNS, buf);
      return (netOK);

    case netIF_OptionIP4_SecondaryDNS:
      DEBUGF (WIFI," Sec_DNS=%s\n",net_addr4_ntoa(buf));
      EvrNetWiFi_SetIp4SecondaryDNS (h->IfNum, buf);
      net_addr4_copy (LocM.SecDNS, buf);
      return (netOK);

    case netIF_OptionIP4_MTU:
      /* Possibly non-aligned buffer */
      memcpy (&bval16, buf, 2);
      LocM.Mtu = wifi_mtu_limit (bval16, IP_VER4);
      DEBUGF (WIFI," IP4_MTU=%d\n",LocM.Mtu);
      EvrNetWiFi_SetIp4Mtu (h->IfNum, LocM.Mtu);
      return (netOK);

#ifdef Network_IPv6
    case netIF_OptionIP6_StaticAddress:
      DEBUGF (WIFI," IP6_Addr=%s\n",net_addr6_ntoa(buf));
      EvrNetWiFi_SetIp6Address (h->IfNum, buf);
      net_addr6_copy (LocM6.IpAddr, buf);
      return (netOK);

    case netIF_OptionIP6_DefaultGateway:
      DEBUGF (WIFI," Def_GW6=%s\n",net_addr6_ntoa(buf));
      EvrNetWiFi_SetIp6DefaultGateway (h->IfNum, buf);
      net_addr6_copy (LocM6.DefGW, buf);
      return (netOK);

    case netIF_OptionIP6_PrimaryDNS:
      DEBUGF (WIFI," Pri_DNS6=%s\n",net_addr6_ntoa(buf));
      EvrNetWiFi_SetIp6PrimaryDNS (h->IfNum, buf);
      net_addr6_copy (LocM6.PriDNS, buf);
      return (netOK);

    case netIF_OptionIP6_SecondaryDNS:
      DEBUGF (WIFI," Sec_DNS6=%s\n",net_addr6_ntoa(buf));
      EvrNetWiFi_SetIp6SecondaryDNS (h->IfNum, buf);
      net_addr6_copy (LocM6.SecDNS, buf);
      return (netOK);

    case netIF_OptionIP6_SubnetPrefixLength:
      /* Error if prefix length out of range */
      if ((buf[0] == 0) || (buf[0] > 127)) break;
      DEBUGF (WIFI," PrefixLen=%d\n",buf[0]);
      EvrNetWiFi_SetIp6PrefixLength (h->IfNum, buf[0]);
      LocM6.PrefixLen = buf[0];
      return (netOK);

    case netIF_OptionIP6_MTU:
      /* Possibly non-aligned buffer */
      memcpy (&bval16, buf, 2);
      LocM6.Mtu = wifi_mtu_limit (bval16, IP_VER6);
      DEBUGF (WIFI," IP6_MTU=%d\n",LocM6.Mtu);
      EvrNetWiFi_SetIp6Mtu (h->IfNum, LocM6.Mtu);
      return (netOK);
#endif
    default:
      /* Read only options */
      break;
  }
invalid_param:
  ERRORF (WIFI,"SetOption %d, Invalid parameter\n",h->IfNum);
  EvrNetWiFi_SetOptionInvalidParameter (h->IfNum);
  return (netInvalidParameter);
}

/**
  \brief       Get requested interface option length.
  \param[in]   h       wifi interface handle.
  \param[in]   option  option as specified by netIF_Option.
  \return      length in bytes, =0 when IPv4 or IPv6 not enabled.
*/
static uint32_t wifi_opt_len (NET_WIFI_CFG *h, netIF_Option option) {
  switch ((int32_t)option) {
    /* MAC address option */
    case netIF_OptionMAC_Address:
      return (NET_ADDR_ETH_LEN);

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
static uint16_t wifi_mtu_limit (uint16_t mtu, uint8_t ip_ver) {
  uint16_t min_mtu;

  if (mtu != 0) {
    min_mtu = (ip_ver == IP_VER4) ? 576 : 1280;
    if (mtu <  min_mtu) mtu = min_mtu;
    if (mtu > 1500)     mtu = 1500;
  }
  return (mtu);
}

/**
  \brief       Scan available WiFi access points.
  \param[in]     if_num     interface number.
  \param[out]    scan_info  array of structures for storing the scan information.
  \param[in,out] scan_num   input max number, return number of access points found.
  \return      status code as defined with netStatus.
*/
netStatus netWiFi_Scan (uint32_t if_num,
                        NET_WIFI_SCAN_INFO scan_info[], uint32_t *scan_num) {
  NET_WIFI_CFG *h = wifi_if_map (if_num);
  uint32_t max_num = (scan_num) ? *scan_num : 0;
  int32_t  rc;

  START_LOCK (netStatus);

  if (h == NULL || max_num == 0 || scan_info == NULL) {
    ERRORF (WIFI,"Scan %d, Invalid parameter\n",h->IfNum);
    EvrNetWiFi_ScanInvalidParameter (h->IfNum);
    RETURN (netInvalidParameter);
  }

  DEBUGF (WIFI,"Scan %d, max=%d\n",h->IfNum,max_num);
  EvrNetWiFi_Scan (h->IfNum, max_num);

  if (h->Mode != WIFI_IF_STA) {
    ERRORF (WIFI,"Scan %d, Wrong mode %s\n",h->IfNum,mode_ascii(WIFI_IF_AP));
    EvrNetWiFi_ScanWrongMode (h->IfNum, WIFI_IF_AP);
    RETURN (netError);
  }

  /* Structures 'NET_WIFI_SCAN_INFO' and 'ARM_WIFI_SCAN_INFO_t' */
  /* are binary compatible, enumerators are 'short-enums'       */

  wifi_lock (h);
  net_sys_unlock ();
  rc = drv_wifi->Scan ((ARM_WIFI_SCAN_INFO_t *)scan_info, max_num);
  net_sys_lock ();
  wifi_unlock (h);

  if (rc >= 0) {
    DEBUGF (WIFI,"Scan %d complete, num=%d\n",h->IfNum,rc);
    EvrNetWiFi_ScanComplete (h->IfNum, (uint32_t)rc);
    *scan_num = (uint32_t)rc;
    rc = ARM_DRIVER_OK;
  }
  RETURN (rc_to_net (rc));

  END_LOCK;
}

/**
  \brief       Get the value of the WiFi driver option.
  \param[in]   if_num   interface number.
  \param[in]   option   option as specified by netWiFi_Option.
  \param[out]  buf      buffer to store the option value to
  \param[in]   buf_len  length of buffer
  \return      status code as defined with netStatus.
*/
netStatus netWiFi_GetOption (uint32_t if_num, netWiFi_Option option,
                                              void *buf, uint32_t buf_len) {
  NET_WIFI_CFG *h = wifi_if_map (if_num);
  uint32_t opt = opt_check (option, buf_len);
  int32_t  rc;

  START_LOCK (netStatus);

  if (h == NULL || opt == 0 || buf == NULL) {
    ERRORF (WIFI,"GetOption %d, Invalid parameter\n",h->IfNum);
    EvrNetWiFi_GetOptionInvalidParameter (h->IfNum);
    RETURN (netInvalidParameter);
  }

  DEBUGF (WIFI,"GetOption %d, %s\n",h->IfNum,opt_ascii(opt));
  EvrNetWiFi_GetOption (h->IfNum, opt);

  wifi_lock (h);
  net_sys_unlock ();
  rc = drv_wifi->GetOption (h->Mode, opt, buf, &buf_len);
  net_sys_lock ();
  wifi_unlock (h);
  RETURN (rc_to_net (rc));

  END_LOCK;
}

/**
  \brief       Set the value of the WiFi driver option.
  \param[in]   if_num   interface number.
  \param[in]   option   option as specified by netWiFi_Option.
  \param[out]  buf      buffer containing the option value
  \param[in]   buf_len  length of buffer
  \return      status code as defined with netStatus.
*/
netStatus netWiFi_SetOption (uint32_t if_num, netWiFi_Option option,
                                              const void *buf, uint32_t buf_len) {
  NET_WIFI_CFG *h = wifi_if_map (if_num);
  uint32_t opt = opt_check (option, buf_len);
  int32_t  rc;

  START_LOCK (netStatus);

  if (h == NULL || opt == 0 || buf == NULL) {
    ERRORF (WIFI,"SetOption %d, Invalid parameter\n",h->IfNum);
    EvrNetWiFi_SetOptionInvalidParameter (h->IfNum);
    RETURN (netInvalidParameter);
  }

  DEBUGF (WIFI,"SetOption %d, %s\n",h->IfNum,opt_ascii(opt));
  EvrNetWiFi_SetOption (h->IfNum, opt);

  wifi_lock (h);
  net_sys_unlock ();
  rc = drv_wifi->SetOption (h->Mode, opt, buf, buf_len);
  net_sys_lock ();
  wifi_unlock (h);
  RETURN (rc_to_net (rc));

  END_LOCK;
}

/**
  \brief       Activate the WiFi interface.
  \param[in]   if_num  interface number.
  \param[in]   config  pointer to the structure with configuration parameters.
  \return      status code as defined with netStatus.
*/
netStatus netWiFi_Activate (uint32_t if_num, const NET_WIFI_CONFIG *config) {
  NET_WIFI_CFG *h = wifi_if_map (if_num);
  int32_t rc;

  START_LOCK (netStatus);

  if (h == NULL || config == NULL) {
    ERRORF (WIFI,"Activate %d, Invalid parameter\n",h->IfNum);
    EvrNetWiFi_ActivateInvalidParameter (h->IfNum);
    RETURN (netInvalidParameter);
  }

  DEBUGF (WIFI,"Activate %d, %s\n",h->IfNum,mode_ascii(h->Mode));
  EvrNetWiFi_Activate (h->IfNum, h->Mode);

  /* Structures 'NET_WIFI_CONFIG' and 'ARM_WIFI_CONFIG_t' */
  /* are binary compatible, enumerators are 'short-enums' */

  wifi_lock (h);
  net_sys_unlock ();
  rc = drv_wifi->Activate (h->Mode, (const ARM_WIFI_CONFIG_t *)config);
  net_sys_lock ();
  wifi_unlock (h);
  RETURN (rc_to_net (rc));

  END_LOCK;
}

/**
  \brief       Deactivate the WiFi interface.
  \param[in]   if_num  interface number.
  \return      status code as defined with netStatus.
*/
netStatus netWiFi_Deactivate (uint32_t if_num) {
  NET_WIFI_CFG *h = wifi_if_map (if_num);
  int32_t rc;

  START_LOCK (netStatus);

  if (h == NULL) {
    ERRORF (WIFI,"Deactivate %d, Invalid parameter\n",h->IfNum);
    EvrNetWiFi_DeactivateInvalidParam (h->IfNum);
    RETURN (netInvalidParameter);
  }

  DEBUGF (WIFI,"Deactivate %d, %s\n",h->IfNum,mode_ascii(h->Mode));
  EvrNetWiFi_Deactivate (h->IfNum, h->Mode);

  wifi_lock (h);
  net_sys_unlock ();
  rc = drv_wifi->Deactivate (h->Mode);
  net_sys_lock ();
  wifi_unlock (h);
  RETURN (rc_to_net (rc));

  END_LOCK;
}

/**
  \brief       Get connection state of the WiFi interface.
  \param[in]   if_num  interface number.
  \return      connection state:
               - true  = connected to access point,
               - false = not connected.
*/
bool netWiFi_IsConnected (uint32_t if_num) {
  NET_WIFI_CFG *h = wifi_if_map (if_num);

  if (h == NULL || h->Mode != WIFI_IF_STA) {
    return (false);
  }
  return (ctrl->th.LinkState ? true : false);
}

/**
  \brief       Get network information of the WiFi station.
  \param[in]   if_num  interface number.
  \return      status code as defined with netStatus.
*/
netStatus netWiFi_GetNetInfo (uint32_t if_num, NET_WIFI_NET_INFO *net_info) {
  NET_WIFI_CFG *h = wifi_if_map (if_num);
  int32_t rc;

  START_LOCK (netStatus);

  if (h == NULL || net_info == NULL) {
    ERRORF (WIFI,"GetNetInfo %d, Invalid parameter\n",h->IfNum);
    EvrNetWiFi_GetNetInfoInvalidParam (h->IfNum);
    RETURN (netInvalidParameter);
  }

  DEBUGF (WIFI,"GetNetInfo %d\n",h->IfNum);
  EvrNetWiFi_GetNetInfo (h->IfNum);

  if (h->Mode != WIFI_IF_STA) {
    ERRORF (WIFI,"GetNetInfo %d, Wrong mode %s\n",h->IfNum,mode_ascii(WIFI_IF_AP));
    EvrNetWiFi_GetNetInfoWrongMode (h->IfNum, WIFI_IF_AP);
    RETURN (netError);
  }

  /* Structures 'NET_WIFI_NET_INFO' and 'ARM_WIFI_NET_INFO_t' */
  /* are binary compatible, enumerators are 'short-enums'     */

  wifi_lock (h);
  net_sys_unlock ();
  rc = drv_wifi->GetNetInfo ((ARM_WIFI_NET_INFO_t *)net_info);
  net_sys_lock ();
  wifi_unlock (h);
  RETURN (rc_to_net (rc));

  END_LOCK;
}

/**
  \brief       Convert driver return codes.
  \param[in]   rc_drv  driver return code.
  \return      status code as defined with netStatus.
*/
static netStatus rc_to_net (int32_t rc_drv) {
  switch (rc_drv) {
    case ARM_DRIVER_OK:
      return (netOK);
    case ARM_DRIVER_ERROR:
      return (netDriverError);
    case ARM_DRIVER_ERROR_TIMEOUT:
      return (netTimeout);
    case ARM_DRIVER_ERROR_PARAMETER:
      return (netInvalidParameter);
    default:
      return (netError);
  }
}

/**
  \brief       Check WiFi driver options.
  \param[in]   opt  network WiFi option.
  \param[out]  len  length of the option.
  \return      ARM driver option.
*/
static uint32_t opt_check (netWiFi_Option opt, uint32_t len) {
  switch ((int32_t)opt) {
    case netWiFi_OptionBSSID:
      if (len != 6) break;
      return (ARM_WIFI_BSSID);
    case netWiFi_OptionTxPower:
      if (len != 4) break;
      return (ARM_WIFI_TX_POWER);
    case netWiFi_OptionLpTimer:
      if (len != 4) break;
      return (ARM_WIFI_LP_TIMER);
    case netWiFi_OptionDTIM:
      if (len != 4) break;
      return (ARM_WIFI_DTIM);
    case netWiFi_OptionBeacon:
      if (len != 4) break;
      return (ARM_WIFI_BEACON);
    default:
      /* Other options not accepted */
      break;
  }
  /* Unassigned in CMSIS driver */
  return (0);
}

/**
  \brief       Receive WiFi frame.
  \param[in]   h  wifi interface handle.
  \return      none.
  \details     Called from WiFi thread!
*/
static void wifi_receive (NET_WIFI_CFG *h) {
  NET_FRAME *frame;
  uint32_t size;

  /* Fetch all available frames */
  while ((size = drv_wifi->EthGetRxFrameSize (WIFI_IF_STA)) != 0) {
    if ((size < PHY_HEADER_LEN) || (size > ctrl->Mtu)) {
      /* Frame error, release it */
      drv_wifi->EthReadFrame (WIFI_IF_STA, NULL, 0);
      continue;
    }
    if (((ctrl->q_head - ctrl->q_tail) & 0xFF) >= WIFI_QSIZE) {
      /* Queue overflow, dump this frame */
      drv_wifi->EthReadFrame (WIFI_IF_STA, NULL, 0);
      continue;
    }
    /* Do not call sys_error() if out of memory */
    frame = net_mem_alloc (size | 0x80000000);
    if (frame == NULL) {
      /* Out of memory, dump this frame */
      drv_wifi->EthReadFrame (WIFI_IF_STA, NULL, 0);
      continue;
    }
    /* Valid frame, read it and release it */
    drv_wifi->EthReadFrame (WIFI_IF_STA, &frame->data[0], size);
    ctrl->rx_q[ctrl->q_head & (WIFI_QSIZE-1)] = frame;
    ctrl->RxCount += size;
    ctrl->q_head++;
  }
  if (ctrl->q_head == ctrl->q_tail) {
    return;
  }
  if (ctrl->Flags & WIFI_FLAG_POLLING) {
    ctrl->th.osDelay = 2;
  }
  net_sys_wakeup ();
}

/**
  \brief       Transmit frame from tx-queue.
  \param[in]   h  wifi interface handle.
  \return      none.
  \details     Called from WiFi thread!
*/
static void wifi_transmit (NET_WIFI_CFG *h) {
  NET_FRAME *frame;
  int32_t rc;
  uint32_t i;

  if (ctrl->x_head == ctrl->x_tail) {
    return;
  }
  frame = ctrl->tx_q[ctrl->x_tail & (WIFI_QSIZE-1)];
  ctrl->x_tail++;
  if (ctrl->th.LinkState == 0) {
    /* When not connected, discard TX frames */
    net_mem_free (frame);
    return;
  }
  for (i = 0; i < 8; i++) {
    rc = drv_wifi->EthSendFrame (WIFI_IF_STA, &frame->data[0], frame->length);
    /* Retry if the driver is busy */
    if (rc == ARM_DRIVER_ERROR_BUSY) {
      /* After 3 retries start using os delays */
      /* Max.delay=150ms (10ms+20ms+30ms+40ms+50ms) */
      if (i > 2) netos_delay ((i-2)*10);
      continue;
    }
    if (rc == ARM_DRIVER_OK){
      ctrl->TxCount += frame->length;
    }
    break;
  }
  net_mem_free (frame);
}

/**
  \brief       Check WiFi link state.
  \param[in]   h  wifi interface handle.
  \return      none.
  \details     Called from WiFi thread!
*/
static void wifi_check_link (NET_WIFI_CFG *h) {
  uint8_t link;

  link = drv_wifi->IsConnected () ? 1 : 0;
  if (link != ctrl->th.LinkState) {
    /* Link state has changed */
    ctrl->th.LinkState = link;
    ctrl->th.ChangeSt  = true;
  }
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
bool net_wifi_send_frame (uint32_t if_num, NET_FRAME *frame, uint8_t ip_ver) {
  NET_WIFI_CFG *h = wifi_if_map (if_num);
  NET_ARP_INFO *arp_t;
#ifdef Network_IPv6
  NET_NDP_INFO *ndp_t;
#endif

  if (h == NULL) {
    return (false);
  }
  DEBUGF (WIFI,"Send_frame %d\n",h->IfNum);
  EvrNetWiFi_SendFrame (h->IfNum, frame->length-PHY_HEADER_LEN, ip_ver);
  if (ctrl->th.LinkState == 0) {
    /* Discard frame, station not connected */
    ERRORF (WIFI,"Send %d, Not connected\n",h->IfNum);
    EvrNetWiFi_NotConnected(h->IfNum);
    return (false);
  }
  if (frame->length > (PHY_HEADER_LEN + WIFI_MTU)) {
    /* Discard frame, payload is too large */
    ERRORF (WIFI,"Send %d, Data too large\n",h->IfNum);
    EvrNetWiFi_SendDataTooLarge(h->IfNum, frame->length-PHY_HEADER_LEN, WIFI_MTU);
    return (false);
  }

  /* Set destination MAC address */
  switch (ip_ver) {
    case IP_VER4:
      if (!h->If->Ip4Cfg) {
        ERRORF (WIFI,"Send %d, IP4 disabled\n",h->IfNum);
        EvrNetWiFi_SendIp4Disabled (h->IfNum);
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
        ERRORF (WIFI,"Send %d, Local-IP4 undefined\n",h->IfNum);
        EvrNetWiFi_Ip4LocalAddressUndefined (h->IfNum);
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
        ERRORF (WIFI,"Send %d, Unresolved MAC for %s\n",h->IfNum,
                     net_addr4_ntoa(IP4_FRAME(frame)->DstAddr));
        EvrNetWiFi_Ip4MacAddressUnresolved (h->IfNum, IP4_FRAME(frame)->DstAddr);
        return (false);
      }
      if (arp_t->State < ARP_STATE_RESOLVED) {
        /* MAC resolving in progress, add frame to the queue */
        DEBUGF (WIFI," Add Que=%d, len=%d\n",arp_t->Id, frame->length);
        EvrNetWiFi_EnqueueFrame (h->IfNum, arp_t->Id, frame->length);
        return (net_arp_enqueue (h->If, arp_t, frame));
      }
      net_mac_copy (ETH_FRAME(frame)->DstAddr, arp_t->MacAddr);
      break;
#ifdef Network_IPv6
    case IP_VER6:
      if (!h->If->Ip6Cfg) {
        ERRORF (WIFI,"Send %d, IP6 disabled\n",h->IfNum);
        EvrNetWiFi_SendIp6Disabled (h->IfNum);
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
        ERRORF (WIFI,"Send %d, Local-IP6 undefined\n",h->IfNum);
        EvrNetWiFi_Ip6LocalAddressUndefined (h->IfNum);
        return (false);
      }
      /* Try to get MAC address from Neigbor cache */
      ndp_t = net_ndp_cache_find (h->If, IP6_FRAME(frame)->DstAddr);
      if (ndp_t == NULL) {
        /* This entry was not found */
        ERRORF (WIFI,"Send %d, Unresolved MAC for [%s]\n",h->IfNum,
                     net_addr6_ntoa(IP6_FRAME(frame)->DstAddr));
        EvrNetWiFi_Ip6MacAddressUnresolved (h->IfNum, IP6_FRAME(frame)->DstAddr);
        return (false);
      }
      if (ndp_t->State < NDP_STATE_RESOLVED) {
        /* MAC resolving in progress, add frame to the queue */
        DEBUGF (WIFI," Add Que=%d, len=%d\n",ndp_t->Id, frame->length);
        EvrNetWiFi_EnqueueFrame (h->IfNum, ndp_t->Id, frame->length);
        return (net_ndp_enqueue (h->If, ndp_t, frame));
      }
      net_mac_copy (ETH_FRAME(frame)->DstAddr, ndp_t->MacAddr);
      break;
#endif
    default:
      ERRORF (WIFI,"Send %d, Invalid IP version\n",h->IfNum);
      EvrNetWiFi_InvalidIpVersion (h->IfNum, ip_ver);
      return (false);
  }

  /* Set source MAC address */
  net_mac_copy (ETH_FRAME(frame)->SrcAddr, h->MacAddr);

  DEBUG_INFO (frame);
  EvrNetWiFi_ShowFrameHeader (frame->data);
  return (net_wifi_output (h->IfNum, frame));
}

/**
  \brief       Protected output of the WiFi frame.
  \param[in]   if_num  interface number.
  \param[in]   frame   network frame.
  \return      output status:
               - true  = success,
               - false = failed.
*/
bool net_wifi_output (uint32_t if_num, NET_FRAME *frame) {
  NET_WIFI_CFG *h = wifi_if_map (if_num);
  NET_FRAME *txframe;
  uint32_t i,len;

  if (h == NULL) {
    return (false);
  }
  DEBUGF (WIFI,"Output %d, %d bytes\n",h->IfNum,frame->length);
  EvrNetWiFi_OutputLowLevel (h->IfNum, frame->length);

  if (((ctrl->x_head - ctrl->x_tail) & 0xFF) >= WIFI_QSIZE) {
    /* Queue overflow, dump this frame */
    ERRORF (WIFI,"Output %d, TxQueue overflow\n",h->IfNum);
    EvrNetWiFi_TxQueueOverflow (h->IfNum);
    return (false);
  }
  if (!(net_mem_avail (0))) {
    /* System out of memory, dump this frame */
    ERRORF (WIFI,"Output %d, Out of memory\n",h->IfNum);
    EvrNetWiFi_OutputNoMemory (h->IfNum);
    return (false);
  }
  txframe = net_mem_alloc (frame->length);
  /* Fast copy of 4-byte aligned buffers */
  len = frame->length;
  for (i = 0; i < len-4; i += 8) {
    __ALIGNED_UINT64(&txframe->data[i]) = __ALIGNED_UINT64(&frame->data[i]);
  }
  if (i < len) {
    /* Copy remaining 1 to 4 bytes */
    __ALIGNED_UINT32(&txframe->data[i]) = __ALIGNED_UINT32(&frame->data[i]);
  }
  ctrl->tx_q[ctrl->x_head & (WIFI_QSIZE-1)] = txframe;
  ctrl->x_head++;
  /* Notify the thread there is something to send */
  netos_flag_set (ctrl->thread, 0x0001);
  return (true);
}

/**
  \brief       Run main process for WiFi interface.
  \return      none.
*/
void net_wifi_iface_run (void) {
  NET_WIFI_CFG *const *p;

  for (p = &net_wifi_list[0]; *p; p++) {
    wifi_iface_run (*p);
  }
}

/**
  \brief       Run main process for WiFi interface.
  \param[in]   h  wifi interface handle.
  \return      none.
*/
static void wifi_iface_run (NET_WIFI_CFG *h) {
  NET_FRAME *frame;

  if (sys->Flags & SYS_FLAG_SEC2) {
    /* Sync timings for WiFi thread */
    ctrl->th.SecTick = true;
  }
  else if (ctrl->th.ChangeSt) {
    if (ctrl->th.LinkState != 0) {
      DEBUGF (WIFI,"AP %d connected\n",h->IfNum);
      EvrNetWiFi_LinkStateChange (h->IfNum, 1);
      if (h->If->Ip4Cfg && !h->If->Ip4Cfg->DhcpCfg) {
        /* Send Gratuitous ARP here if DHCP is disabled */
        net_arp_notify (h->If);
      }
      h->If->State->LinkUp = true;
    }
    else {
      DEBUGF (WIFI,"AP %d disconnected\n",h->IfNum);
      EvrNetWiFi_LinkStateChange (h->IfNum, 0);
      h->If->State->LinkUp = false;
    }
    ctrl->th.ChangeSt = false;
  }
  /* Check if a frame has been received */
  if (ctrl->q_head == ctrl->q_tail) {
    return;
  }
  sys->Busy = true;

  /* Global parameters for input frame processing */
  sys->RxIpAddr  = NULL;
  sys->RxOffload = h->If->State->Offload;

  DEBUGF (WIFI,"*** Process_frame %d ***\n",h->IfNum);
  frame = ctrl->rx_q[ctrl->q_tail & (WIFI_QSIZE-1)];
  ctrl->q_tail++;

  DEBUG_INFO (frame);
  EvrNetWiFi_ReceiveFrame (h->IfNum, frame->length);
  EvrNetWiFi_ShowFrameHeader (frame->data);

  switch (ntohs(ETH_FRAME(frame)->Prot)) {
    case ETH_PROT_ARP:
      if (!h->If->Ip4Cfg) {
        DEBUGF (WIFI," Discarded, %s:IPv4 disabled\n",h->If->Name);
        EvrNetWiFi_Ip4Disabled (h->IfNum);
        break;
      }
      net_arp_process (h->If, frame);
      break;

    case ETH_PROT_IP4:
      if (!h->If->Ip4Cfg) {
        DEBUGF (WIFI," Discarded, %s:IPv4 disabled\n",h->If->Name);
        EvrNetWiFi_Ip4Disabled (h->IfNum);
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
      /* Add the sender's IP to the ARP cache */
      net_arp_cache_add (h->If, IP4_FRAME(frame)->SrcAddr,
                                ETH_FRAME(frame)->SrcAddr);

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
        DEBUGF (WIFI," Discarded, %s:IPv6 disabled\n",h->If->Name);
        EvrNetWiFi_Ip6Disabled (h->IfNum);
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
      DEBUGF (WIFI," Discarded, Unknown protocol\n");
      EvrNetWiFi_ProtocolUnknown(h->IfNum, ntohs(ETH_FRAME(frame)->Prot));
      break;
  }
  /* Release memory allocated for frame */
  net_mem_free (frame);
}

/**
  \brief       Map interface number to WIFI interface.
  \param[in]   if_num  WIFI interface number.
  \return      pointer to interface object.
*/
static NET_WIFI_CFG *wifi_if_map (uint32_t if_num) {
  NET_WIFI_CFG *const *p;

  for (p = &net_wifi_list[0]; *p; p++) {
    if ((*p)->IfNum== if_num) {
      return (*p);
    }
  }
  return (NULL);
}

/**
  \brief       Lock WiFi interface.
  \param[in]   h  wifi interface handle.
  \return      none.
*/
static void wifi_lock (NET_WIFI_CFG *h) {
  netif_lock (ctrl->semaphore);
}

/**
  \brief       Unlock WiFi interface.
  \param[in]   h  wifi interface handle.
  \return      none.
*/
static void wifi_unlock (NET_WIFI_CFG *h) {
  netif_unlock (ctrl->semaphore);
}

#ifdef DEBUG_STDIO
/**
  \brief       Convert WiFi interface mode to ascii.
  \param[in]   mode  WiFi interface mode.
  \return      pointer to text representation of an interface identifier.
*/
static const char *mode_ascii (uint32_t mode) {
  switch (mode) {
    case WIFI_IF_STA:
      return ("Station");
    case WIFI_IF_AP:
      return ("AP");
    default:
      return ("Err");
  }
}

/**
  \brief       Convert option code to ascii.
  \param[in]   opt  driver option code.
  \return      pointer to text representation of an option.
*/
static const char *opt_ascii (uint32_t opt) {
  switch (opt) {
    case ARM_WIFI_BSSID:
      return ("BSSID");
    case ARM_WIFI_TX_POWER:
      return ("TX-Power");
    case ARM_WIFI_LP_TIMER:
      return ("LP-Timer");
    case ARM_WIFI_DTIM:
      return ("DTIM");
    case ARM_WIFI_BEACON:
      return ("Beacon");
    default:
      return ("Unknown");
  }
}

/**
  \brief       Debug print ethernet header information.
  \param[in]   frame  network frame.
  \return      none.
*/
static void debug_info (NET_FRAME *frame) {
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

  DEBUGF (WIFI," DstMAC %s\n",net_mac_ntoa(ETH_FRAME(frame)->DstAddr));
  DEBUGF (WIFI," SrcMAC %s\n",net_mac_ntoa(ETH_FRAME(frame)->SrcAddr));
  /* Print user friendly WiFi protocols */
  prot = ntohs(ETH_FRAME(frame)->Prot);
  for (i = 0; i < sizeof (p_bin)/sizeof(uint16_t); i++) {
    if (p_bin[i] == prot) {
      DEBUGF (WIFI," Proto %s, %d bytes\n",p_asc[i],frame->length);
      return;
    }
  }
  DEBUGF (WIFI," Proto (0x%04X), %d bytes\n",prot,frame->length);
}
#endif
