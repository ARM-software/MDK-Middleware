/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Net_Legacy.c
 * Purpose: Network Legacy API Wrapper Functions
 * Rev.:    V7.11.0
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"
#include "rl_net_legacy.h"
#include "Net_Config_Legacy.h"

//  ==== UDP Sockets ====

#ifdef RTE_Network_Socket_UDP
/* Convert callback type netUDP_cb_t to net_udp_cb_t */
static uint32_t udp_cb_wrapper (int32_t socket, const NET_ADDR *addr,
                                const uint8_t *buf, uint32_t len) {
  if (addr->addr_type != NET_ADDR_IP4) {
    /* Ignore IPv6 */
    return (0);
  }
  return (net_udp_cb_legacy[socket-1] (socket, addr->addr, addr->port, buf, len));
}

/* Allocate a free UDP socket */
int32_t udp_get_socket (uint8_t tos, uint8_t opt, net_udp_cb_t cb_func) {
  int32_t sock;

  /* Use callback wrapper */
  sock = netUDP_GetSocket (udp_cb_wrapper);
  if (sock <= 0) {
    return (sock);
  }
  net_udp_cb_legacy[sock-1] = cb_func;
  /* Default: tos = 0 */
  if (tos != 0) {
    netUDP_SetOption (sock, netUDP_OptionTOS, tos);
  }
  /* Default: Send Checksum and Verify Checksum */
  if (opt != (NET_UDP_CHECKSUM_SEND | NET_UDP_CHECKSUM_VERIFY)) {
    netUDP_SetOption (sock, netUDP_OptionChecksum, opt);
  }
  return (sock);
}

/* Send data to a remote node */
netStatus udp_send (int32_t socket, const uint8_t *ip_addr, uint16_t port,
                                    uint8_t *buf, uint32_t len) {
  NET_ADDR4 addr4;

  /* Convert ip_addr and port to a structure */
  addr4.addr_type = NET_ADDR_IP4;
  addr4.port      = port;
  memcpy (addr4.addr, ip_addr, NET_ADDR_IP4_LEN);
  return (netUDP_Send (socket, (NET_ADDR *)&addr4, buf, len));
}

#endif /* RTE_Network_Socket_UDP */

//  ==== TCP Sockets ====

#ifdef RTE_Network_Socket_TCP
/* Convert callback type netTCP_cb_t to net_tcp_cb_t */
static uint32_t tcp_cb_wrapper (int32_t socket, netTCP_Event event,
                                const NET_ADDR *addr, const uint8_t *buf, uint32_t len) {
  if (addr->addr_type != NET_ADDR_IP4) {
    /* Ignore IPv6 */
    return (0);
  }
  if (event != netTCP_EventData) {
    /* Provide IP4 address and port */
    buf = addr->addr;
    len = addr->port;
  }
  return (net_tcp_cb_legacy[socket-1] (socket, event, buf, len));
}

/* Allocate a free UDP socket */
int32_t tcp_get_socket (uint8_t type, uint8_t tos, uint32_t tout, net_tcp_cb_t cb_func) {
  int32_t sock;

  /* Use callback wrapper */
  sock = netTCP_GetSocket (tcp_cb_wrapper);
  if (sock <= 0) {
    return (sock);
  }
  net_tcp_cb_legacy[sock-1] = cb_func;
  /* Default tos = 0 */
  if (tos != 0) {
    netTCP_SetOption (sock, netTCP_OptionTOS, tos);
  }
  /* Default tout = 0 (using timeout from Net_Config_TCP.h) */
  if (tout != 0) {
    netTCP_SetOption (sock, netTCP_OptionTimeout, tout);
  }
  /* Delay ACK: default not enabled */
  if (type & TCP_TYPE_DELAY_ACK) {
    netTCP_SetOption (sock, netTCP_OptionDelayedACK, 1);
  }
  /* Flow Control: default not enabled */
  if (type & TCP_TYPE_FLOW_CTRL) {
    netTCP_SetOption (sock, netTCP_OptionFlowControl, 1);
  }
  /* Keep Alive: default not enabled */
  if (type & TCP_TYPE_KEEP_ALIVE) {
    netTCP_SetOption (sock, netTCP_OptionKeepAlive, 1);
  }
  return (sock);
}

/* Initiate TCP connection to a remote node */
netStatus tcp_connect (int32_t socket,
                       const uint8_t *ip_addr, uint16_t port, uint16_t local_port) {
  NET_ADDR4 addr4;

  /* Convert ip_addr and port to a structure */
  addr4.addr_type = NET_ADDR_IP4;
  addr4.port      = port;
  memcpy (addr4.addr, ip_addr, NET_ADDR_IP4_LEN);
  return (netTCP_Connect (socket, (NET_ADDR *)&addr4, local_port));
}

/* Determine current state of a TCP socket */
tcpState tcp_get_state (int32_t socket) {
  netTCP_State state = netTCP_GetState (socket);
  if (state == netTCP_StateINVALID) {
    /* New state in Network-DS only */
    return (netTCP_StateUNUSED);
  }
  return (state);
}

/* Convert TCP socket state into an ASCII string */
const char *tcp_ntoa (tcpState state) {
  return (net_tcp_ntoa (state));
}
#endif /* RTE_Network_Socket_TCP */

//  ==== Ethernet Interface ====

#ifdef RTE_Network_Interface_ETH_0
/* Notify the user of Ethernet link state change event */
#if (ETH_LEGACY != 0)
void netETH_Notify (uint32_t if_num, netETH_Event event, uint32_t val) {
  if (event == netETH_LinkDown) {
    eth_link_notify (if_num, ethLinkDown);
  }
  else if (event == netETH_LinkUp) {
    switch (val) {
      case (NET_ETH_DUPLEX_HALF << 2) | NET_ETH_SPEED_10M:
        eth_link_notify (if_num, ethLinkUp_10MHalfDuplex);
        break;
      case (NET_ETH_DUPLEX_FULL << 2) | NET_ETH_SPEED_10M:
        eth_link_notify (if_num, ethLinkUp_10MFullDuplex);
        break;
      case (NET_ETH_DUPLEX_HALF << 2) | NET_ETH_SPEED_100M:
        eth_link_notify (if_num, ethLinkUp_100MHalfDuplex);
        break;
      case (NET_ETH_DUPLEX_FULL << 2) | NET_ETH_SPEED_100M:
        eth_link_notify (if_num, ethLinkUp_100MFullDuplex);
        break;
      case (NET_ETH_DUPLEX_HALF << 2) | NET_ETH_SPEED_1G:
        eth_link_notify (if_num, ethLinkUp_1GHalfDuplex);
        break;
      case (NET_ETH_DUPLEX_FULL << 2) | NET_ETH_SPEED_1G:
        eth_link_notify (if_num, ethLinkUp_1GFullDuplex);
        break;
    }
  }
}
/* Default function, when not provided by the user */
__weak void eth_link_notify (uint32_t if_num, ethLinkEvent event) {
  (void)if_num;
  (void)event;
}
#endif /* ETH_LEGACY */

/* Notify the user of DHCP event or extended DHCP option */
#if (ETH_LEGACY != 0)
void netDHCP_Notify (uint32_t if_id, uint8_t option, const uint8_t *val, uint32_t len) {
  uint32_t if_num = if_id & 0xFF;
  switch (option) {
    case NET_DHCP_OPTION_IP_ADDRESS:
      dhcp_client_notify (if_num, dhcpClientIPaddress, val, len);
      break;
    case NET_DHCP_OPTION_NTP_SERVERS:
      dhcp_client_notify (if_num, dhcpClientNTPservers, val, len);
      break;
    case NET_DHCP_OPTION_BOOTFILE_NAME:
      dhcp_client_notify (if_num, dhcpClientBootfileName, val, len);
      break;
  }
}
/* Default function, when not provided by the user */
__weak void dhcp_client_notify (uint32_t if_id, dhcpClientOption opt,
                                const uint8_t *val, uint32_t len) {
  (void)if_id;
  (void)opt;
  (void)val;
  (void)len;
}
#endif /* ETH_LEGACY */
#endif /* RTE_Network_Interface_ETH_0 */

//  ==== ICMP Ping ====

/* Start ICMP ping process */
netStatus icmp_ping (const uint8_t *ip_addr, net_icmp_cb_t cb_func) {
  NET_ADDR4 addr4;

  /* Convert ip_addr to a structure */
  addr4.addr_type = NET_ADDR_IP4;
  memcpy (addr4.addr, ip_addr, NET_ADDR_IP4_LEN);
  return (netPing_Echo ((NET_ADDR *)&addr4, cb_func));
}

//  ==== DNS Client ====

#ifdef RTE_Network_DNS_Client
static net_dns_client_cb_t dns_cb_func;

/* Convert callback type netDNSc_cb_t to net_dns_client_cb_t */
static void dns_cb_wrapper (netDNSc_Event event, const NET_ADDR *addr) {
  if (addr->addr_type != NET_ADDR_IP4) {
    /* Ignore IPv6 hosts */
    dns_cb_func (dnsClientError, NULL);
    return;
  }
  dns_cb_func (event, addr->addr);
}

/* Resolve IP address of a host from a hostname */
netStatus get_host_by_name (const char *name, net_dns_client_cb_t cb_func) {
  netStatus retv;

  retv = netDNSc_GetHostByName (name, NET_ADDR_IP4, dns_cb_wrapper);
  if (retv == netOK) {
    dns_cb_func = cb_func;
  }
  return (retv);
}
#endif /* RTE_Network_DNS_Client */

//  ==== FTP Server ====

#ifdef RTE_Network_FTP_Server
/* Accept or deny connection from remote FTP client */
#if (FTP_SERVER_LEGACY != 0)
bool netFTPs_AcceptClient (const NET_ADDR *addr) {
  if (addr->addr_type != NET_ADDR_IP4) {
    /* Deny IPv6 clients */
    return (false);
  }
  return (ftp_accept_client (addr->addr, addr->port));
}
/* Default function, when not provided by the user */
__weak bool ftp_accept_client (const uint8_t *ip_addr, uint16_t port) {
  (void)ip_addr;
  (void)port;
  return (true);
}
#endif /* FTP_SERVER_LEGACY */

/* Check if remote user is allowed to access a file on FTP server */
#if (FTP_SERVER_LEGACY != 0)
bool netFTPs_FileAccess (uint8_t user_id, const char *fname, uint32_t access) {
  uint8_t mode;

  switch (access) {
    case NET_ACCESS_FILE_READ:
      mode = 0;
      break;
    case NET_ACCESS_FILE_WRITE:
      mode = 1;
      break;
    case NET_ACCESS_DIRECTORY_CREATE:
    case NET_ACCESS_DIRECTORY_REMOVE:
      mode = 2;
      break;
    case NET_ACCESS_DIRECTORY_LIST:
      mode = 3;
      break;
    default:
      return (false);
  }
  return (ftp_file_access (user_id, fname, mode));
}
/* Default function, when not provided by the user */
__weak bool ftp_file_access (uint8_t user_id, const char *fname, uint8_t mode) {
  (void)user_id;
  (void)fname;
  (void)mode;
  return (true);
}
#endif /* FTP_SERVER_LEGACY */
#endif /* RTE_Network_FTP_Server */

//  ==== FTP Client ====

#ifdef RTE_Network_FTP_Client
/* Start FTP client file operation session */
netStatus ftp_client_connect (const uint8_t *ip_addr, uint16_t port, ftpCommand command) {
  NET_ADDR4 addr4;

  /* Convert ip_addr and port to a structure */
  addr4.addr_type = NET_ADDR_IP4;
  addr4.port      = port;
  memcpy (addr4.addr, ip_addr, NET_ADDR_IP4_LEN);
  return (netFTPc_Connect ((NET_ADDR *)&addr4, command));
}
#endif /* RTE_Network_FTP_Client */

//  ==== TFTP Server ====

#ifdef RTE_Network_TFTP_Server
/* Accept or deny connection from remote TFTP client */
#if (TFTP_SERVER_LEGACY != 0)
bool netTFTPs_AcceptClient (const NET_ADDR *addr) {
  if (addr->addr_type != NET_ADDR_IP4) {
    /* Deny IPv6 clients */
    return (false);
  }
  return (tftp_accept_client (addr->addr, addr->port));
}
/* Default function, when not provided by the user */
__weak bool tftp_accept_client (const uint8_t *ip_addr, uint16_t port) {
  (void)ip_addr;
  (void)port;
  return (true);
}
#endif /* TFTP_SERVER_LEGACY */
#endif /* RTE_Network_TFTP_Server */

//  ==== TFTP Client ====

#ifdef RTE_Network_TFTP_Client
/* Put a file to remote TFTP server */
netStatus tftp_client_put (const uint8_t *ip_addr, uint16_t port, const char *src, const char *dst) {
  NET_ADDR4 addr4;

  /* Convert ip_addr and port to a structure */
  addr4.addr_type = NET_ADDR_IP4;
  addr4.port      = port;
  memcpy (addr4.addr, ip_addr, NET_ADDR_IP4_LEN);
  return (netTFTPc_Put((NET_ADDR *)&addr4, dst, src));
}

/* Retrieve a file from remote TFTP server */
netStatus tftp_client_get (const uint8_t *ip_addr, uint16_t port, const char *src, const char *dst) {
  NET_ADDR4 addr4;

  /* Convert ip_addr and port to a structure */
  addr4.addr_type = NET_ADDR_IP4;
  addr4.port      = port;
  memcpy (addr4.addr, ip_addr, NET_ADDR_IP4_LEN);
  return (netTFTPc_Get((NET_ADDR *)&addr4, src, dst));
}

/* Notify the user application when TFTP client operation ends */
#if (TFTP_CLIENT_LEGACY != 0)
void netTFTPc_Notify (netTFTPc_Event event) {
  if (event == netTFTPc_EventLocalFileError) {
    tftp_client_notify (tftpClientDiskFull);
    return;
  }
  tftp_client_notify (event);
}
/* Default function, when not provided by the user */
__weak void tftp_client_notify (tftpClientEvent event) {
  (void)event;
}
#endif /* TFTP_CLIENT_LEGACY */
#endif /* RTE_Network_TFTP_Client */

//  ==== Telnet Server ====

#ifdef RTE_Network_Telnet_Server
/* Get IP and MAC address of connected remote machine */
netStatus telnet_server_get_client (uint8_t *ip_addr, uint8_t *mac_addr) {
  NET_ADDR4 addr4;
  netStatus retv;

  retv = netTELNETs_GetClient ((NET_ADDR *)&addr4, sizeof (addr4));
  if (retv != netOK) {
    return (retv);
  }
  memcpy (ip_addr, addr4.addr, NET_ADDR_IP4_LEN);
  if (mac_addr != NULL) {
    const uint8_t *mac = net_eth_get_addr ((__ADDR *)&addr4);
    if (mac == NULL) {
      mac = net_addr_unspec;
    }
    memcpy (mac_addr, mac, NET_ADDR_ETH_LEN);
  }
  return (netOK);
}

/* Accept or deny connection from remote Telnet client */
#if (TELNET_SERVER_LEGACY != 0)
bool netTELNETs_AcceptClient (const NET_ADDR *addr) {
  if (addr->addr_type != NET_ADDR_IP4) {
    /* Deny IPv6 clients */
    return (false);
  }
  return (telnet_accept_client (addr->addr, addr->port));
}
/* Default function, when not provided by the user */
__weak bool telnet_accept_client (const uint8_t *ip_addr, uint16_t port) {
  (void)ip_addr;
  (void)port;
  return (true);
}
#endif /* TELNET_SERVER_LEGACY */
#endif /* RTE_Network_Telnet_Server */

//  ==== HTTP Server ====

#if defined(RTE_Network_Web_Server_RO) || defined(RTE_Network_Web_Server_FS)
/* Get IP and MAC address of connected remote machine */
netStatus http_server_get_client (uint8_t *ip_addr, uint8_t *mac_addr) {
  NET_ADDR4 addr4;
  netStatus retv;

  retv = netHTTPs_GetClient ((NET_ADDR *)&addr4, sizeof (addr4));
  if (retv != netOK) {
    return (retv);
  }
  memcpy (ip_addr, addr4.addr, NET_ADDR_IP4_LEN);
  if (mac_addr != NULL) {
    const uint8_t *mac = net_eth_get_addr ((__ADDR *)&addr4);
    if (mac == NULL) {
      mac = net_addr_unspec;
    }
    memcpy (mac_addr, mac, NET_ADDR_ETH_LEN);
  }
  return (netOK);
}

/* Accept or deny connection from remote HTTP client */
#if (HTTP_SERVER_LEGACY != 0)
bool netHTTPs_AcceptClient (const NET_ADDR *addr) {
  if (addr->addr_type != NET_ADDR_IP4) {
    /* Deny IPv6 clients */
    return (false);
  }
  return (http_accept_client (addr->addr, addr->port));
}
/* Default function, when not provided by the user */
__weak bool http_accept_client (const uint8_t *ip_addr, uint16_t port) {
  (void)ip_addr;
  (void)port;
  return (true);
}
#endif /* HTTP_SERVER_LEGACY */
#endif /* RTE_Network_Web_Server_RO || RTE_Network_Web_Server_FS */

//  ==== SMTP Client ====

#ifdef RTE_Network_SMTP_Client
/* Start SMTP client to send an email */
netStatus smtp_client_connect (const uint8_t *ip_addr, uint16_t port) {
  NET_ADDR4 addr4;

  /* Convert ip_addr and port to a structure */
  addr4.addr_type = NET_ADDR_IP4;
  addr4.port      = port;
  memcpy (addr4.addr, ip_addr, NET_ADDR_IP4_LEN);
  return (netSMTPc_Connect ((NET_ADDR *)&addr4));
}

/* Accept or deny authentication requested by SMTP server */
#if (SMTP_CLIENT_LEGACY != 0)
bool netSMTPc_AcceptAuthentication (const NET_ADDR *addr) {
  if (addr->addr_type != NET_ADDR_IP4) {
    /* Deny IPv6 servers */
    return (false);
  }
  return (smtp_client_accept_authentication (addr->addr));
}
#endif /* SMTP_CLIENT_LEGACY */
#endif /* RTE_Network_SMTP_Client */

//  ==== SNTP Client ====

#ifdef RTE_Network_SNTP_Client
/* Determine current time from NTP or SNTP time server */
netStatus sntp_get_time (const uint8_t *ip_addr, net_sntp_client_cb_t cb_func) {
  NET_ADDR4 addr4;

  /* Convert ip_addr to a structure */
  addr4.addr_type = NET_ADDR_IP4;
  memcpy (addr4.addr, ip_addr, NET_ADDR_IP4_LEN);
  return (netSNTPc_GetTime ((NET_ADDR *)&addr4, (netSNTPc_cb_t)cb_func));
}
#endif /* RTE_Network_SNTP_Client */

//  ==== SNMP Agent ====

#ifdef RTE_Network_SNMP_Agent
/* Send a trap message to the Trap Manager */
netStatus snmp_trap (const uint8_t *ip_addr, uint8_t generic, uint8_t specific, const uint16_t *obj_list) {
  NET_ADDR4 addr4;

  /* Convert ip_addr to a structure */
  addr4.addr_type = NET_ADDR_IP4;
  memcpy (addr4.addr, ip_addr, NET_ADDR_IP4_LEN);
  return (netSNMP_Trap ((NET_ADDR *)&addr4, generic, specific, obj_list));
}
#endif /* RTE_Network_SNMP_Agent */

//  ==== Network address conversion ===

/* Convert an IPv4 Network address into an ASCII string */
const char *ip4_ntoa (const uint8_t *ip4_addr) {
  return (net_addr4_ntoa (ip4_addr));
}

/* Convert a string containing an IPv4 address into a Network address */
bool ip4_aton (const char *cp, uint8_t *ip4_addr) {
  return (net_addr4_aton (cp, ip4_addr));
}

/* Convert a MAC address into an ASCII string */
const char *mac_ntoa (const uint8_t *mac_addr) {
  return (net_mac_ntoa (mac_addr));
}
