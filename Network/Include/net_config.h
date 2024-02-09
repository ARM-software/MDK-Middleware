/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_config.h
 * Purpose: Network Library Configuration
 *----------------------------------------------------------------------------*/

#if defined(__clang__)
  #pragma clang diagnostic ignored "-Wundef"
  #pragma clang diagnostic ignored "-Wpadded"
  #pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif

#include "RTE_Components.h"

#if (defined(RTE_Network_Interface_ETH_0)  && !ETH0_ENABLE)
  #include "Net_Config_ETH_0.h"
#endif
#if (defined(RTE_Network_Interface_ETH_1)  && !ETH1_ENABLE)
  #include "Net_Config_ETH_1.h"
#endif
#if (defined(RTE_Network_Interface_WiFi_0) && !WIFI0_ENABLE)
  #include "Net_Config_WiFi_0.h"
#endif
#if (defined(RTE_Network_Interface_WiFi_1) && !WIFI1_ENABLE)
  #include "Net_Config_WiFi_1.h"
#endif
#if (defined(RTE_Network_Interface_PPP)    && !PPP_ENABLE)
  #include "Net_Config_PPP.h"
#endif
#if (defined(RTE_Network_Interface_SLIP)   && !SLIP_ENABLE)
  #include "Net_Config_SLIP.h"
#endif

#if (defined(RTE_Network_Socket_UDP)    && !UDP_ENABLE)
  #include "Net_Config_UDP.h"
#endif
#if (defined(RTE_Network_Socket_TCP)    && !TCP_ENABLE)
  #include "Net_Config_TCP.h"
#endif
#if (defined(RTE_Network_Socket_BSD)    && !BSD_ENABLE)
  #include "Net_Config_BSD.h"
#endif

#if (defined(RTE_Network_Web_Server_RO) && !HTTP_SERVER_ENABLE)
  #include "Net_Config_HTTP_Server.h"
#endif
#if (defined(RTE_Network_Web_Server_FS) && !HTTP_SERVER_ENABLE)
  #include "Net_Config_HTTP_Server.h"
#endif
#if (defined(RTE_Network_Telnet_Server) && !TELNET_SERVER_ENABLE)
  #include "Net_Config_Telnet_Server.h"
#endif
#if (defined(RTE_Network_TFTP_Server)   && !TFTP_SERVER_ENABLE)
  #include "Net_Config_TFTP_Server.h"
#endif
#if (defined(RTE_Network_TFTP_Client)   && !TFTP_CLIENT_ENABLE)
  #include "Net_Config_TFTP_Client.h"
#endif
#if (defined(RTE_Network_FTP_Server)    && !FTP_SERVER_ENABLE)
  #include "Net_Config_FTP_Server.h"
#endif
#if (defined(RTE_Network_FTP_Client)    && !FTP_CLIENT_ENABLE)
  #include "Net_Config_FTP_Client.h"
#endif
#if (defined(RTE_Network_DNS_Client)    && !DNS_CLIENT_ENABLE)
  #include "Net_Config_DNS_Client.h"
#endif
#if (defined(RTE_Network_SMTP_Client)   && !SMTP_CLIENT_ENABLE)
  #include "Net_Config_SMTP_Client.h"
#endif
#if (defined(RTE_Network_SNMP_Agent)    && !SNMP_AGENT_ENABLE)
  #include "Net_Config_SNMP_Agent.h"
#endif
#if (defined(RTE_Network_SNTP_Client)   && !SNTP_CLIENT_ENABLE)
  #include "Net_Config_SNTP_Client.h"
#endif

#include "rl_net_lib.h"

/* Forward declarations */
static NET_ETH_CFG  eth0_ll_config;
static NET_ETH_CFG  eth1_ll_config;
static NET_WIFI_CFG wifi0_ll_config;
static NET_WIFI_CFG wifi1_ll_config;
static NET_PPP_CFG  ppp0_ll_config;
static NET_SLIP_CFG slip0_ll_config;

#if defined(RTE_CMSIS_RTOS2)
  #include "net_rtos2.h"
#else
  #error "::CMSIS:RTOS selection invalid"
#endif

#if (ETH0_ENABLE || ETH1_ENABLE)
  #include "Driver_ETH_MAC.h"
  #include "Driver_ETH_PHY.h"
#endif

#if (WIFI0_ENABLE || WIFI1_ENABLE)
  #include "Driver_WiFi.h"
#endif

#if (PPP_ENABLE || SLIP_ENABLE)
  #include "Driver_USART.h"
  #include "net_modem.h"
#endif

/* Non-Critical configuration upgrade */
#if (SMTP_CLIENT_ENABLE && !defined(SMTP_CLIENT_ATTACH_ENABLE))
  #define SMTP_CLIENT_ATTACH_ENABLE 0
#endif
#if (HTTP_SERVER_ENABLE && !defined(HTTP_SERVER_AUTH_TYPE))
  #define HTTP_SERVER_AUTH_TYPE     0
#endif
#if (TELNET_SERVER_ENABLE && defined(TELNET_SERVER_NUM_SESSISONS))
  #define TELNET_SERVER_NUM_SESSIONS TELNET_SERVER_NUM_SESSISONS
#endif

/* Check configuration integrity */
#if (!defined(NET_THREAD_STACK_SIZE))
  #error "::Network:CORE: Configuration update required"
#endif

#if (ETH0_ENABLE && !defined(ETH0_THREAD_STACK_SIZE))
  #error "::Network:Interface:ETH0: Configuration update required"
#endif

#if (PPP_ENABLE && !defined(PPP_THREAD_STACK_SIZE))
  #error "::Network:Interface:PPP: Configuration update required"
#endif

#if (SLIP_ENABLE && !defined(SLIP_THREAD_STACK_SIZE))
  #error "::Network:Interface:SLIP: Configuration update required"
#endif

#if (TCP_ENABLE && (TCP_MAX_SEG_SIZE > 1440))
  #error "::Network:Socket:TCP: Configuration update required"
#endif

#if (HTTP_SERVER_ENABLE && !defined(HTTP_SERVER_AUTH_ADMIN))
  #error "::Network:Service:HTTP Server: Configuration update required"
#endif

#if (FTP_SERVER_ENABLE && !defined(FTP_SERVER_AUTH_ADMIN))
  #error "::Network:Service:FTP Server: Configuration update required"
#endif

#if (TELNET_SERVER_ENABLE && !defined(TELNET_SERVER_AUTH_ADMIN))
  #error "::Network:Service:Telnet Server: Configuration update required"
#endif

#if (TFTP_SERVER_ENABLE && !defined(TFTP_SERVER_ROOT_ENABLE))
  #error "::Network:Service:TFTP Server: Configuration update required"
#endif

#if (SNTP_CLIENT_ENABLE && !defined(SNTP_CLIENT_NTP_SERVER))
  #error "::Network:Service:SNTP Client: Configuration update required"
#endif

#if (SNMP_AGENT_ENABLE && !defined(SNMP_AGENT_TRAP_IP))
  #error "::Network:Service:SNMP Agent: Configuration update required"
#endif

/* Check interfaces and protocols */
#ifndef RTE_Network_IPv6
  #undef ETH0_IP6_ENABLE
  #undef ETH0_DHCP6_ENABLE
  #undef ETH1_IP6_ENABLE
  #undef ETH1_DHCP6_ENABLE
  #undef WIFI0_IP6_ENABLE
  #undef WIFI0_DHCP6_ENABLE
  #undef WIFI1_IP6_ENABLE
  #undef WIFI1_DHCP6_ENABLE
#endif

#if (!ETH0_IP4_ENABLE)
  #undef ETH0_IP4_FRAG_ENABLE
  #undef ETH0_IGMP_ENABLE
  #undef ETH0_NBNS_ENABLE
  #undef ETH0_DHCP_ENABLE
#endif
#if (!ETH1_IP4_ENABLE)
  #undef ETH1_IP4_FRAG_ENABLE
  #undef ETH1_IGMP_ENABLE
  #undef ETH1_NBNS_ENABLE
  #undef ETH1_DHCP_ENABLE
#endif
#if (!WIFI0_IP4_ENABLE)
  #undef WIFI0_IP4_FRAG_ENABLE
  #undef WIFI0_IGMP_ENABLE
  #undef WIFI0_NBNS_ENABLE
  #undef WIFI0_DHCP_ENABLE
#endif
#if (!WIFI1_IP4_ENABLE)
  #undef WIFI1_IP4_FRAG_ENABLE
  #undef WIFI1_IGMP_ENABLE
  #undef WIFI1_NBNS_ENABLE
  #undef WIFI1_DHCP_ENABLE
#endif

#if (!ETH0_IP6_ENABLE)
  #undef ETH0_IP6_FRAG_ENABLE
  #undef ETH0_DHCP6_ENABLE
#endif
#if (!ETH1_IP6_ENABLE)
  #undef ETH1_IP6_FRAG_ENABLE
  #undef ETH1_DHCP6_ENABLE
#endif
#if (!WIFI0_IP6_ENABLE)
  #undef WIFI0_IP6_FRAG_ENABLE
  #undef WIFI0_DHCP6_ENABLE
#endif
#if (!WIFI1_IP6_ENABLE)
  #undef WIFI1_IP6_FRAG_ENABLE
  #undef WIFI1_DHCP6_ENABLE
#endif

/* IP version usage definitions */
#define __IP4_ENA   (ETH0_IP4_ENABLE   + ETH1_IP4_ENABLE   + \
                     WIFI0_IP4_ENABLE  + WIFI1_IP4_ENABLE  + \
                     PPP_ENABLE        + SLIP_ENABLE)

#define __IP6_ENA   (ETH0_IP6_ENABLE   + ETH1_IP6_ENABLE   + \
                     WIFI0_IP6_ENABLE  + WIFI1_IP6_ENABLE)

/* Ethernet usage definitions */
#define __ETH_ENA   (ETH0_ENABLE       + ETH1_ENABLE)
#define __WIFI_ENA  (WIFI0_ENABLE      + WIFI1_ENABLE)

/* Interface protocol usage definitions */
#define __ARP_ENA   (ETH0_IP4_ENABLE   + ETH1_IP4_ENABLE   + \
                     WIFI0_IP4_ENABLE  + WIFI1_IP4_ENABLE)

#define __IGMP_ENA  (ETH0_IGMP_ENABLE  + ETH1_IGMP_ENABLE  + \
                     WIFI0_IGMP_ENABLE + WIFI1_IGMP_ENABLE)

#define __NBNS_ENA  (ETH0_NBNS_ENABLE  + ETH1_NBNS_ENABLE  + \
                     WIFI0_NBNS_ENABLE + WIFI1_NBNS_ENABLE)

#define __DHCP_ENA  (ETH0_DHCP_ENABLE  + ETH1_DHCP_ENABLE  + \
                     WIFI0_DHCP_ENABLE + WIFI1_DHCP_ENABLE)

#define __NDP_ENA   (ETH0_IP6_ENABLE   + ETH1_IP6_ENABLE   + \
                     WIFI0_IP6_ENABLE  + WIFI1_IP6_ENABLE)

#define __DHCP6_ENA (ETH0_DHCP6_ENABLE + ETH1_DHCP6_ENABLE + \
                     WIFI0_DHCP6_ENABLE+ WIFI1_DHCP6_ENABLE)

/* IP fragmentation usage definitions */
#define __IP4_FRAG_ENA (                                             \
                     ETH0_IP4_FRAG_ENABLE  + ETH1_IP4_FRAG_ENABLE  + \
                     WIFI0_IP4_FRAG_ENABLE + WIFI1_IP4_FRAG_ENABLE + \
                     PPP_IP4_FRAG_ENABLE   + SLIP_IP4_FRAG_ENABLE)

#define __IP6_FRAG_ENA (                                             \
                     ETH0_IP6_FRAG_ENABLE  + ETH1_IP6_FRAG_ENABLE  + \
                     WIFI0_IP6_FRAG_ENABLE + WIFI1_IP6_FRAG_ENABLE)

/* Check enabled interfaces */
#if !(__ETH_ENA || __WIFI_ENA || PPP_ENABLE || SLIP_ENABLE)
  #error "::Network:Interface: No interface enabled in configuration"
#endif

/* Check interface drivers */
#if (ETH1_ENABLE && ETH0_ENABLE && (ETH1_DRIVER == ETH0_DRIVER))
  #error "::Network:Interface:ETH1: Driver conflict with ETH0 interface"
#endif

#if (WIFI1_ENABLE && WIFI0_ENABLE && (WIFI1_DRIVER == WIFI0_DRIVER))
  #error "::Network:Interface:WIFI1: Driver conflict with WIFI0 interface"
#endif

#if (SLIP_ENABLE && PPP_ENABLE && (SLIP_USART_DRIVER == PPP_USART_DRIVER))
  #error "::Network:Interface:SLIP: Driver conflict with PPP interface"
#endif


/* Check ETH enabled protocols */
#if (ETH0_ENABLE && !(ETH0_IP4_ENABLE || ETH0_IP6_ENABLE))
  #error "::Network:Interface:ETH0: IP protocol not enabled"
#endif

#if (ETH1_ENABLE && !(ETH1_IP4_ENABLE || ETH1_IP6_ENABLE))
  #error "::Network:Interface:ETH1: IP protocol not enabled"
#endif

/* Check WIFI enabled protocols */
#if (WIFI0_ENABLE && !(WIFI0_IP4_ENABLE || WIFI0_IP6_ENABLE))
  #error "::Network:Interface:WiFi0: IP protocol not enabled"
#endif

#if (WIFI1_ENABLE && !(WIFI1_IP4_ENABLE || WIFI1_IP6_ENABLE))
  #error "::Network:Interface:WiFi1: IP protocol not enabled"
#endif

/* Check TCP socket configuration */
#define __TCPNS    ((BSD_ENABLE           * BSD_NUM_SOCKS)               + \
                    (HTTP_SERVER_ENABLE   * HTTP_SERVER_NUM_SESSIONS)    + \
                    (TELNET_SERVER_ENABLE * TELNET_SERVER_NUM_SESSIONS)  + \
                    (FTP_SERVER_ENABLE    * FTP_SERVER_NUM_SESSIONS * 2) + \
                    (FTP_CLIENT_ENABLE    * 2)                           + \
                    (SMTP_CLIENT_ENABLE   * 1))

#if (__TCPNS > 0 && !TCP_ENABLE)
  #error "::Network:Socket:TCP component required"
#elif (__TCPNS > TCP_NUM_SOCKS)
  #error "::Network:Socket:TCP: Number of TCP Sockets too small"
#endif

/* Calculate number of UDP sockets required for TFTP server */
#if (TFTP_SERVER_FIREWALL_ENABLE)
  #define __TFTPNS  1
#else
  #define __TFTPNS (1 + TFTP_SERVER_NUM_SESSIONS)
#endif

/* Check UDP socket configuration */
#define __UDPNS    ((BSD_ENABLE         * BSD_NUM_SOCKS) + \
                    (TFTP_SERVER_ENABLE * __TFTPNS)      + \
                    (TFTP_CLIENT_ENABLE * 1)             + \
                    (DNS_CLIENT_ENABLE  * 1)             + \
                    (SNMP_AGENT_ENABLE  * 1)             + \
                    (SNTP_CLIENT_ENABLE * 1)             + \
                    (__DHCP_ENA  ?    1 : 0)             + \
                    (__NBNS_ENA  ?    1 : 0)             + \
                    (__DHCP6_ENA ?    1 : 0))

#if (__UDPNS > 0 && !UDP_ENABLE)
  #error "::Network:Socket:UDP component required"
#elif (__UDPNS > UDP_NUM_SOCKS)
  #error "::Network:Socket:UDP: Number of UDP Sockets too small"
#endif

/* Check Maximum Segment Size of TCP Socket */
#if (TCP_ENABLE && ((TCP_MAX_SEG_SIZE < 536) || (TCP_MAX_SEG_SIZE > 1440)))
  #error "::Network:Socket:TCP: Maximum Segment Size out of range"
#endif

/* Check Receive Window Size of TCP Socket */
#if (TCP_ENABLE && (TCP_RECEIVE_WIN_SIZE < TCP_MAX_SEG_SIZE))
  #error "::Network:Socket:TCP: Receive Window Size too small"
#endif

/* Check BSD Server sockets */
#if (BSD_ENABLE && (BSD_SERVER_SOCKS > BSD_NUM_SOCKS))
  #error "::Network:Socket:BSD: Number of BSD Server sockets too large"
#endif

/* Check BSD Hostname resolver */
#if (BSD_ENABLE && BSD_HOSTNAME_ENABLE && !DNS_CLIENT_ENABLE)
  #error "::Network:Socket:BSD: DNS Client service required"
#endif

/* Check PPP Authentication protocols */
#if (PPP_ENABLE && PPP_AUTH_ENABLE && !(PPP_PAP_ENABLE || PPP_CHAP_ENABLE))
  #error "::Network:Interface:PPP: Authentication needs PAP or/and CHAP enabled"
#endif

/* Check Block Size of TFTP Client */
#if (TFTP_CLIENT_ENABLE && ((TFTP_CLIENT_BLOCK_SIZE < 128) || (TFTP_CLIENT_BLOCK_SIZE > 1428)))
  #error "::Network:Service:TFTP Client: Block Size out of range"
#endif

/* Check SMTP client attachments */
#if (SMTP_CLIENT_ENABLE && SMTP_CLIENT_ATTACH_ENABLE && !defined(RTE_FileSystem_Core))
  #error "::Network:Service:SMTP Client: File System component required"
#endif

/* Check SMTP advanced client */
#if (SMTP_CLIENT_ENABLE && SMTP_CLIENT_ATTACH_ENABLE && !DNS_CLIENT_ENABLE)
  #error "::Network:Service:SMTP Client: DNS Client service required"
#endif

/* Avoid syntax-checker errors in editor */
#ifndef NET_HOST_NAME
  #define NET_HOST_NAME         "my_host"
  #define NET_MEM_POOL_SIZE     8000
  #define NET_START_SERVICE     1
#endif

#ifndef NET_MEM_POOL_SIZE
  #define NET_MEM_POOL_SIZE     (NET_MEM_SIZE*4)
#endif

/* Tick interval is 100 ms */
#define NET_TICK_RATE       10
#define STRLEN(str)         sizeof(str)-1
#define MAX_DELAY(speed)    ((2000000/speed) < 25 ? (2000000/speed) : 25)
#define MAX_TOUT(speed)     ((speed < 10000) ? 5 : 1)

#define EXPAND_SYMBOL(name, port) name##port
#define CREATE_SYMBOL(name, port) EXPAND_SYMBOL(name, port)

/* System configuration */
static uint32_t mem_pool [NET_MEM_POOL_SIZE/4];

#if ((HTTP_SERVER_ENABLE && HTTP_SERVER_ROOT_ENABLE) || \
     (FTP_SERVER_ENABLE  && FTP_SERVER_ROOT_ENABLE ) || \
     (TFTP_SERVER_ENABLE && TFTP_SERVER_ROOT_ENABLE))
  /* Buffer for services with root folder enabled */
  static char root_path [NET_ROOT_PATH_SIZE];
#else
  #define root_path      NULL
#endif

/* Forward declarations */
static const net_sys_fn_t sys_fn_init[];
static const net_sys_fn_t sys_fn_run[];

NET_SYS_CFG net_sys_config = {
  mem_pool,
  sizeof (mem_pool),
  root_path,
  NET_HOST_NAME,
  NET_START_SERVICE,
 #if (PPP_ENABLE || SLIP_ENABLE)
  1,
 #else
  0,
 #endif
  sys_fn_init,
  sys_fn_run
};

/* mbedTLS interface configuration */
#if defined(RTE_Network_Web_Server_RO_TLS) || \
    defined(RTE_Network_Web_Server_FS_TLS) || \
    defined(RTE_Network_SMTP_Client_TLS)
  /* mbedTLS interface functions */
  static NET_TLS_IF tls_if_func = {
    netTLS_GetContext,
    netTLS_Connect,
    netTLS_Listen,
    netTLS_GetBuffer,
    netTLS_Write,
    netTLS_Close
  };
  #define __TLS_USED        1
#else
  #define __TLS_USED        0
#endif

#if (ETH0_IP4_ENABLE)
  /* ETH0: ARP configuration */
  extern struct net_arp_ctrl net_eth0_arp_control;
  static NET_ARP_INFO eth0_arp_table[ETH0_ARP_TAB_SIZE];
  static NET_ARP_CFG  eth0_arp_config = {
    &net_eth0_arp_control,
    &net_eth0_if_config,
    eth0_arp_table,
    ETH0_ARP_TAB_SIZE,
    ETH0_ARP_CACHE_TOUT,
    ETH0_ARP_MAX_RETRY,
    ETH0_ARP_RESEND_TOUT * NET_TICK_RATE,
    ETH0_ARP_NOTIFY
  };

  /* ETH0: ICMP configuration */
  extern struct net_icmp_ctrl net_eth0_icmp_control;
  static NET_ICMP_CFG  eth0_icmp_config = {
    &net_eth0_icmp_control,
    &net_eth0_if_config,
  #if (ETH0_ICMP_NO_ECHO)
    1
  #else
    0
  #endif
  };

  /* ETH0: IGMP configuration */
 #if (ETH0_IGMP_ENABLE)
  extern struct net_igmp_ctrl net_eth0_igmp_control;
  static NET_IGMP_INFO eth0_igmp_table[ETH0_IGMP_TAB_SIZE];
  static NET_IGMP_CFG  eth0_igmp_config = {
    &net_eth0_igmp_control,
    &net_eth0_if_config,
    eth0_igmp_table,
    ETH0_IGMP_TAB_SIZE
  };
 #endif

  /* ETH0: DHCP configuration */
 #if (ETH0_DHCP_ENABLE)
  extern struct net_dhcp_ctrl net_eth0_dhcp_control;
  static NET_DHCP_CFG eth0_dhcp_config = {
    &net_eth0_dhcp_control,
    &net_eth0_if_config,
    ETH0_DHCP_VCID,
    STRLEN(ETH0_DHCP_VCID),
    ETH0_DHCP_BOOTFILE,
    ETH0_DHCP_NTP_SERVERS
  };
 #endif

  /* ETH0: IPv4 configuration */
  static NET_LOCALM eth0_localm __attribute__((aligned(4)));
  static NET_IP4_CFG eth0_ip4_config = {
    ETH0_IP4_ADDR,
    ETH0_IP4_MASK,
    ETH0_IP4_GATEWAY,
    ETH0_IP4_PRIMARY_DNS,
    ETH0_IP4_SECONDARY_DNS,
  #if (ETH0_IP4_FRAG_ENABLE)
    ETH0_IP4_MTU,
  #else
    0,
  #endif
    ETH0_NBNS_ENABLE,
    &eth0_arp_config,
    &eth0_icmp_config,
  #if (ETH0_IGMP_ENABLE)
    &eth0_igmp_config,
  #else
    NULL,
  #endif
  #if (ETH0_DHCP_ENABLE)
    &eth0_dhcp_config
  #else
    NULL
  #endif
  };
#endif

#if (ETH0_IP6_ENABLE)
  /* ETH0: NDP configuration */
  extern struct net_ndp_ctrl net_eth0_ndp_control;
  static NET_NDP_INFO eth0_ndp_table[ETH0_NDP_TAB_SIZE];
  static NET_NDP_CFG  eth0_ndp_config = {
    &net_eth0_ndp_control,
    &net_eth0_if_config,
    eth0_ndp_table,
    ETH0_NDP_TAB_SIZE,
    ETH0_NDP_CACHE_TOUT,
    ETH0_NDP_MAX_RETRY,
    ETH0_NDP_RESEND_TOUT * NET_TICK_RATE
  };

  /* ETH0: ICMPv6 configuration */
  extern struct net_icmp6_ctrl net_eth0_icmp6_control;
  static NET_ICMP6_CFG  eth0_icmp6_config = {
    &net_eth0_icmp6_control,
    &net_eth0_if_config,
  #if (ETH0_ICMP6_NO_ECHO)
    1
  #else
    0
  #endif
  };

  /* ETH0: DHCPv6 configuration */
 #if (ETH0_DHCP6_ENABLE)
  extern struct net_dhcp6_ctrl net_eth0_dhcp6_control;
  static NET_DHCP6_CFG eth0_dhcp6_config = {
    &net_eth0_dhcp6_control,
    &net_eth0_if_config,
    ETH0_DHCP6_MODE,
    ETH0_DHCP6_VCLASS_ENABLE,
    ETH0_DHCP6_VCLASS_ENABLE ? ETH0_DHCP6_VCLASS_EID  : 0,
    ETH0_DHCP6_VCLASS_ENABLE ? ETH0_DHCP6_VCLASS_DATA : NULL
  };
 #endif

  /* ETH0: IPv6 configuration */
  static NET_LOCALM6 eth0_localm6 __attribute__((aligned(4)));
  static NET_IP6_CFG eth0_ip6_config = {
    ETH0_IP6_ADDR,
    ETH0_IP6_GATEWAY,
    ETH0_IP6_PRIMARY_DNS,
    ETH0_IP6_SECONDARY_DNS,
    ETH0_IP6_PREFIX_LEN,
  #if (ETH0_IP6_FRAG_ENABLE)
    ETH0_IP6_MTU,
  #else
    0,
  #endif
    &eth0_ndp_config,
    &eth0_icmp6_config,
  #if (ETH0_DHCP6_ENABLE)
    &eth0_dhcp6_config
  #else
    NULL
  #endif
  };
#endif

#if (ETH0_ENABLE)
  static uint8_t eth0_mac_addr[];

  /* ETH0: Interface configuration */
  static NET_IF_STATE eth0_state;
  NET_IF_CFG net_eth0_if_config = {
    &eth0_state,
    NET_IF_CLASS_ETH+0,
    "ETH0",
    eth0_mac_addr,
   #if (ETH0_IP4_ENABLE)
    &eth0_localm,
    &eth0_ip4_config,
   #else
    NULL, NULL,
   #endif
   #if (ETH0_IP6_ENABLE)
    &eth0_localm6,
    &eth0_ip6_config,
   #else
    NULL, NULL,
   #endif
    net_eth_send_frame,
    net_eth_config_mcast,
    net_eth_output
  };

  /* ETH0: low layer configuration */
  extern ARM_DRIVER_ETH_MAC ARM_Driver_ETH_MAC_(ETH0_DRIVER);
  extern ARM_DRIVER_ETH_PHY ARM_Driver_ETH_PHY_(ETH0_DRIVER);
  extern struct net_eth_ctrl net_eth0_if_control;
  static uint8_t eth0_mac_addr[NET_ADDR_ETH_LEN];
  static void eth0_callback (uint32_t event) {
    net_eth_callback (event, &net_eth0_if_control);
  }
  static NET_ETH_CFG eth0_ll_config = {
    &net_eth0_if_control,
    &net_eth0_if_config,
    &ARM_Driver_ETH_MAC_(ETH0_DRIVER),
    &ARM_Driver_ETH_PHY_(ETH0_DRIVER),
    eth0_mac_addr,
    ETH0_MAC_ADDR,
  #if (ETH0_VLAN_ENABLE)
    ETH0_VLAN_ID,
  #else
    0,
  #endif
    0,
    eth0_callback
  };
#endif

#if (ETH1_IP4_ENABLE)
  /* ETH1: ARP configuration */
  extern struct net_arp_ctrl net_eth1_arp_control;
  static NET_ARP_INFO eth1_arp_table[ETH1_ARP_TAB_SIZE];
  static NET_ARP_CFG  eth1_arp_config = {
    &net_eth1_arp_control,
    &net_eth1_if_config,
    eth1_arp_table,
    ETH1_ARP_TAB_SIZE,
    ETH1_ARP_CACHE_TOUT,
    ETH1_ARP_MAX_RETRY,
    ETH1_ARP_RESEND_TOUT * NET_TICK_RATE,
    ETH1_ARP_NOTIFY
  };

  /* ETH1: ICMP configuration */
  extern struct net_icmp_ctrl net_eth1_icmp_control;
  static NET_ICMP_CFG  eth1_icmp_config = {
    &net_eth1_icmp_control,
    &net_eth1_if_config,
  #if (ETH1_ICMP_NO_ECHO)
    1
  #else
    0
  #endif
  };

  /* ETH1: IGMP configuration */
 #if (ETH1_IGMP_ENABLE)
  extern struct net_igmp_ctrl net_eth1_igmp_control;
  static NET_IGMP_INFO eth1_igmp_table[ETH1_IGMP_TAB_SIZE];
  static NET_IGMP_CFG  eth1_igmp_config = {
    &net_eth1_igmp_control,
    &net_eth1_if_config,
    eth1_igmp_table,
    ETH1_IGMP_TAB_SIZE
  };
 #endif

  /* ETH1: DHCP configuration */
 #if (ETH1_DHCP_ENABLE)
  extern struct net_dhcp_ctrl net_eth1_dhcp_control;
  static NET_DHCP_CFG eth1_dhcp_config = {
    &net_eth1_dhcp_control,
    &net_eth1_if_config,
    ETH1_DHCP_VCID,
    STRLEN(ETH1_DHCP_VCID),
    ETH1_DHCP_BOOTFILE,
    ETH1_DHCP_NTP_SERVERS
  };
 #endif

  /* ETH1: IPv4 configuration */
  static NET_LOCALM eth1_localm __attribute__((aligned(4)));
  static NET_IP4_CFG eth1_ip4_config = {
    ETH1_IP4_ADDR,
    ETH1_IP4_MASK,
    ETH1_IP4_GATEWAY,
    ETH1_IP4_PRIMARY_DNS,
    ETH1_IP4_SECONDARY_DNS,
    ETH1_IP4_FRAG_ENABLE ? ETH1_IP4_MTU : 0,
    ETH1_NBNS_ENABLE,
    &eth1_arp_config,
    &eth1_icmp_config,
  #if (ETH1_IGMP_ENABLE)
    &eth1_igmp_config,
  #else
    NULL,
  #endif
  #if (ETH1_DHCP_ENABLE)
    &eth1_dhcp_config
  #else
    NULL
  #endif
  };
#endif

#if (ETH1_IP6_ENABLE)
  /* ETH1: NDP configuration */
  extern struct net_ndp_ctrl net_eth1_ndp_control;
  static NET_NDP_INFO eth1_ndp_table[ETH1_NDP_TAB_SIZE];
  static NET_NDP_CFG  eth1_ndp_config = {
    &net_eth1_ndp_control,
    &net_eth1_if_config,
    eth1_ndp_table,
    ETH1_NDP_TAB_SIZE,
    ETH1_NDP_CACHE_TOUT,
    ETH1_NDP_MAX_RETRY,
    ETH1_NDP_RESEND_TOUT * NET_TICK_RATE
  };

  /* ETH1: ICMPv6 configuration */
  extern struct net_icmp6_ctrl net_eth1_icmp6_control;
  static NET_ICMP6_CFG  eth1_icmp6_config = {
    &net_eth1_icmp6_control,
    &net_eth1_if_config,
  #if (ETH1_ICMP6_NO_ECHO)
    1
  #else
    0
  #endif
  };

  /* ETH1: DHCPv6 configuration */
 #if (ETH1_DHCP6_ENABLE)
  extern struct net_dhcp6_ctrl net_eth1_dhcp6_control;
  static NET_DHCP6_CFG eth1_dhcp6_config = {
    &net_eth1_dhcp6_control,
    &net_eth1_if_config,
    ETH1_DHCP6_MODE,
    ETH1_DHCP6_VCLASS_ENABLE,
    ETH1_DHCP6_VCLASS_ENABLE ? ETH1_DHCP6_VCLASS_EID  : 0,
    ETH1_DHCP6_VCLASS_ENABLE ? ETH1_DHCP6_VCLASS_DATA : NULL
  };
 #endif

  /* ETH1: IPv6 configuration */
  static NET_LOCALM6 eth1_localm6 __attribute__((aligned(4)));
  static NET_IP6_CFG eth1_ip6_config = {
    ETH1_IP6_ADDR,
    ETH1_IP6_GATEWAY,
    ETH1_IP6_PRIMARY_DNS,
    ETH1_IP6_SECONDARY_DNS,
    ETH1_IP6_PREFIX_LEN,
  #if (ETH1_IP6_FRAG_ENABLE)
    ETH1_IP6_MTU,
  #else
    0,
  #endif
    &eth1_ndp_config,
    &eth1_icmp6_config,
  #if (ETH1_DHCP6_ENABLE)
    &eth1_dhcp6_config
  #else
    NULL
  #endif
  };
#endif

#if (ETH1_ENABLE)
  static uint8_t eth1_mac_addr[];

  /* ETH1: Interface configuration */
  static NET_IF_STATE eth1_state;
  NET_IF_CFG net_eth1_if_config = {
    &eth1_state,
    NET_IF_CLASS_ETH+1,
    "ETH1",
    eth1_mac_addr,
   #if (ETH1_IP4_ENABLE)
    &eth1_localm,
    &eth1_ip4_config,
   #else
    NULL, NULL,
   #endif
   #if (ETH1_IP6_ENABLE)
    &eth1_localm6,
    &eth1_ip6_config,
   #else
    NULL, NULL,
   #endif
    net_eth_send_frame,
    net_eth_config_mcast,
    net_eth_output
  };

  /* ETH1: low layer configuration */
  extern ARM_DRIVER_ETH_MAC ARM_Driver_ETH_MAC_(ETH1_DRIVER);
  extern ARM_DRIVER_ETH_PHY ARM_Driver_ETH_PHY_(ETH1_DRIVER);
  extern struct net_eth_ctrl net_eth1_if_control;
  static uint8_t eth1_mac_addr[NET_ADDR_ETH_LEN];
  static void eth1_callback (uint32_t event) {
    net_eth_callback (event, &net_eth1_if_control);
  }
  static NET_ETH_CFG eth1_ll_config = {
    &net_eth1_if_control,
    &net_eth1_if_config,
    &ARM_Driver_ETH_MAC_(ETH1_DRIVER),
    &ARM_Driver_ETH_PHY_(ETH1_DRIVER),
    eth1_mac_addr,
    ETH1_MAC_ADDR,
    ETH1_VLAN_ID * ETH1_VLAN_ENABLE,
    1,
    eth1_callback
  };
#endif

#if (WIFI0_IP4_ENABLE)
  /* WIFI0: ARP configuration */
  extern struct net_arp_ctrl net_wifi0_arp_control;
  static NET_ARP_INFO wifi0_arp_table[WIFI0_ARP_TAB_SIZE];
  static NET_ARP_CFG  wifi0_arp_config = {
    &net_wifi0_arp_control,
    &net_wifi0_if_config,
    wifi0_arp_table,
    WIFI0_ARP_TAB_SIZE,
    WIFI0_ARP_CACHE_TOUT,
    WIFI0_ARP_MAX_RETRY,
    WIFI0_ARP_RESEND_TOUT * NET_TICK_RATE,
    WIFI0_ARP_NOTIFY
  };

  /* WIFI0: ICMP configuration */
  extern struct net_icmp_ctrl net_wifi0_icmp_control;
  static NET_ICMP_CFG  wifi0_icmp_config = {
    &net_wifi0_icmp_control,
    &net_wifi0_if_config,
  #if (WIFI0_ICMP_NO_ECHO)
    1
  #else
    0
  #endif
  };

  /* WIFI0: IGMP configuration */
 #if (WIFI0_IGMP_ENABLE)
  extern struct net_igmp_ctrl net_wifi0_igmp_control;
  static NET_IGMP_INFO wifi0_igmp_table[WIFI0_IGMP_TAB_SIZE];
  static NET_IGMP_CFG  wifi0_igmp_config = {
    &net_wifi0_igmp_control,
    &net_wifi0_if_config,
    wifi0_igmp_table,
    WIFI0_IGMP_TAB_SIZE
  };
 #endif

  /* WIFI0: DHCP configuration */
 #if (WIFI0_DHCP_ENABLE)
  extern struct net_dhcp_ctrl net_wifi0_dhcp_control;
  static NET_DHCP_CFG wifi0_dhcp_config = {
    &net_wifi0_dhcp_control,
    &net_wifi0_if_config,
    WIFI0_DHCP_VCID,
    STRLEN(WIFI0_DHCP_VCID),
    WIFI0_DHCP_BOOTFILE,
    WIFI0_DHCP_NTP_SERVERS
  };
 #endif

  /* WIFI0: IPv4 configuration */
  static NET_LOCALM wifi0_localm __attribute__((aligned(4)));
  static NET_IP4_CFG wifi0_ip4_config = {
    WIFI0_IP4_ADDR,
    WIFI0_IP4_MASK,
    WIFI0_IP4_GATEWAY,
    WIFI0_IP4_PRIMARY_DNS,
    WIFI0_IP4_SECONDARY_DNS,
    WIFI0_IP4_FRAG_ENABLE ? WIFI0_IP4_MTU : 0,
    WIFI0_NBNS_ENABLE,
    &wifi0_arp_config,
    &wifi0_icmp_config,
  #if (WIFI0_IGMP_ENABLE)
    &wifi0_igmp_config,
  #else
    NULL,
  #endif
  #if (WIFI0_DHCP_ENABLE)
    &wifi0_dhcp_config
  #else
    NULL
  #endif
  };
#endif

#if (WIFI0_IP6_ENABLE)
  /* WIFI0: NDP configuration */
  extern struct net_ndp_ctrl net_wifi0_ndp_control;
  static NET_NDP_INFO wifi0_ndp_table[WIFI0_NDP_TAB_SIZE];
  static NET_NDP_CFG  wifi0_ndp_config = {
    &net_wifi0_ndp_control,
    &net_wifi0_if_config,
    wifi0_ndp_table,
    WIFI0_NDP_TAB_SIZE,
    WIFI0_NDP_CACHE_TOUT,
    WIFI0_NDP_MAX_RETRY,
    WIFI0_NDP_RESEND_TOUT * NET_TICK_RATE
  };

  /* WIFI0: ICMPv6 configuration */
  extern struct net_icmp6_ctrl net_wifi0_icmp6_control;
  static NET_ICMP6_CFG  wifi0_icmp6_config = {
    &net_wifi0_icmp6_control,
    &net_wifi0_if_config,
  #if (WIFI0_ICMP6_NO_ECHO)
    1
  #else
    0
  #endif
  };

  /* WIFI0: DHCPv6 configuration */
 #if (WIFI0_DHCP6_ENABLE)
  extern struct net_dhcp6_ctrl net_wifi0_dhcp6_control;
  static NET_DHCP6_CFG wifi0_dhcp6_config = {
    &net_wifi0_dhcp6_control,
    &net_wifi0_if_config,
    WIFI0_DHCP6_MODE,
    WIFI0_DHCP6_VCLASS_ENABLE,
    WIFI0_DHCP6_VCLASS_ENABLE ? WIFI0_DHCP6_VCLASS_EID  : 0,
    WIFI0_DHCP6_VCLASS_ENABLE ? WIFI0_DHCP6_VCLASS_DATA : NULL
  };
 #endif

  /* WIFI0: IPv6 configuration */
  static NET_LOCALM6 wifi0_localm6 __attribute__((aligned(4)));
  static NET_IP6_CFG wifi0_ip6_config = {
    WIFI0_IP6_ADDR,
    WIFI0_IP6_GATEWAY,
    WIFI0_IP6_PRIMARY_DNS,
    WIFI0_IP6_SECONDARY_DNS,
    WIFI0_IP6_PREFIX_LEN,
  #if (WIFI0_IP6_FRAG_ENABLE)
    WIFI0_IP6_MTU,
  #else
    0,
  #endif
    &wifi0_ndp_config,
    &wifi0_icmp6_config,
  #if (WIFI0_DHCP6_ENABLE)
    &wifi0_dhcp6_config
  #else
    NULL
  #endif
  };
#endif

#if (WIFI0_ENABLE)
  static uint8_t wifi0_mac_addr[];

  /* WIFI0: Interface configuration */
  static NET_IF_STATE wifi0_state;
  NET_IF_CFG net_wifi0_if_config = {
    &wifi0_state,
    NET_IF_CLASS_WIFI+0,
    "WIFI0",
    wifi0_mac_addr,
   #if (WIFI0_IP4_ENABLE)
    &wifi0_localm,
    &wifi0_ip4_config,
   #else
    NULL, NULL,
   #endif
   #if (WIFI0_IP6_ENABLE)
    &wifi0_localm6,
    &wifi0_ip6_config,
   #else
    NULL, NULL,
   #endif
    net_wifi_send_frame,
    NULL,
    net_wifi_output
  };

  /* WIFI0: low layer configuration */
  extern ARM_DRIVER_WIFI ARM_Driver_WiFi_(WIFI0_DRIVER);
  extern struct net_wifi_ctrl net_wifi0_if_control;
  static uint8_t wifi0_mac_addr[NET_ADDR_ETH_LEN];
  static void wifi0_callback (uint32_t event, void *arg) {
    net_wifi_callback (event, &net_wifi0_if_control); (void)arg;
  }
  static NET_WIFI_CFG wifi0_ll_config = {
    &net_wifi0_if_control,
    &net_wifi0_if_config,
    &ARM_Driver_WiFi_(WIFI0_DRIVER),
    wifi0_mac_addr,
    WIFI0_MAC_CONFIG ? WIFI0_MAC_ADDR : NULL,
    WIFI0_MODE,
    0,
    wifi0_callback
  };
#endif

#if (WIFI1_IP4_ENABLE)
  /* WIFI1: ARP configuration */
  extern struct net_arp_ctrl net_wifi1_arp_control;
  static NET_ARP_INFO wifi1_arp_table[WIFI1_ARP_TAB_SIZE];
  static NET_ARP_CFG  wifi1_arp_config = {
    &net_wifi1_arp_control,
    &net_wifi1_if_config,
    wifi1_arp_table,
    WIFI1_ARP_TAB_SIZE,
    WIFI1_ARP_CACHE_TOUT,
    WIFI1_ARP_MAX_RETRY,
    WIFI1_ARP_RESEND_TOUT * NET_TICK_RATE,
    WIFI1_ARP_NOTIFY
  };

  /* WIFI1: ICMP configuration */
  extern struct net_icmp_ctrl net_wifi1_icmp_control;
  static NET_ICMP_CFG  wifi1_icmp_config = {
    &net_wifi1_icmp_control,
    &net_wifi1_if_config,
  #if (WIFI1_ICMP_NO_ECHO)
    1
  #else
    0
  #endif
  };

  /* WIFI1: IGMP configuration */
 #if (WIFI1_IGMP_ENABLE)
  extern struct net_igmp_ctrl net_wifi1_igmp_control;
  static NET_IGMP_INFO wifi1_igmp_table[WIFI1_IGMP_TAB_SIZE];
  static NET_IGMP_CFG  wifi1_igmp_config = {
    &net_wifi1_igmp_control,
    &net_wifi1_if_config,
    wifi1_igmp_table,
    WIFI1_IGMP_TAB_SIZE
  };
 #endif

  /* WIFI1: DHCP configuration */
 #if (WIFI1_DHCP_ENABLE)
  extern struct net_dhcp_ctrl net_wifi1_dhcp_control;
  static NET_DHCP_CFG wifi1_dhcp_config = {
    &net_wifi1_dhcp_control,
    &net_wifi1_if_config,
    WIFI1_DHCP_VCID,
    STRLEN(WIFI1_DHCP_VCID),
    WIFI1_DHCP_BOOTFILE,
    WIFI1_DHCP_NTP_SERVERS
  };
 #endif

  /* WIFI1: IPv4 configuration */
  static NET_LOCALM wifi1_localm __attribute__((aligned(4)));
  static NET_IP4_CFG wifi1_ip4_config = {
    WIFI1_IP4_ADDR,
    WIFI1_IP4_MASK,
    WIFI1_IP4_GATEWAY,
    WIFI1_IP4_PRIMARY_DNS,
    WIFI1_IP4_SECONDARY_DNS,
    WIFI1_IP4_FRAG_ENABLE ? WIFI1_IP4_MTU : 0,
    WIFI1_NBNS_ENABLE,
    &wifi1_arp_config,
    &wifi1_icmp_config,
  #if (WIFI1_IGMP_ENABLE)
    &wifi1_igmp_config,
  #else
    NULL,
  #endif
  #if (WIFI1_DHCP_ENABLE)
    &wifi1_dhcp_config
  #else
    NULL
  #endif
  };
#endif

#if (WIFI1_IP6_ENABLE)
  /* WIFI1: NDP configuration */
  extern struct net_ndp_ctrl net_wifi1_ndp_control;
  static NET_NDP_INFO wifi1_ndp_table[WIFI1_NDP_TAB_SIZE];
  static NET_NDP_CFG  wifi1_ndp_config = {
    &net_wifi1_ndp_control,
    &net_wifi1_if_config,
    wifi1_ndp_table,
    WIFI1_NDP_TAB_SIZE,
    WIFI1_NDP_CACHE_TOUT,
    WIFI1_NDP_MAX_RETRY,
    WIFI1_NDP_RESEND_TOUT * NET_TICK_RATE
  };

  /* WIFI1: ICMPv6 configuration */
  extern struct net_icmp6_ctrl net_wifi1_icmp6_control;
  static NET_ICMP6_CFG  wifi1_icmp6_config = {
    &net_wifi1_icmp6_control,
    &net_wifi1_if_config,
  #if (WIFI1_ICMP6_NO_ECHO)
    1
  #else
    0
  #endif
  };

  /* WIFI1: DHCPv6 configuration */
 #if (WIFI1_DHCP6_ENABLE)
  extern struct net_dhcp6_ctrl net_wifi1_dhcp6_control;
  static NET_DHCP6_CFG wifi1_dhcp6_config = {
    &net_wifi1_dhcp6_control,
    &net_wifi1_if_config,
    WIFI1_DHCP6_MODE,
    WIFI1_DHCP6_VCLASS_ENABLE,
    WIFI1_DHCP6_VCLASS_ENABLE ? WIFI1_DHCP6_VCLASS_EID  : 0,
    WIFI1_DHCP6_VCLASS_ENABLE ? WIFI1_DHCP6_VCLASS_DATA : NULL
  };
 #endif

  /* WIFI1: IPv6 configuration */
  static NET_LOCALM6 wifi1_localm6 __attribute__((aligned(4)));
  static NET_IP6_CFG wifi1_ip6_config = {
    WIFI1_IP6_ADDR,
    WIFI1_IP6_GATEWAY,
    WIFI1_IP6_PRIMARY_DNS,
    WIFI1_IP6_SECONDARY_DNS,
    WIFI1_IP6_PREFIX_LEN,
  #if (WIFI1_IP6_FRAG_ENABLE)
    WIFI1_IP6_MTU,
  #else
    0,
  #endif
    &wifi1_ndp_config,
    &wifi1_icmp6_config,
  #if (WIFI1_DHCP6_ENABLE)
    &wifi1_dhcp6_config
  #else
    NULL
  #endif
  };
#endif

#if (WIFI1_ENABLE)
  static uint8_t wifi1_mac_addr[];

  /* WIFI1: Interface configuration */
  static NET_IF_STATE wifi1_state;
  NET_IF_CFG net_wifi1_if_config = {
    &wifi1_state,
    NET_IF_CLASS_WIFI+1,
    "WIFI1",
    wifi1_mac_addr,
   #if (WIFI1_IP4_ENABLE)
    &wifi1_localm,
    &wifi1_ip4_config,
   #else
    NULL, NULL,
   #endif
   #if (WIFI1_IP6_ENABLE)
    &wifi1_localm6,
    &wifi1_ip6_config,
   #else
    NULL, NULL,
   #endif
    net_wifi_send_frame,
    NULL,
    net_wifi_output
  };

  /* WIFI1: low layer configuration */
  extern ARM_DRIVER_WIFI ARM_Driver_WiFi_(WIFI1_DRIVER);
  extern struct net_wifi_ctrl net_wifi1_if_control;
  static uint8_t wifi1_mac_addr[NET_ADDR_ETH_LEN];
  static void wifi1_callback (uint32_t event, void *arg) {
    net_wifi_callback (event, &net_wifi1_if_control); (void)arg;
  }
  static NET_WIFI_CFG wifi1_ll_config = {
    &net_wifi1_if_control,
    &net_wifi1_if_config,
    &ARM_Driver_WiFi_(WIFI1_DRIVER),
    wifi1_mac_addr,
    WIFI1_MAC_CONFIG ? WIFI1_MAC_ADDR : NULL,
    WIFI1_MODE,
    1,
    wifi1_callback
  };
#endif

#if (PPP_ENABLE)
  /* PPP: ICMP configuration */
  extern struct net_icmp_ctrl net_ppp0_icmp_control;
  static NET_ICMP_CFG  ppp0_icmp_config = {
    &net_ppp0_icmp_control,
    &net_ppp0_if_config,
  #if (PPP_ICMP_NO_ECHO)
    1
  #else
    0
  #endif
  };

  /* PPP: IPv4 configuration */
  static NET_LOCALM ppp0_localm __attribute__((aligned(4)));
  static NET_IP4_CFG ppp0_ip4_config = {
    PPP_IP4_ADDR,
    NULL,
    NULL,
    PPP_IP4_PRIMARY_DNS,
    PPP_IP4_SECONDARY_DNS,
  #if (PPP_IP4_FRAG_ENABLE)
    PPP_IP4_MTU,
  #else
    0,
  #endif
    0,
    NULL,
    &ppp0_icmp_config,
    NULL,
    NULL
  };

  /* PPP: Interface configuration */
  NET_IF_CFG net_ppp0_if_config = {
    NULL,
    NET_IF_CLASS_PPP,
    "PPP0",
    NULL,
    &ppp0_localm,
    &ppp0_ip4_config,
    NULL,
    NULL,
    net_ppp_send_frame,
    NULL,
    NULL
  };

  /* PPP: Communication device */
  extern ARM_DRIVER_USART CREATE_SYMBOL (Driver_USART, PPP_USART_DRIVER);
  extern DRIVER_MODEM          Driver_MODEM;
  extern struct net_com_ctrl   net_ppp0_com_control;
  extern struct net_modem_ctrl net_ppp0_modem_control;
  static void ppp0_com_callback (uint32_t event) {
    net_com_callback (event, &net_ppp0_com_control);
  }
  static void ppp0_modem_callback (const char *cmd, const char *response,
                                   uint32_t timeout, uint32_t retries) {
    net_modem_callback (cmd, response, timeout, retries, &net_ppp0_modem_control);
  }
  static NET_COM_CFG ppp0_com_config = {
    &net_ppp0_com_control,
    &net_ppp0_modem_control,
    &CREATE_SYMBOL (Driver_USART, PPP_USART_DRIVER),
    &Driver_MODEM,
    PPP_MODEM_INIT_STRING,
    PPP_MODEM_SPEED,
    PPP_MODEM_FLOW_CONTROL,
    MAX_DELAY(PPP_MODEM_SPEED),
    MAX_TOUT(PPP_MODEM_SPEED) * NET_TICK_RATE,
    ppp0_com_callback,
    ppp0_modem_callback
  };

  /* PPP: low layer configuration */
  extern struct net_ppp_ctrl  net_ppp0_if_control;
  extern struct net_lcp_ctrl  net_ppp0_lcp_control;
  extern struct net_ipcp_ctrl net_ppp0_ipcp_control;
  static NET_PPP_CFG ppp0_ll_config = {
    &net_ppp0_if_control,
    &net_ppp0_if_config,
    &ppp0_com_config,
    PPP_ACCM,
    PPP_RETRY_TOUT * NET_TICK_RATE,
    PPP_ECHO_TOUT,
    PPP_MAX_RETRY,
    PPP_AUTH_ENABLE,
    PPP_GET_IP,
    PPP_DEFAULT_GW,
    0,
    &net_ppp0_lcp_control,
  #if (PPP_AUTH_ENABLE && PPP_PAP_ENABLE)
    &net_ppp0_pap_auth,
  #else
    NULL,
  #endif
  #if (PPP_AUTH_ENABLE && PPP_CHAP_ENABLE)
    &net_ppp0_chap_auth,
  #else
    NULL,
  #endif
    &net_ppp0_ipcp_control
  };
#endif

#if (SLIP_ENABLE)
  /* SLIP: ICMP configuration */
  extern struct net_icmp_ctrl net_slip0_icmp_control;
  static NET_ICMP_CFG  slip0_icmp_config = {
    &net_slip0_icmp_control,
    &net_slip0_if_config,
  #if (SLIP_ICMP_NO_ECHO)
    1
  #else
    0
  #endif
  };

  /* SLIP: IPv4 configuration */
  static NET_LOCALM slip0_localm __attribute__((aligned(4)));
  static NET_IP4_CFG slip0_ip4_config = {
    SLIP_IP4_ADDR,
    NULL,
    NULL,
    SLIP_IP4_PRIMARY_DNS,
    SLIP_IP4_SECONDARY_DNS,
  #if (SLIP_IP4_FRAG_ENABLE)
    SLIP_IP4_MTU,
  #else
    0,
  #endif
    0,
    NULL,
    &slip0_icmp_config,
    NULL,
    NULL
  };

  /* SLIP: Interface configuration */
  NET_IF_CFG net_slip0_if_config = {
    NULL,
    NET_IF_CLASS_SLIP,
    "SLIP0",
    NULL,
    &slip0_localm,
    &slip0_ip4_config,
    NULL,
    NULL,
    net_slip_send_frame,
    NULL,
    NULL
  };

  /* SLIP: Communication device */
  extern ARM_DRIVER_USART CREATE_SYMBOL (Driver_USART, SLIP_USART_DRIVER);
  extern DRIVER_MODEM          Driver_MODEM;
  extern struct net_com_ctrl   net_slip0_com_control;
  extern struct net_modem_ctrl net_slip0_modem_control;
  static void slip0_com_callback (uint32_t event) {
    net_com_callback (event, &net_slip0_com_control);
  }
  static void slip0_modem_callback (const char *cmd, const char *response,
                                    uint32_t timeout, uint32_t retries) {
    net_modem_callback (cmd, response, timeout, retries, &net_slip0_modem_control);
  }
  static NET_COM_CFG slip0_com_config = {
    &net_slip0_com_control,
    &net_slip0_modem_control,
    &CREATE_SYMBOL (Driver_USART, SLIP_USART_DRIVER),
    &Driver_MODEM,
    SLIP_MODEM_INIT_STRING,
    SLIP_MODEM_SPEED,
    SLIP_MODEM_FLOW_CONTROL,
    MAX_DELAY(SLIP_MODEM_SPEED),
    MAX_TOUT(SLIP_MODEM_SPEED) * NET_TICK_RATE,
    slip0_com_callback,
    slip0_modem_callback
  };

  /* SLIP: low layer configuration */
  extern struct net_slip_ctrl net_slip0_if_control;
  static NET_SLIP_CFG slip0_ll_config = {
    &net_slip0_if_control,
    &net_slip0_if_config,
    &slip0_com_config,
    SLIP_DEFAULT_GW,
    0
  };
#endif

/* List of ETH network interfaces */
#if (__ETH_ENA)
  NET_ETH_CFG *const net_eth_list[] = {
  #if (ETH0_ENABLE)
    &eth0_ll_config,
  #endif
  #if (ETH1_ENABLE)
    &eth1_ll_config,
  #endif
    NULL
  };
#endif

/* List of WIFI network interfaces */
#if (__WIFI_ENA)
  NET_WIFI_CFG *const net_wifi_list[] = {
  #if (WIFI0_ENABLE)
    &wifi0_ll_config,
  #endif
  #if (WIFI1_ENABLE)
    &wifi1_ll_config,
  #endif
    NULL
  };
#endif

/* List of PPP network interfaces */
#if (PPP_ENABLE)
  NET_PPP_CFG *const net_ppp_list[] = {
    &ppp0_ll_config,
    NULL
  };
#endif

/* List of SLIP network interfaces */
#if (SLIP_ENABLE)
  NET_SLIP_CFG *const net_slip_list[] = {
    &slip0_ll_config,
    NULL
  };
#endif

/* List of LAN network interfaces */
NET_IF_CFG *const net_if_list_lan[] = {
 #if (ETH0_ENABLE)
  &net_eth0_if_config,
 #endif
 #if (ETH1_ENABLE)
  &net_eth1_if_config,
 #endif
 #if (WIFI0_ENABLE)
  &net_wifi0_if_config,
 #endif
 #if (WIFI1_ENABLE)
  &net_wifi1_if_config,
 #endif
  NULL
};

/* List of external network interfaces */
NET_IF_CFG *const net_if_list_all[] = {
 #if (ETH0_ENABLE)
  &net_eth0_if_config,
 #endif
 #if (ETH1_ENABLE)
  &net_eth1_if_config,
 #endif
 #if (WIFI0_ENABLE)
  &net_wifi0_if_config,
 #endif
 #if (WIFI1_ENABLE)
  &net_wifi1_if_config,
 #endif
 #if (PPP_ENABLE)
  &net_ppp0_if_config,
 #endif
 #if (SLIP_ENABLE)
  &net_slip0_if_config,
 #endif
  NULL
};

/* Default internet access interface(s) */
NET_IF_CFG *const net_if_inet_def[] = {
 #if   (PPP_DEFAULT_GW)
  &net_ppp0_if_config
 #elif (SLIP_DEFAULT_GW)
  &net_slip0_if_config
 #elif (ETH0_IP4_ENABLE)
  &net_eth0_if_config
 #elif (ETH1_IP4_ENABLE)
  &net_eth1_if_config
 #elif (WIFI0_IP4_ENABLE)
  &net_wifi0_if_config
 #elif (WIFI1_IP4_ENABLE)
  &net_wifi1_if_config
 #else
  NULL
 #endif
#ifdef RTE_Network_IPv6
 #if   (ETH0_IP6_ENABLE)
, &net_eth0_if_config
 #elif (ETH1_IP6_ENABLE)
, &net_eth1_if_config
 #elif (WIFI0_IP6_ENABLE)
, &net_wifi0_if_config
 #elif (WIFI1_IP6_ENABLE)
, &net_wifi1_if_config
 #else
, NULL
 #endif
#endif
};

/* Default link interface(s) */
NET_IF_CFG *const net_if_link_def[] = {
 #if   (ETH0_IP4_ENABLE)
  &net_eth0_if_config
 #elif (ETH1_IP4_ENABLE)
  &net_eth1_if_config
 #elif (WIFI0_IP4_ENABLE)
  &net_wifi0_if_config
 #elif (WIFI1_IP4_ENABLE)
  &net_wifi1_if_config
 #else
  NULL
 #endif
#ifdef RTE_Network_IPv6
 #if   (ETH0_IP6_ENABLE)
, &net_eth0_if_config
 #elif (ETH1_IP6_ENABLE)
, &net_eth1_if_config
 #elif (WIFI0_IP6_ENABLE)
, &net_wifi0_if_config
 #elif (WIFI1_IP6_ENABLE)
, &net_wifi1_if_config
 #else
, NULL
 #endif
#endif
};

/* List of ARP instances */
NET_ARP_CFG *const net_arp_list[] = {
  /* Required in IPv4 */
 #if (ETH0_IP4_ENABLE)
  &eth0_arp_config,
 #endif
 #if (ETH1_IP4_ENABLE)
  &eth1_arp_config,
 #endif
 #if (WIFI0_IP4_ENABLE)
  &wifi0_arp_config,
 #endif
 #if (WIFI1_IP4_ENABLE)
  &wifi1_arp_config,
 #endif
  NULL
};

/* List of ICMP instances */
NET_ICMP_CFG *const net_icmp_list[] = {
  /* Required in IPv4 */
 #if (ETH0_IP4_ENABLE)
  &eth0_icmp_config,
 #endif
 #if (ETH1_IP4_ENABLE)
  &eth1_icmp_config,
 #endif
 #if (WIFI0_IP4_ENABLE)
  &wifi0_icmp_config,
 #endif
 #if (WIFI1_IP4_ENABLE)
  &wifi1_icmp_config,
 #endif
 #if (PPP_ENABLE)
  &ppp0_icmp_config,
 #endif
 #if (SLIP_ENABLE)
  slip0_icmp_config,
 #endif
  NULL
};

/* List of IGMP instances */
#if (__IGMP_ENA) || defined(RTE_Network_Source)
  NET_IGMP_CFG *const net_igmp_list[] = {
  #if (ETH0_IGMP_ENABLE)
    &eth0_igmp_config,
  #endif
  #if (ETH1_IGMP_ENABLE)
    &eth1_igmp_config,
  #endif
  #if (WIFI0_IGMP_ENABLE)
    &wifi0_igmp_config,
  #endif
  #if (WIFI1_IGMP_ENABLE)
    &wifi1_igmp_config,
  #endif
    NULL
  };
#endif

/* List of DHCP instances */
#if (__DHCP_ENA) || defined(RTE_Network_Source)
  NET_DHCP_CFG *const net_dhcp_list[] = {
  #if (ETH0_DHCP_ENABLE)
    &eth0_dhcp_config,
  #endif
  #if (ETH1_DHCP_ENABLE)
    &eth1_dhcp_config,
  #endif
  #if (WIFI0_DHCP_ENABLE)
    &wifi0_dhcp_config,
  #endif
  #if (WIFI1_DHCP_ENABLE)
    &wifi1_dhcp_config,
  #endif
    NULL
  };
#endif

/* List of NDP instances */
#ifdef RTE_Network_IPv6
  /* Required in IPv6 */
  NET_NDP_CFG *const net_ndp_list[] = {
  #if (ETH0_IP6_ENABLE)
    &eth0_ndp_config,
  #endif
  #if (ETH1_IP6_ENABLE)
    &eth1_ndp_config,
  #endif
  #if (WIFI0_IP6_ENABLE)
    &wifi0_ndp_config,
  #endif
  #if (WIFI1_IP6_ENABLE)
    &wifi1_ndp_config,
  #endif
    NULL
  };
#endif

/* List of ICMPv6 instances */
#ifdef RTE_Network_IPv6
  NET_ICMP6_CFG *const net_icmp6_list[] = {
  /* Required in IPv6 */
  #if (ETH0_IP6_ENABLE)
    &eth0_icmp6_config,
  #endif
  #if (ETH1_IP6_ENABLE)
    &eth1_icmp6_config,
  #endif
  #if (WIFI0_IP6_ENABLE)
    &wifi0_icmp6_config,
  #endif
  #if (WIFI1_IP6_ENABLE)
    &wifi1_icmp6_config,
  #endif
  NULL
};
#endif

#if (__DHCP6_ENA) || defined(RTE_Network_Source)
  /* List of DHCPv6 instances */
  NET_DHCP6_CFG *const net_dhcp6_list[] = {
  #if (ETH0_DHCP6_ENABLE)
    &eth0_dhcp6_config,
  #endif
  #if (ETH1_DHCP6_ENABLE)
    &eth1_dhcp6_config,
  #endif
  #if (WIFI0_DHCP6_ENABLE)
    &wifi0_dhcp6_config,
  #endif
  #if (WIFI1_DHCP6_ENABLE)
    &wifi1_dhcp6_config,
  #endif
    NULL
  };
#endif

/* IPv4 fragmentation and reassembly */
#if (__IP4_FRAG_ENA)
 #ifndef IP4_FRAG_REASS_NUM
  /* Number of reassembly sessions */
  #define IP4_FRAG_REASS_NUM    5
 #endif
 #ifndef IP4_FRAG_REASS_TOUT
  /* Reassembly timeout in seconds */
  #define IP4_FRAG_REASS_TOUT   10
 #endif
  static NET_IP_FRAG_INFO ip4_frag_scb[IP4_FRAG_REASS_NUM];
  NET_IP_FRAG_CFG net_ip4_frag_config = {
    ip4_frag_scb,
    IP4_FRAG_REASS_NUM,
    IP4_FRAG_REASS_TOUT * NET_TICK_RATE
  };
#elif defined(RTE_Network_Source)
  NET_IP_FRAG_CFG net_ip4_frag_config = { NULL, 0, 0 };
#endif

/* IPv6 fragmentation and reassembly */
#if (__IP6_FRAG_ENA)
 #ifndef IP6_FRAG_REASS_NUM
  /* Number of reassembly sessions */
  #define IP6_FRAG_REASS_NUM    5
 #endif
 #ifndef IP6_FRAG_REASS_TOUT
  /* Reassembly timeout in seconds */
  #define IP6_FRAG_REASS_TOUT   10
 #endif
  static NET_IP_FRAG_INFO ip6_frag_scb[IP6_FRAG_REASS_NUM];
  NET_IP_FRAG_CFG net_ip6_frag_config = {
    ip6_frag_scb,
    IP6_FRAG_REASS_NUM,
    IP6_FRAG_REASS_TOUT * NET_TICK_RATE
  };
#elif defined(RTE_Network_Source)
  NET_IP_FRAG_CFG net_ip6_frag_config = { NULL, 0, 0 };
#endif

/* NBNS cache */
#if (__NBNS_ENA)
 #ifndef NBNS_CLIENT_TAB_SIZE
  /* Number of NBNS cache entries */
  #define NBNS_CLIENT_TAB_SIZE  5
 #endif
 #ifndef NBNS_CLIENT_TAB_TOUT
  /* Cache timeout in seconds */
  #define NBNS_CLIENT_TAB_TOUT  300
 #endif
  static NET_NBNS_INFO nbns_table[NBNS_CLIENT_TAB_SIZE];
  NET_NBNS_CFG net_nbns_config = {
    nbns_table,
    NBNS_CLIENT_TAB_SIZE,
    NBNS_CLIENT_TAB_TOUT
  };
#elif defined(RTE_Network_Source)
  NET_NBNS_CFG net_nbns_config = { NULL, 0, 0 };
#endif

#if (UDP_ENABLE)
  static NET_UDP_INFO udp_scb[UDP_NUM_SOCKS];
  NET_UDP_CFG net_udp_config = {
    udp_scb,
    UDP_NUM_SOCKS,
  #ifdef UDP_DYN_PORT_START
    UDP_DYN_PORT_START,
    UDP_DYN_PORT_END
  #else
    49152,
    65535
  #endif
  };
#endif

#if (TCP_ENABLE)
  static NET_TCP_INFO tcp_scb[TCP_NUM_SOCKS];
  NET_TCP_CFG net_tcp_config = {
    tcp_scb,
    TCP_NUM_SOCKS,
    TCP_MAX_RETRY,
    TCP_RETRY_TOUT * NET_TICK_RATE,
    TCP_SYN_RETRY_TOUT * NET_TICK_RATE,
    TCP_INITIAL_RETRY_TOUT * NET_TICK_RATE,
    TCP_DEFAULT_TOUT,
    TCP_MAX_SEG_SIZE,
    TCP_RECEIVE_WIN_SIZE,
    TCP_CONNECT_RETRY,
  #ifdef TCP_DYN_PORT_START
    TCP_DYN_PORT_START,
    TCP_DYN_PORT_END
  #else
    49152,
    65535
  #endif
  };
#endif

#if (BSD_ENABLE)
  static NET_BSD_INFO bsd_scb[BSD_NUM_SOCKS + BSD_SERVER_SOCKS];
  NET_BSD_CFG net_bsd_config = {
    bsd_scb,
    BSD_NUM_SOCKS + BSD_SERVER_SOCKS,
    BSD_RECEIVE_TOUT * NET_TICK_RATE
  };
#endif

#if (HTTP_SERVER_ENABLE)
  static NET_HTTP_INFO http_scb[HTTP_SERVER_NUM_SESSIONS];
 /* HTTP Digest authentication */
 #if (HTTP_SERVER_AUTH_ENABLE && HTTP_SERVER_AUTH_TYPE == 1)
  #ifndef HTTP_NONCE_TAB_SIZE
   /* Number of cache entries */
   #ifdef HTTP_SERVER_NONCE_TAB_SIZE
    #define HTTP_NONCE_TAB_SIZE   HTTP_SERVER_NONCE_TAB_SIZE
   #else
    #define HTTP_NONCE_TAB_SIZE   8
   #endif
  #endif
  #ifndef HTTP_NONCE_CACHE_TOUT
   /* Nonce cache timeout in seconds */
   #ifdef HTTP_SERVER_NONCE_CACHE_TOUT
    #define HTTP_NONCE_CACHE_TOUT HTTP_SERVER_NONCE_CACHE_TOUT
   #else
    #define HTTP_NONCE_CACHE_TOUT 60
   #endif
  #endif
  static NET_HTTP_NONCE http_nonce[HTTP_NONCE_TAB_SIZE];
 #endif
  NET_HTTP_CFG net_http_config = {
    http_scb,
    HTTP_SERVER_NUM_SESSIONS,
    HTTP_SERVER_AUTH_ENABLE,
    HTTP_SERVER_PORT_NUM,
    HTTP_SERVER_ID,
  #if (HTTP_SERVER_ROOT_ENABLE && !defined(RTE_Network_Web_Server_RO))
    HTTP_SERVER_ROOT_FOLDER,
  #else
    NULL,
  #endif
  #if (HTTP_SERVER_AUTH_ENABLE)
    HTTP_SERVER_AUTH_REALM,
    STRLEN(HTTP_SERVER_AUTH_REALM),
  #else
    NULL,0,
  #endif
  #if (HTTP_SERVER_AUTH_ENABLE && HTTP_SERVER_AUTH_ADMIN)
    HTTP_SERVER_AUTH_USER,
    HTTP_SERVER_AUTH_PASS,
    STRLEN(HTTP_SERVER_AUTH_USER),
    STRLEN(HTTP_SERVER_AUTH_PASS),
  #else
    NULL,NULL,0,0,
  #endif
  #if (HTTP_SERVER_AUTH_ENABLE && HTTP_SERVER_AUTH_TYPE == 1)
    http_nonce,
    HTTP_NONCE_TAB_SIZE,
    HTTP_NONCE_CACHE_TOUT * NET_TICK_RATE,
  #else
    NULL,0,0,
  #endif
  #if (HTTP_SERVER_AUTH_ENABLE && HTTP_SERVER_AUTH_TYPE == 0)
    &net_http_auth_basic_func,
  #elif (HTTP_SERVER_AUTH_ENABLE && HTTP_SERVER_AUTH_TYPE == 1)
    &net_http_auth_digest_func,
  #else
    NULL,
  #endif
  #if defined(RTE_Network_Web_Server_RO_TLS) || defined(RTE_Network_Web_Server_FS_TLS)
    &tls_if_func
  #else
    NULL
  #endif
  };
#endif

#if (TELNET_SERVER_ENABLE)
  static NET_TELNET_INFO telnet_scb[TELNET_SERVER_NUM_SESSIONS];
  NET_TELNET_CFG net_telnet_config = {
    telnet_scb,
    TELNET_SERVER_NUM_SESSIONS,
    TELNET_SERVER_AUTH_ENABLE,
    TELNET_SERVER_NO_ECHO,
    TELNET_SERVER_PORT_NUM,
    TELNET_SERVER_TOUT,
  #if (TELNET_SERVER_AUTH_ENABLE && TELNET_SERVER_AUTH_ADMIN)
    TELNET_SERVER_AUTH_USER,
    TELNET_SERVER_AUTH_PASS,
    STRLEN(TELNET_SERVER_AUTH_USER),
    STRLEN(TELNET_SERVER_AUTH_PASS),
  #else
    NULL,NULL,0,0
  #endif
  };
#endif

#if (TFTP_SERVER_ENABLE)
  static NET_TFTP_INFO tftp_scb[TFTP_SERVER_NUM_SESSIONS];
  NET_TFTP_CFG net_tftp_config = {
    tftp_scb,
    TFTP_SERVER_NUM_SESSIONS,
    TFTP_SERVER_MAX_RETRY,
    TFTP_SERVER_PORT_NUM,
    TFTP_SERVER_TOUT,
    TFTP_SERVER_FIREWALL_ENABLE,
  #if (TFTP_SERVER_ROOT_ENABLE)
    TFTP_SERVER_ROOT_FOLDER
  #else
    NULL
  #endif
  };
#endif

#if (TFTP_CLIENT_ENABLE)
  NET_TFTPC_CFG net_tftpc_config = {
    TFTP_CLIENT_BLOCK_SIZE,
    TFTP_CLIENT_RETRY_TOUT,
    TFTP_CLIENT_MAX_RETRY
  };
#endif

#if (FTP_SERVER_ENABLE)
  static NET_FTP_INFO ftp_scb[FTP_SERVER_NUM_SESSIONS];
  NET_FTP_CFG net_ftp_config = {
    ftp_scb,
    FTP_SERVER_NUM_SESSIONS,
    FTP_SERVER_AUTH_ENABLE,
    FTP_SERVER_PORT_NUM,
    FTP_SERVER_TOUT,
    STRLEN(FTP_SERVER_MESSAGE),
    FTP_SERVER_MESSAGE,
  #if (FTP_SERVER_ROOT_ENABLE)
    FTP_SERVER_ROOT_FOLDER,
  #else
    NULL,
  #endif
  #if (FTP_SERVER_AUTH_ENABLE && FTP_SERVER_AUTH_ADMIN)
    FTP_SERVER_AUTH_USER,
    FTP_SERVER_AUTH_PASS,
    STRLEN(FTP_SERVER_AUTH_USER),
    STRLEN(FTP_SERVER_AUTH_PASS),
  #else
    NULL,NULL,0,0
  #endif
  };
#endif

#if (FTP_CLIENT_ENABLE)
  NET_FTPC_CFG net_ftpc_config = {
    FTP_CLIENT_TOUT,
    FTP_CLIENT_PASSIVE_MODE
  };
#endif

#if (DNS_CLIENT_ENABLE)
  static NET_DNS_INFO dns_table[DNS_CLIENT_TAB_SIZE];
  NET_DNS_CFG net_dns_config = {
    dns_table,
    DNS_CLIENT_TAB_SIZE
  };
#endif

#if (SMTP_CLIENT_ENABLE)
  NET_SMTP_CFG net_smtp_config = {
    SMTP_CLIENT_TOUT,
  #if (SMTP_CLIENT_ATTACH_ENABLE)
    /* File system interface */
    { netSMTPc_fopen,
      netSMTPc_fread,
      netSMTPc_fclose
    },
    /* Attachment support functions */
    &net_smtp_attach_func,
  #else
    { NULL,NULL,NULL },
    NULL,
  #endif
  #if defined(RTE_Network_SMTP_Client_TLS)
    &tls_if_func
  #else
    NULL
  #endif
  };
#endif

#if (SNMP_AGENT_ENABLE)
  NET_SNMP_CFG net_snmp_config = {
    SNMP_AGENT_PORT_NUM,
    SNMP_AGENT_TRAP_PORT,
    SNMP_AGENT_TRAP_IP,
    SNMP_AGENT_COMMUNITY
  };
#endif

#if (SNTP_CLIENT_ENABLE)
  NET_SNTP_CFG net_sntp_config = {
    SNTP_CLIENT_NTP_SERVER,
    SNTP_CLIENT_BCAST_MODE
  };
#endif

/* Network init functions */
static const net_sys_fn_t sys_fn_init[] = {
#if (__ETH_ENA)
  net_eth_iface_init,
#endif
#if (__WIFI_ENA)
  net_wifi_iface_init,
#endif
#if (PPP_ENABLE)
  net_ppp_iface_init,
#endif
#if (SLIP_ENABLE)
  net_slip_iface_init,
#endif
  net_loop_iface_init,
#if (__ARP_ENA)
  net_arp_cache_init,
#endif
#if (__NDP_ENA)
  net_ndp_cache_init,
#endif
#if (__IP4_FRAG_ENA)
  net_ip4_frag_init,
#endif
#if (__IP6_FRAG_ENA)
  net_ip6_frag_init,
#endif
  net_ping_client_init,
#if (__IGMP_ENA)
  net_igmp_host_init,
#endif
#if (UDP_ENABLE)
  net_udp_socket_init,
#endif
#if (TCP_ENABLE)
  net_tcp_socket_init,
#endif
#if (BSD_ENABLE)
  net_bsd_socket_init,
 #if (BSD_HOSTNAME_ENABLE)
  net_bsd_host_init,
 #endif
#endif
#if (__TLS_USED)
  /* Must initialize before services */
  netTLS_InterfaceInit,
#endif
#if (HTTP_SERVER_ENABLE)
  net_http_server_init,
#endif
#if (TELNET_SERVER_ENABLE)
  net_telnet_server_init,
#endif
#if (TFTP_SERVER_ENABLE)
  net_tftp_server_init,
#endif
#if (TFTP_CLIENT_ENABLE)
  net_tftp_client_init,
#endif
#if (FTP_SERVER_ENABLE)
  net_ftp_server_init,
#endif
#if (FTP_CLIENT_ENABLE)
  net_ftp_client_init,
#endif
#if (__NBNS_ENA)
  net_nbns_client_init,
#endif
#if (__DHCP_ENA)
  net_dhcp_client_init,
#endif
#if (__DHCP6_ENA)
  net_dhcp6_client_init,
#endif
#if (DNS_CLIENT_ENABLE)
  net_dns_client_init,
#endif
#if (SMTP_CLIENT_ENABLE)
  net_smtp_client_init,
#endif
#if (SNMP_AGENT_ENABLE)
  net_snmp_agent_init,
#endif
#if (SNTP_CLIENT_ENABLE)
  net_sntp_client_init,
#endif
  /* End of table */
  NULL
};

/* Network run functions */
static const net_sys_fn_t sys_fn_run[] = {
#if (__ETH_ENA)
  net_eth_iface_run,
#endif
#if (__WIFI_ENA)
  net_wifi_iface_run,
#endif
#if (PPP_ENABLE)
  net_ppp_iface_run,
#endif
#if (SLIP_ENABLE)
  net_slip_iface_run,
#endif
  net_loop_iface_run,
#if (__ARP_ENA)
  net_arp_cache_run,
#endif
#if (__NDP_ENA)
  net_ndp_cache_run,
#endif
#if (__IP4_FRAG_ENA)
  net_ip4_frag_run,
#endif
#if (__IP6_FRAG_ENA)
  net_ip6_frag_run,
#endif
  net_ping_client_run,
#if (__IGMP_ENA)
  net_igmp_host_run,
#endif
#if (UDP_ENABLE)
  /* No function */
#endif
#if (TCP_ENABLE)
  net_tcp_socket_run,
#endif
#if (BSD_ENABLE)
  net_bsd_socket_run,
 #if (BSD_HOSTNAME_ENABLE)
  /* No function */
 #endif
#endif
#if (__TLS_USED)
  /* No function */
#endif
#if (HTTP_SERVER_ENABLE)
  net_http_server_run,
#endif
#if (TELNET_SERVER_ENABLE)
  net_telnet_server_run,
#endif
#if (TFTP_SERVER_ENABLE)
  net_tftp_server_run,
#endif
#if (TFTP_CLIENT_ENABLE)
  net_tftp_client_run,
#endif
#if (FTP_SERVER_ENABLE)
  net_ftp_server_run,
#endif
#if (FTP_CLIENT_ENABLE)
  net_ftp_client_run,
#endif
#if (__NBNS_ENA)
  net_nbns_client_run,
#endif
#if (__DHCP_ENA)
  net_dhcp_client_run,
#endif
#if (__DHCP6_ENA)
  net_dhcp6_client_run,
#endif
#if (DNS_CLIENT_ENABLE)
  net_dns_client_run,
#endif
#if (SMTP_CLIENT_ENABLE)
  net_smtp_client_run,
#endif
#if (SNMP_AGENT_ENABLE)
  net_snmp_agent_run,
#endif
#if (SNTP_CLIENT_ENABLE)
  net_sntp_client_run,
#endif
  /* End of table */
  NULL
};

/* Network uninit functions */
const net_sys_fn_t net_sys_fn_uninit[] = {
#if (__ETH_ENA)
  net_eth_iface_uninit,
#endif
#if (__WIFI_ENA)
  net_wifi_iface_uninit,
#endif
#if (PPP_ENABLE)
  net_ppp_iface_uninit,
#endif
#if (SLIP_ENABLE)
  net_slip_iface_uninit,
#endif
  net_loop_iface_uninit,
#if (__ARP_ENA)
  net_arp_cache_uninit,
#endif
#if (__NDP_ENA)
  net_ndp_cache_uninit,
#endif
#if (__IP4_FRAG_ENA)
  net_ip4_frag_uninit,
#endif
#if (__IP6_FRAG_ENA)
  net_ip6_frag_uninit,
#endif
  net_ping_client_uninit,
#if (__IGMP_ENA)
  net_igmp_host_uninit,
#endif
#if (UDP_ENABLE)
  net_udp_socket_uninit,
#endif
#if (TCP_ENABLE)
  net_tcp_socket_uninit,
#endif
#if (BSD_ENABLE)
  net_bsd_socket_uninit,
 #if (BSD_HOSTNAME_ENABLE)
  net_bsd_host_uninit,
 #endif
#endif
#if (__TLS_USED)
  netTLS_InterfaceUninit,
#endif
#if (HTTP_SERVER_ENABLE)
  net_http_server_uninit,
#endif
#if (TELNET_SERVER_ENABLE)
  net_telnet_server_uninit,
#endif
#if (TFTP_SERVER_ENABLE)
  net_tftp_server_uninit,
#endif
#if (TFTP_CLIENT_ENABLE)
  net_tftp_client_uninit,
#endif
#if (FTP_SERVER_ENABLE)
  net_ftp_server_uninit,
#endif
#if (FTP_CLIENT_ENABLE)
  net_ftp_client_uninit,
#endif
#if (__NBNS_ENA)
  net_nbns_client_uninit,
#endif
#if (__DHCP_ENA)
  net_dhcp_client_uninit,
#endif
#if (__DHCP6_ENA)
  net_dhcp6_client_uninit,
#endif
#if (DNS_CLIENT_ENABLE)
  net_dns_client_uninit,
#endif
#if (SMTP_CLIENT_ENABLE)
  net_smtp_client_uninit,
#endif
#if (SNMP_AGENT_ENABLE)
  net_snmp_agent_uninit,
#endif
#if (SNTP_CLIENT_ENABLE)
  net_sntp_client_uninit,
#endif
  /* End of table */
  NULL
};

/* netIF_GetOption functions */
NETIF_GETOPT_FUNC netif_getopt_func = {
 #if (__ETH_ENA)
  net_eth_get_option,
 #else
  NULL,
 #endif
 #if (__WIFI_ENA)
  net_wifi_get_option,
 #else
  NULL,
 #endif
 #if (PPP_ENABLE)
  net_ppp_get_option,
 #else
  NULL,
 #endif
 #if (SLIP_ENABLE)
  net_slip_get_option
 #else
  NULL
 #endif
};

/* netIF_SetOption functions */
NETIF_SETOPT_FUNC netif_setopt_func = {
 #if (__ETH_ENA)
  net_eth_set_option,
 #else
  NULL,
 #endif
 #if (__WIFI_ENA)
  net_wifi_set_option,
 #else
  NULL,
 #endif
 #if (PPP_ENABLE)
  net_ppp_set_option,
 #else
  NULL,
 #endif
 #if (SLIP_ENABLE)
  net_slip_set_option
 #else
  NULL
 #endif
};

/* Executable image size optimization */
#if !(__ETH_ENA)
/* Empty functions when Ethernet Interface is disabled */
netStatus netETH_SendRaw (uint32_t if_num, const uint8_t *buf, uint32_t len) {
  (void)if_num; (void)buf; (void)len; return (netError); }
const uint8_t *net_eth_get_addr (const __ADDR *addr) {
  (void)addr; return (NULL); }
#endif
#if !(__ETH_ENA || __WIFI_ENA)
#ifdef RTE_Network_IPv6
void net_ndp_process (NET_IF_CFG *net_if, NET_FRAME *frame) {
  (void)net_if; (void)frame; }
#endif
#endif

#if !(__IGMP_ENA)
/* Empty functions when IP Multicasting is not enabled */
netStatus netIGMP_Join (uint32_t if_id, const uint8_t *ip4_addr) {
  (void)if_id; (void)ip4_addr; return (netError); }
netStatus netIGMP_Leave (uint32_t if_id, const uint8_t *ip4_addr) {
  (void)if_id; (void)ip4_addr; return (netError); }
bool net_igmp_is_member (NET_IF_CFG *net_if, const uint8_t *ip4_addr) {
  (void)net_if; (void)ip4_addr; return (false); }
#if (__ETH_ENA)
uint32_t net_igmp_collect_mcast (NET_IF_CFG *net_if, uint8_t *buf) {
  (void)net_if; (void)buf; return (0); }
#endif
void net_igmp_process (NET_IF_CFG *net_if, NET_FRAME *frame) {
  (void)net_if; (void)frame; }
#endif

#if !(__DHCP_ENA)
/* Empty functions when DHCP not enabled */
netStatus netDHCP_Enable (uint32_t if_id) {
  (void)if_id; return (netError); }
netStatus netDHCP_Disable (uint32_t if_id) {
  (void)if_id; return (netError); }
netStatus netDHCP_SetOption (uint32_t if_id, uint8_t option,
                             const uint8_t *val, uint32_t len) {
  (void)if_id; (void)option; (void)val; (void)len; return (netError); }
#endif

#if !(__NBNS_ENA)
/* Empty functions when NBNS not enabled */
netStatus netNBNS_Resolve (uint32_t if_id, const char *name, uint8_t *ip4_addr) {
  (void)if_id; (void)name; (void)ip4_addr; return (netError); }
netStatus netNBNS_ClearCache (uint32_t if_id) {
  (void)if_id; return (netError); }
#endif

#if !(__DHCP6_ENA)
/* Empty functions when DHCPv6 not enabled */
#ifdef RTE_Network_IPv6
netStatus netDHCP6_Enable (uint32_t if_id, netDHCP6_Mode mode) {
  (void)if_id; (void)mode; return (netError); }
netStatus netDHCP6_Disable (uint32_t if_id) {
  (void)if_id; return (netError); }
#endif
#endif

#if !(__IP4_FRAG_ENA)
/* Empty functions when IP fragmentation not enabled */
NET_FRAME *net_ip4_frag_add (NET_FRAME *frame) {
  return (frame); }
NET_FRAME *net_ip4_frag_get (NET_FRAME *frame, uint16_t mtu) {
  (void)mtu; return (frame); }
#endif

#if !(__IP6_FRAG_ENA) && defined(RTE_Network_IPv6)
/* Empty functions when IPv6 fragmentation not enabled */
NET_FRAME *net_ip6_frag_add (NET_FRAME *frame) {
  return (frame); }
NET_FRAME *net_ip6_frag_get (NET_FRAME *frame, uint16_t mtu) {
  (void)mtu; return (frame); }
#endif

#if !(UDP_ENABLE)
/* Empty function when UDP socket not enabled */
void net_udp_process (NET_IF_CFG *net_if, NET_FRAME *frame, uint8_t ip_ver) {
  (void)net_if; (void)frame; (void)ip_ver; }
#endif

#if !(TCP_ENABLE)
/* Empty function when TCP socket not enabled */
void net_tcp_process (NET_IF_CFG *net_if, NET_FRAME *frame, uint8_t ip_ver) {
  (void)net_if; (void)frame; (void)ip_ver; }
#endif

#if !(SNTP_CLIENT_ENABLE)
/* Empty function when SNTP not enabled */
netStatus netSNTPc_GetTime (const NET_ADDR *addr, netSNTPc_cb_t cb_func) {
  (void)addr; (void)cb_func; return (netError); }
#endif

#if (HTTP_SERVER_ENABLE && defined(RTE_Network_Web_Server_RO))
/* Empty interface functions for Compact Web server */
__WEAK void *netHTTPs_fopen (const char *fname) {
  (void)fname; return (NULL); }
__WEAK void netHTTPs_fclose (void *file) {
  (void)file; }
__WEAK uint32_t netHTTPs_fread (void *file, uint8_t *buf, uint32_t len) {
  (void)file; (void)buf; (void)len; return (0); }
__WEAK char *netHTTPs_fgets (void *file, char *buf, uint32_t size) {
  (void)file; (void)buf; (void)size; return (NULL); }
__WEAK void netHTTPs_fstat (const char *fname, uint32_t *fsize, uint32_t *ftime) {
  (void)fname; (void)fsize; (void)ftime; }
#endif
