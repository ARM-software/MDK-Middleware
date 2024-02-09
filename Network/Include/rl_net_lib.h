/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    rl_net_lib.h
 * Purpose: Network Library Definitions
 *----------------------------------------------------------------------------*/

#ifndef __RL_NET_LIB_H
#define __RL_NET_LIB_H

#include "rl_net.h"
#include "rl_net_rte.h"
#include "cmsis_compiler.h"

#ifdef __clang__
  #pragma clang diagnostic ignored "-Wpadded"
#endif

/// General definitions
#define NET_HEADER_LEN      4           ///< Network frame header length
#define PHY_HEADER_LEN      14          ///< Physical/ethernet header length
#define ETH_MAX_FRAME_SIZE  1514        ///< Maximum size of ethernet frame
#define NET_USERNAME_SIZE   16          ///< Authentication username buffer size
#define NET_PASSWORD_SIZE   16          ///< Authentication password buffer size
#define NET_HA1_HASH_LEN    32          ///< HTTP Authentication HA1 hash length
#define NET_ROOT_PATH_SIZE  128         ///< Root path buffer size
#define NET_TCP_NO_ACK      0xFFFFFFFF  ///< Callback return value for no ACK
#define NET_IF_CLASS_LOOP   (0 << 8)    ///< Loopback interface class

/// Debug process definitions
#ifdef RTE_Network_Debug_STDIO
#define NET_SYSTEM_CORE     0           ///< Main system core
#define NET_DYNAMIC_MEMORY  1           ///< Dynamic memory management
#define NET_ETH_INTERFACE   2           ///< Ethernet interface 0
#define NET_WIFI_INTERFACE  3           ///< WiFi interface
#define NET_PPP_INTERFACE   4           ///< PPP interface
#define NET_SLIP_INTERFACE  5           ///< SLIP interface
#define NET_LOCAL_LOOPBACK  6           ///< Loopback virtual interface
#define NET_ARP_CACHE       7           ///< Ethernet ARP cache
#define NET_IP4_CORE        8           ///< IPv4 core and localhost
#define NET_ICMP_CONTROL    9           ///< ICMP control service for IPv4

#define NET_IGMP_HOST       10          ///< IGMP group managememt host for IPv4
#define NET_IP6_CORE        11          ///< IPv6 core and localhost
#define NET_ICMP6_CONTROL   12          ///< ICMP6 control service for IPv6
#define NET_NDP_CACHE       13          ///< Ethernet Neighbor Discovery for IPv6
#define NET_MLD_NODE        14          ///< Multicast Listener Discovery for IPv6
#define NET_UDP_SOCKET      15          ///< UDP native socket
#define NET_TCP_SOCKET      16          ///< TCP native socket
#define NET_BSD_SOCKET      17          ///< BSD socket interface
#define NET_NBNS_CLIENT     18          ///< NBNS client service for IPv4
#define NET_DHCP_CLIENT     19          ///< DHCP client service for IPv4
#define NET_DHCP6_CLIENT    20          ///< DHCP client service for IPv6

#define NET_DNS_CLIENT      21          ///< DNS client service
#define NET_SNMP_AGENT      22          ///< SNMP agent module
#define NET_HTTP_SERVER     23          ///< HTTP server service
#define NET_FTP_SERVER      24          ///< FTP server service
#define NET_FTP_CLIENT      25          ///< FTP client service
#define NET_TELNET_SERVER   26          ///< Telnet server service
#define NET_TFTP_SERVER     27          ///< TFTP server service
#define NET_TFTP_CLIENT     28          ///< TFTP client service
#define NET_SMTP_CLIENT     29          ///< SMTP client service
#define NET_SNTP_CLIENT     30          ///< SNTP client service
#endif /* RTE_Network_Debug_STDIO */

/// Telnet definitions
#define TELNET_LBUF_SZ      96          ///< Command line buffer size in bytes
#define TELNET_HIST_SZ      128         ///< Command history buffer size in bytes
#define TELNET_FIFO_SZ      128         ///< Input character Fifo buffer in bytes

/// BSD Socket ioctl commands
#define FIONBIO             1           ///< Set mode (blocking/non-blocking)
#define FIO_DELAY_ACK       2           ///< Set DELAY_ACK mode for stream socket
#define FIO_KEEP_ALIVE      3           ///< Set KEEP_ALIVE mode for stream socket
#define FIO_FLOW_CTRL       4           ///< Set FLOW_CTRL mode for stream socket

#ifdef __cplusplus
extern "C"  {
#endif

/// Network OS identifier
typedef void *NETOS_ID;

/// Network scheduler function type
typedef void (*net_sys_fn_t) (void);

/// System Error codes
typedef enum net_error {
  NET_ERROR_MEM_ALLOC,                  ///< Alloc memory failed, out of memory
  NET_ERROR_MEM_FREE,                   ///< Free memory failed, memory slot invalid
  NET_ERROR_MEM_CORRUPT,                ///< Memory corruption detected
  NET_ERROR_CONFIG,                     ///< Network configuration error detected
  NET_ERROR_UDP_ALLOC,                  ///< No free UDP sockets available
  NET_ERROR_TCP_ALLOC,                  ///< No free TCP sockets available
  NET_ERROR_TCP_STATE                   ///< TCP socket in undefined state
} NET_ERROR;

/// Frame buffer structure
typedef struct net_frame {
  uint16_t length;                      ///< Length of data in frame
  uint16_t index;                       ///< Buffer position index
  uint8_t  data[];                      ///< Buffer data (protocol headers + data)
} NET_FRAME;

/// Buffer queue structure
typedef struct net_buffer {
  struct net_buffer *next;              ///< Next buffer in the list
  uint16_t length;                      ///< Length of data in buffer
  uint16_t index;                       ///< Buffer position index
  uint8_t  data[];                      ///< Buffer data
} NET_BUFFER;

/// IP Fragment queue structure
typedef struct net_ip_frag_list {
  NET_FRAME *next;                      ///< Next frame in the list
  uint16_t start;                       ///< Start offset of data fragment
  uint16_t end;                         ///< End offset (start + len)
} NET_IP_FRAG_LIST;

/// Library variant optimization
#ifdef RTE_Network_IPv6
  #define __ADDR            NET_ADDR
  #define __ADDR_IP_LEN     NET_ADDR_IP6_LEN
#else
  #define __ADDR            NET_ADDR4
  #define __ADDR_IP_LEN     NET_ADDR_IP4_LEN
#endif
#define __FRAME(buf)        ((NET_FRAME *)(uint32_t)(buf))
#define __BUFFER(buf)       ((NET_BUFFER *)(uint32_t)(buf))

/// Local Machine info version 4
typedef struct net_localm {
  uint16_t Mtu;                         ///< Maximum IP transmission unit
  uint16_t Reserved;                    ///< Reserved (used for alignment)
  uint8_t  IpAddr[NET_ADDR_IP4_LEN];    ///< Local IP address
  uint8_t  NetMask[NET_ADDR_IP4_LEN];   ///< Netmask
  uint8_t  DefGW[NET_ADDR_IP4_LEN];     ///< Default gateway
  uint8_t  PriDNS[NET_ADDR_IP4_LEN];    ///< Primary DNS server
  uint8_t  SecDNS[NET_ADDR_IP4_LEN];    ///< Secondary DNS server
} NET_LOCALM;

/// Local Machine info version 6
typedef struct net_localm6 {
  uint16_t Mtu;                         ///< Maximum IP transmission unit
  uint8_t  HopLimit;                    ///< Hop limit for outging IP packets
  uint8_t  PrefixLen;                   ///< Subnet prefix length
  uint8_t  IpAddr[NET_ADDR_IP6_LEN];    ///< IPv6 address
  uint8_t  TempAddr[NET_ADDR_IP6_LEN];  ///< Temporary IPv6 address
  uint8_t  LLAddr[NET_ADDR_IP6_LEN];    ///< Link-local address
  uint8_t  DefGW[NET_ADDR_IP6_LEN];     ///< Default gateway
  uint8_t  PriDNS[NET_ADDR_IP6_LEN];    ///< Primary DNS server
  uint8_t  SecDNS[NET_ADDR_IP6_LEN];    ///< Secondary DNS server
} NET_LOCALM6;

/// ARP Cache Entry info
typedef struct net_arp_info {
  uint8_t  State;                       ///< Cache entry state
  uint8_t  Type;                        ///< Cache entry type
  uint8_t  Retries;                     ///< Number of retries left
  uint8_t  Tout;                        ///< Timeout timer
  uint8_t  IpAddr[NET_ADDR_IP4_LEN];    ///< IPv4 address
  uint8_t  MacAddr[NET_ADDR_ETH_LEN];   ///< Hardware MAC address
  uint8_t  Id;                          ///< Entry identification number
  uint8_t  Reserved;                    ///< Reserved (not used)
  NET_FRAME *tx_list;                   ///< Frames waiting to resolve MAC address
} NET_ARP_INFO;

/// IGMP Group info
typedef struct net_igmp_info {
  uint8_t  State;                       ///< Group membership state
  uint8_t  Flags;                       ///< State flags
  uint8_t  Tout;                        ///< Timeout timer for reports
  uint8_t  Id;                          ///< Entry identification number
  uint8_t  IpAddr[NET_ADDR_IP4_LEN];    ///< Group IPv4 address
} NET_IGMP_INFO;

/// NBNS Cache Entry info
typedef struct net_nbns_info {
  uint32_t HostId;                      ///< Host Id (CRC32 value of host name)
  uint16_t IfaceId;                     ///< Interface Identifier
  uint16_t Tout;                        ///< Cache timeout timer
  uint8_t  IpAddr[NET_ADDR_IP4_LEN];    ///< Host IPv4 address
} NET_NBNS_INFO;

/// NDP Cache Entry info
typedef struct net_ndp_info {
  uint8_t  State;                       ///< Cache state
  uint8_t  Flags;                       ///< State flags
  uint16_t Tout;                        ///< Timeout timer
  uint8_t  IpAddr[NET_ADDR_IP6_LEN];    ///< IPv6 address
  uint8_t  MacAddr[NET_ADDR_ETH_LEN];   ///< Hardware MAC address
  uint8_t  Retries;                     ///< Number of retries left
  uint8_t  Id;                          ///< Entry identification number
  NET_FRAME *tx_list;                   ///< Frames waiting to resolve MAC address
} NET_NDP_INFO;

/// MLD Listener info
typedef struct net_mld_info {
  uint8_t  State;                       ///< Listener membership state
  uint8_t  Flags;                       ///< State flags
  uint16_t Tout;                        ///< Timeout timer for reports
  uint8_t  IpAddr[NET_ADDR_IP6_LEN];    ///< Listener IPv6 address
  uint8_t  Id;                          ///< Entry identification number
} NET_MLD_INFO;

/// IP Fragmentation session info
typedef struct net_ip_frag_info {
  uint16_t Tout;                        ///< Timeout timer
  uint16_t Len;                         ///< Total length of datagram fragments
  NET_FRAME *frag_list;                 ///< Fragments waiting to reassemble
} NET_IP_FRAG_INFO;

/// UDP Socket info
typedef struct net_udp_info {
  uint8_t  State;                       ///< Socket state
  uint8_t  Flags;                       ///< State flags and options
  uint16_t LocPort;                     ///< Local UDP port
  uint8_t  Tos;                         ///< Type of Service for IPv4
  uint8_t  TClass;                      ///< Traffic class/packet priority for IPv6
  uint8_t  McastTtl;                    ///< Multicast time to live for IPv4
  uint8_t  HopLimit;                    ///< Multicast hop limit for IPv6
  const struct net_if_cfg *net_if;      ///< Network interface for broadcast frames
  netUDP_cb_t cb_func;                  ///< Event callback function
} NET_UDP_INFO;

/// TCP Socket info
typedef struct net_tcp_info {
  netTCP_State State;                   ///< Socket state
  uint8_t  Type;                        ///< Socket type
  uint8_t  Flags;                       ///< State flags
  uint16_t LocPort;                     ///< Local TCP port
  uint8_t  Tos;                         ///< Type of Service for IPv4
  uint8_t  TClass;                      ///< Traffic class/packet priority for IPv6
  const uint8_t *LocAddr;               ///< Pointer to local IPv6 address
  __ADDR   Peer;                        ///< Peer IP address and port
  uint16_t MaxSegSize;                  ///< Transmit max. segment size
  uint16_t ConnTout;                    ///< Connection timeout
  uint16_t AliveTimer;                  ///< Keep alive timer
  uint16_t RetryTimer;                  ///< Retransmission timer
  uint8_t  AckTimer;                    ///< Receive delay-ack timer
  uint8_t  Id;                          ///< Socket identification number
  uint8_t  Retries;                     ///< Number of retries left
  uint8_t  DupAcks;                     ///< Number of duplicate acks (fast recovery)
  uint32_t SendUna;                     ///< Send sequence number unacknowledged
  uint32_t SendNext;                    ///< Next send sequence number
  uint32_t SendChk;                     ///< Check sequence number for dupacks
  uint32_t SendWl1;                     ///< Sequence number of last window update
  uint32_t SendWl2;                     ///< Acknowledge number of last window update
  uint16_t SendWin;                     ///< Current send window
  int16_t  RttSa;                       ///< Scaled average for RTT estimator
  int16_t  RttSv;                       ///< Scaled deviation for RTT estimator
  uint16_t CWnd;                        ///< Congestion window
  uint16_t SsThresh;                    ///< Slow start treshold
  uint16_t RecWin;                      ///< Current receive window
  uint32_t RecNext;                     ///< Next receive sequence number
  NET_FRAME *unack_list;                ///< Unacked queue list
  const struct net_if_cfg *net_if;      ///< Network interface for the route
  netTCP_cb_t cb_func;                  ///< Event callback function
} NET_TCP_INFO;

/// BSD Socket info
typedef struct net_bsd_info {
  uint8_t  State;                       ///< Socket state
  uint8_t  Socket;                      ///< Assigned TCP/UDP socket
  uint8_t  Flags;                       ///< Flags for socket option
  uint8_t  Family;                      ///< Socket address family
  uint8_t  Type;                        ///< Type of socket
  __ADDR   RemHost;                     ///< Remote IP address and port
  __ADDR   LocHost;                     ///< Local IP address and port
  uint8_t  AcceptSock;                  ///< Accept socket list
  uint8_t  ParentSock;                  ///< Parent (server) socket
  uint16_t RecvTout;                    ///< Blocking receive timeout
  uint16_t Timer;                       ///< Receiver timer
  uint8_t  SendTout;                    ///< Blocking send timeout
  uint8_t  xTimer;                      ///< Sender timer
  uint8_t  Id;                          ///< Socket identification number
  uint8_t  Event;                       ///< Owner wait-for events
  uint8_t  xEvent;                      ///< Sender wait-for events
  uint8_t  Tos;                         ///< Type of Service for IPv4
  uint8_t  TClass;                      ///< Traffic class for IPv6
  NETOS_ID Thread;                      ///< Socket owner thread
  NETOS_ID xThread;                     ///< Socket sender thread
  NET_BUFFER *buf_list;                 ///< Rx data buffer list
} NET_BSD_INFO;

/// DNS Cache Entry info
typedef struct net_dns_info {
  uint32_t HostId;                      ///< Host Id (CRC32 value of host name)
  uint16_t Tout;                        ///< Cache timeout timer
  int16_t  Type;                        ///< Address type: IPv4 or IPv6
  uint8_t  IpAddr[__ADDR_IP_LEN];       ///< Internet IP address
} NET_DNS_INFO;

/// TFTP Server Session info
typedef struct net_tftp_info {
  uint8_t  State;                       ///< Session state
  uint8_t  Socket;                      ///< Assigned UDP socket
  uint8_t  Flags;                       ///< State flags
  uint8_t  Retries;                     ///< Retry counter
  __ADDR   Client;                      ///< Client IP address and port (TID)
  uint16_t BlockSz;                     ///< Transfer block size
  uint16_t BlockNr;                     ///< Block number
  uint8_t  Timer;                       ///< Timeout timer
  uint8_t  Id;                          ///< Session identification number
  uint16_t BufLen;                      ///< Length of retransmit buffer
  uint8_t  *Buf;                        ///< Transmit/retransmit buffer
  void     *File;                       ///< File handle pointer
} NET_TFTP_INFO;

/// Telnet Session info
typedef struct net_telnet_info {
  uint8_t  State;                       ///< Session state
  uint8_t  Socket;                      ///< Assigned TCP socket
  uint8_t  Flags;                       ///< State flags
  uint8_t  BCnt;                        ///< Received data byte count
  uint16_t Tout;                        ///< Timeout delay timer
  uint8_t  Id;                          ///< Session identification number
  uint8_t  UserId;                      ///< User Id (authentication enabled)
  uint8_t  Widx;                        ///< Fifo buffer write index
  uint8_t  Ridx;                        ///< Fifo buffer read index
  uint8_t  hNext;                       ///< History command next position
  uint8_t  hCurr;                       ///< History command current position
  uint32_t SVar;                        ///< Session private variable
  char     LBuf[TELNET_LBUF_SZ];        ///< Data line buffer
  char     Fifo[TELNET_FIFO_SZ];        ///< Fifo buffer for received line/keycodes
  char     Hist[TELNET_HIST_SZ];        ///< Command history buffer
} NET_TELNET_INFO;

/// FTP Session info
typedef struct net_ftp_info {
  uint8_t  State;                       ///< Session state
  uint8_t  Socket;                      ///< TCP control socket
  uint16_t Flags;                       ///< State flags
  __ADDR   Client;                      ///< Client IP address and port
  uint16_t DPort;                       ///< Data port (active or passive)
  uint8_t  DSocket;                     ///< TCP data socket
  uint8_t  Id;                          ///< Session identification number
  uint8_t  UserId;                      ///< User Id (authentication enabled)
  uint8_t  Resp;                        ///< Server response code
  uint8_t  PathLen;                     ///< Size of the path string
  char    *Path;                        ///< Current working directory
  char    *Name;                        ///< Absolute file/folder path name
  void    *File;                        ///< File handle pointer
} NET_FTP_INFO;

/// HTTP Session info
typedef struct net_http_info {
  uint8_t  State;                       ///< Session state
  uint8_t  Socket;                      ///< Assigned TCP socket
  uint16_t Flags;                       ///< State flags
  uint8_t  FType;                       ///< File type: html, jpeg, gif,...
  uint8_t  PostSt;                      ///< POST method processing state
  uint16_t DelimSz;                     ///< Boundary delimiter size
  uint8_t  Id;                          ///< Session identification number
  uint8_t  UserId;                      ///< User Id (authentication enabled)
  uint8_t  TlsId;                       ///< TLS context identifier
  uint8_t  Method;                      ///< Request method (GET,POST,...)
  uint32_t CGIvar;                      ///< CGI session private variable
  uint32_t DLen;                        ///< Data length (number of bytes to send)
  uint32_t Count;                       ///< Total number of bytes sent
  uint16_t BCnt;                        ///< Number of bytes sent in last packet
  char     Lang[6];                     ///< Selected web language: en-us, de, fr...
  uint32_t LMDate;                      ///< Last modified date (UTC format)
  char    *Script;                      ///< Script buffer pointer
  char    *pDelim;                      ///< Multipart boundary delimiter value
  char    *pUser;                       ///< User MIME type or Redirect URL
  void    *sFile;                       ///< Script interpreter file pointer
  void    *dFile;                       ///< Data file pointer
  NET_BUFFER *cookie_list;              ///< List of cookies to send
} NET_HTTP_INFO;

/// HTTP Nonce Cache Entry info
typedef struct net_http_nonce {
  uint32_t Ticks;                       ///< Current time stamp in ticks
  uint16_t Count;                       ///< Number of references to nonce
  uint16_t Reserved;                    ///< Reserved (used for alignment)
  char     Val[24];                     ///< Nonce value base64 encoded
} NET_HTTP_NONCE;

/// HTTP Error page info
typedef struct net_http_error {
  const char *header;                   ///< Common page header
  const char *footer;                   ///< Common page footer
  const char *e401;                     ///< 401 Unauthorized
  const char *e403;                     ///< 403 Forbidden
  const char *e404;                     ///< 404 Not Found
  const char *e501;                     ///< 501 Not Implemented
} const NET_HTTP_ERROR;

/// SYS Configuration info
typedef struct net_sys_cfg {
  uint32_t   *MemPool;                  ///< Dynamic memory pool buffer
  uint32_t   MemSize;                   ///< Memory pool size in bytes
  char       *Path;                     ///< Path buffer for root-enabled services
  const char *HostName;                 ///< Configured Local host name
  bool       AutoStart;                 ///< Auto-Start server service
  bool       TxQueUsed;                 ///< Tx queue used for PPP or SLIP
  const net_sys_fn_t *fn_init;          ///< Network Init-function table
  const net_sys_fn_t *fn_run;           ///< Network Run-function table
} const NET_SYS_CFG;

/// netIF_GetOption functions
typedef struct {
  netStatus (*eth_get_option) (uint32_t,netIF_Option,uint8_t*,uint32_t);
  netStatus (*wifi_get_option)(uint32_t,netIF_Option,uint8_t*,uint32_t);
  netStatus (*ppp_get_option) (netIF_Option,uint8_t*,uint32_t);
  netStatus (*slip_get_option)(netIF_Option,uint8_t*,uint32_t);
} const NETIF_GETOPT_FUNC;

/// netIF_SetOption functions
typedef struct {
  netStatus (*eth_set_option) (uint32_t,netIF_Option,const uint8_t*,uint32_t);
  netStatus (*wifi_set_option)(uint32_t,netIF_Option,const uint8_t*,uint32_t);
  netStatus (*ppp_set_option) (netIF_Option,const uint8_t*,uint32_t);
  netStatus (*slip_set_option)(netIF_Option,const uint8_t*,uint32_t);
} const NETIF_SETOPT_FUNC;

/// ARP Configuration info
typedef struct net_arp_cfg {
  struct net_arp_ctrl     *Ctrl;        ///< Instance control block
  const struct net_if_cfg *If;          ///< Link to general interface descriptor
  NET_ARP_INFO *Table;                  ///< Cache table array
  uint8_t  TabSize;                     ///< Cache table size
  uint8_t  TimeOut;                     ///< Cache expiration time in seconds
  uint8_t  MaxRetry;                    ///< Number of retries to resolve MAC address
  uint8_t  Resend;                      ///< Resend timeout in seconds
  bool     Notify;                      ///< Notify on IP address changes
} const NET_ARP_CFG;

/// ICMP Configuration info
typedef struct net_icmp_cfg {
  struct net_icmp_ctrl    *Ctrl;        ///< Instance control block
  const struct net_if_cfg *If;          ///< Link to general interface descriptor
  bool     NoEcho;                      ///< Disable ping/echo response
} const NET_ICMP_CFG;

/// IGMP Configuration info
typedef struct net_igmp_cfg {
  struct net_igmp_ctrl    *Ctrl;        ///< Instance control block
  const struct net_if_cfg *If;          ///< Link to general interface descriptor
  NET_IGMP_INFO *Table;                 ///< Group table array
  uint16_t TabSize;                     ///< Group table size
  void (*process)(                      ///< Process IGMP message function
    const struct net_if_cfg*,NET_FRAME*);
  bool (*is_member)(                    ///< Check if host member of a group
    const struct net_if_cfg*,const uint8_t*);
  uint32_t (*collect_mcast)(            ///< Collect multicast MAC addresses
    const struct net_if_cfg*,uint8_t*);
} const NET_IGMP_CFG;

/// DHCP Client Configuration info
typedef struct net_dhcp_cfg {
  struct net_dhcp_ctrl    *Ctrl;        ///< Instance control block
  const struct net_if_cfg *If;          ///< Link to general interface descriptor
  const char *Vcid;                     ///< Vendor class identifier
  uint8_t    VcidLen;                   ///< Length of Vendor class identifier
  uint8_t    OptBootfile:1;             ///< Option bootfile enable
  uint8_t    OptNtpServ :1;             ///< Option NTP servers enable
} const NET_DHCP_CFG;

/// NBNS Configuration info
typedef struct net_nbns_cfg {
  NET_NBNS_INFO *Table;                 ///< Cache table array
  uint8_t  TabSize;                     ///< Cache table size
  uint16_t TimeOut;                     ///< Cache expiration time in seconds
} const NET_NBNS_CFG;

/// IP4 Configuration info
typedef struct net_ip4_cfg {
  const char  *IpAddr;                  ///< IPv4 address
  const char  *NetMask;                 ///< IPv4 network mask
  const char  *DefGW;                   ///< Default gateway IPv4 address
  const char  *PriDNS;                  ///< Primary DNS server
  const char  *SecDNS;                  ///< Secondary DNS server
  uint16_t     Mtu;                     ///< Maximum transmission unit
  uint8_t      EnNbns  :1;              ///< Enable NBNS protocol
  NET_ARP_CFG  *ArpCfg;                 ///< ARP configuration
  NET_ICMP_CFG *IcmpCfg;                ///< ICMP configuration
  NET_IGMP_CFG *IgmpCfg;                ///< IGMP configuration
  NET_DHCP_CFG *DhcpCfg;                ///< DHCP configuraton
} const NET_IP4_CFG;

/// NDP Configuration info
typedef struct net_ndp_cfg {
  struct net_ndp_ctrl     *Ctrl;        ///< Instance control block
  const struct net_if_cfg *If;          ///< Link to general interface descriptor
  NET_NDP_INFO *Table;                  ///< Neighbor cache table
  uint8_t   TabSize;                    ///< Cache table size
  uint8_t   TimeOut;                    ///< Cache expiration time in seconds
  uint8_t   MaxRetry;                   ///< Number of retries to resolve MAC address
  uint8_t   Resend;                     ///< Resend timeout in seconds
  void (*process)(                      ///< Process NDP message function
    const struct net_if_cfg*,NET_FRAME*);
} const NET_NDP_CFG;

/// ICMP6 Configuration info
typedef struct net_icmp6_cfg {
  struct net_icmp6_ctrl   *Ctrl;        ///< Instance control block
  const struct net_if_cfg *If;          ///< Link to general interface descriptor
  bool     NoEcho;                      ///< Disable ping/echo response
} const NET_ICMP6_CFG;

/// MLD Configuration info
typedef struct net_mld_cfg {
  struct net_mld_ctrl     *Ctrl;        ///< Instance control block
  const struct net_if_cfg *If;          ///< Link to general interface descriptor
  NET_MLD_INFO *Table;                  ///< Listener table array
  uint16_t TabSize;                     ///< Listener table size
  void (*process)(                      ///< Process MLD message function
    const struct net_if_cfg*,NET_FRAME*);
  bool (*listening)(                    ///< Check if node is listening
    const struct net_if_cfg*,const uint8_t*);
  uint32_t (*collect_mcast)(            ///< Collect multicast MAC addresses
    const struct net_if_cfg*,uint8_t*);
} const NET_MLD_CFG;

/// DHCP6 Client Configuration info
typedef struct net_dhcp6_cfg {
  struct net_dhcp6_ctrl   *Ctrl;        ///< Instance control block
  const struct net_if_cfg *If;          ///< Link to general interface descriptor
  uint8_t    Stateful  :1;              ///< Statefull mode enabled
  uint8_t    OptVclass :1;              ///< Option Vendor class enabled
  uint32_t   EnterpId;                  ///< Enterprise-number
  const char *VcData;                   ///< Vendor class data
} const NET_DHCP6_CFG;

/// IP6 Configuration info
typedef struct net_ip6_cfg {
  const char   *IpAddr;                 ///< IPv6 address
  const char   *DefGW;                  ///< Default gateway IPv6 address
  const char   *PriDNS;                 ///< Primary DNS server
  const char   *SecDNS;                 ///< Secondary DNS server
  uint8_t       PrefixLen;              ///< Address prefix length
  uint16_t      Mtu;                    ///< Maximum transmission unit
  NET_NDP_CFG   *NdpCfg;                ///< Neighbor discovery configuration
  NET_ICMP6_CFG *Icmp6Cfg;              ///< ICMPv6 configuration
  NET_MLD_CFG   *MldCfg;                ///< Multicast listener discovery configuration
  NET_DHCP6_CFG *Dhcp6Cfg;              ///< DHCPv6 configuration
  uint32_t (*collect_mcast)(            ///< Collect multicast MAC addresses
    const struct net_if_cfg*,uint8_t*);
} const NET_IP6_CFG;

/// Interface status info
typedef struct net_if_state {
  bool     LinkUp;                      ///< Link up or WiFi connected
  bool     MacNew;                      ///< Mac address changed
  bool     ConfigMcast;                 ///< Configure multicast Mac address filtering
  uint16_t Offload;                     ///< Checksum offload flags
} NET_IF_STATE;

/// General Interface descriptor
typedef struct net_if_cfg {
  NET_IF_STATE *State;                  ///< Interface Status
  uint16_t     Id;                      ///< Interface Identifier
  char         Name[6];                 ///< Interface Name null-terminated
  uint8_t     *MacAddr;                 ///< Active MAC address
  NET_LOCALM  *localm;                  ///< Local machine info IPv4
  NET_IP4_CFG *Ip4Cfg;                  ///< IPv4 configuration
  NET_LOCALM6 *localm6;                 ///< Local machine info IPv6
  NET_IP6_CFG *Ip6Cfg;                  ///< IPv6 configuration
  bool (*send_frame)(uint32_t,          ///< Send frame function
                     NET_FRAME*,uint8_t);
  bool (*output_lan)(uint32_t,          ///< Low level output for LAN (Eth, WiFi)
                     NET_FRAME*);
} const NET_IF_CFG;

/// Ethernet Interface descriptor
typedef struct net_eth_cfg {
  struct net_eth_ctrl     *Ctrl;        ///< Interface control block
  const struct net_if_cfg *If;          ///< Link to general interface descriptor
  const void  *DrvMac;                  ///< Registered MAC driver
  const void  *DrvPhy;                  ///< Registered PHY driver
  uint8_t     *MacAddr;                 ///< Active MAC address
  const char  *MacCfg;                  ///< Configured MAC address
  uint16_t     VlanTag;                 ///< Vlan tag identifier
  uint8_t      IfNum;                   ///< Interface number (0,1)
  void (*cb_event)(uint32_t);           ///< Driver event notification callback
} const NET_ETH_CFG;

/// WiFi Interface descriptor
typedef struct net_wifi_cfg {
  struct net_wifi_ctrl    *Ctrl;        ///< Interface control block
  const struct net_if_cfg *If;          ///< Link to general interface descriptor
  const void  *DrvWifi;                 ///< Registered WiFi driver
  uint8_t     *MacAddr;                 ///< Active MAC address
  const char  *MacCfg;                  ///< Configured MAC address
  uint8_t      Mode;                    ///< WiFi Mode (Station, AP)
  uint8_t      IfNum;                   ///< Interface number (0,1)
  void (*cb_event)(uint32_t,void*);     ///< Driver event notification callback
} const NET_WIFI_CFG;

/// Serial Device Configuration info
typedef struct net_com_cfg {
  struct net_com_ctrl   *Ctrl;          ///< Serial control block
  struct net_modem_ctrl *ModemCtrl;     ///< Modem service control block
  const void *DrvUsart;                 ///< Registered USART driver
  const void *DrvModem;                 ///< Registered MODEM driver
  const char *InitString;               ///< Modem initialization string
  uint32_t    Speed;                    ///< Connection speed
  uint8_t     FlowCtrl;                 ///< Flow control
  uint8_t     MaxDelay;                 ///< Max. thread polling delay in ms
  uint8_t     RecTout;                  ///< Frame receive timeout in ticks
  void (*cb_event)  (uint32_t);         ///< USART driver event callback
  void (*cb_request)(const char*,       ///< Modem driver request callback
                     const char*,uint32_t,uint32_t);
} const NET_COM_CFG;

/// PPP Interface descriptor
typedef struct net_ppp_cfg {
  struct net_ppp_ctrl     *Ctrl;        ///< Interface control block
  const struct net_if_cfg *If;          ///< Link to general interface descriptor
  NET_COM_CFG *ComCfg;                  ///< Serial device configuration
  uint32_t     ACCmap;                  ///< Async control character map
  uint16_t     RetryTout;               ///< Retry timeout in ticks
  uint16_t     EchoTout;                ///< LCP echo timeout in seconds
  uint8_t      MaxRetry;                ///< Number of retries
  uint8_t      EnAuth   :1;             ///< Enable authentication
  uint8_t      UseDefGW :1;             ///< Use default gateway on remote network
  uint8_t      ObtainIp :1;             ///< Obtain client IP address automatically
  uint8_t      IfNum;                   ///< Interface number (0,1)
  struct net_lcp_ctrl       *LcpCtrl;   ///< LCP Service control block
  const struct net_ppp_auth *PapAuth;   ///< PAP Authentication service
  const struct net_ppp_auth *ChapAuth;  ///< CHAP Authentication service
  struct net_ipcp_ctrl      *IpcpCtrl;  ///< IPCP Service control block
} const NET_PPP_CFG;

/// PPP PAP/CHAP Configuration info
typedef struct net_ppp_auth {
  void *Ctrl;                           ///< Authentication control block
  void (*init)   (NET_PPP_CFG*);        ///< Initialize authentication
  void (*uninit) (NET_PPP_CFG*);        ///< De-initialize authentication
  void (*run)    (NET_PPP_CFG*);        ///< Run authentication
  void (*process)(NET_PPP_CFG*,NET_FRAME*);///< Process authentication frame
} const NET_PPP_AUTH;

/// SLIP Interface descriptor
typedef struct net_slip_cfg {
  struct net_slip_ctrl    *Ctrl;        ///< SLIP Interface control block
  const struct net_if_cfg *If;          ///< Link to general interface descriptor
  NET_COM_CFG *ComCfg;                  ///< Serial device configuration
  uint8_t      UseDefGW :1;             ///< Use default gateway on remote network
  uint8_t      IfNum;                   ///< Interface number (0,1)
} const NET_SLIP_CFG;

/// IP Fragmentation Configuration info
typedef struct net_ip_frag_cfg {
  NET_IP_FRAG_INFO *Scb;                ///< Reassembly control block array
  uint8_t  NumSess;                     ///< Max. number of reassembly sessions
  uint16_t ReassTout;                   ///< Reassembly timeout in ticks
} const NET_IP_FRAG_CFG;

/// UDP Socket Configuration info
typedef struct net_udp_cfg {
  NET_UDP_INFO *Scb;                    ///< Socket control block array
  uint8_t  NumSocks;                    ///< Number of UDP sockets
  uint16_t DynPortStart;                ///< Dynamic port start
  uint16_t DynPortEnd;                  ///< Dynamic port end
} const NET_UDP_CFG;

/// TCP Socket Configuration info
typedef struct net_tcp_cfg {
  NET_TCP_INFO *Scb;                    ///< Socket control block array
  uint8_t  NumSocks;                    ///< Number of TCP sockets
  uint8_t  MaxRetry;                    ///< Number of retries
  uint16_t RetryTout;                   ///< Retry timeout in ticks
  uint16_t SynRetryTout;                ///< SYN retry timeout in ticks
  uint16_t InitRetryTout;               ///< Initial retransmit timeout in ticks
  uint16_t DefTout;                     ///< Default connect timeout in seconds
  uint16_t MaxSegSize;                  ///< Maximum segment size value
  uint16_t RecWinSize;                  ///< Receiving window size in bytes
  uint8_t  ConnRetry;                   ///< Number of retries to connect
  uint16_t DynPortStart;                ///< Dynamic port start
  uint16_t DynPortEnd;                  ///< Dynamic port end
} const NET_TCP_CFG;

/// BSD Socket Configuration info
typedef struct net_bsd_cfg {
  NET_BSD_INFO *Scb;                    ///< Socket control block array
  uint8_t  NumSocks;                    ///< Number of BSD sockets
  uint16_t RecvTout;                    ///< Blocking recv timeout in ticks
} const NET_BSD_CFG;

/// DNS Client Configuration info
typedef struct net_dns_cfg {
  NET_DNS_INFO *Table;                  ///< Cache table array
  uint8_t   TabSize;                    ///< Cache table size
} const NET_DNS_CFG;

/// mbedTLS interface functions
typedef struct net_tls_if {
  uint8_t  (*get_context)(int32_t,netTCP_cb_t);  ///< Allocate secure TLS context
  void     (*connect)    (uint8_t,const char *); ///< Connect to remote endpoint
  void     (*listen)     (uint8_t);              ///< Listen for incomming connections
  uint8_t *(*get_buf)    (uint32_t);             ///< Get memory for send buffer
  void     (*write)      (uint8_t,const uint8_t*,uint32_t);///< Write data to TLS session
  void     (*close)      (uint8_t,uint8_t);      ///< Close TLS session
} const NET_TLS_IF;

/// SMTP Client Attachment functions
typedef struct {
  uint32_t (*mail_attach)  (char*,uint32_t);     ///< Compose attachment
  uint32_t (*mime_header)  (char*,const void*);  ///< Add MIME multipart header
  const char*(*parse_fname)(const char*,void*);  ///< Parse attachment file name(s)
} const NET_SMTP_ATTACH;

/// SMTP Client Configuration info
typedef struct net_smtp_cfg {
  uint8_t   DefTout;                    ///< Default inactivity timeout
  struct {                              ///< File System interface functions
    void    *(*fopen) (const char*);             ///< Open file for reading
    uint32_t (*fread) (void*,uint8_t*,uint32_t); ///< Read file data
    void     (*fclose)(void*);                   ///< Close file
  } fs_if;
  NET_SMTP_ATTACH *attach_if;           ///< Attachment support interface
  NET_TLS_IF      *tls_if;              ///< mbedTLS interface functions
} const NET_SMTP_CFG;

/// SNMP Agent Configuration info
typedef struct net_snmp_cfg {
  uint16_t   PortNum;                   ///< Listening port number
  uint16_t   TrapPort;                  ///< Trap port number
  const char *TrapIp;                   ///< Trap server IP address
  const char *Community;                ///< Community name
} const NET_SNMP_CFG;

/// SNTP Client Configuration info
typedef struct net_sntp_cfg {
  const char *SrvIp;                    ///< NTP/SNTP server IP address
  bool       Bcast;                     ///< Broadcast client mode (for LAN only)
} const NET_SNTP_CFG;

/// TFTP Server Configuration info
typedef struct net_tftp_cfg {
  NET_TFTP_INFO *Scb;                   ///< Session control block array
  uint8_t  NumSess;                     ///< Max. number of active sessions
  uint8_t  MaxRetry;                    ///< Number of retries
  uint16_t PortNum;                     ///< Listening port number
  uint8_t  DefTout;                     ///< Inactive session timeout in seconds
  bool     EnFwall;                     ///< Enable firewall support
  const char *Root;                     ///< Root folder path
} const NET_TFTP_CFG;

/// TFTP Client Configuration info
typedef struct net_tftpc_cfg {
  uint16_t BlockSize;                   ///< Transfer block size
  uint16_t RetryTout;                   ///< Retry timeout in ticks
  uint8_t  MaxRetry;                    ///< Number of retries
} const NET_TFTPC_CFG;

/// Telnet Server Configuration info
typedef struct net_telnet_cfg {
  NET_TELNET_INFO *Scb;                 ///< Session control block array
  uint8_t  NumSess;                     ///< Max. number of active sessions
  bool     EnAuth;                      ///< Enable user authentication
  uint8_t  NoEcho;                      ///< Disable server echo mode
  uint16_t PortNum;                     ///< Listening port number
  uint16_t IdleTout;                    ///< Idle connection timeout in ticks
  const char *User;                     ///< Default admin username
  const char *Passw;                    ///< Default admin password
  uint8_t  UserLen;                     ///< Length of admin username
  uint8_t  PasswLen;                    ///< Length of admin password
} const NET_TELNET_CFG;

/// FTP Server Configuration info
typedef struct net_ftp_cfg {
  NET_FTP_INFO *Scb;                    ///< Session control block array
  uint8_t  NumSess;                     ///< Max. number of active sessions
  bool     EnAuth;                      ///< Enable user authentication
  uint16_t PortNum;                     ///< Listening port number
  uint16_t IdleTout;                    ///< Idle connection timeout in ticks
  uint8_t  MsgLen;                      ///< Length of welcome message
  const char *Msg;                      ///< Server welcome message
  const char *Root;                     ///< Root folder path
  const char *User;                     ///< Default admin username
  const char *Passw;                    ///< Default admin password
  uint8_t  UserLen;                     ///< Length of admin username
  uint8_t  PasswLen;                    ///< Length of admin password
} const NET_FTP_CFG;

/// FTP Client Configuration info
typedef struct net_ftpc_cfg {
  uint8_t DefTout;                      ///< Default inactivity timeout
  bool    PasvMode;                     ///< FTP passive mode used
} const NET_FTPC_CFG;

/// HTTP Server Authentication functions
typedef struct {
  void    (*init)      (void);          ///< Initialize HTTP authentication
  void    (*uninit)    (void);          ///< De-initialize HTTP authentication
  void    (*run)       (void);          ///< Run HTTP authentication
  int32_t (*add_line)  (void*,char*);   ///< Add authentication header line
  bool    (*parse_line)(void*,char*);   ///< Parse authentication header line
  void    (*calc_hash) (const char*,    ///< Calculate Digest hash HA1 value
                        const char*,uint8_t*);
} const NET_HTTP_AUTH;

/// HTTP Server Configuration info
typedef struct net_http_cfg {
  NET_HTTP_INFO *Scb;                   ///< Session control block array
  uint8_t   NumSess;                    ///< Max. number of active sessions
  bool      EnAuth;                     ///< Enable user authentication
  uint16_t  PortNum;                    ///< Listening port number
  const char *SrvId;                    ///< Server-Id http header
  const char *Root;                     ///< Root folder path
  const char *Realm;                    ///< Authentication realm space
  uint8_t    RealmLen;                  ///< Length of realm string
  const char *User;                     ///< Default admin username
  const char *Passw;                    ///< Default admin password
  uint8_t   UserLen;                    ///< Length of admin username
  uint8_t   PasswLen;                   ///< Length of admin password
  NET_HTTP_NONCE *Nonce;                ///< Nonce cache table array
  uint8_t   NumNonce;                   ///< Max. number of cache entries
  uint16_t  NonceTout;                  ///< Cache entry timeout in ticks
  NET_HTTP_AUTH *auth_if;               ///< Authentication support interface
  NET_TLS_IF    *tls_if;                ///< mbedTLS interface functions
} const NET_HTTP_CFG;

/// Generic unspecified address
extern const uint8_t net_addr_unspec[];

/// Network configuration data
extern NET_SYS_CFG      net_sys_config;
extern NET_IF_CFG       net_eth0_if_config;
extern NET_IF_CFG       net_eth1_if_config;
extern NET_IF_CFG       net_wifi0_if_config;
extern NET_IF_CFG       net_wifi1_if_config;
extern NET_IF_CFG       net_ppp0_if_config;
extern NET_IF_CFG       net_slip0_if_config;

extern NET_ETH_CFG   *const net_eth_list[];
extern NET_WIFI_CFG  *const net_wifi_list[];
extern NET_PPP_CFG   *const net_ppp_list[];
extern NET_SLIP_CFG  *const net_slip_list[];
extern NET_IF_CFG    *const net_if_list_lan[];
extern NET_IF_CFG    *const net_if_list_all[];
extern NET_IF_CFG    *const net_if_inet_def[];
extern NET_IF_CFG    *const net_if_link_def[];
extern NET_ARP_CFG   *const net_arp_list[];
extern NET_ICMP_CFG  *const net_icmp_list[];
extern NET_IGMP_CFG  *const net_igmp_list[];
extern NET_DHCP_CFG  *const net_dhcp_list[];
extern NET_ICMP6_CFG *const net_icmp6_list[];
extern NET_NDP_CFG   *const net_ndp_list[];
extern NET_MLD_CFG   *const net_mld_list[];
extern NET_DHCP6_CFG *const net_dhcp6_list[];

extern NET_IP_FRAG_CFG  net_ip4_frag_config;
extern NET_IP_FRAG_CFG  net_ip6_frag_config;
extern NET_NBNS_CFG     net_nbns_config;
extern NET_UDP_CFG      net_udp_config;
extern NET_TCP_CFG      net_tcp_config;
extern NET_BSD_CFG      net_bsd_config;
extern NET_HTTP_CFG     net_http_config;
extern NET_HTTP_ERROR   net_http_error;
extern NET_TELNET_CFG   net_telnet_config;
extern NET_TFTP_CFG     net_tftp_config;
extern NET_TFTPC_CFG    net_tftpc_config;
extern NET_FTP_CFG      net_ftp_config;
extern NET_FTPC_CFG     net_ftpc_config;
extern NET_DNS_CFG      net_dns_config;
extern NET_SMTP_CFG     net_smtp_config;
extern NET_SNMP_CFG     net_snmp_config;
extern NET_SNTP_CFG     net_sntp_config;

/// Network uninit functions
extern const net_sys_fn_t net_sys_fn_uninit[];

/// Network interface functions
extern NETIF_GETOPT_FUNC  netif_getopt_func;
extern NETIF_SETOPT_FUNC  netif_setopt_func;

/// Network exported functions
extern NET_PPP_AUTH     net_ppp0_pap_auth;
extern NET_PPP_AUTH     net_ppp0_chap_auth;
extern NET_SMTP_ATTACH  net_smtp_attach_func;
extern NET_HTTP_AUTH    net_http_auth_basic_func;
extern NET_HTTP_AUTH    net_http_auth_digest_func;

/// Network Security Credentials
extern const uint8_t  NetSecurity_ServerCA[];
extern const uint32_t NetSecurity_ServerCA_Len;
extern const uint8_t  NetSecurity_ServerCert[];
extern const uint32_t NetSecurity_ServerCert_Len;
extern const uint8_t  NetSecurity_ServerKey[];
extern const uint32_t NetSecurity_ServerKey_Len;
extern const uint8_t  NetSecurity_EmailServerCA[];
extern const uint32_t NetSecurity_EmailServerCA_Len;

//  ==== Network system ====

/// \brief Network Component core thread.
/// \param[in]     arg           dummy parameter.
/// \return        None.
extern void netCore_Thread (void *arg);

/// \brief Network core tick timer callback.
/// \param[in]     arg           dummy parameter.
/// \return        None.
extern void net_sys_tick (void *arg);

/// \brief Signal Network Component error.
/// \param[in]     error         system error code.
/// \return        None.
extern void net_sys_error (NET_ERROR error);

/// \brief Acquire Network core protection mutex.
/// \return        None.
extern void net_sys_lock   (void);

/// \brief Release Network core protection mutex.
/// \return        None.
extern void net_sys_unlock (void);

//  ==== OS abstraction layer ====

/// \brief Initialize OS abstraction layer.
/// \return        None.
extern void netos_init (void);

/// \brief Create network core thread.
/// \return        thread identifier.
extern NETOS_ID netos_thread_create (void);

/// \brief Delete network core thread.
/// \param[in]     thread        thread identifier.
/// \return        None.
extern void netos_thread_delete (NETOS_ID thread);

/// \brief Get running thread identifier.
/// \return        thread identifier.
extern NETOS_ID netos_thread_id (void);

/// \brief Pass control to next ready thread.
/// \return        None.
extern void netos_thread_pass (void);

/// \brief Create network periodic tick timer.
/// \return        timer identifier.
extern NETOS_ID netos_timer_create (void);

/// \brief Delete network periodic tick timer.
/// \param[in]     timer         timer identifier.
/// \return        None.
extern void netos_timer_delete (NETOS_ID timer);

/// \brief Start network periodic tick timer.
/// \param[in]     timer         timer identifier.
/// \param[in]     interval_ms   tick interval in millisec.
/// \return        None.
extern void netos_timer_start (NETOS_ID timer, uint32_t interval_ms);

/// \brief Create network protection mutex.
/// \param[in]     sys_id        system identifier (0=core, 1=memory).
/// \return        mutex identifier.
extern NETOS_ID netos_mutex_create (uint8_t sys_id);

/// \brief Delete network protection mutex.
/// \param[in]     mutex         mutex identifier.
/// \return        None.
extern void netos_mutex_delete (NETOS_ID mutex);

/// \brief Lock network protection mutex.
/// \param[in]     mutex         mutex identifier.
/// \return        None.
extern void netos_lock (NETOS_ID mutex);

/// \brief Unlock network protection mutex.
/// \param[in]     mutex         mutex identifier.
/// \return        None.
extern void netos_unlock (NETOS_ID mutex);

/// \brief Delay network thread execution.
/// \param[in]     ms            millisec to delay.
/// \return        None.
extern void netos_delay (uint32_t ms);

/// \brief Suspend thread until event flag is set.
/// \param[in]     flag          event flag to wait for.
/// \param[in]     ms            millisec to delay.
/// \return        None.
extern void netos_flag_wait (uint32_t flag, uint32_t ms);

/// \brief Set thread event flag.
/// \param[in]     thread        thread identifier.
/// \param[in]     flag          event flag to wait for.
/// \return        None.
extern void netos_flag_set (NETOS_ID thread, uint32_t flag);

/// \brief Clear thread event flag.
/// \param[in]     thread        thread identifier.
/// \param[in]     flag          event flag to wait for.
/// \return        None.
extern void netos_flag_clear (NETOS_ID thread, uint32_t flag);

/// \brief Create network interface thread and protection semaphore.
/// \param[in]     if_id         Network interface identification.
/// \param[out]    semaphore     pointer to semaphore identifier.
/// \return        thread identifier.
extern NETOS_ID netif_create (uint32_t if_id, NETOS_ID *semaphore);

/// \brief Delete network interface thread and protection semaphore.
/// \param[in]     thread        thread identifier.
/// \param[in]     semaphore     semaphore identifier.
/// \return        None.
extern void netif_delete (NETOS_ID thread, NETOS_ID semaphore);

/// \brief Lock network interface protection semaphore.
/// \param[in]     semaphore     semaphore identifier.
/// \return        None.
extern void netif_lock (NETOS_ID semaphore);

/// \brief Unlock network interface protection semaphore.
/// \param[in]     semaphore     semaphore identifier.
/// \return        None.
extern void netif_unlock (NETOS_ID semaphore);

//  ==== Memory management ====

/// \brief Allocate memory for the network frame.
/// \param[in]     byte_size     buffer size in bytes.
/// \return        Pointer to the allocated memory.
extern NET_FRAME *net_mem_alloc (uint32_t byte_size);

/// \brief Release allocated memory.
/// \param[in]     mem_ptr       pointer to the allocated memory.
/// \return        None.
extern void net_mem_free (NET_FRAME *mem_ptr);

//  ==== Ethernet interface ====

/// \brief Ethernet interface thread.
/// \param[in]     arg           dummy parameter.
/// \return        None.
extern void netETH_Thread (void *arg);

/// \brief Initialize ethernet interface.
/// \return        None.
extern void net_eth_iface_init (void);

/// \brief De-initialize ethernet interface.
/// \return        None.
extern void net_eth_iface_uninit (void);

/// \brief Run ethernet interface main function.
/// \return None.
extern void net_eth_iface_run (void);

/// \brief Construct ethernet header for the frame and send it.
/// \param[in]     if_num        Interface number.
/// \param[in]     frame         frame to be sent.
/// \param[in]     ip_ver        IP version of the frame.
/// \return
///              - true        = Frame successfuly sent.
///              - false       = Failed to send a frame.
extern bool net_eth_send_frame (uint32_t if_num, NET_FRAME *frame, uint8_t ip_ver);

/// \brief Protected output of ethernet frame.
/// \param[in]     if_num        Interface number.
/// \param[in]     frame         network frame.
/// \return
///              - true  = output success.
///              - false = output failed.
extern bool net_eth_output (uint32_t if_num, NET_FRAME *frame);

/// \brief Send event notification from eth driver.
/// \param[in]     event         receive event.
/// \param[in]     ctx           interface context (eth0, eth1).
/// \return        None.
extern void net_eth_callback (uint32_t event, struct net_eth_ctrl *ctx);

/// \brief Get MAC address for the given IP address (legacy).
/// \param[in]     addr        structure containing IP address of a host.
/// \return        Pointer to the MAC address in Neighbor cache.
extern const uint8_t *net_eth_get_addr (const __ADDR *addr);

/// \brief Get current value of Ethernet Interface option.
/// \param[in]     if_num        Interface number.
/// \param[in]     option        Interface option as specified by \ref netIF_Option.
/// \param[out]    buf           buffer to store the option value to
/// \param[in]     buf_len       length of buffer
/// \return      status code that indicates the execution status of the function.
extern netStatus net_eth_get_option (uint32_t if_num, netIF_Option option, uint8_t *buf, uint32_t buf_len);

/// \brief Set the value of Ethernet Interface option.
/// \param[in]     if_num        Interface number.
/// \param[in]     option        Interface option as specified by \ref netIF_Option.
/// \param[in]     buf           buffer containing the option value
/// \param[in]     buf_len       length of buffer
/// \return      status code that indicates the execution status of the function.
extern netStatus net_eth_set_option (uint32_t if_num, netIF_Option option, const uint8_t *buf, uint32_t buf_len);

/// \brief Initialize ARP cache.
/// \return        None.
extern void net_arp_cache_init (void);

/// \brief De-initialize ARP cache.
/// \return        None.
extern void net_arp_cache_uninit (void);

/// \brief Run ARP cache main function.
/// \return None.
extern void net_arp_cache_run (void);

/// \brief Send a notification of local IP address change (gratuitous ARP).
/// \param[in]     net_if        network interface descriptor.
/// \return        None.
extern void net_arp_notify (NET_IF_CFG *net_if);

/// \brief Initialize IGMP group management.
/// \return        None.
extern void net_igmp_host_init (void);

/// \brief De-initialize IGMP group management.
/// \return        None.
extern void net_igmp_host_uninit (void);

/// \brief Run IGMP group management main function.
/// \return        None.
extern void net_igmp_host_run (void);

/// \brief Process IGMP group management frame.
/// \param[in]     net_if        network interface descriptor.
/// \param[in]     frame         received IGMP group management frame.
/// \return        None.
extern void net_igmp_process (NET_IF_CFG *net_if, NET_FRAME *frame);

/// \brief Check if local host is a member of provided group.
/// \param[in]     net_if        network interface descriptor.
/// \param[in]     ip4_addr      multicast IPv4 address to be checked.
/// \return
///              - true        = Local host is member of IGMP group ipadr.
///              - false       = Local host is not member.
extern bool net_igmp_is_member (NET_IF_CFG *net_if, const uint8_t *ip4_addr);

/// \brief Collect multicast MAC addresses of active IGMP groups.
/// \param[in]     net_if        network interface descriptor.
/// \param[out]    buf           buffer to copy MAC addresses to.
/// \return        Number of MAC addresses copied.
extern uint32_t net_igmp_collect_mcast (NET_IF_CFG *net_if, uint8_t *buf);

/// \brief Initialize MLD nodes.
/// \return        None.
extern void net_mld_node_init (void);

/// \brief De-initialize MLD nodes.
/// \return        None.
extern void net_mld_node_uninit (void);

/// \brief Run MLD node main function.
/// \return        None.
extern void net_mld_node_run (void);

/// \brief Process MLD message frame.
/// \param[in]     net_if        network interface descriptor.
/// \param[in]     frame         received MLD message frame.
/// \return        None.
extern void net_mld_process (NET_IF_CFG *net_if, NET_FRAME *frame);

/// \brief Check if local node is listening on this address.
/// \param[in]     net_if        network interface descriptor.
/// \param[in]     ip6_addr      multicast IPv6 address to be checked.
/// \return
///              - true        = Local node is listening.
///              - false       = Local node is not listening.
extern bool net_mld_listening (NET_IF_CFG *net_if, const uint8_t *ip4_addr);

/// \brief Collect multicast MAC addresses of listening nodes.
/// \param[in]     net_if        network interface descriptor.
/// \param[out]    buf           buffer to copy MAC addresses to.
/// \return        Number of MAC addresses copied.
extern uint32_t net_mld_collect_mcast (NET_IF_CFG *net_if, uint8_t *buf);

/// \brief Initialize NDP cache.
/// \return        None.
extern void net_ndp_cache_init (void);

/// \brief De-initialize NDP cache.
/// \return        None.
extern void net_ndp_cache_uninit (void);

/// \brief Run NDP cache main function.
/// \return None.
extern void net_ndp_cache_run (void);

/// \brief Process Neighbor Discovery frame.
/// \param[in]     net_if        network interface descriptor.
/// \param[in]     frame         received ND frame.
/// \return        None.
extern void net_ndp_process (NET_IF_CFG *net_if, NET_FRAME *frame);

/// \brief Initialize NBNS client.
/// \return        None.
extern void net_nbns_client_init (void);

/// \brief De-initialize NBNS client.
/// \return        None.
extern void net_nbns_client_uninit (void);

/// \brief Run NBNS client main function.
/// \return        None.
extern void net_nbns_client_run (void);

/// \brief Initialize DHCP client.
/// \return        None.
extern void net_dhcp_client_init (void);

/// \brief De-initialize DHCP client.
/// \return        None.
extern void net_dhcp_client_uninit (void);

/// \brief Run DHCP client main function.
/// \return        None.
extern void net_dhcp_client_run (void);

/// \brief Initialize DHCP6 client.
/// \return        None.
extern void net_dhcp6_client_init (void);

/// \brief De-initialize DHCP6 client.
/// \return        None.
extern void net_dhcp6_client_uninit (void);

/// \brief Run DHCP6 client main function.
/// \return        None.
extern void net_dhcp6_client_run (void);

//  ==== WiFi interface ====

/// \brief WiFi interface thread.
/// \param[in]     arg           dummy parameter.
/// \return        None.
extern void netWiFi_Thread (void *arg);

/// \brief Initialize WiFi interface.
/// \return        None.
extern void net_wifi_iface_init (void);

/// \brief De-initialize WiFi interface.
/// \return        None.
extern void net_wifi_iface_uninit (void);

/// \brief Run WiFi interface main function.
/// \return None.
extern void net_wifi_iface_run (void);

/// \brief Construct ethernet header for the frame and send it.
/// \param[in]     if_num        Interface number.
/// \param[in]     frame         frame to be sent.
/// \param[in]     ip_ver        IP version of the frame.
/// \return
///              - true        = Frame successfuly sent.
///              - false       = Failed to send a frame.
extern bool net_wifi_send_frame (uint32_t if_num, NET_FRAME *frame, uint8_t ip_ver);

/// \brief Protected output of ethernet frame.
/// \param[in]     if_num        Interface number.
/// \param[in]     frame         network frame.
/// \return
///              - true  = output success.
///              - false = output failed.
extern bool net_wifi_output (uint32_t if_num, NET_FRAME *frame);

/// \brief Send event notification from wifi driver.
/// \param[in]     event         receive event.
/// \param[in]     ctx           interface context (wifi0, wifi1).
/// \return        None.
extern void net_wifi_callback (uint32_t event, struct net_wifi_ctrl *ctx);

/// \brief Get current value of WiFi Interface option.
/// \param[in]     if_num        Interface number.
/// \param[in]     option        Interface option as specified by \ref netIF_Option.
/// \param[out]    buf           buffer to store the option value to
/// \param[in]     buf_len       length of buffer
/// \return      status code that indicates the execution status of the function.
extern netStatus net_wifi_get_option (uint32_t if_num, netIF_Option option, uint8_t *buf, uint32_t buf_len);

/// \brief Set the value of WiFi Interface option.
/// \param[in]     if_num        Interface number.
/// \param[in]     option        Interface option as specified by \ref netIF_Option.
/// \param[in]     buf           buffer containing the option value
/// \param[in]     buf_len       length of buffer
/// \return      status code that indicates the execution status of the function.
extern netStatus net_wifi_set_option (uint32_t if_num, netIF_Option option, const uint8_t *buf, uint32_t buf_len);

//  ==== PPP interface ====

/// \brief PPP interface thread.
/// \param[in]     arg           dummy parameter.
/// \return        None.
extern void netPPP_Thread (void *arg);

/// \brief Initialize PPP interface.
/// \return        None.
extern void net_ppp_iface_init (void);

/// \brief De-initialize PPP interface.
/// \return        None.
extern void net_ppp_iface_uninit (void);

/// \brief Run PPP interface main function.
/// \return        None.
extern void net_ppp_iface_run (void);

/// \brief Construct PPP header for the frame and send it.
/// \param[in]     if_num        Interface number.
/// \param[in]     frame         frame to be sent.
/// \param[in]     ip_ver        IP version of the frame.
/// \return
///              - true        = Frame successfuly sent.
///              - false       = Failed to send a frame.
extern bool net_ppp_send_frame (uint32_t if_num, NET_FRAME *frame, uint8_t ip_ver);

/// \brief Get current value of PPP Interface option.
/// \param[in]     option        Interface option as specified by \ref netIF_Option.
/// \param[out]    buf           buffer to store the option value to
/// \param[in]     buf_len       length of buffer
/// \return      status code that indicates the execution status of the function.
extern netStatus net_ppp_get_option (netIF_Option option, uint8_t *buf, uint32_t buf_len);

/// \brief Set the value of PPP Interface option.
/// \param[in]     option        Interface option as specified by \ref netIF_Option.
/// \param[in]     buf           buffer containing the option value
/// \param[in]     buf_len       length of buffer
/// \return      status code that indicates the execution status of the function.
extern netStatus net_ppp_set_option (netIF_Option option, const uint8_t *buf, uint32_t buf_len);

//  ==== SLIP interface ====

/// \brief SLIP interface thread.
/// \param[in]     arg           dummy parameter.
/// \return        None.
extern void netSLIP_Thread (void *arg);

/// \brief Initialize SLIP interface.
/// \return        None.
extern void net_slip_iface_init (void);

/// \brief De-initialize SLIP interface.
/// \return        None.
extern void net_slip_iface_uninit (void);

/// \brief Run SLIP interface main function.
/// \return        None.
extern void net_slip_iface_run (void);

/// \brief Send a SLIP frame.
/// \param[in]     if_num        Interface number.
/// \param[in]     frame         frame to be sent.
/// \param[in]     ip_ver        IP version of the frame.
/// \return
///              - true        = Frame successfuly sent.
///              - false       = Failed to send a frame.
extern bool net_slip_send_frame (uint32_t if_num, NET_FRAME *frame, uint8_t ip_ver);

/// \brief Get current value of SLIP Interface option.
/// \param[in]     option        Interface option as specified by \ref netIF_Option.
/// \param[out]    buf           buffer to store the option value to
/// \param[in]     buf_len       length of buffer
/// \return      status code that indicates the execution status of the function.
extern netStatus net_slip_get_option (netIF_Option option, uint8_t *buf, uint32_t buf_len);

/// \brief Set the value of PPP Interface option.
/// \param[in]     option        Interface option as specified by \ref netIF_Option.
/// \param[in]     buf           buffer containing the option value
/// \param[in]     buf_len       length of buffer
/// \return      status code that indicates the execution status of the function.
extern netStatus net_slip_set_option (netIF_Option option, const uint8_t *buf, uint32_t buf_len);

//  ==== Serial and Modem interface ====

/// \brief Send event notification from the USART driver.
/// \param[in]     event         receive USART event.
/// \param[in]     ctx           serial device context (ppp, slip).
/// \return        None.
extern void net_com_callback (uint32_t event, struct net_com_ctrl *ctx);

/// \brief Request callback from the modem driver.
/// \param[in]     cmd           command to send to a modem driver.
/// \param[in]     response      expected response to wait for.
/// \param[in]     timeout       response timeout.
/// \param[in]     retries       number of retries on failure.
/// \param[in]     ctx           modem device context.
/// \return        None.
extern void net_modem_callback (const char *cmd, const char *response, uint32_t timeout,
                                uint32_t retries, struct net_modem_ctrl *ctx);

//  ==== Network core ====

/// \brief Initialize loopback interface.
/// \return        None.
extern void net_loop_iface_init (void);

/// \brief De-initialize loopback interface.
/// \return        None.
extern void net_loop_iface_uninit (void);

/// \brief Run loopback interface main function.
/// \return        None.
extern void net_loop_iface_run (void);

/// \brief Initialize IPv4 fragmentation and reassembly.
/// \return        None.
extern void net_ip4_frag_init (void);

/// \brief De-initialize IPv4 fragmentation and reassembly.
/// \return        None.
extern void net_ip4_frag_uninit (void);

/// \brief Run IPv4 reassembly main function.
/// \return        None.
extern void net_ip4_frag_run (void);

/// \brief Reassemble incoming IPv4 fragment.
/// \param[in]     frame         incoming IPv4 datagram fragment.
/// \return        pointer to assembled frame or NULL if not completed yet.
extern NET_FRAME *net_ip4_frag_add (NET_FRAME *frame);

/// \brief Split outgoing IPv4 datagram to small fragments.
/// \param[in]     frame         outgoing IPv4 datagram.
/// \param[in]     mtu           maximum transmission unit.
/// \return        pointer to generated frame fragment.
extern NET_FRAME *net_ip4_frag_get (NET_FRAME *frame, uint16_t mtu);

/// \brief Initialize IPv6 fragmentation and reassembly.
/// \return        None.
extern void net_ip6_frag_init (void);

/// \brief De-initialize IPv6 fragmentation and reassembly.
/// \return        None.
extern void net_ip6_frag_uninit (void);

/// \brief Run IPv6 reassembly main function.
/// \return        None.
extern void net_ip6_frag_run (void);

/// \brief Reassemble incoming IPv6 fragment.
/// \param[in]     frame         incoming IPv6 datagram fragment.
/// \return        pointer to assembled frame or NULL if not completed yet.
extern NET_FRAME *net_ip6_frag_add (NET_FRAME *frame);

/// \brief Split outgoing IPv6 datagram to small fragments.
/// \param[in]     frame         outgoing IPv6 datagram.
/// \param[in]     mtu           maximum transmission unit.
/// \return        pointer to generated frame fragment.
extern NET_FRAME *net_ip6_frag_get (NET_FRAME *frame, uint16_t mtu);

/// \brief Collect multicast MAC addresses of IPv6 core.
/// \param[in]     net_if        network interface descriptor.
/// \param[out]    buf           buffer to copy MAC addresses to.
/// \return        Number of MAC addresses copied.
extern uint32_t net_ip6_collect_mcast (NET_IF_CFG *net_if, uint8_t *buf);

/// \brief Initialize Ping client.
/// \return        None.
extern void net_ping_client_init (void);

/// \brief De-initialize Ping client.
/// \return        None.
extern void net_ping_client_uninit (void);

/// \brief Run Ping client main function.
/// \return        None.
extern void net_ping_client_run (void);

/// \brief Initialize UDP sockets.
/// \return        None.
extern void net_udp_socket_init (void);

/// \brief De-initialize UDP sockets.
/// \return        None.
extern void net_udp_socket_uninit (void);

/// \brief Process UDP frame.
/// \param[in]     net_if        network interface descriptor.
/// \param[in]     frame         received UDP frame.
/// \param[in]     ip_ver        received IP version.
/// \return        None.
extern void net_udp_process (NET_IF_CFG *net_if, NET_FRAME *frame, uint8_t ip_ver);

/// \brief Initialize TCP sockets.
/// \return        None.
extern void net_tcp_socket_init (void);

/// \brief De-initialize TCP sockets.
/// \return        None.
extern void net_tcp_socket_uninit (void);

/// \brief Run TCP socket main function.
/// \return        None.
extern void net_tcp_socket_run (void);

/// \brief Process TCP frame.
/// \param[in]     net_if        network interface descriptor.
/// \param[in]     frame         received TCP frame.
/// \param[in]     ip_ver        received IP version.
/// \return        None.
extern void net_tcp_process (NET_IF_CFG *net_if, NET_FRAME *frame, uint8_t ip_ver);

/// \brief Set or change TCP socket callback function.
/// \param[in]     socket        socket handle.
/// \param[in]     cb_func       event listening callback function.
/// \return        None.
extern void net_tcp_set_cb (int32_t socket, netTCP_cb_t cb_func);

/// \brief Retrieve TCP socket remote peer address.
/// \param[in]     socket        socket handle.
/// \return        pointer to network address structure.
extern NET_ADDR *net_tcp_get_peer_ptr (int32_t socket);

/// \brief Convert TCP socket state into a string.
/// \param[in]     state         socket state as defined with netTCP_State.
/// \return        pointer to the text representation of a socket state.
extern const char *net_tcp_ntoa (netTCP_State state);

/// \brief Initialize DNS client.
/// \return        None.
extern void net_dns_client_init (void);

/// \brief De-initialize DNS client.
/// \return        None.
extern void net_dns_client_uninit (void);

/// \brief Run DNS client main function.
/// \return        None.
extern void net_dns_client_run (void);

/// \brief Initialize BSD sockets.
/// \return        None.
extern void net_bsd_socket_init (void);

/// \brief De-initialize BSD sockets.
/// \return        None.
extern void net_bsd_socket_uninit (void);

/// \brief Run BSD socket main function.
/// \return        None.
extern void net_bsd_socket_run (void);

/// \brief Initialize BSD host resolver.
/// \return        None.
extern void net_bsd_host_init (void);

/// \brief De-initialize BSD host resolver.
/// \return        None.
extern void net_bsd_host_uninit (void);

//  ==== FTP server service ====

/// \brief Initialize FTP server.
/// \return        None.
extern void net_ftp_server_init (void);

/// \brief De-initialize FTP server.
/// \return        None.
extern void net_ftp_server_uninit (void);

/// \brief Run FTP server main function.
/// \return        None.
extern void net_ftp_server_run (void);

//  ==== FTP client application ====

/// \brief Initialize FTP client.
/// \return        None.
extern void net_ftp_client_init (void);

/// \brief De-initialize FTP client.
/// \return        None.
extern void net_ftp_client_uninit (void);

/// \brief Run FTP client main function.
/// \return        None.
extern void net_ftp_client_run (void);

//  ==== TFTP server service ====

/// \brief Initialize TFTP server.
/// \return        None.
extern void net_tftp_server_init (void);

/// \brief De-initialize TFTP server.
/// \return        None.
extern void net_tftp_server_uninit (void);

/// \brief Run TFTP server main function.
/// \return        None.
extern void net_tftp_server_run (void);

//  ==== TFTP client application ====

/// \brief Initialize TFTP client.
/// \return        None.
extern void net_tftp_client_init (void);

/// \brief De-initialize TFTP client.
/// \return        None.
extern void net_tftp_client_uninit (void);

/// \brief Run TFTP client main function.
/// \return        None.
extern void net_tftp_client_run (void);

//  ==== SMTP client application ====

/// \brief Initialize SMTP client.
/// \return        None.
extern void net_smtp_client_init (void);

/// \brief De-initialize SMTP client.
/// \return        None.
extern void net_smtp_client_uninit (void);

/// \brief Run SMTP client main function.
/// \return        None.
extern void net_smtp_client_run (void);

//  ==== SNMP agent application ====

/// \brief Initialize SNMP agent.
/// \return        None.
extern void net_snmp_agent_init (void);

/// \brief De-initialize SNMP agent.
/// \return        None.
extern void net_snmp_agent_uninit (void);

/// \brief Run SNMP agent main function.
/// \return        None.
extern void net_snmp_agent_run (void);

//  ==== SNTP client application ====

/// \brief Initialize SNTP client.
/// \return        None.
extern void net_sntp_client_init (void);

/// \brief De-initialize SNTP client.
/// \return        None.
extern void net_sntp_client_uninit (void);

/// \brief Run SNTP client main function.
/// \return        None.
extern void net_sntp_client_run (void);

//  ==== Telnet server service ====

/// \brief Initialize Telnet server.
/// \return        None.
extern void net_telnet_server_init (void);

/// \brief De-initialize Telnet server.
/// \return        None.
extern void net_telnet_server_uninit (void);

/// \brief Run Telnet server main function.
/// \return        None.
extern void net_telnet_server_run (void);

//  ==== HTTP server service ====

/// \brief Initialize HTTP server.
/// \return        None.
extern void net_http_server_init (void);

/// \brief De-initialize HTTP server.
/// \return        None.
extern void net_http_server_uninit (void);

/// \brief Run HTTP server main function.
/// \return        None.
extern void net_http_server_run (void);

/// \brief Convert generic time to internal HTTP time format.
/// \param[in]     hr            hours   [0..23].
/// \param[in]     min           minutes [0..59].
/// \param[in]     sec           seconds [0..59].
/// \param[in]     day           day     [1..31].
/// \param[in]     mon           month   [1..12].
/// \param[in]     year          year    [1980..2107].
/// \return        converted time in UTC format.
extern uint32_t net_http_time (uint8_t hr, uint8_t min, uint8_t sec,
                               uint8_t day, uint8_t mon, uint16_t year);

//  ==== TLS interface ====

/// \brief Initialize TLS interface.
/// \return        None.
extern void netTLS_InterfaceInit (void);

/// \brief De-initialize TLS interface.
/// \return        None.
extern void netTLS_InterfaceUninit (void);

/// \brief Allocate secure TLS context.
/// \param[in]     socket        associated TCP socket.
/// \param[in]     cb_func       event listening TCP callback function.
/// \return        Secure session identifier or 0=none.
extern uint8_t netTLS_GetContext (int32_t socket, netTCP_cb_t cb_func);

/// \brief Establish encrypted TLS connection to remote endpoint.
/// \param[in]     tls_id        TLS session id.
/// \param[in]     srv_name      hostname of the server.
/// \return        None.
extern void netTLS_Connect (uint8_t tls_id, const char *srv_name);

/// \brief Start TLS server listening for encrypted connection.
/// \param[in]     tls_id        TLS session id.
/// \return        None.
extern void netTLS_Listen (uint8_t tls_id);

/// \brief Allocate memory for TLS send buffer.
/// \param[in]     size          number of bytes to allocate.
/// \return        pointer to the allocated memory.
extern uint8_t *netTLS_GetBuffer (uint32_t size);

///  \brief Write data to TLS session.
///  \param[in]    tls_id        TLS context id.
///  \param[in]    buf           pointer to data buffer.
///  \param[in]    len           length of data (in bytes).
///  \return       None.
extern void netTLS_Write (uint8_t tls_id, const uint8_t *buf, uint32_t len);

/// \brief Stop TLS communication and close socket.
/// \param[in]     tls_id        TLS context id.
/// \param[in]     sock_mode     socket close mode: 1=normal, 0=abort.
/// \return        None.
extern void netTLS_Close (uint8_t tls_id, uint8_t sock_mode);

//  ==== Network address conversion ====

/// \brief Copy IP address and port.
/// \param[out]    dst_addr      destination IPv4 or IPv6 address.
/// \param[in]     src_addr      source IPv4 or IPv6 address.
/// \return        None.
extern void net_addr_copy (__ADDR *dst_addr, const __ADDR *src_addr);

/// \brief Convert IPv4 address from binary to string.
/// \param[in]     ip4_addr      IPv4 address in binary form.
/// \return        pointer to converted string.
extern const char *net_addr4_ntoa (const uint8_t *ip4_addr);

/// \brief Convert IPv4 address from text to binary form.
/// \param[in]     cp            IPv4 address string (ie. "192.168.1.100").
/// \param[out]    ip4_addr      4-byte address in binary form.
/// \return
///                - true  = successfull,
///                - false = failed.
extern bool net_addr4_aton (const char *cp, uint8_t *ip4_addr);

/// \brief Convert MAC address from binary to string.
/// \param[in]     mac_addr      MAC address in binary form.
/// \return        pointer to converted string.
extern const char *net_mac_ntoa (const uint8_t *mac_addr);

//  ==== Network debug ====

/// \brief Initialize STDIO debug interface.
/// \return        None.
extern void net_debug_init (void);

/// \brief Debug print information message.
/// \param[in]     proc          network process id.
/// \param[in]     fmt           printf format string.
/// \return        None.
extern void net_dbg_info (int32_t proc, const char *fmt, ...);

/// \brief Debug print error message.
/// \param[in]     proc          network process id.
/// \param[in]     fmt           printf format string.
/// \return        None.
extern void net_dbg_error (int32_t proc, const char *fmt, ...);

/// \brief Convert process id into a string.
/// \param[in]     proc          network process id.
/// \return        pointer to process id string.
extern const char *net_dbg_proc (int32_t proc);

/// \brief Get current network time for debug.
/// \return        pointer to current time string.
extern const char *net_dbg_time (void);

/// \brief Initialize Event Recorder debug interface.
/// \return        None.
extern void net_evr_init (void);

#ifdef __cplusplus
}
#endif

#endif /* __RL_NET_LIB_H */
