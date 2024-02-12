/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    rl_net.h
 * Purpose: Network API
 *----------------------------------------------------------------------------*/

#ifndef __RL_NET_H
#define __RL_NET_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "cmsis_compiler.h"
#include "RTE_Components.h"

#ifdef __cplusplus
extern "C"  {
#endif

/// Network to host byte order conversion.
#if defined(__ARM_BIG_ENDIAN)
  #define ntohl(v)              (uint32_t)(v)
  #define ntohs(v)              (uint16_t)(v)
#else
  #define ntohl(v)              (uint32_t)__REV(v)
  #define ntohs(v)              (uint16_t)__REV16(v)
#endif

/// Host to network byte order conversion.
#define htons(v)                ntohs(v)
#define htonl(v)                ntohl(v)

/// General definitions.
#define NET_ADDR_ETH_LEN        6       ///< Ethernet MAC Address Length in bytes
#define NET_ADDR_IP4_LEN        4       ///< IPv4 Address Length in bytes
#define NET_ADDR_IP6_LEN        16      ///< IPv6 Address Length in bytes
#define NET_HOSTNAME_LEN        16      ///< Hostname Buffer Length in bytes
#define NET_ROOT_DIR_LEN        80      ///< Service Root Folder Length in bytes

/// Network Address types.
#define NET_ADDR_ANY           (-1)     ///< IP address any
#define NET_ADDR_IP4            0       ///< IPv4 Address
#define NET_ADDR_IP6            1       ///< IPv6 Address

/// Network Address IPv4/IPv6 capable.
typedef struct net_addr {
  int16_t  addr_type;                   ///< IP address type: \ref NET_ADDR_IP4 or \ref NET_ADDR_IP6
  uint16_t port;                        ///< Internet socket port number
  uint8_t  addr[NET_ADDR_IP6_LEN];      ///< IPv4 or IPv6 address (array 16 bytes, MSB first)
} NET_ADDR;

/// Network Address IPv4 only.
typedef struct net_addr4 {
  int16_t  addr_type;                   ///< IP address type: \ref NET_ADDR_IP4
  uint16_t port;                        ///< Internet socket port number
  uint8_t  addr[NET_ADDR_IP4_LEN];      ///< IPv4 address (array 4 bytes, MSB first)
} NET_ADDR4;

/// Service Authentication definitions.
#define NET_USERNAME_LEN        16      ///< Username Buffer Length in bytes
#define NET_PASSWORD_LEN        16      ///< Password Buffer Length in bytes

/// Network Access definitions.
#define NET_ACCESS_FILE_READ            0x01  ///< File Read is allowed
#define NET_ACCESS_FILE_WRITE           0x02  ///< File Write is allowed
#define NET_ACCESS_DIRECTORY_CREATE     0x04  ///< Directory Create is allowed
#define NET_ACCESS_DIRECTORY_REMOVE     0x08  ///< Directory Remove is allowed
#define NET_ACCESS_DIRECTORY_LIST       0x10  ///< Directory List is allowed

/// Status code values returned by Network library functions.
typedef enum {
  netOK                       = 0,      ///< Operation succeeded
  netBusy,                              ///< Process is busy
  netError,                             ///< Unspecified error
  netInvalidParameter,                  ///< Invalid parameter specified
  netWrongState,                        ///< Wrong state error
  netDriverError,                       ///< Driver error
  netServerError,                       ///< Server error
  netAuthenticationFailed,              ///< User authentication failed
  netDnsResolverError,                  ///< DNS host resolver failed
  netFileError,                         ///< File not found or file r/w error
  netTimeout                            ///< Operation timeout
} netStatus;

/// UDP Checksum Options.
#define NET_UDP_CHECKSUM_SEND   0x01    ///< Calculate Checksum for UDP send frames (default)
#define NET_UDP_CHECKSUM_VERIFY 0x02    ///< Verify Checksum for received UDP frames (default)

/// UDP Socket Options.
typedef enum {
  netUDP_OptionTOS            = 0,      ///< IPv4 Type of Service; val=TOS
  netUDP_OptionTTL,                     ///< IPv4 Multi-cast Time to Live; val=TTL
  netUDP_OptionTrafficClass,            ///< IPv6 Traffic Class; val=TrafficClass
  netUDP_OptionHopLimit,                ///< IPv6 Multi-cast Hop Limit; val=HopLimit
  netUDP_OptionInterface,               ///< IPv4 Broadcast Interface; val=if_id (class and number)
  netUDP_OptionChecksum                 ///< UDP Checksum Options
} netUDP_Option;

/// UDP Event callback function.
typedef uint32_t (*netUDP_cb_t)(int32_t socket, const NET_ADDR *addr, const uint8_t *buf, uint32_t len);

/// TCP Socket Events.
typedef enum {
  netTCP_EventConnect         = 0,      ///< Connect request received event
  netTCP_EventEstablished,              ///< Connection established event
  netTCP_EventClosed,                   ///< Connection was properly closed
  netTCP_EventAborted,                  ///< Connection is for some reason aborted
  netTCP_EventACK,                      ///< Previously send data acknowledged
  netTCP_EventData                      ///< Data received event
} netTCP_Event;

/// TCP Socket States.
typedef enum {
  netTCP_StateINVALID         =-1,      ///< Invalid Socket
  netTCP_StateUNUSED          = 0,      ///< Entry is free and unused
  netTCP_StateCLOSED,                   ///< Entry allocated, socket still closed
  netTCP_StateLISTEN,                   ///< Socket waiting for incoming connection
  netTCP_StateSYN_RECEIVED,             ///< SYN frame received
  netTCP_StateSYN_SENT,                 ///< SYN packet sent to establish a connection
  netTCP_StateFIN_WAIT_1,               ///< Close started FIN packet was sent
  netTCP_StateFIN_WAIT_2,               ///< Our FIN ACK-ed, waiting for remote FIN
  netTCP_StateCLOSING,                  ///< Received FIN independently of our FIN
  netTCP_StateLAST_ACK,                 ///< Waiting for last ACK for our FIN
  netTCP_StateTIME_WAIT,                ///< Timed waiting for 2MSL
  netTCP_StateESTABLISHED               ///< TCP Connection established
} netTCP_State;

/// TCP Socket Options.
typedef enum {
  netTCP_OptionTOS            = 0,      ///< IPv4 Type of Service; val=TOS
  netTCP_OptionTrafficClass,            ///< IPv6 Traffic Class; val=TrafficClass
  netTCP_OptionTimeout,                 ///< TCP Idle Timeout; val=timeout (in seconds)
  netTCP_OptionKeepAlive,               ///< TCP Keep Alive; val: 0=disabled (default), 1=enabled
  netTCP_OptionFlowControl,             ///< TCP Flow Control; val: 0=disabled (default), 1=enabled
  netTCP_OptionDelayedACK               ///< TCP Delayed Acknowledgment; val: 0=disabled (default), 1=enabled
} netTCP_Option;

/// TCP Event callback function.
typedef uint32_t (*netTCP_cb_t)(int32_t socket, netTCP_Event event, const NET_ADDR *addr, const uint8_t *buf, uint32_t len);

#ifdef RTE_Network_Socket_BSD

//  ==== BSD Socket definitions ====

/// BSD Socket Address Family.
#define AF_UNSPEC               0       ///< Unspecified
#define AF_INET                 1       ///< Internet Address Family
#define AF_NETBIOS              2       ///< NetBios-style addresses
#define AF_INET6                3       ///< Internet Address Family version 6

/// BSD Protocol families (same as address families).
#define PF_UNSPEC               0       ///< Unspecified
#define PF_INET                 1       ///< Internet Address Family
#define PF_NETBIOS              2       ///< NetBios-style addresses
#define PF_INET6                3       ///< Internet Address Family version 6

/// BSD Socket Type.
#define SOCK_STREAM             1       ///< Stream Socket (Connection oriented)
#define SOCK_DGRAM              2       ///< Datagram Socket (Connectionless)

/// BSD Socket Protocol.
#define IPPROTO_TCP             1       ///< TCP Protocol
#define IPPROTO_UDP             2       ///< UDP Protocol

/// BSD Internet Addresses IPv4.
#define INADDR_ANY              0x00000000  ///< All IP addresses accepted
#define INADDR_NONE             0xffffffff  ///< No IP address accepted
#define INADDR_LOOPBACK         0x7f000001  ///< Localhost IP address

/// BSD Socket flags parameter.
#define MSG_DONTWAIT            0x01    ///< Enables non-blocking operation
#define MSG_PEEK                0x02    ///< Peeks at the incoming data
#define MSG_TRUNC               0x04    ///< Normal data was truncated
#define MSG_CTRUNC              0x08    ///< Control data was truncated

/// BSD Socket ioctl commands.
#define FIONBIO                 1       ///< Set mode (blocking/non-blocking)

/// BSD Socket level.
#define SOL_SOCKET              1       ///< Socket Level
#define IPPROTO_IP              2       ///< IPv4 Level
#define IPPROTO_IPV6            3       ///< IPv6 Level

/// BSD Socket options.
#define SO_KEEPALIVE            1       ///< Keep Alive
#define SO_RCVTIMEO             2       ///< Timeout for blocking receive (in milliseconds)
#define SO_SNDTIMEO             3       ///< Timeout for blocking send (in milliseconds)
#define SO_TYPE                 4       ///< Socket type (read only)

/// BSD Socket IPv4 options.
#define IP_TOS                  1       ///< Type of Service (TOS)
#define IP_TTL                  2       ///< Time to Live (TTL)
#define IP_RECVDSTADDR          3       ///< Receive destination IPv4 address

/// BSD Socket IPv6 options.
#define IPV6_TCLASS             1       ///< Traffic Class
#define IPV6_MULTICAST_HOPS     2       ///< Multi-cast Hop Limit
#define IPV6_RECVDSTADDR        3       ///< Receive destination IPv6 address
#define IPV6_V6ONLY             4       ///< Restrict to IPv6 communications only (default on)

/// BSD Socket Error codes.
#define BSD_ERROR               (-1)    ///< Unspecified error
#define BSD_ESOCK               (-2)    ///< Invalid socket descriptor
#define BSD_EINVAL              (-3)    ///< Invalid parameter
#define BSD_ENOTSUP             (-11)   ///< Operation or feature not supported
#define BSD_ENOMEM              (-5)    ///< Not enough memory
#define BSD_ELOCKED             (-7)    ///< Socket locked by another thread
#define BSD_EWOULDBLOCK         (-4)    ///< Operation would block
#define BSD_ETIMEDOUT           (-8)    ///< Operation timed out
#define BSD_EINPROGRESS         (-9)    ///< Operation in progress
#define BSD_ENOTCONN            (-6)    ///< Socket not connected
#define BSD_EISCONN             (-12)   ///< Socket is connected
#define BSD_ECONNREFUSED        (-13)   ///< Connection rejected by the peer
#define BSD_ECONNRESET          (-14)   ///< Connection reset by the peer
#define BSD_ECONNABORTED        (-15)   ///< Connection aborted locally
#define BSD_EALREADY            (-16)   ///< Connection already in progress
#define BSD_EADDRINUSE          (-17)   ///< Address already in use
#define BSD_EDESTADDRREQ        (-18)   ///< Destination address required
#define BSD_EHOSTNOTFOUND       (-10)   ///< Host not found
#define BSD_EMSGSIZE            (-19)   ///< Message too large

//  ==== BSD Socket structures ====

/// Generic Socket Address structure.
typedef struct sockaddr {
  uint16_t sa_family;                   ///< Address family
  int8_t   sa_data[14];                 ///< Direct address (up to 14 bytes)
} SOCKADDR;

#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
#endif

#ifndef __DOXYGEN__

/// Generic IPv4 Address structure.
typedef struct in_addr {
  union {
    struct {
      uint8_t s_b1,s_b2,s_b3,s_b4;      ///< IP address, byte access
    };
    struct {
      uint16_t s_w1,s_w2;               ///< IP address, short int access
    };
    uint32_t s_addr;                    ///< IP address in network byte order
  };
} IN_ADDR;

/// Generic IPv6 Address structure.
typedef struct in6_addr {
  union {
    uint8_t  s6_b[16];                  ///< IP6 address, byte access
    uint16_t s6_w[8];                   ///< IP6 address, short int access
  };
} IN6_ADDR;
#define s6_addr     s6_b

#endif

#if defined(__clang__)
  #pragma clang diagnostic pop
#endif

/// BSD Internet Addresses IPv6.
extern const IN6_ADDR in6addr_any;      ///< All IPv6 addresses accepted
extern const IN6_ADDR in6addr_loopback; ///< Localhost IPv6 address

/// IPv4 Socket Address structure.
typedef struct sockaddr_in {
  int16_t  sin_family;                  ///< Socket domain
  uint16_t sin_port;                    ///< Port
  IN_ADDR  sin_addr;                    ///< IP address
  int8_t   sin_zero[8];                 ///< reserved
} SOCKADDR_IN;

/// IPv6 Socket Address structure.
typedef struct sockaddr_in6 {
  int16_t  sin6_family;                 ///< Socket domain
  uint16_t sin6_port;                   ///< Port
  uint32_t sin6_flowinfo;               ///< IP6 flow information
  IN6_ADDR sin6_addr;                   ///< IP6 address
} SOCKADDR_IN6;

/// Socket Address storage structure.
typedef struct sockaddr_storage {
  int16_t ss_family;                    ///< Address family
  int8_t  __ss_pad1[2];                 ///< reserved
  int32_t __ss_align;                   ///< reserved, structure alignment
  int8_t  __ss_pad2[16];                ///< reserved
} SOCKADDR_STORAGE;

/// BSD Host Entry structure.
typedef struct hostent {
  char   *h_name;                       ///< Official name of host
  char  **h_aliases;                    ///< Pointer to an array of alias names
  int16_t h_addrtype;                   ///< Address Type: AF_INET, AF_NETBIOS
  int16_t h_length;                     ///< Length of address in bytes
  char  **h_addr_list;                  ///< Pointer to an array of IPv4 addresses
} HOSTENT;

/// BSD address string length.
#define INET_ADDRSTRLEN         16      ///< IP address string length
#define INET6_ADDRSTRLEN        46      ///< IP6 address string length

/// BSD fd_set size.
#define FD_SETSIZE              64      ///< Maximum number of sockets in fd_set structure

/// BSD fd_set structure.
typedef struct fd_set {
  uint32_t fd_bits[(FD_SETSIZE+31)>>5]; ///< Set of sockets bit-mask
} fd_set;

/// BSD timeval structure.
typedef struct timeval {
  uint32_t tv_sec;                      ///< Time interval: seconds
  uint32_t tv_usec;                     ///< Time interval: microseconds
} timeval;

/// BSD safe read/write fd_set macros.
#define FD_WR(fd,code)          if ((fd > 0) && (fd <= FD_SETSIZE)) { code; }
#define FD_RD(fd,code)          (((fd > 0) && (fd <= FD_SETSIZE)) ? (code) : 0)

/// BSD initialize and test fd_set macros.
#define FD_SET(fd,set)          FD_WR(fd, (set)->fd_bits[(fd-1)>>5] |=  (1U << ((fd-1)&0x1F)))
#define FD_CLR(fd,set)          FD_WR(fd, (set)->fd_bits[(fd-1)>>5] &= ~(1U << ((fd-1)&0x1F)))
#define FD_ISSET(fd,set)        FD_RD(fd, (set)->fd_bits[(fd-1)>>5] &   (1U << ((fd-1)&0x1F)))
#define FD_ZERO(set)            memset(set, 0, sizeof(*set))

/// BSD scatter/gather array of items.
typedef struct iovec {
  void    *iov_base;                    ///< Starting address
  uint32_t iov_len;                     ///< Number of bytes to transfer
} IOVEC;

/// BSD message header structure.
typedef struct msghdr {
  void    *msg_name;                    ///< Optional pointer to source address
  uint32_t msg_namelen;                 ///< Size of address buffer
  IOVEC   *msg_iov;                     ///< An array of iovec buffers for the message
  int32_t  msg_iovlen;                  ///< Number of elements in msg_iov
  void    *msg_control;                 ///< Ancillary data
  uint32_t msg_controllen;              ///< Ancillary data buffer length
  int32_t  msg_flags;                   ///< Flags on received message
} MSGHDR;

/// BSD cmsg header structure.
typedef struct cmsghdr {
  uint32_t cmsg_len;                    ///< Data byte count, including the cmsghdr
  int32_t  cmsg_level;                  ///< Originating protocol
  int32_t  cmsg_type;                   ///< Protocol-specific type
} CMSGHDR;

/// BSD access ancillary data macros (RFC 2292).
#define CMSG_FIRSTHDR(mhdr)     ((mhdr)->msg_controllen >= sizeof(CMSGHDR)) ? \
                                 (CMSGHDR *)(mhdr)->msg_control             : \
                                 (CMSGHDR *)NULL
#define CMSG_NXTHDR(mhdr,cmsg)  (CMSG_ALIGN((uint32_t)(cmsg) + (cmsg)->cmsg_len) + sizeof(CMSGHDR) > \
                                 (uint32_t)(mhdr)->msg_control + (mhdr)->msg_controllen)           ? \
                                 (CMSGHDR *)NULL                                                   : \
                                 (CMSGHDR *)CMSG_ALIGN((uint32_t)(cmsg) + (cmsg)->cmsg_len)
#define CMSG_DATA(cmsg)         ((uint8_t *)(cmsg) + sizeof(CMSGHDR))
#define CMSG_ALIGN(len)         (((len) + 3) & ~3U)
#define CMSG_LEN(len)           ((len) + sizeof(CMSGHDR))
#define CMSG_SPACE(len)          CMSG_ALIGN((len) + sizeof(CMSGHDR))

#endif /* RTE_Network_Socket_BSD */

/// Interface Class
#define NET_IF_CLASS_ETH        (1U << 8)   ///< Ethernet interface
#define NET_IF_CLASS_WIFI       (2U << 8)   ///< WiFi interface
#define NET_IF_CLASS_PPP        (3U << 8)   ///< PPP interface
#define NET_IF_CLASS_SLIP       (4U << 8)   ///< SLIP interface

/// Interface Option codes.
typedef enum {
  netIF_OptionMAC_Address,              ///< Ethernet MAC Address (6 bytes)
  netIF_OptionVLAN_Identifier,          ///< Ethernet VLAN Identifier (2 bytes)
  netIF_OptionIP4_MTU,                  ///< IPv4 Maximum Transmission Unit (2 bytes)
  netIF_OptionIP4_Address,              ///< IPv4 Address (4 bytes)
  netIF_OptionIP4_SubnetMask,           ///< IPv4 Subnet mask (4 bytes)
  netIF_OptionIP4_DefaultGateway,       ///< IPv4 Default Gateway (4 bytes)
  netIF_OptionIP4_PrimaryDNS,           ///< IPv4 Primary DNS (4 bytes)
  netIF_OptionIP4_SecondaryDNS,         ///< IPv4 Secondary DNS (4 bytes)
  netIF_OptionIP6_MTU,                  ///< IPv6 Maximum Transmission Unit (2 bytes)
  netIF_OptionIP6_LinkLocalAddress,     ///< IPv6 Link-local Address (16 bytes)
  netIF_OptionIP6_StaticAddress,        ///< IPv6 Static Address (16 bytes)
  netIF_OptionIP6_DynamicAddress,       ///< IPv6 Dynamic Address (16 bytes)
  netIF_OptionIP6_SubnetPrefixLength,   ///< IPv6 Subnet Prefix-length (1 byte)
  netIF_OptionIP6_DefaultGateway,       ///< IPv6 Default Gateway (16 bytes)
  netIF_OptionIP6_PrimaryDNS,           ///< IPv6 Primary DNS (16 bytes)
  netIF_OptionIP6_SecondaryDNS          ///< IPv6 Secondary DNS (16 bytes)
} netIF_Option;

/// Interface IP Versions.
typedef enum {
  netIF_VersionIP4,                     ///< IP version 4
  netIF_VersionIP6                      ///< IP version 6
} netIF_Version;

/// Ethernet link speed.
#define NET_ETH_SPEED_10M       0       ///< 10 Mbps link speed
#define NET_ETH_SPEED_100M      1       ///< 100 Mbps link speed
#define NET_ETH_SPEED_1G        2       ///< 1 Gpbs link speed

/// Ethernet duplex mode.
#define NET_ETH_DUPLEX_HALF     0       ///< Half duplex link
#define NET_ETH_DUPLEX_FULL     1       ///< Full duplex link

/// Ethernet link information.
typedef struct net_eth_link_info {
  uint32_t speed  : 2;                  ///< Link speed: 0= 10 MBit, 1= 100 MBit, 2= 1 GBit
  uint32_t duplex : 1;                  ///< Duplex mode: 0= Half, 1= Full
} NET_ETH_LINK_INFO;

/// Ethernet Callback Events.
typedef enum {
  netETH_LinkDown             = 0,      ///< Link down
  netETH_LinkUp,                        ///< Link up; val=link_info
  netETH_Wakeup,                        ///< Wake-up (on Magic Packet)
  netETH_TimerAlarm                     ///< Timer Alarm (PTP)
} netETH_Event;

/// WiFi Security Types.
typedef enum {
  netWiFi_SecurityOpen        = 0,      ///< Open
  netWiFi_SecurityWEP,                  ///< Wired Equivalent Privacy
  netWiFi_SecurityWPA,                  ///< WiFi Protected Access
  netWiFi_SecurityWPA2,                 ///< WiFi Protected Access 2
  netWiFi_SecurityUnknown     = 255     ///< Unknown security
} netWiFi_Security;

/// WiFi Driver Options.
typedef enum {
  netWiFi_OptionBSSID         = 1,      ///< BSSID of AP
  netWiFi_OptionTxPower,                ///< Transmit Power
  netWiFi_OptionLpTimer,                ///< Low Power deep-sleep timer
  netWiFi_OptionDTIM,                   ///< DTIM interval
  netWiFi_OptionBeacon                  ///< Beacon interval
} netWiFi_Option;

/// WiFi WPS Methods.
typedef enum {
  netWiFi_WPS_None            = 0,      ///< Not used
  netWiFi_WPS_PBC,                      ///< With Push Button Configuration
  netWiFi_WPS_PIN                       ///< With PIN
} netWiFi_WPS;

/// WiFi Configuration.
typedef struct net_wifi_config {
  const char      *ssid;                ///< Network name, a null-terminated string
  const char      *password;            ///< Password, a null-terminated string
  netWiFi_Security security;            ///< Security type
  uint8_t          channel;             ///< WiFi Channel (0=auto)
  uint8_t          reserved;            ///< Reserved
  netWiFi_WPS      wps_method;          ///< WiFi Protected Setup method
  const char      *wps_pin;             ///< WPS PIN, a null-terminated string
} NET_WIFI_CONFIG;

/// WiFi Network information.
typedef struct net_wifi_net_info {
  char             ssid[32+1];          ///< Network name, a null-terminated string
  char             password[64+1];      ///< Password, a null-terminated string
  netWiFi_Security security;            ///< Security type
  uint8_t          channel;             ///< WiFi Channel
  uint8_t          rssi;                ///< Received Signal Strength Indicator
} NET_WIFI_NET_INFO;

/// WiFi Scan information.
typedef struct net_wifi_scan_info {
  char             ssid[32+1];          ///< Service Set Identifier (null-terminated)
  uint8_t          bssid[6];            ///< Basic Service Set Identifier
  netWiFi_Security security;            ///< Security type
  uint8_t          channel;             ///< WiFi Channel
  uint8_t          rssi;                ///< Received Signal Strength Indicator
} NET_WIFI_SCAN_INFO;

/// ARP Cache Entry types.
typedef enum {
  netARP_CacheFixedIP,                  ///< Fixed IP address is refreshed after timeout
  netARP_CacheTemporaryIP               ///< Temporary IP address is removed after timeout
} netARP_CacheType;

/// DHCP Option Codes.
#define NET_DHCP_OPTION_IP_ADDRESS     0 ///< IP address change event
#define NET_DHCP_OPTION_NTP_SERVERS   42 ///< NTP Servers option
#define NET_DHCP_OPTION_CLIENT_ID     61 ///< Client-identifier option
#define NET_DHCP_OPTION_BOOTFILE_NAME 67 ///< Bootfile name option

/// DHCPv6 Option Codes.
#define NET_DHCP6_OPTION_IP_ADDRESS   0 ///< IPv6 address change event

/// DHCP Option Item.
typedef struct net_dhcp_option_item {
  uint8_t code;                         ///< Option type code
  uint8_t length;                       ///< Length of Option value
  uint8_t reserved[2];                  ///< Reserved
  uint8_t *value;                       ///< Pointer to Option value
} NET_DHCP_OPTION_ITEM;

/// DHCP Private Options.
extern NET_DHCP_OPTION_ITEM netDHCP_PrivateOptionsTableN[];  ///< DHCP Private Options Table
extern uint8_t              netDHCP_PrivateOptionsCountN;    ///< Number of DHCP Private Options

/// DHCPv6 Modes.
typedef enum {
  netDHCP6_ModeStateless      = 0,      ///< Stateless DHCPv6 mode
  netDHCP6_ModeStateful                 ///< Stateful DHCPv6 mode
} netDHCP6_Mode;

/// Ping Callback Events.
typedef enum {
  netPing_EventSuccess        = 0,      ///< Pinged Host responded
  netPing_EventTimeout                  ///< Timeout, no ping response received
} netPing_Event;

/// Ping Control Flags.
#define NET_PING_IP4_ONLY       0x01    ///< Force using IPv4 only
#define NET_PING_IP6_ONLY       0x02    ///< Force using IPv6 only

/// ARP Probe Callback Events.
typedef enum {
  netARP_EventSuccess         = 0,      ///< Probed Host responded
  netARP_EventTimeout                   ///< Timeout, no response to ARP probe
} netARP_Event;

/// NDP Probe Callback Events.
typedef enum {
  netNDP_EventSuccess         = 0,      ///< Probed Host responded
  netNDP_EventTimeout                   ///< Timeout, no response to NDP probe
} netNDP_Event;

/// DNS Client Callback Events.
typedef enum {
  netDNSc_EventSuccess        = 0,      ///< Host name successfully resolved
  netDNSc_EventTimeout,                 ///< Timeout resolving host
  netDNSc_EventNotResolved,             ///< DNS Error, no such name
  netDNSc_EventError                    ///< Erroneous response packet
} netDNSc_Event;

/// FTP Commands.
typedef enum {
  netFTP_CommandPUT,                    ///< Puts a file on FTP server
  netFTP_CommandGET,                    ///< Retrieves a file from FTP server
  netFTP_CommandAPPEND,                 ///< Append file on FTP server (with create)
  netFTP_CommandDELETE,                 ///< Deletes a file on FTP server
  netFTP_CommandLIST,                   ///< Lists files stored on FTP server
  netFTP_CommandRENAME,                 ///< Renames a file on FTP server
  netFTP_CommandMKDIR,                  ///< Makes a directory on FTP server
  netFTP_CommandRMDIR,                  ///< Removes an empty directory on FTP server
  netFTP_CommandNLIST                   ///< Lists file names only (short format)
} netFTP_Command;

/// FTP Server Events.
typedef enum {
  netFTPs_EventLogin,                   ///< User logged in, session is busy
  netFTPs_EventLogout,                  ///< User logged out, session is idle
  netFTPs_EventLoginFailed,             ///< User login failed (invalid credentials)
  netFTPs_EventDownload,                ///< File download ended
  netFTPs_EventUpload,                  ///< File upload ended
  netFTPs_EventDelete,                  ///< File deleted
  netFTPs_EventRename,                  ///< File or directory renamed
  netFTPs_EventMakeDirectory,           ///< Directory created
  netFTPs_EventRemoveDirectory,         ///< Directory removed
  netFTPs_EventOperationDenied,         ///< Requested file operation denied
  netFTPs_EventLocalFileError,          ///< Local file operation error
  netFTPs_EventFileError,               ///< Generic file operation error
  netFTPs_EventError                    ///< Generic FTP server error
} netFTPs_Event;

/// FTP Client Requests.
typedef enum {
  netFTPc_RequestUsername,              ///< Username to login to FTP server
  netFTPc_RequestPassword,              ///< Password to login to FTP server
  netFTPc_RequestDirectory,             ///< Working directory path on server for all commands
  netFTPc_RequestName,                  ///< File or Directory name for FTP commands
  netFTPc_RequestNewName,               ///< New File or Directory name for RENAME command
  netFTPc_RequestListMask,              ///< File filter/mask for LIST command (wildcards allowed)
  netFTPc_RequestList,                  ///< Received data if LIST command is given
  netFTPc_RequestLocalFilename          ///< Local filename (including path)
} netFTPc_Request;

/// FTP Client Events.
typedef enum {
  netFTPc_EventSuccess        = 0,      ///< File operation successful
  netFTPc_EventTimeout,                 ///< Timeout on file operation
  netFTPc_EventLoginFailed,             ///< Login error, username/password invalid
  netFTPc_EventAccessDenied,            ///< File access not allowed
  netFTPc_EventFileNotFound,            ///< File not found
  netFTPc_EventInvalidDirectory,        ///< Working directory path not found
  netFTPc_EventLocalFileError,          ///< Local file read/write error
  netFTPc_EventError                    ///< Generic FTP client error
} netFTPc_Event;

/// TFTP Client Events.
typedef enum {
  netTFTPc_EventSuccess       = 0,      ///< File operation successful
  netTFTPc_EventTimeout,                ///< Timeout on file operation
  netTFTPc_EventAccessDenied,           ///< File access not allowed
  netTFTPc_EventFileNotFound,           ///< File not found
  netTFTPc_EventDiskFull,               ///< Disk full
  netTFTPc_EventLocalFileError,         ///< Local file read/write error
  netTFTPc_EventError                   ///< Generic TFTP client error
} netTFTPc_Event;

/// Telnet Server Messages.
typedef enum {
  netTELNETs_MessageWelcome,            ///< Initial welcome message
  netTELNETs_MessageLogin,              ///< Login message, if authentication is enabled
  netTELNETs_MessageUsername,           ///< Username request login message
  netTELNETs_MessagePassword,           ///< Password request login message
  netTELNETs_MessageLoginFailed,        ///< Incorrect login error message
  netTELNETs_MessageLoginTimeout,       ///< Login timeout error message
  netTELNETs_MessagePrompt,             ///< Prompt message
  netTELNETs_MessageUnsolicited         ///< Unsolicited message (triggered by netTELNETs_RequestMessage)
} netTELNETs_Message;

/// SMTP Client Request.
typedef enum {
  netSMTPc_RequestUsername,             ///< Username to login to SMTP server
  netSMTPc_RequestPassword,             ///< Password to login to SMTP server
  netSMTPc_RequestSender,               ///< Email address of the sender
  netSMTPc_RequestRecipient,            ///< Email address of the recipient
  netSMTPc_RequestSubject,              ///< Subject of email
  netSMTPc_RequestBody                  ///< Email body in plain ASCII format
} netSMTPc_Request;

/// SMTP Client Events.
typedef enum {
  netSMTPc_EventSuccess       = 0,      ///< Email successfully sent
  netSMTPc_EventTimeout,                ///< Timeout sending email
  netSMTPc_EventAuthenticationFailed,   ///< Authentication failed, username/password invalid
  netSMTPc_EventError                   ///< Error when sending email
} netSMTPc_Event;

/// SMTP Mail Transfer Agent Flags.
#define NET_SMTP_MTA_USETLS     0x01    ///< Use secure TLS mode (Implicit TLS)

/// SMTP Email Descriptor.
typedef struct net_smtp_mail {
  const char *From;                     ///< Sender address, can be NULL
  const char *To;                       ///< Recipient(s), can be NULL
  const char *Cc;                       ///< Carbon copy recipient(s), can be NULL
  const char *Bcc;                      ///< Blind carbon copy recipient(s), can be NULL
  const char *Subject;                  ///< Subject of email, can be NULL
  const char *Message;                  ///< Email message body, can be NULL
  const char *Attachment;               ///< Email attachment(s), can be NULL
  const char *Encoding;                 ///< Default encoding type, can be NULL
} NET_SMTP_MAIL;

/// SMTP Mail Transfer Agent Descriptor.
typedef struct net_smtp_mta {
  const char *Address;                  ///< Server address (FQDN or IP address)
  uint16_t    Port;                     ///< Server port number, can be 0
  uint16_t    Flags;                    ///< Service control flags
  const char *Username;                 ///< Account user name, can be NULL
  const char *Password;                 ///< Account password, can be NULL
} NET_SMTP_MTA;

/// SNTP Client Mode.
typedef enum {
  netSNTPc_ModeUnicast        = 0,      ///< Unicast mode to access public NTP server
  netSNTPc_ModeBroadcast                ///< Broadcast mode for local LAN
} netSNTPc_Mode;

/// Ping Event callback function.
typedef void (*netPing_cb_t)(netPing_Event event);

/// ARP Probe Event callback function.
typedef void (*netARP_cb_t)(netARP_Event event);

/// NDP Probe Event callback function.
typedef void (*netNDP_cb_t)(netNDP_Event event);

/// DNS Client Event callback function.
typedef void (*netDNSc_cb_t)(netDNSc_Event event, const NET_ADDR *addr);

/// SNTP Client callback function.
typedef void (*netSNTPc_cb_t)(uint32_t seconds, uint32_t seconds_fraction);

/// SNMP-MIB definitions.
#define NET_SNMP_MIB_INTEGER    0x02    ///< MIB entry type INTEGER
#define NET_SNMP_MIB_OCTET_STR  0x04    ///< MIB entry type OCTET_STRING (null-terminated)
#define NET_SNMP_MIB_OBJECT_ID  0x06    ///< MIB entry type OBJECT_IDENTIFIER
#define NET_SNMP_MIB_BYTE_STR   0x07    ///< MIB entry type BYTE_STRING (length encoded)
#define NET_SNMP_MIB_IP_ADDR    0x40    ///< MIB entry type IP ADDRESS (uint8_t[4])
#define NET_SNMP_MIB_COUNTER    0x41    ///< MIB entry type COUNTER (uint32_t)
#define NET_SNMP_MIB_GAUGE      0x42    ///< MIB entry type GAUGE (uint32_t)
#define NET_SNMP_MIB_TIME_TICKS 0x43    ///< MIB entry type TIME_TICKS
#define NET_SNMP_MIB_ATR_RO     0x80    ///< MIB entry attribute READ_ONLY
#define NET_SNMP_MIB_OID_SIZE   17      ///< Max.size of Object ID value
#define NET_SNMP_MIB_STR_SIZE   110     ///< Max.size of Octet String variable
#define NET_SNMP_MIB_READ       0       ///< MIB entry Read access
#define NET_SNMP_MIB_WRITE      1       ///< MIB entry Write access

/// SNMP-MIB macros.
#define NET_SNMP_MIB_STR(s)     sizeof(s)-1, s
#define NET_SNMP_MIB_INT(o)     sizeof(o), (void *)&o
#define NET_SNMP_MIB_IP(ip)     4, (void *)&ip
#define NET_SNMP_MIB_OID0(f,s)  (f*40 + s)

/// SNMP-MIB Entry information.
typedef struct net_snmp_mib_info {
  uint8_t type;                         ///< Object Type
  uint8_t oid_len;                      ///< Object ID length
  uint8_t oid[NET_SNMP_MIB_OID_SIZE];   ///< Object ID value
  uint8_t var_size;                     ///< Size of a variable
  void    *var;                         ///< Pointer to a variable
  void    (*cb_func)(int32_t mode);     ///< Write/Read event callback function
} const NET_SNMP_MIB_INFO;

/// SNMP-MIB byte-string.
typedef struct net_snmp_byte_str {
  uint8_t len;                          ///< Length of a string
  uint8_t data[];                       ///< String content
} NET_SNMP_BYTE_STR;

#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wpadded"
#endif

/// FS Interface Time info
typedef struct net_fs_time {
  uint8_t  hr;                          ///< Hours    [0..23]
  uint8_t  min;                         ///< Minutes  [0..59]
  uint8_t  sec;                         ///< Seconds  [0..59]
  uint8_t  day;                         ///< Day      [1..31]
  uint8_t  mon;                         ///< Month    [1..12]
  uint16_t year;                        ///< Year     [1980..2107]
} NET_FS_TIME;

#if defined(__clang__)
  #pragma clang diagnostic pop
#endif

/// FS Interface Attributes
#define NET_FS_ATTR_FILE        1       ///< File entry
#define NET_FS_ATTR_DIRECTORY   2       ///< Directory entry


//  ==== Network System API ====

/// \brief Initialize Network Component and interfaces. [\ref not_thread-safe]
/// \return      status code that indicates the execution status of the function.
extern netStatus netInitialize (void);

/// \brief De-initialize Network Component and interfaces. [\ref not_thread-safe]
/// \return      status code that indicates the execution status of the function.
extern netStatus netUninitialize (void);

/// \brief Retrieve localhost name.  [\ref thread-safe]
/// \return        pointer to localhost name, a null-terminated string.
extern const char *netSYS_GetHostName (void);

/// \brief Set localhost name. [\ref thread-safe]
/// \param[in]     hostname      new localhost name, a null-terminated string.
/// \return      status code that indicates the execution status of the function.
extern netStatus netSYS_SetHostName (const char *hostname);

//  ==== UDP Socket API ====

/// \brief Allocate a free UDP socket. [\ref thread-safe]
/// \param[in]     cb_func       event listening callback function.
/// \return      socket handle number or execution status:
///              - value >= 0:   socket handle number.
///              - value < 0:    error occurred, -value is execution status as defined with \ref netStatus.
extern int32_t   netUDP_GetSocket (netUDP_cb_t cb_func);

/// \brief Release UDP socket and free resources. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netUDP_GetSocket.
/// \return      status code that indicates the execution status of the function.
extern netStatus netUDP_ReleaseSocket (int32_t socket);

/// \brief Open UDP socket for communication. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netUDP_GetSocket.
/// \param[in]     port          local port number.
///                              - 0 = system assigned local port.
/// \return      status code that indicates the execution status of the function.
extern netStatus netUDP_Open (int32_t socket, uint16_t port);

/// \brief Stop UDP communication and close socket. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netUDP_GetSocket.
/// \return      status code that indicates the execution status of the function.
extern netStatus netUDP_Close (int32_t socket);

/// \brief Allocate memory for UDP send buffer. [\ref thread-safe]
/// \param[in]     size          number of bytes to allocate.
/// \return        pointer to the allocated memory.
///                - NULL      = out of memory.
extern uint8_t  *netUDP_GetBuffer (uint32_t size);

/// \brief Send data to a remote node. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netUDP_GetSocket.
/// \param[in]     addr          structure containing remote IP address and port.
/// \param[in]     buf           buffer containing the data.
/// \param[in]     len           length of data in bytes.
/// \return      status code that indicates the execution status of the function.
extern netStatus netUDP_Send (int32_t socket, const NET_ADDR *addr, uint8_t *buf, uint32_t len);

/// \brief Set UDP socket IP option. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netUDP_GetSocket.
/// \param[in]     option        option name as defined with \ref netUDP_Option.
/// \param[in]     val           option value.
/// \return      status code that indicates the execution status of the function.
extern netStatus netUDP_SetOption (int32_t socket, netUDP_Option option, uint32_t val);

/// \brief Retrieve local port number of UDP socket. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netUDP_GetSocket.
/// \return        local port number.
///                - 0           = socket invalid or in invalid state.
extern uint16_t  netUDP_GetLocalPort (int32_t socket);

//  ==== TCP Socket API ====

/// \brief Allocate a free TCP socket. [\ref thread-safe]
/// \param[in]     cb_func       event listening callback function.
/// \return      socket handle number or execution status:
///              - value >= 0:   socket handle number.
///              - value < 0:    error occurred, -value is execution status as defined with \ref netStatus.
extern int32_t   netTCP_GetSocket (netTCP_cb_t cb_func);

/// \brief Release TCP socket and free resources. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netTCP_GetSocket.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTCP_ReleaseSocket (int32_t socket);

/// \brief Open TCP socket for incoming connection. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netTCP_GetSocket.
/// \param[in]     port          local port number.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTCP_Listen (int32_t socket, uint16_t port);

/// \brief Initiate a TCP connection to a remote node. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netTCP_GetSocket.
/// \param[in]     addr          structure containing remote IP address and port.
/// \param[in]     local_port    local port number.
///                              - 0 = system assigned local port.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTCP_Connect (int32_t socket, const NET_ADDR *addr, uint16_t local_port);

/// \brief Stop TCP communication and start closing procedure. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netTCP_GetSocket.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTCP_Close (int32_t socket);

/// \brief Instantly stop TCP communication. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netTCP_GetSocket.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTCP_Abort (int32_t socket);

/// \brief Determine maximum number of data bytes that can be sent in TCP packet. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netTCP_GetSocket.
/// \return        maximum segment size in bytes.
extern uint32_t  netTCP_GetMaxSegmentSize (int32_t socket);

/// \brief Allocate memory for TCP send buffer. [\ref thread-safe]
/// \param[in]     size          number of bytes to allocate.
/// \return        pointer to the allocated memory.
///                - NULL      = out of memory.
extern uint8_t  *netTCP_GetBuffer (uint32_t size);

/// \brief Check if TCP socket can send data. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netTCP_GetSocket.
/// \return      send status:
///              - true        = Ready to send data.
///              - false       = Not ready.
extern bool      netTCP_SendReady (int32_t socket);

/// \brief Send a data packet to remote node. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netTCP_GetSocket.
/// \param[in]     buf           buffer containing the data.
/// \param[in]     len           length of data in bytes.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTCP_Send (int32_t socket, uint8_t *buf, uint32_t len);

/// \brief Determine current state of a TCP socket. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netTCP_GetSocket.
/// \return      state information as defined with \ref netTCP_State.
extern netTCP_State netTCP_GetState (int32_t socket);

/// \brief Reset TCP window size to a default value from the configuration. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netTCP_GetSocket.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTCP_ResetReceiveWindow (int32_t socket);

/// \brief Set TCP socket IP option. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netTCP_GetSocket.
/// \param[in]     option        option name as defined with \ref netTCP_Option.
/// \param[in]     val           option value.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTCP_SetOption (int32_t socket, netTCP_Option option, uint32_t val);

/// \brief Retrieve local port number of TCP socket. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netTCP_GetSocket.
/// \return        local port number.
///                - 0         = socket invalid or in invalid state.
extern uint16_t  netTCP_GetLocalPort (int32_t socket);

/// \brief Retrieve IP address and port number of remote peer. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netTCP_GetSocket.
/// \param[out]    addr          structure that will receive IP address and port number.
/// \param[in]     addr_len      size of NET_ADDR structure for remote peer.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTCP_GetPeer (int32_t socket, NET_ADDR *addr, uint32_t addr_len);

/// \brief Determine TCP socket connection timeout. [\ref thread-safe]
/// \param[in]     socket        socket handle obtained with \ref netTCP_GetSocket.
/// \return        connection timeout timer in seconds.
///                - 0         = socket invalid or in invalid state.
extern uint32_t  netTCP_GetTimer (int32_t socket);

//  ==== BSD Socket API ====

#ifdef RTE_Network_Socket_BSD
/// \brief Create a communication endpoint called socket. [\ref thread-safe]
/// \param[in]     family        address family:
///                              - AF_INET        = address family IPv4.
///                              - AF_INET6       = address family IPv6.
/// \param[in]     type          connection type of a socket:
///                              - SOCK_STREAM    = connection based type.
///                              - SOCK_DGRAM     = datagram connectionless type.
/// \param[in]     protocol      protocol type:
///                              - IPPROTO_TCP    = must be used with SOCK_STREAM type.
///                              - IPPROTO_UDP    = must be used with SOCK_DGRAM TYPE.
///                              - 0              = for system auto-select.
/// \return      status information:
///              - Socket descriptor (>0).
///              - BSD_EINVAL          = Invalid parameter.
///              - BSD_ENOMEM          = No free sockets available.
extern int32_t socket (int32_t family, int32_t type, int32_t protocol);

/// \brief Assign a local address and port to a socket. [\ref thread-safe]
/// \param[in]     sock          socket descriptor obtained with \ref socket.
/// \param[in]     addr          structure containing local IP address and port.
/// \param[in]     addrlen       length of \ref SOCKADDR structure.
/// \return      status information:
///              - 0                   = Operation successful.
///              - BSD_ESOCK           = Invalid socket descriptor or socket not created.
///              - BSD_EINVAL          = Invalid parameter or already bound.
///              - BSD_EADDRINUSE      = Address or port already in use.
///              - BSD_EISCONN         = Socket already connected.
extern int32_t bind (int32_t sock, const SOCKADDR *addr, int32_t addrlen);

/// \brief Set a socket in a listening mode. [\ref thread-safe]
/// \param[in]     sock          socket descriptor obtained with \ref socket.
/// \param[in]     backlog       number of connection requests that can be accepted.
/// \return      status information:
///              - 0                   = Operation successful.
///              - BSD_ESOCK           = Invalid socket descriptor or socket not created.
///              - BSD_EINVAL          = Invalid parameter, socket not bound or already listening.
///              - BSD_ENOTSUP         = Operation not supported for this socket type.
///              - BSD_ERROR           = Failed to create socket backlog.
extern int32_t listen (int32_t sock, int32_t backlog);

/// \brief Accept connect request for a listening socket. [\ref thread-safe]
/// \param[in]     sock          socket descriptor obtained with \ref socket.
/// \param[out]    addr          structure that will receive IP address and port number.
///                              - NULL for none.
/// \param[in,out] addrlen       length of \ref SOCKADDR structure.
/// \return      status information:
///              - New socket descriptor (>0).
///              - BSD_ESOCK           = Invalid socket descriptor or socket not created.
///              - BSD_EINVAL          = Socket not in listen mode.
///              - BSD_ENOTSUP         = Operation not supported for this socket type.
///              - BSD_ELOCKED         = Socket locked by another thread.
///              - BSD_EWOULDBLOCK     = Operation would block.
///              - BSD_ECONNRESET      = Connection reset by the peer.
///              - BSD_ECONNABORTED    = Connection aborted locally.
///              - BSD_ERROR           = Unspecified error.
extern int32_t accept (int32_t sock, SOCKADDR *addr, int32_t *addrlen);

/// \brief Connect a socket to a remote host. [\ref thread-safe]
/// \param[in]     sock          socket descriptor obtained with \ref socket.
/// \param[in]     addr          structure containing remote IP address and port.
/// \param[in]     addrlen       length of \ref SOCKADDR structure.
/// \return      status information:
///              - 0                   = Operation successful.
///              - BSD_ESOCK           = Invalid socket descriptor or socket not created.
///              - BSD_EINVAL          = Invalid parameter or socket in wrong state.
///              - BSD_ELOCKED         = Socket locked by another thread.
///              - BSD_EALREADY        = Connection already in progress.
///              - BSD_EINPROGRESS     = Operation in progress.
///              - BSD_EISCONN         = Socket is connected.
///              - BSD_ECONNREFUSED    = Connection rejected by the peer.
///              - BSD_ETIMEDOUT       = Operation timed out.
///              - BSD_ECONNABORTED    = Connection aborted locally.
///              - BSD_ERROR           = Unspecified error.
extern int32_t connect (int32_t sock, const SOCKADDR *addr, int32_t addrlen);

/// \brief Send data on already connected socket. [\ref thread-safe]
/// \param[in]     sock          socket descriptor obtained with \ref socket.
/// \param[in]     buf           pointer to application data buffer to transmit.
/// \param[in]     len           length of data (in bytes).
/// \param[in]     flags         message flags:
///                              - MSG_DONTWAIT   = don't wait to send data.
///                              - 0              = for none.
/// \return      status information:
///              - Number of bytes sent (>0).
///              - BSD_ESOCK           = Invalid socket descriptor or socket not created.
///              - BSD_EINVAL          = Invalid parameter.
///              - BSD_ELOCKED         = Socket locked by another thread.
///              - BSD_ENOTCONN        = Socket not connected.
///              - BSD_ECONNRESET      = Connection reset by the peer.
///              - BSD_EWOULDBLOCK     = Operation would block.
///              - BSD_ETIMEDOUT       = Operation timed out.
///              - BSD_EDESTADDRREQ    = Destination address required.
///              - BSD_ECONNABORTED    = Connection aborted locally.
///              - BSD_EMSGSIZE        = Message too large.
///              - BSD_ENOMEM          = Not enough memory.
///              - BSD_ERROR           = Unspecified error.
extern int32_t send (int32_t sock, const char *buf, int32_t len, int32_t flags);

/// \brief Send data to endpoint node. [\ref thread-safe]
/// \param[in]     sock          socket descriptor obtained with \ref socket.
/// \param[in]     buf           pointer to application data buffer to transmit.
/// \param[in]     len           length of data (in bytes).
/// \param[in]     flags         message flags:
///                              - MSG_DONTWAIT   = don't wait to send data.
///                              - 0              = for none.
/// \param[in]     to            structure containing remote IP address and port.
/// \param[in]     tolen         length of \ref SOCKADDR structure.
/// \return      status information:
///              - Number of bytes sent (>0).
///              - BSD_ESOCK           = Invalid socket descriptor or socket not created.
///              - BSD_EINVAL          = Invalid parameter.
///              - BSD_ELOCKED         = Socket locked by another thread.
///              - BSD_ENOTCONN        = Socket not connected.
///              - BSD_ECONNRESET      = Connection reset by the peer.
///              - BSD_EWOULDBLOCK     = Operation would block.
///              - BSD_ETIMEDOUT       = Operation timed out.
///              - BSD_EDESTADDRREQ    = Destination address required.
///              - BSD_ECONNABORTED    = Connection aborted locally.
///              - BSD_EMSGSIZE        = Message too large.
///              - BSD_ENOMEM          = Not enough memory.
///              - BSD_ERROR           = Unspecified error.
extern int32_t sendto (int32_t sock, const char *buf, int32_t len, int32_t flags, const SOCKADDR *to, int32_t tolen);

/// \brief Send a message to endpoint node. [\ref thread-safe]
/// \param[in]     sock          socket descriptor obtained with \ref socket.
/// \param[in]     msg           pointer to \ref MSGHDR structure containing:
///                              - pointer to target address (NULL for none).
///                              - array of application buffer(s) containing the message.
///                              - pointer to the ancillary data (NULL for none).
/// \param[in]     flags         message flags:
///                              - 0              = for none.
/// \return      status information:
///              - Number of bytes sent (>0).
///              - BSD_ESOCK           = Invalid socket descriptor or socket not created.
///              - BSD_EINVAL          = Invalid parameter.
///              - BSD_ELOCKED         = Socket locked by another thread.
///              - BSD_ENOTSUP         = Operation not supported.
///              - BSD_EDESTADDRREQ    = Destination address required.
///              - BSD_EWOULDBLOCK     = Operation would block.
///              - BSD_ECONNABORTED    = Connection aborted locally.
///              - BSD_EMSGSIZE        = Message too large.
///              - BSD_ENOMEM          = Not enough memory.
///              - BSD_ERROR           = Unspecified error.
extern int32_t sendmsg (int32_t sock, const MSGHDR *msg, int32_t flags);

/// \brief Receive data on already connected socket. [\ref thread-safe]
/// \param[in]     sock          socket descriptor obtained with \ref socket.
/// \param[out]    buf           pointer to application data buffer to store the data to.
/// \param[in]     len           size of application data buffer (in bytes).
/// \param[in]     flags         message flags:
///                              - MSG_DONTWAIT   = don't wait for data.
///                              - MSG_PEEK       = peek at incoming data.
///                              - 0              = for none.
/// \return      status information:
///              - Number of bytes received (>0).
///              - BSD_ESOCK           = Invalid socket descriptor or socket not created.
///              - BSD_EINVAL          = Invalid parameter.
///              - BSD_ELOCKED         = Socket locked by another thread.
///              - BSD_ENOTCONN        = Socket not connected.
///              - BSD_ECONNRESET      = Connection reset by the peer.
///              - BSD_EWOULDBLOCK     = Operation would block.
///              - BSD_ETIMEDOUT       = Operation timed out.
///              - BSD_ECONNABORTED    = Connection aborted locally.
///              - BSD_ERROR           = Unspecified error.
extern int32_t recv (int32_t sock, char *buf, int32_t len, int32_t flags);

/// \brief Receive data from endpoint node. [\ref thread-safe]
/// \param[in]     sock          socket descriptor obtained with \ref socket.
/// \param[out]    buf           pointer to application data buffer to store the data to.
/// \param[in]     len           size of application data buffer (in bytes).
/// \param[in]     flags         message flags:
///                              - MSG_DONTWAIT   = don't wait for data.
///                              - MSG_PEEK       = peek at incoming data.
///                              - 0              = for none.
/// \param[out]    from          structure that will receive IP address and port number.
///                              - NULL for none.
/// \param[in,out] fromlen       length of \ref SOCKADDR structure.
/// \return      status information:
///              - Number of bytes received (>0).
///              - BSD_ESOCK           = Invalid socket descriptor or socket not created.
///              - BSD_EINVAL          = Invalid parameter.
///              - BSD_ELOCKED         = Socket locked by another thread.
///              - BSD_ENOTCONN        = Socket not connected.
///              - BSD_ECONNRESET      = Connection reset by the peer.
///              - BSD_EWOULDBLOCK     = Operation would block.
///              - BSD_ETIMEDOUT       = Operation timed out.
///              - BSD_ECONNABORTED    = Connection aborted locally.
///              - BSD_ERROR           = Unspecified error.
extern int32_t recvfrom (int32_t sock, char *buf, int32_t len, int32_t flags, SOCKADDR *from, int32_t *fromlen);

/// \brief Receive a message from a socket. [\ref thread-safe]
/// \param[in]     sock          socket descriptor obtained with \ref socket.
/// \param[in,out] msg           pointer to \ref MSGHDR structure containing:
///                              - pointer to buffer to store the source address to (NULL for none).
///                              - array of application buffer(s) for the incomming message.
///                              - pointer to buffer for the ancillary data (NULL for none).
/// \param[in]     flags         message flags:
///                              - MSG_DONTWAIT   = don't wait for data.
///                              - MSG_PEEK       = peek at incoming data.
///                              - 0              = for none.
/// \return      status information:
///              - Number of bytes received (>0).
///              - BSD_ESOCK           = Invalid socket descriptor or socket not created.
///              - BSD_EINVAL          = Invalid parameter.
///              - BSD_ELOCKED         = Socket locked by another thread.
///              - BSD_ENOTSUP         = Operation not supported.
///              - BSD_ENOTCONN        = Socket not connected.
///              - BSD_EWOULDBLOCK     = Operation would block.
///              - BSD_ETIMEDOUT       = Operation timed out.
///              - BSD_ECONNABORTED    = Connection aborted locally.
///              - BSD_ERROR           = Unspecified error.
extern int32_t recvmsg (int32_t sock, MSGHDR *msg, int32_t flags);

/// \brief Close socket and release socket descriptor. [\ref thread-safe]
/// \param[in]     sock          socket descriptor obtained with \ref socket.
/// \return      status information:
///              - 0                   = Operation successful.
///              - BSD_ESOCK           = Invalid socket descriptor or socket not created.
///              - BSD_EWOULDBLOCK     = Operation would block.
///              - BSD_ERROR           = Unspecified error.
extern int32_t closesocket (int32_t sock);

/// \brief Retrieve IP address and port number of the endpoint node. [\ref thread-safe]
/// \param[in]     sock          socket descriptor obtained with \ref socket.
/// \param[out]    name          structure that will receive IP address and port number.
/// \param[in,out] namelen       length of \ref SOCKADDR structure.
/// \return      status information:
///              - 0                   = Operation successful.
///              - BSD_ESOCK           = Invalid socket descriptor or socket not created.
///              - BSD_EINVAL          = Invalid parameter.
///              - BSD_ENOTCONN        = Socket not connected.
extern int32_t getpeername (int32_t sock, SOCKADDR *name, int32_t *namelen);

/// \brief Retrieve local IP address and port number. [\ref thread-safe]
/// \param[in]     sock          socket descriptor obtained with \ref socket.
/// \param[out]    name          structure that will receive IP address and port number.
/// \param[in,out] namelen       length of \ref SOCKADDR structure.
/// \return      status information:
///              - 0                   = Operation successful.
///              - BSD_ESOCK           = Invalid socket descriptor or socket not created.
///              - BSD_EINVAL          = Invalid parameter or socket not bound.
extern int32_t getsockname (int32_t sock, SOCKADDR *name, int32_t *namelen);

/// \brief Retrieve options for the socket.  [\ref thread-safe]
/// \param[in]     sock          socket descriptor obtained with \ref socket.
/// \param[in]     level         level at which the option is defined:
///                              - SOL_SOCKET   = Socket level.
///                              - IPPROTO_IP   = IPv4 protocol level.
///                              - IPPROTO_IPV6 = IPv6 protocol level.
/// \param[in]     optname       socket option for which the value is to be retrieved:
///                              - SO_TYPE             = Type of a socket.
///                              - SO_KEEPALIVE        = Keep Alive.
///                              - SO_RCVTIMEO         = Timeout for blocking receive (in ms).
///                              - SO_SNDTIMEO         = Timeout for blocking send (in ms).
///                              - IP_RECVDSTADDR      = Receive Destination IP Address.
///                              - IP_TOS              = Type of Service (TOS).
///                              - IP_TTL              = Time to Live (TTL).
///                              - IPV6_TCLASS         = Traffic Class.
///                              - IPV6_MULTICAST_HOPS = Multi-cast Hop Limit.
///                              - IPV6_RECVDSTADDR    = Receive Destination IPv6 Address.
///                              - IPV6_V6ONLY         = Restrict to IPv6 communications only.
/// \param[out]    optval        pointer to the buffer that will receive the option value.
/// \param[in,out] optlen        input length of buffer, return length of the data.
/// \return      status information:
///              - 0                   = Operation successful.
///              - BSD_ESOCK           = Invalid socket descriptor or socket not created.
///              - BSD_EINVAL          = Invalid parameter.
///              - BSD_ENOTSUP         = Option not supported for this socket type.
extern int32_t getsockopt (int32_t sock, int32_t level, int32_t optname, char *optval, int32_t *optlen);

/// \brief Manipulate options for the socket. [\ref thread-safe]
/// \param[in]     sock          socket descriptor obtained with \ref socket.
/// \param[in]     level         level at which the option is defined:
///                              - SOL_SOCKET   = Socket level.
///                              - IPPROTO_IP   = IPv4 protocol level.
///                              - IPPROTO_IPV6 = IPv6 protocol level.
/// \param[in]     optname       socket option for which the value is to be set:
///                              - SO_KEEPALIVE        = Keep Alive.
///                              - SO_RCVTIMEO         = Timeout for blocking receive (in ms).
///                              - SO_SNDTIMEO         = Timeout for blocking send (in ms).
///                              - IP_TOS              = Type of Service (TOS).
///                              - IP_TTL              = Time to Live (TTL).
///                              - IP_RECVDSTADDR      = Receive Destination IP Address.
///                              - IPV6_TCLASS         = Traffic Class.
///                              - IPV6_MULTICAST_HOPS = Multi-cast Hop Limit.
///                              - IPV6_RECVDSTADDR    = Receive Destination IPv6 Address.
///                              - IPV6_V6ONLY         = Restrict to IPv6 communications only.
/// \param[in]     optval        pointer to the buffer containing the option value.
/// \param[in]     optlen        size of the buffer containing the option value.
/// \return      status information:
///              - 0                   = Operation successful.
///              - BSD_ESOCK           = Invalid socket descriptor or socket not created.
///              - BSD_EINVAL          = Invalid parameter.
///              - BSD_ENOTSUP         = Option not supported for this socket type.
///              - BSD_ERROR           = Option failed, socket already bound.
extern int32_t setsockopt (int32_t sock, int32_t level, int32_t optname, const char *optval, int32_t optlen);

/// \brief Control IO mode of a socket. [\ref thread-safe]
/// \param[in]     sock          socket descriptor obtained with \ref socket.
/// \param[in]     cmd           command to perform:
///                              - FIONBIO  = enable non-blocking mode.
/// \param[in]     argp          command's parameter.
/// \return      status information:
///              - 0                   = Operation successful.
///              - BSD_ESOCK           = Invalid socket descriptor or socket not created.
///              - BSD_EINVAL          = Invalid parameter.
///              - BSD_ENOTSUP         = Option not supported for this socket type.
extern int32_t ioctlsocket (int32_t sock, long cmd, unsigned long *argp);

/// \brief Check the status of one or more sockets. [\ref thread-safe]
/// \param[in]     nfds          range of sockets to be checked.
/// \param[in,out] readfds       pointer to the set of sockets to check for read.
///                              - NULL for none.
/// \param[in,out] writefds      pointer to the set of sockets to check for write.
///                              - NULL for none.
/// \param[in,out] errorfds      pointer to the set of sockets to check for error.
///                              - NULL for none.
/// \param[in]     timeout       pointer to maximum time for select to wait.
///                              - NULL for blocking wait for event.
/// \return      status information:
///              - number of ready sockets (>0)
///              - 0                   = Operation timed out.
///              - BSD_EINVAL          = Invalid parameter.
///              - BSD_ERROR           = Suspend operation failed.
extern int32_t select (int32_t nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout);

/// \brief Retrieve host IP address from host name. [\ref thread-safe]
/// \param[in]     name          host name.
/// \param[out]    err           pointer to where to return error code (NULL for none):
///                              - 0                   = Operation successful.
///                              - BSD_EINVAL          = Invalid parameter.
///                              - BSD_ELOCKED         = Resolver locked by another thread.
///                              - BSD_ETIMEDOUT       = Operation timed out.
///                              - BSD_EHOSTNOTFOUND   = Host not found.
///                              - BSD_ERROR           = Unspecified error.
/// \return      status information:
///              - \ref HOSTENT result structure.
///              - NULL in case of error.
extern HOSTENT *gethostbyname (const char *name, int32_t *err);

/// \brief Convert from text address to a network address. [\ref thread-safe]
/// \param[in]     cp            text address in standard dotted-decimal notation.
/// \return      status information:
///              - Internet address on success.
///              - INADDR_NONE = on error.
extern IN_ADDR inet_addr (const char *cp);

/// \brief Convert from text address to a network address. [\ref thread-safe]
/// \param[in]     cp            text address in standard dotted-decimal notation.
/// \param[out]    addr          buffer where the converted IPv4 address is to be stored.
/// \return      status information:
///              - 1           = Conversion successful.
///              - 0           = Conversion failed.
extern int32_t inet_aton (const char *cp, IN_ADDR *addr);

/// \brief Convert from network address to a text string. [\ref not_thread-safe]
/// \param[in]     in            Internet IPv4 host address to convert.
/// \return      pointer to the formatted string.
extern const char *inet_ntoa (IN_ADDR in);

/// \brief Convert from text address to a binary network address. [\ref thread-safe]
/// \param[in]     af            address family:
///                              - AF_INET  = Internet Address Family (IPv4).
///                              - AF_INET6 = Internet Address Family version 6 (IPv6).
/// \param[in]     src           text address to be converted.
/// \param[out]    dst           buffer where the converted address is to be stored.
/// \return      status information:
///              - 1           = Conversion successful.
///              - 0           = Conversion failed.
extern int32_t inet_pton (int32_t af, const char *src, void *dst);

/// \brief Convert from binary network address to a text string. [\ref thread-safe]
/// \param[in]     af            address family:
///                              - AF_INET  = Internet Address Family (IPv4).
///                              - AF_INET6 = Internet Address Family version 6 (IPv6).
/// \param[in]     src           binary address in network byte order to be converted.
/// \param[out]    dst           buffer where the converted text address is to be stored.
/// \param[in]     size          size of the buffer, at least:
///                              - INET_ADDRSTRLEN for AF_INET.
///                              - INET6_ADDRSTRLEN for AF_INET6.
/// \return      pointer to the formatted string.
///              - NULL in case of error.
extern const char *inet_ntop (int32_t af, const void *src, char *dst, int32_t size);
#endif /* RTE_Network_Socket_BSD */

//  ==== Interface User API ====

/// \brief Get the current value of an Interface option. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     option        Interface option as specified by \ref netIF_Option.
/// \param[out]    buf           buffer to store the option value to.
/// \param[in]     buf_len       length of buffer.
/// \return      status code that indicates the execution status of the function.
extern netStatus netIF_GetOption (uint32_t if_id, netIF_Option option, uint8_t *buf, uint32_t buf_len);

/// \brief Set the value of an Interface option. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     option        Interface option as specified by \ref netIF_Option.
/// \param[in]     buf           buffer containing the option value.
/// \param[in]     buf_len       length of buffer.
/// \return      status code that indicates the execution status of the function.
extern netStatus netIF_SetOption (uint32_t if_id, netIF_Option option, const uint8_t *buf, uint32_t buf_len);

/// \brief Set default network interface for Internet access. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     ip_version    IP version as specified by \ref netIF_Version.
/// \return      status code that indicates the execution status of the function.
extern netStatus netIF_SetDefault (uint32_t if_id, netIF_Version ip_version);

/// \brief Enable or disable ICMP Echo response. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     no_echo       new state of NoEcho attribute:
///                              - true=  disable ICMP echo response,
///                              - false= enable ICMP echo response.
/// \return      status code that indicates the execution status of the function.
extern netStatus netICMP_SetNoEcho (uint32_t if_id, bool no_echo);

/// \brief Enable or disable ICMPv6 Echo response. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     no_echo       new state of NoEcho attribute:
///                              - true=  disable ICMPv6 echo response,
///                              - false= enable ICMPv6 echo response.
/// \return      status code that indicates the execution status of the function.
extern netStatus netICMP6_SetNoEcho (uint32_t if_id, bool no_echo);

//  ==== Ethernet Interface User API ====

/// \brief Send raw Ethernet data. [\ref thread-safe]
/// \param[in]     if_num        Ethernet interface number.
/// \param[in]     buf           buffer containing the data.
/// \param[in]     len           length of data in bytes.
/// \return      status code that indicates the execution status of the function.
extern netStatus netETH_SendRaw (uint32_t if_num, const uint8_t *buf, uint32_t len);

/// \brief Determine whether the ARP table has MAC address resolved for requested IP address. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     ip4_addr      requested IPv4 address.
/// \param[in]     type          address cache type.
/// \return      status code that indicates the execution status of the function.
extern netStatus netARP_CacheIP (uint32_t if_id, const uint8_t *ip4_addr, netARP_CacheType type);

/// \brief Determine whether the ARP table has IP address resolved for requested MAC address. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     mac_addr      requested MAC address.
/// \return      status code that indicates the execution status of the function.
extern netStatus netARP_CacheMAC (uint32_t if_id, const uint8_t *mac_addr);

/// \brief Get IP address from the ARP cache. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     mac_addr      requested MAC address.
/// \param[out]    ip4_addr      resolved IPv4 address.
/// \return      status code that indicates the execution status of the function.
extern netStatus netARP_GetIP (uint32_t if_id, const uint8_t *mac_addr, uint8_t *ip4_addr);

/// \brief Get MAC address from the ARP cache. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     ip4_addr      requested IPv4 address.
/// \param[out]    mac_addr      resolved MAC address.
/// \return      status code that indicates the execution status of the function.
extern netStatus netARP_GetMAC (uint32_t if_id, const uint8_t *ip4_addr, uint8_t *mac_addr);

/// \brief Determine whether the IP address is already in use. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     ip4_addr      requested IPv4 address.
/// \param[in]     cb_func       callback function to call, when probe session ends.
/// \return      status code that indicates the execution status of the function.
extern netStatus netARP_Probe (uint32_t if_id, const uint8_t *ip4_addr, netARP_cb_t cb_func);

/// \brief Determine whether the IP address is already in use in blocking mode. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     ip4_addr      requested IPv4 address.
/// \return      status code that indicates the execution status of the function.
extern netStatus netARP_ProbeX (uint32_t if_id, const uint8_t *ip4_addr);

/// \brief Flush or clear the local ARP cache. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \return      status code that indicates the execution status of the function.
extern netStatus netARP_ClearCache (uint32_t if_id);

/// \brief Determine whether neighbor cache has MAC address resolved for requested IP address. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     ip6_addr      requested IPv6 address.
/// \return      status code that indicates the execution status of the function.
extern netStatus netNDP_CacheIP (uint32_t if_id, const uint8_t *ip6_addr);

/// \brief Get IP address from neighbor discovery cache. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     mac_addr      requested MAC address.
/// \param[out]    ip6_addr      resolved IPv6 address.
/// \return      status code that indicates the execution status of the function.
extern netStatus netNDP_GetIP (uint32_t if_id, const uint8_t *mac_addr, uint8_t *ip6_addr);

/// \brief Get MAC address from neighbor discovery cache. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     ip6_addr      requested IPv6 address.
/// \param[out]    mac_addr      resolved MAC address.
/// \return      status code that indicates the execution status of the function.
extern netStatus netNDP_GetMAC (uint32_t if_id, const uint8_t *ip6_addr, uint8_t *mac_addr);

/// \brief Determine whether the IP address is already in use. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     ip6_addr      requested IPv6 address.
/// \param[in]     cb_func       callback function to call, when probe session ends.
/// \return      status code that indicates the execution status of the function.
extern netStatus netNDP_Probe (uint32_t if_id, const uint8_t *ip6_addr, netNDP_cb_t cb_func);

/// \brief Determine whether the IP address is already in use in blocking mode. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     ip6_addr      requested IPv6 address.
/// \return      status code that indicates the execution status of the function.
extern netStatus netNDP_ProbeX (uint32_t if_id, const uint8_t *ip6_addr);

/// \brief Flush or clear the local NDP cache. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \return      status code that indicates the execution status of the function.
extern netStatus netNDP_ClearCache (uint32_t if_id);

/// \brief Join this host to a host group specified with IP address. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     ip4_addr      group IPv4 address.
/// \return      status code that indicates the execution status of the function.
extern netStatus netIGMP_Join (uint32_t if_id, const uint8_t *ip4_addr);

/// \brief Leave a host group specified with IP address. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     ip4_addr      group IPv4 address.
/// \return      status code that indicates the execution status of the function.
extern netStatus netIGMP_Leave (uint32_t if_id, const uint8_t *ip4_addr);

/// \brief Join this node to a multicast group specified with IP address. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     ip6_addr      multicast IPv6 address.
/// \return      status code that indicates the execution status of the function.
extern netStatus netMLD_Join (uint32_t if_id, const uint8_t *ip6_addr);

/// \brief Leave a multicast group specified with IP address. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     ip6_addr      multicast IPv6 address.
/// \return      status code that indicates the execution status of the function.
extern netStatus netMLD_Leave (uint32_t if_id, const uint8_t *ip6_addr);

/// \brief Resolve IP address of a host from a NetBIOS hostname. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     name          NetBIOS hostname, a null-terminated string.
/// \param[out]    ip4_addr      resolved IPv4 address.
/// \return      status code that indicates the execution status of the function.
extern netStatus netNBNS_Resolve (uint32_t if_id, const char *name, uint8_t *ip4_addr);

/// \brief Flush or clear the local NBNS cache. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \return      status code that indicates the execution status of the function.
extern netStatus netNBNS_ClearCache (uint32_t if_id);

/// \brief Enable Dynamic Host Configuration at runtime. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \return      status code that indicates the execution status of the function.
extern netStatus netDHCP_Enable (uint32_t if_id);

/// \brief Disable Dynamic Host Configuration at runtime. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \return      status code that indicates the execution status of the function.
extern netStatus netDHCP_Disable (uint32_t if_id);

/// \brief Set DHCP Option value at runtime. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     option        DHCP option code.
/// \param[in]     val           pointer to option value.
/// \param[in]     len           length of option value in bytes.
/// \return      status code that indicates the execution status of the function.
extern netStatus netDHCP_SetOption (uint32_t if_id, uint8_t option, const uint8_t *val, uint32_t len);

/// \brief Enable Dynamic Host Configuration version 6 at runtime. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     mode          DHCPv6 operation mode.
/// \return      status code that indicates the execution status of the function.
extern netStatus netDHCP6_Enable (uint32_t if_id, netDHCP6_Mode mode);

/// \brief Disable Dynamic Host Configuration version 6 at runtime. [\ref thread-safe]
/// \param[in]     if_id         Interface identification (class and number).
/// \return      status code that indicates the execution status of the function.
extern netStatus netDHCP6_Disable (uint32_t if_id);

//  ==== Ethernet Interface Callbacks ====

/// \brief Notify the user of Ethernet link state change event. [\ref user-provided]
/// \param[in]     if_num        Ethernet interface number.
/// \param[in]     event         Ethernet link state event as defined in \ref netETH_Event.
/// \param[in]     val           pointer to the event value.
/// \return        none.
extern void      netETH_Notify (uint32_t if_num, netETH_Event event, uint32_t val);

/// \brief Receive raw Ethernet data. [\ref user-provided]
/// \param[in]     if_num        Ethernet interface number.
/// \param[in]     buf           buffer containing the received data.
/// \param[in]     len           length of received data in bytes.
/// \return        none.
extern void      netETH_ReceiveRaw (uint32_t if_num, const uint8_t *buf, uint32_t len);

/// \brief Notify the user of DHCP event or extended DHCP option. [\ref user-provided]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     option        DHCP option code.
/// \param[in]     val           pointer to option value.
/// \param[in]     len           length of option value in bytes.
/// \return        none.
extern void      netDHCP_Notify (uint32_t if_id, uint8_t option, const uint8_t *val, uint32_t len);

/// \brief Notify the user of DHCPv6 event or extended DHCPv6 option. [\ref user-provided]
/// \param[in]     if_id         Interface identification (class and number).
/// \param[in]     option        DHCPv6 option code.
/// \param[in]     val           pointer to option value.
/// \param[in]     len           length of option value in bytes.
/// \return        none.
extern void      netDHCP6_Notify (uint32_t if_id, uint8_t option, const uint8_t *val, uint32_t len);

//  ==== WiFi Interface User API ====

/// \brief Search for available WiFi networks. [\ref thread-safe]
/// \param[in]     if_num        WiFi interface number.
/// \param[out]    scan_info     array of structures for storing the scan information.
/// \param[in,out] scan_num      input maximum number, return number of WiFi networks found.
/// \return      status code that indicates the execution status of the function.
extern netStatus netWiFi_Scan (uint32_t if_num, NET_WIFI_SCAN_INFO scan_info[], uint32_t *scan_num);

/// \brief Get the value of the WiFi driver option. [\ref thread-safe]
/// \param[in]     if_num        WiFi interface number.
/// \param[in]     option        driver option as specified by \ref netWiFi_Option.
/// \param[out]    buf           buffer to store the option value to.
/// \param[in]     buf_len       length of buffer.
/// \return      status code that indicates the execution status of the function.
extern netStatus netWiFi_GetOption (uint32_t if_num, netWiFi_Option option, void *buf, uint32_t buf_len);

/// \brief Set the value of the WiFi driver option. [\ref thread-safe]
/// \param[in]     if_num        WiFi interface number.
/// \param[in]     option        driver option as specified by \ref netWiFi_Option.
/// \param[in]     buf           buffer containing the option value.
/// \param[in]     buf_len       length of buffer.
/// \return      status code that indicates the execution status of the function.
extern netStatus netWiFi_SetOption (uint32_t if_num, netWiFi_Option option, const void *buf, uint32_t buf_len);

/// \brief Activate the WiFi interface. [\ref thread-safe]
/// \param[in]     if_num        WiFi interface number.
/// \param[in]     config        pointer to the structure with configuration parameters.
/// \return      status code that indicates the execution status of the function.
extern netStatus netWiFi_Activate (uint32_t if_num, const NET_WIFI_CONFIG *config);

/// \brief Deactivate the WiFi interface. [\ref thread-safe]
/// \param[in]     if_num        WiFi interface number.
/// \return      status code that indicates the execution status of the function.
extern netStatus netWiFi_Deactivate (uint32_t if_num);

/// \brief Get the connection state of the WiFi interface. [\ref thread-safe]
/// \param[in]     if_num        WiFi interface number.
/// \return      connection state:
///               - true       = Station connected to access point.
///               - false      = Station not connected.
extern bool      netWiFi_IsConnected (uint32_t if_num);

/// \brief Get the network information of the WiFi interface. [\ref thread-safe]
/// \param[in]     if_num        WiFi interface number.
/// \param[out]    net_info      structure for storing the network information.
/// \return      status code that indicates the execution status of the function.
extern netStatus netWiFi_GetNetInfo (uint32_t if_num, NET_WIFI_NET_INFO *net_info);

//  ==== PPP Interface User API ====

/// \brief Start PPP interface to accept incoming PPP connection. [\ref thread-safe]
/// \param[in]     username      remote username for authentication.
/// \param[in]     password      remote password for authentication.
/// \return      status code that indicates the execution status of the function.
extern netStatus netPPP_Listen (const char *username, const char *password);

/// \brief Start a dial-up connection to remote PPP server. [\ref thread-safe]
/// \param[in]     dial_num      phone number of remote PPP server.
/// \param[in]     username      username for authentication.
/// \param[in]     password      password for authentication.
/// \return      status code that indicates the execution status of the function.
extern netStatus netPPP_Connect (const char *dial_num, const char *username, const char *password);

/// \brief Disconnect PPP link between two modems. [\ref thread-safe]
/// \return      status code that indicates the execution status of the function.
extern netStatus netPPP_Close (void);

/// \brief Determine the state of PPP link. [\ref thread-safe]
/// \return      link state:
///              - true        = Link is up, IP frames can be exchanged.
///              - false       = Link is down.
extern bool      netPPP_LinkUp (void);

//  ==== SLIP Interface User API ====

/// \brief Start SLIP interface to accept incoming SLIP connections. [\ref thread-safe]
/// \return      status code that indicates the execution status of the function.
extern netStatus netSLIP_Listen (void);

/// \brief Start a dial-up connection to remote SLIP server. [\ref thread-safe]
/// \param[in]     dial_num      phone number of remote SLIP server.
/// \return      status code that indicates the execution status of the function.
extern netStatus netSLIP_Connect (const char *dial_num);

/// \brief Disconnect SLIP link between two modems. [\ref thread-safe]
/// \return      status code that indicates the execution status of the function.
extern netStatus netSLIP_Close (void);

/// \brief Determine the state of SLIP link. [\ref thread-safe]
/// \return      link state:
///              - true        = Link is up, IP frames can be exchanged.
///              - false       = Link is down.
extern bool      netSLIP_LinkUp (void);

//  ==== Ping User API ====

/// \brief Start ICMP ping process. [\ref thread-safe]
/// \param[in]     addr          structure containing IP address of remote host.
/// \param[in]     cb_func       callback function to call, when ping session ends.
/// \return      status code that indicates the execution status of the function.
extern netStatus netPing_Echo (const NET_ADDR *addr, netPing_cb_t cb_func);

/// \brief Start ICMP ping process in blocking mode. [\ref thread-safe]
/// \param[in]     target        remote hostname or absolute IP address.
/// \param[in]     flags         ping process control flags.
/// \return      status code that indicates the execution status of the function.
extern netStatus netPing_EchoX (const char *target, uint32_t flags);

//  ==== DNS Client User API ====

/// \brief Resolve IP address of a host from a hostname. [\ref thread-safe]
/// \param[in]     name          hostname, a null-terminated string.
/// \param[in]     addr_type     network address type to resolve:
///                              - NET_ADDR_IP4 = IPv4 address.
///                              - NET_ADDR_IP6 = IPv6 address.
/// \param[in]     cb_func       callback function to call, when DNS session ends.
/// \return      status code that indicates the execution status of the function.
extern netStatus netDNSc_GetHostByName (const char *name, int16_t addr_type, netDNSc_cb_t cb_func);

/// \brief Resolve IP address of a host from a hostname in blocking mode. [\ref thread-safe]
/// \param[in]     name          hostname, a null-terminated string.
/// \param[in]     addr_type     network address type to resolve:
///                              - NET_ADDR_IP4 = IPv4 address.
///                              - NET_ADDR_IP6 = IPv6 address.
/// \param[out]    addr          structure that will receive resolved IP address of the hostname.
/// \return      status code that indicates the execution status of the function.
extern netStatus netDNSc_GetHostByNameX (const char *name, int16_t addr_type, NET_ADDR *addr);

/// \brief Flush or clear the local DNS cache. [\ref thread-safe]
/// \return      status code that indicates the execution status of the function.
extern netStatus netDNSc_ClearCache (void);

//  ==== FTP Server User API ====

/// \brief Start FTP server. [\ref thread-safe]
/// \return      status code that indicates the execution status of the function.
extern netStatus netFTPs_Start (void);

/// \brief Stop FTP server. [\ref thread-safe]
/// \return      status code that indicates the execution status of the function.
extern netStatus netFTPs_Stop (void);

/// \brief Check if FTP server is running. [\ref thread-safe]
/// \return
///              - true        = Server is running.
///              - false       = Server is not running.
extern bool      netFTPs_Running (void);

/// \brief Get port number of FTP server. [\ref thread-safe]
/// \return        port number.
extern uint16_t  netFTPs_GetPort (void);

/// \brief Set port number of FTP server. [\ref thread-safe]
/// \param[in]     port          port number.
/// \return      status code that indicates the execution status of the function.
extern netStatus netFTPs_SetPort (uint16_t port);

/// \brief Retrieve path to the root directory on FTP server. [\ref thread-safe]
/// \return        pointer to root path, a null-terminated string.
///                - NULL if root folder is disabled in the configuration.
extern const char *netFTPs_GetRootPath (void);

/// \brief Set path to the root directory on FTP server. [\ref thread-safe]
/// \param[in]     path          new root path, a null-terminated string.
/// \return      status code that indicates the execution status of the function.
extern netStatus netFTPs_SetRootPath (const char *path);

/// \brief Retrieve username of the built-in user account. [\ref thread-safe]
/// \return        pointer to username, a null-terminated string.
///                - NULL if authentication is disabled in the configuration.
extern const char *netFTPs_GetUsername (void);

/// \brief Set username of the built-in user account. [\ref thread-safe]
/// \param[in]     username      new username, a null-terminated string.
/// \return      status code that indicates the execution status of the function.
extern netStatus netFTPs_SetUsername (const char *username);

/// \brief Retrieve password of the built-in user account. [\ref thread-safe]
/// \return        pointer to password, a null-terminated string.
///                - NULL if authentication is disabled in the configuration.
extern const char *netFTPs_GetPassword (void);

/// \brief Reset password of the built-in user account. [\ref thread-safe]
/// \param[in]     password      new password, a null-terminated string.
/// \return      status code that indicates the execution status of the function.
extern netStatus netFTPs_SetPassword (const char *password);

/// \brief Determine if FTP server authentication is enabled. [\ref thread-safe]
/// \return
///              - true        = Authentication enabled in the configuration.
///              - false       = Authentication is not enabled.
extern bool      netFTPs_LoginActive (void);

/// \brief Enable or disable FTP server authentication. [\ref thread-safe]
/// \param[in]     login       new authentication state:
///                            - true  = Enable authentication.
///                            - false = Disable authentication.
/// \return      status code that indicates the execution status of the function.
extern netStatus netFTPs_LoginOnOff (bool login);

//  ==== FTP Server Access Interface ====

/// \brief Accept or deny connection from remote FTP client. [\ref user-provided]
/// \param[in]     addr          structure containing IP address and port of remote FTP client.
/// \return
///              - true        = Connection from the remote client is allowed.
///              - false       = Connection is denied.
extern bool      netFTPs_AcceptClient (const NET_ADDR *addr);

//  ==== FTP Server Multi-User Interface ====

/// \brief Check if an user account exists in the user database. [\ref user-provided]
/// \param[in]     username      pointer to username.
/// \return      status information:
///              - User identification number.
///              - 0 if the user is not existing.
extern uint8_t   netFTPs_CheckUsername (const char *username);

/// \brief Check user account password in the user database. [\ref user-provided]
/// \param[in]     user_id       user identification number.
/// \param[in]     password      pointer to password.
/// \return
///              - true        = password accepted.
///              - false       = invalid password.
extern bool      netFTPs_CheckPassword (uint8_t user_id, const char *password);

/// \brief Check if remote user is allowed to access a file on FTP server. [\ref user-provided]
/// \param[in]     user_id       user identification number.
/// \param[in]     fname         full path of a file to access.
/// \param[in]     access        access mode as defined with Network Access definitions.
/// \return
///              - true        = File access is allowed.
///              - false       = File access is denied.
extern bool      netFTPs_FileAccess (uint8_t user_id, const char *fname, uint32_t access);

/// \brief Retrieve the user identification number. [\ref thread-safe]
/// \return        user identification number (0 = system administrator).
extern uint8_t   netFTPs_GetUserId (void);

//  ==== FTP Server User Callbacks ====

/// \brief Notify the user application about events in FTP server service. [\ref user-provided]
/// \param[in]     event         FTP Server notification event as specified in \ref netFTPs_Event.
/// \return        none.
/// \note Network library calls this function to inform the user about events.
extern void      netFTPs_Notify (netFTPs_Event event);

//  ==== FTP Server File System Interface ====

/// \brief Open a file for reading or writing on FTP server. [\ref interface]
/// \param[in]     fname         name of the file to open.
/// \param[in]     mode          type of access:
///                              - "rb" = opens a file for reading.
///                              - "wb" = opens a file for writing.
/// \return      status information:
///              - Pointer to an open file.
///              - NULL in case of an error.
extern void     *netFTPs_fopen (const char *fname, const char *mode);

/// \brief Close a file previously open on FTP server. [\ref interface]
/// \param[in]     file          pointer to the file to close.
/// \return        none.
extern void      netFTPs_fclose (void *file);

/// \brief Read block of data from a file on FTP server. [\ref interface]
/// \param[in]     file          pointer to the file to read from.
/// \param[out]    buf           block of memory to write data to.
/// \param[in]     len           length of data to read in bytes.
/// \return        number of bytes successfully read.
extern uint32_t  netFTPs_fread (void *file, uint8_t *buf, uint32_t len);

/// \brief Write block of data to a file on FTP server. [\ref interface]
/// \param[in]     file          pointer to the file to write to.
/// \param[in]     buf           block of memory to be written.
/// \param[in]     len           length of data to write in bytes.
/// \return        number of bytes successfully written.
extern uint32_t  netFTPs_fwrite (void *file, const uint8_t *buf, uint32_t len);

/// \brief Delete a file on FTP server. [\ref interface]
/// \param[in]     fname         name of the file to delete.
/// \return
///              - true        = File successfully deleted.
///              - false       = Failed to delete a file.
extern bool      netFTPs_fdelete (const char *fname);

/// \brief Rename a file or directory on FTP server. [\ref interface]
/// \param[in]     fname         old name to rename from.
/// \param[in]     newname       new name to rename to.
/// \return
///              - true        = File or directory successfully renamed.
///              - false       = Failed to rename a file or directory.
extern bool      netFTPs_frename (const char *fname, const char *newname);

/// \brief Make a new directory on FTP server. [\ref interface]
/// \param[in]     path          directory path to create.
/// \return
///              - true        = Directory successfully created.
///              - false       = Failed to create a directory.
extern bool      netFTPs_mkdir (const char *path);

/// \brief Remove an empty directory on FTP server. [\ref interface]
/// \param[in]     path          directory path to remove.
/// \return
///              - true        = Directory successfully removed.
///              - false       = Failed to remove a directory.
extern bool      netFTPs_rmdir (const char *path);

/// \brief Check that the directory exists on FTP server. [\ref interface]
/// \param[in]     path          directory path to check.
/// \return
///              - true        = Directory exists.
///              - false       = Directory not found.

extern bool      netFTPs_chdir (const char *path);

/// \brief Search the file system directory for matching files. [\ref interface]
/// \param[in]     mask          file mask filter.
/// \param[out]    fname         buffer to write filename to.
///                              - NULL for none.
/// \param[out]    fsize         pointer to where to return the file size.
///                              - NULL for none.
/// \param[out]    ftime         pointer to where to return the created or last modified time.
///                              - NULL for none.
/// \param[in]     first         find first file.
/// \return      status information:
///              - NET_FS_ATTR_FILE      = File found.
///              - NET_FS_ATTR_DIRECTORY = Directory found.
///              - 0                     = No entry found.
extern int32_t   netFTPs_ffind (const char *mask, char *fname, uint32_t *fsize, NET_FS_TIME *ftime, bool first);

//  ==== FTP Client User API ====

/// \brief Start FTP client file operation session. [\ref thread-safe]
/// \param[in]     addr          structure containing IP address and port of remote FTP server.
/// \param[in]     command       FTP command to perform.
/// \return      status code that indicates the execution status of the function.
extern netStatus netFTPc_Connect (const NET_ADDR *addr, netFTP_Command command);

//  ==== FTP Client User Callbacks ====

/// \brief Request parameters for FTP client session. [\ref user-provided]
/// \param[in]     request       request code.
/// \param[out]    buf           output buffer to write the data to.
/// \param[in]     buf_len       length of the output buffer in bytes.
/// \return        number of bytes written to output buffer.
extern uint32_t  netFTPc_Process (netFTPc_Request request, char *buf, uint32_t buf_len);

/// \brief Notify the user application when FTP client operation ends. [\ref user-provided]
/// \param[in]     event         FTP client notification event as specified in \ref netFTPc_Event.
/// \return        none.
/// \note Network library calls this function to inform the user about events.
extern void      netFTPc_Notify (netFTPc_Event event);

//  ==== FTP Client File System Interface ====

/// \brief Open local file for reading or writing in FTP client. [\ref interface]
/// \param[in]     fname         name of the file to open.
/// \param[in]     mode          type of access:
///                              - "rb" = opens a file for reading.
///                              - "wb" = opens a file for writing.
/// \return      status information:
///              - Pointer to an open file.
///              - NULL in case of an error.
extern void     *netFTPc_fopen (const char *fname, const char *mode);

/// \brief Close local file previously open in FTP client. [\ref interface]
/// \param[in]     file          pointer to the file to close.
/// \return        none.
extern void      netFTPc_fclose (void *file);

/// \brief Read block of data from local file in FTP client. [\ref interface]
/// \param[in]     file          pointer to the file to read from.
/// \param[out]    buf           block of memory to write data to.
/// \param[in]     len           length of data to read in bytes.
/// \return        number of bytes successfully read.
extern uint32_t  netFTPc_fread (void *file, uint8_t *buf, uint32_t len);

/// \brief Write block of data to local file in FTP client. [\ref interface]
/// \param[in]     file          pointer to the file to write to.
/// \param[in]     buf           block of memory to be written.
/// \param[in]     len           length of data to write in bytes.
/// \return        number of bytes successfully written.
extern uint32_t  netFTPc_fwrite (void *file, const uint8_t *buf, uint32_t len);

//  ==== TFTP Server User API ====

/// \brief Start the TFTP server. [\ref thread-safe]
/// \return      status code that indicates the execution status of the function.
extern netStatus netTFTPs_Start (void);

/// \brief Stop the TFTP server. [\ref thread-safe]
/// \return      status code that indicates the execution status of the function.
extern netStatus netTFTPs_Stop (void);

/// \brief Check if the TFTP server is running. [\ref thread-safe]
/// \return
///              - true        = Server is running.
///              - false       = Server is not running.
extern bool      netTFTPs_Running (void);

/// \brief Get port number of the TFTP server. [\ref thread-safe]
/// \return        port number.
extern uint16_t  netTFTPs_GetPort (void);

/// \brief Set port number of the TFTP server. [\ref thread-safe]
/// \param[in]     port          port number.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTFTPs_SetPort (uint16_t port);

/// \brief Retrieve path to the root directory on TFTP server. [\ref thread-safe]
/// \return        pointer to root path, a null-terminated string.
///                - NULL if root folder is disabled in the configuration.
extern const char *netTFTPs_GetRootPath (void);

/// \brief Set path to the root directory on TFTP server. [\ref thread-safe]
/// \param[in]     path          new root path, a null-terminated string.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTFTPs_SetRootPath (const char *path);

//  ==== TFTP Server Access Interface ====

/// \brief Accept or deny connection from a remote TFTP client. [\ref user-provided]
/// \param[in]     addr          structure containing IP address and port of remote TFTP client.
/// \return
///              - true        = Connection from the remote client is allowed.
///              - false       = Connection is denied.
extern bool      netTFTPs_AcceptClient (const NET_ADDR *addr);

//  ==== TFTP Server File System Interface ====

/// \brief Open a file for reading or writing on the TFTP server. [\ref interface]
/// \param[in]     fname         name of the file to open.
/// \param[in]     mode          type of access:
///                              - "rb" = opens a file for reading.
///                              - "wb" = opens a file for writing.
/// \return      status information:
///              - Pointer to an open file.
///              - NULL in case of an error.
extern void     *netTFTPs_fopen (const char *fname, const char *mode);

/// \brief Close a file previously open on the TFTP server. [\ref interface]
/// \param[in]     file          pointer to the file to close.
/// \return        none.
extern void      netTFTPs_fclose (void *file);

/// \brief Read block of data from a file on the TFTP server. [\ref interface]
/// \param[in]     file          pointer to the file to read from.
/// \param[out]    buf           block of memory to write data to.
/// \param[in]     len           length of data to read in bytes.
/// \return        number of bytes successfully read.
extern uint32_t  netTFTPs_fread (void *file, uint8_t *buf, uint32_t len);

/// \brief Write block of data to a file on the TFTP server. [\ref interface]
/// \param[in]     file          pointer to the file to write to.
/// \param[in]     buf           block of memory to be written.
/// \param[in]     len           length of data to write in bytes.
/// \return        number of bytes successfully written.
extern uint32_t  netTFTPs_fwrite (void *file, const uint8_t *buf, uint32_t len);

//  ==== TFTP Client User API ====

/// \brief Put a file to a remote TFTP server. [\ref thread-safe]
/// \param[in]     addr          structure containing IP address and port of remote TFTP server.
/// \param[in]     fname         pointer to the remote file name, a null-terminated string.
/// \param[in]     local_fname   pointer to the local file name, a null-terminated string.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTFTPc_Put (const NET_ADDR *addr, const char *fname, const char *local_fname);

/// \brief Retrieve a file from a remote TFTP server. [\ref thread-safe]
/// \param[in]     addr          structure containing IP address and port of remote TFTP server.
/// \param[in]     fname         pointer to the remote file name, a null-terminated string.
/// \param[in]     local_fname   pointer to the local file name, a null-terminated string.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTFTPc_Get (const NET_ADDR *addr, const char *fname, const char *local_fname);

//  ==== TFTP Client User Callbacks ====

/// \brief Notify the user application when TFTP client operation ends. [\ref user-provided]
/// \param[in]     event         TFTP client notification event as specified in \ref netTFTPc_Event.
/// \return        none.
/// \note Network library calls this function to inform the user about events.
extern void      netTFTPc_Notify (netTFTPc_Event event);

//  ==== TFTP Client File System Interface ====

/// \brief Open local file for reading or writing in the TFTP client. [\ref interface]
/// \param[in]     fname         name of the file to open.
/// \param[in]     mode          type of access:
///                              - "rb" = opens a file for reading.
///                              - "wb" = opens a file for writing.
/// \return      status information:
///              - Pointer to an open file.
///              - NULL in case of an error.
extern void     *netTFTPc_fopen (const char *fname, const char *mode);

/// \brief Close local file previously open in the TFTP client. [\ref interface]
/// \param[in]     file          pointer to the file to close.
/// \return        none.
extern void      netTFTPc_fclose (void *file);

/// \brief Read block of data from local file in the TFTP client. [\ref interface]
/// \param[in]     file          pointer to the file to read from.
/// \param[out]    buf           block of memory to write data to.
/// \param[in]     len           length of data to read in bytes.
/// \return        number of bytes successfully read.
extern uint32_t  netTFTPc_fread (void *file, uint8_t *buf, uint32_t len);

/// \brief Write block of data to local file in the TFTP client. [\ref interface]
/// \param[in]     file          pointer to the file to write to.
/// \param[in]     buf           block of memory to be written.
/// \param[in]     len           length of data to write in bytes.
/// \return        number of bytes successfully written.
extern uint32_t  netTFTPc_fwrite (void *file, const uint8_t *buf, uint32_t len);

//  ==== Telnet Server User API ====

/// \brief Start the Telnet server. [\ref thread-safe]
/// \return      status code that indicates the execution status of the function.
extern netStatus netTELNETs_Start (void);

/// \brief Stop the Telnet server. [\ref thread-safe]
/// \return      status code that indicates the execution status of the function.
extern netStatus netTELNETs_Stop (void);

/// \brief Check if the Telnet server is running. [\ref thread-safe]
/// \return
///              - true        = Server is running.
///              - false       = Server is not running.
extern bool      netTELNETs_Running (void);

/// \brief Get port number of the Telnet server. [\ref thread-safe]
/// \return        port number.
extern uint16_t  netTELNETs_GetPort (void);

/// \brief Set port number of the Telnet server. [\ref thread-safe]
/// \param[in]     port          port number.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTELNETs_SetPort (uint16_t port);

/// \brief Retrieve username of the built-in user account. [\ref thread-safe]
/// \return        pointer to username, a null-terminated string.
///                - NULL if authentication is disabled in the configuration.
extern const char *netTELNETs_GetUsername (void);

/// \brief Set username of the built-in user account. [\ref thread-safe]
/// \param[in]     username      new username, a null-terminated string.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTELNETs_SetUsername (const char *username);

/// \brief Retrieve password of the built-in user account. [\ref thread-safe]
/// \return        pointer to password, a null-terminated string.
///                - NULL if authentication is disabled in the configuration.
extern const char *netTELNETs_GetPassword (void);

/// \brief Reset password of the built-in user account. [\ref thread-safe]
/// \param[in]     password      new password, a null-terminated string.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTELNETs_SetPassword (const char *password);

/// \brief Determine if Telnet server authentication is enabled. [\ref thread-safe]
/// \return
///              - true        = Authentication enabled in the configuration.
///              - false       = Authentication is not enabled.
extern bool      netTELNETs_LoginActive (void);

/// \brief Enable or disable Telnet server authentication. [\ref thread-safe]
/// \param[in]     login      new authentication state:
///                            - true  = Enable authentication.
///                            - false = Disable authentication.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTELNETs_LoginOnOff (bool login);

/// \brief Get IP address and port number of a connected Telnet client. [\ref thread-safe]
/// \param[out]    addr          structure that will receive IP address and port number.
/// \param[in]     addr_len      size of NET_ADDR structure for the client.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTELNETs_GetClient (NET_ADDR *addr, uint32_t addr_len);

/// \brief Get current session number of the Telnet server. [\ref thread-safe]
/// \return        current session number.
extern int32_t   netTELNETs_GetSession (void);

/// \brief Check command string for a command. [\ref thread-safe]
/// \param[in]     cmd           pointer to command string from Telnet client.
/// \param[in]     user_cmd      user command to check for (in upper case).
/// \return
///              - true        = Command found in command string.
///              - false       = Command not found.
extern bool      netTELNETs_CheckCommand (const char *cmd, const char *user_cmd);

/// \brief Request a repeated call to netTELNETs_ProcessCommand function. [\ref thread-safe]
/// \param[in]     delay         time period to wait in number of 100ms ticks.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTELNETs_RepeatCommand (uint32_t delay);

/// \brief Request unsolicited message processing in netTELNETs_ProcessMessage function. [\ref thread-safe]
/// \param[in]     session       session identification, when multiple connections are active.
/// \return      status code that indicates the execution status of the function.
extern netStatus netTELNETs_RequestMessage (int32_t session);

//  ==== Telnet Server User Callbacks ====

/// \brief Process and execute a command requested by the Telnet client. [\ref user-provided]
/// \param[in]     cmd           pointer to command string from Telnet client.
/// \param[out]    buf           output buffer to write the return message to.
/// \param[in]     buf_len       length of the output buffer in bytes.
/// \param[in,out] pvar          pointer to a session's local buffer of 4 bytes.
///                              - 1st call = cleared to 0.
///                              - 2nd call = not altered by the system.
///                              - 3rd call = not altered by the system, etc.
/// \return        number of bytes written to output buffer.
///                - return len | (1u<<31) = repeat flag, the system calls this function
///                                          again for the same command.
///                - return len | (1u<<30) = disconnect flag, the system disconnects
///                                          the Telnet client.
extern uint32_t  netTELNETs_ProcessCommand (const char *cmd, char *buf, uint32_t buf_len, uint32_t *pvar);

/// \brief Request a message for a Telnet server session. [\ref user-provided]
/// \param[in]     msg           code of requested message.
/// \param[out]    buf           output buffer to write the message to.
/// \param[in]     buf_len       length of the output buffer in bytes.
/// \return        number of bytes written to output buffer.
extern uint32_t  netTELNETs_ProcessMessage (netTELNETs_Message msg, char *buf, uint32_t buf_len);

//  ==== Telnet Server Access Interface ====

/// \brief Accept or deny a connection from a remote Telnet client. [\ref user-provided]
/// \param[in]     addr          structure containing IP address and port of remote Telnet client.
/// \return
///              - true        = Connection from the remote client is allowed.
///              - false       = Connection is denied.
extern bool      netTELNETs_AcceptClient (const NET_ADDR *addr);

//  ==== Telnet Server Multi-User Interface ====

/// \brief Check if an user account exist in the user database. [\ref user-provided]
/// \param[in]     username      pointer to username.
/// \return      status information:
///              - User identification number.
///              - 0 if the user is not existing.
extern uint8_t   netTELNETs_CheckUsername (const char *username);

/// \brief Check user account password in the user database. [\ref user-provided]
/// \param[in]     user_id       user identification number.
/// \param[in]     password      pointer to password.
/// \return
///              - true        = password accepted.
///              - false       = invalid password.
extern bool      netTELNETs_CheckPassword (uint8_t user_id, const char *password);

/// \brief Retrieve the user identification number. [\ref thread-safe]
/// \return        user identification number (0 = system administrator).
extern uint8_t   netTELNETs_GetUserId (void);

//  ==== HTTP Server User API ====

/// \brief Start the HTTP server. [\ref thread-safe]
/// \return      status code that indicates the execution status of the function.
extern netStatus netHTTPs_Start (void);

/// \brief Stop the HTTP server. [\ref thread-safe]
/// \return      status code that indicates the execution status of the function.
extern netStatus netHTTPs_Stop (void);

/// \brief Check if the HTTP server is running. [\ref thread-safe]
/// \return
///              - true        = Server is running.
///              - false       = Server is not running.
extern bool      netHTTPs_Running (void);

/// \brief Get port number of the HTTP server. [\ref thread-safe]
/// \return        port number.
extern uint16_t  netHTTPs_GetPort (void);

/// \brief Set port number of the HTTP server. [\ref thread-safe]
/// \param[in]     port          port number.
/// \return      status code that indicates the execution status of the function.
extern netStatus netHTTPs_SetPort (uint16_t port);

/// \brief Retrieve path to the root directory on HTTP server. [\ref thread-safe]
/// \return        pointer to root path, a null-terminated string.
///                - NULL if root folder is disabled in the configuration.
extern const char *netHTTPs_GetRootPath (void);

/// \brief Set path to the root directory on HTTP server. [\ref thread-safe]
/// \param[in]     path          new root path, a null-terminated string.
/// \return      status code that indicates the execution status of the function.
extern netStatus netHTTPs_SetRootPath (const char *path);

/// \brief Retrieve username of the built-in user account. [\ref thread-safe]
/// \return        pointer to username, a null-terminated string.
///                - NULL if authentication is disabled in the configuration.
extern const char *netHTTPs_GetUsername (void);

/// \brief Set username of the built-in user account. [\ref thread-safe]
/// \param[in]     username      new username, a null-terminated string.
/// \return      status code that indicates the execution status of the function.
extern netStatus netHTTPs_SetUsername (const char *username);

/// \brief Retrieve password of the built-in user account. [\ref thread-safe]
/// \return        pointer to password, a null-terminated string.
///                - NULL if authentication is disabled in the configuration.
extern const char *netHTTPs_GetPassword (void);

/// \brief Reset password of the built-in user account. [\ref thread-safe]
/// \param[in]     password      new password, a null-terminated string.
/// \return      status code that indicates the execution status of the function.
extern netStatus netHTTPs_SetPassword (const char *password);

/// \brief Determine if the HTTP server authentication is enabled. [\ref thread-safe]
/// \return
///              - true        = Authentication enabled in the configuration.
///              - false       = Authentication is not enabled.
extern bool      netHTTPs_LoginActive (void);

/// \brief Enable or disable HTTP server authentication. [\ref thread-safe]
/// \param[in]     login      new authentication state:
///                            - true  = Enable authentication.
///                            - false = Disable authentication.
/// \return      status code that indicates the execution status of the function.
extern netStatus netHTTPs_LoginOnOff (bool login);

/// \brief Get IP address and port number of a connected remote HTTP client. [\ref thread-safe]
/// \param[out]    addr          structure that will receive IP address and port number.
/// \param[in]     addr_len      size of NET_ADDR structure for the client.
/// \return      status code that indicates the execution status of the function.
extern netStatus netHTTPs_GetClient (NET_ADDR *addr, uint32_t addr_len);

/// \brief Get current session number of the HTTP server. [\ref thread-safe]
/// \return        current session number.
extern int32_t   netHTTPs_GetSession (void);

/// \brief Retrieve the preferred language setting from the browser. [\ref thread-safe]
/// \return        pointer to the language code, a null-terminated string.
extern const char *netHTTPs_GetLanguage (void);

/// \brief Get Content-Type HTML header, received in XML post request. [\ref thread-safe]
/// \return        pointer to content type header, a null-terminated string.
extern const char *netHTTPs_GetContentType (void);

//  ==== HTTP Server Access Interface ====

/// \brief Accept or deny a connection from a remote HTTP client. [\ref user-provided]
/// \param[in]     addr          structure containing IP address and port of remote HTTP client.
/// \return
///              - true        = Connection from the remote client is allowed.
///              - false       = Connection is denied.
extern bool      netHTTPs_AcceptClient (const NET_ADDR *addr);

//  ==== HTTP Server Multi-User Interface ====

/// \brief Check if an user account exist in the user database. [\ref user-provided]
/// \param[in]     username      pointer to username.
/// \param[in]     password      pointer to password.
/// \return      status information:
///              - User identification number.
///              - 0 if the user is not existing.
extern uint8_t   netHTTPs_CheckAccount (const char *username, const char *password);

/// \brief Retrieve the secret word for the selected user. [\ref user-provided]
/// \param[in]     user_id       user identification number.
/// \param[out]    buf           buffer to store the secret word to.
/// \param[in]     buf_len       length of buffer.
/// \return        none.
extern void      netHTTPs_GetUserSecret (uint8_t user_id, char *buf, uint32_t buf_len);

/// \brief Check if remote user is allowed to access a file on HTTP server. [\ref user-provided]
/// \param[in]     user_id       user identification number.
/// \param[in]     fname         name of a file to access.
/// \return
///              - true        = File access is allowed.
///              - false       = File access is denied.
extern bool      netHTTPs_FileAccess (uint8_t user_id, const char *fname);

/// \brief Retrieve the user identification. [\ref thread-safe]
/// \return        user identification number (0 = system administrator).
extern uint8_t   netHTTPs_GetUserId (void);

/// \brief Calculate HA1 hash value for the given credentials. [\ref thread-safe]
/// \param[in]     username      username, a null-terminated string.
/// \param[in]     password      password, a null-terminated string.
/// \param[out]    buf           buffer to store the hash value to.
/// \param[in]     buf_len       length of buffer.
/// \return      status code that indicates the execution status of the function.
extern netStatus netHTTPs_CalcHashHA1 (const char *username, const char *password, char *buf, uint32_t buf_len);

//  ==== HTTP Server File System Interface ====

/// \brief Open a file for reading on HTTP server. [\ref interface]
/// \param[in]     fname         name of the file to open.
/// \return      status information:
///              - Pointer to an open file.
///              - NULL in case of an error.
extern void     *netHTTPs_fopen (const char *fname);

/// \brief Close a file previously open on HTTP server. [\ref interface]
/// \param[in]     file          pointer to the file to close.
/// \return        none.
extern void      netHTTPs_fclose (void *file);

/// \brief Read block of data from a file on HTTP server. [\ref interface]
/// \param[in]     file          pointer to the file to read from.
/// \param[out]    buf           block of memory to write data to.
/// \param[in]     len           length of data to read in bytes.
/// \return        number of bytes successfully read.
extern uint32_t  netHTTPs_fread (void *file, uint8_t *buf, uint32_t len);

/// \brief Read a string from a file on HTTP server. [\ref interface]
/// \param[in]     file          pointer to the file to read from.
/// \param[out]    buf           output buffer to write data to.
/// \param[in]     size          size of output buffer.
/// \return      status information:
///              - Pointer to string on success.
///              - NULL in case of an error.
extern char     *netHTTPs_fgets (void *file, char *buf, uint32_t size);

/// \brief Retrieve file size and last modification time. [\ref interface]
/// \param[in]     fname         name of the file.
/// \param[out]    fsize         file size in bytes.
/// \param[out]    ftime         created or last modified time.
/// \return        none.
extern void      netHTTPs_fstat (const char *fname, uint32_t *fsize, uint32_t *ftime);

//  ==== HTTP Server CGI ====

/// \brief Process query string received by GET or POST request. [\ref user-provided]
/// \param[in]     qstr          pointer to the query string.
/// \return        none.
extern void      netCGI_ProcessQuery (const char *qstr);

/// \brief Process data received by POST request. [\ref user-provided]
/// \param[in]     code          callback context:
///                              - 0 = www-url-encoded form data.
///                              - 1 = filename for file upload (null-terminated string).
///                              - 2 = file upload raw data.
///                              - 3 = end of file upload (file close requested).
///                              - 4 = any other type of POST data (single or last stream).
///                              - 5 = the same as 4, but with more data to follow.
/// \param[in]     data          pointer to POST data.
/// \param[in]     len           length of POST data.
/// \return        none.
extern void      netCGI_ProcessData (uint8_t code, const char *data, uint32_t len);

/// \brief Generate dynamic web data based on a CGI script. [\ref user-provided]
/// \param[in]     env           environment string.
/// \param[out]    buf           output data buffer.
/// \param[in]     buf_len       size of output buffer (from 536 to 1440 bytes).
/// \param[in,out] pcgi          pointer to a session's local buffer of 4 bytes.
///                              - 1st call = cleared to 0.
///                              - 2nd call = not altered by the system.
///                              - 3rd call = not altered by the system, etc.
/// \return        number of bytes written to output buffer.
///                - return len | (1U<<31) = repeat flag, the system calls this function
///                                          again for the same script line.
///                - return len | (1U<<30) = force transmit flag, the system transmits
///                                          current packet immediately.
extern uint32_t  netCGI_Script (const char *env, char *buf, uint32_t buf_len, uint32_t *pcgi);

/// \brief Process environment variables and convert to ANSI format. [\ref thread-safe]
/// \param[in]     env           pointer to environment variables.
/// \param[out]    ansi          output buffer to write converted variable to.
/// \param[in]     max_len       maximum length of environment variable.
/// \return      status information:
///              - pointer to the remaining environment variables to process.
///              - NULL if there are no more environment variables to process.
extern const char *netCGI_GetEnvVar (const char *env, char *ansi, uint32_t max_len);

/// \brief Override default character encoding in HTML documents. [\ref user-provided]
/// \return        pointer to user defined character set type.
extern const char *netCGI_Charset (void);

/// \brief Add custom MIME type for unsupported file types. [\ref user-provided]
/// \param[in]     file_ext      filename extension, a null-terminated string.
/// \return      MIME type information:
///                - pointer to user defined Content-Type.
///                - NULL for unknown type.
extern const char *netCGI_ContentType (const char *file_ext);

/// \brief Redirect resource URL address to a new location. [\ref user-provided]
/// \param[in]     file_name     resource filename, a null-terminated string.
/// \return      URL redirection information:
///                - pointer to user defined Location.
///                - NULL for no URL address redirection.
extern const char *netCGI_Redirect (const char *file_name);

/// \brief Override default Content-Type for CGX script files. [\ref user-provided]
/// \return        pointer to user defined Content-Type.
extern const char *netCGX_ContentType (void);

/// \brief Add custom HTTP response header. [\ref user-provided]
/// \return        pointer to user defined HTTP header.
extern const char *netCGI_CustomHeader (void);

/// \brief Set HTTP cookie to send to the client. [\ref thread-safe]
/// \param[in]     cookie        cookie definition, a null-terminated string.
/// \return      status code that indicates the execution status of the function.
extern netStatus netCGI_SetCookie (const char *cookie);

/// \brief Process HTTP request. [\ref user-provided]
/// \param[in]     method        requested http method, a null-terminated string.
/// \param[in]     uri           resource identifier, a null-terminated string.
/// \param[in]     header        http request header.
/// \param[in]     header_len    length of http request header.
/// \return        none.
extern void netCGI_ProcessRequest (const char *method, const char *uri, const char *header, uint32_t header_len);

/// \brief Process HTTP cookie. [\ref user-provided]
/// \param[in]     method        requested http method, a null-terminated string.
/// \param[in]     uri           resource identifier, a null-terminated string.
/// \param[in]     cookie        http request cookie.
/// \param[in]     cookie_len    length of http request cookie.
/// \return        none.
extern void netCGI_ProcessCookie (const char *method, const char *uri, const char *cookie, uint32_t cookie_len);

//  ==== SMTP Client User API ====

/// \brief Start SMTP client to send an email in legacy mode. [\ref thread-safe]
/// \param[in]     addr          structure containing IP address and port of SMTP server.
/// \return      status code that indicates the execution status of the function.
extern netStatus netSMTPc_Connect (const NET_ADDR *addr);

/// \brief Send an email in blocking mode. [\ref thread-safe]
/// \param[in]     mail          pointer to email content descriptor.
/// \param[in]     mta           pointer to mail transfer agent descriptor.
/// \return      status code that indicates the execution status of the function.
extern netStatus netSMTPc_SendMail (const NET_SMTP_MAIL *mail, const NET_SMTP_MTA *mta);

//  ==== SMTP Client User Callbacks ====

/// \brief Request parameters for SMTP client session. [\ref user-provided]
/// \param[in]     request       request code.
/// \param[out]    buf           output buffer to write the data to.
/// \param[in]     buf_len       length of the output buffer in bytes.
/// \param[in,out] pvar          pointer to a session's local buffer of 4 bytes.
///                              - 1st call = cleared to 0.
///                              - 2nd call = not altered by the system.
///                              - 3rd call = not altered by the system, etc.
/// \return        number of bytes written to output buffer.
///                - return len | (1u<<31) = repeat flag, the system calls this function
///                                          again when \a request is \ref netSMTPc_RequestBody.
extern uint32_t  netSMTPc_Process (netSMTPc_Request request, char *buf, uint32_t buf_len, uint32_t *pvar);

/// \brief Notify the user application when SMTP client operation ends. [\ref user-provided]
/// \param[in]     event         SMTP client notification event as specified in \ref netSMTPc_Event.
/// \return        none.
/// \note Network library calls this function to inform the user about events.
extern void      netSMTPc_Notify (netSMTPc_Event event);

/// \brief Accept or deny authentication requested by SMTP server. [\ref user-provided]
/// \param[in]     addr          structure containing IP address and port of SMTP server.
/// \return
///              - true        = Authentication is accepted.
///              - false       = Authentication is denied.
extern bool      netSMTPc_AcceptAuthentication (const NET_ADDR *addr);

//  ==== SMTP Client File System Interface ====

/// \brief Open a file for reading in SMTP client. [\ref interface]
/// \param[in]     fname         name of the file to open.
/// \return      status information:
///              - Pointer to an open file.
///              - NULL in case of an error.
extern void     *netSMTPc_fopen (const char *fname);

/// \brief Close a file previously open in SMTP client. [\ref interface]
/// \param[in]     file          pointer to the file to close.
/// \return        none.
extern void      netSMTPc_fclose (void *file);

/// \brief Read block of data from a file in SMTP client. [\ref interface]
/// \param[in]     file          pointer to the file to read from.
/// \param[out]    buf           block of memory to write data to.
/// \param[in]     len           length of data to read in bytes.
/// \return        number of bytes successfully read.
extern uint32_t  netSMTPc_fread (void *file, uint8_t *buf, uint32_t len);

//  ==== SNTP Client User API ====

/// \brief Set mode of operation for SNTP client. [\ref thread-safe]
/// \param[in]     mode          SNTP client operation mode.
/// \return      status code that indicates the execution status of the function.
extern netStatus netSNTPc_SetMode (netSNTPc_Mode mode);

/// \brief Determine current time from NTP or SNTP time server. [\ref thread-safe]
/// \param[in]     addr          structure containing IP address of NTP or SNTP server.
///                              - NULL to use NTP server IP address from system configuration.
/// \param[in]     cb_func       callback function to call, when the session ends.
/// \return      status code that indicates the execution status of the function.
extern netStatus netSNTPc_GetTime (const NET_ADDR *addr, netSNTPc_cb_t cb_func);

/// \brief Determine current time from NTP or SNTP time server in blocking mode. [\ref thread-safe]
/// \param[in]     server        server name or absolute IP address (FQDN or IP address).
///                              - NULL to use NTP server IP address from system configuration.
/// \param[out]    seconds       pointer to the variable to return time in seconds.
/// \param[out]    seconds_fraction
///                              pointer to the variable to return fraction of seconds.
///                              - NULL for none.
/// \return      status code that indicates the execution status of the function.
extern netStatus netSNTPc_GetTimeX (const char *server, uint32_t *seconds, uint32_t *seconds_fraction);

//  ==== SNMP Agent User API ====

/// \brief Send a trap message to the Trap Manager. [\ref thread-safe]
/// \param[in]     addr          structure containing IP address of the Trap server.
///                              - NULL to use Trap server IP address from system configuration.
/// \param[in]     generic       generic trap type:
///                              - 0  = ColdStart trap.
///                              - 1  = WarmStart trap.
///                              - 2  = LinkDown trap.
///                              - 3  = LinkUp trap.
///                              - 4  = AuthenticationFailure trap.
///                              - 5  = EgpNeighborLoss trap.
///                              - 6  = EnterpriseSpecific trap.
/// \param[in]     specific      specific trap type for generic enterpriseSpecific trap:
///                              - must be set to 0 for generic traps 0 ... 5
/// \param[in]     obj_list      object list included in trap message.
/// \return      status code that indicates the execution status of the function.
extern netStatus netSNMP_Trap (const NET_ADDR *addr, uint8_t generic, uint8_t specific, const uint16_t *obj_list);

/// \brief Change SNMP community to a new community. [\ref thread-safe]
/// \param[in]     community     new community, a null-terminated string.
/// \return      status code that indicates the execution status of the function.
extern netStatus netSNMP_SetCommunity (const char *community);

/// \brief Register MIB table to SNMP Agent. [\ref thread-safe]
/// \param[in]     info          pointer to MIB table.
/// \param[in]     size          size of MIB table in bytes.
/// \return      status code that indicates the execution status of the function.
extern netStatus netSNMP_SetMIB_Table (const NET_SNMP_MIB_INFO *info, uint32_t size);

// ==== Network Address Conversion ====

/// \brief Convert IP address from binary to text form. [\ref thread-safe]
/// \param[in]     addr_type     network address type:
///                              - NET_ADDR_IP4 = IPv4 address.
///                              - NET_ADDR_IP6 = IPv6 address.
/// \param[in]     ip_addr       IPv4 or IPv6 address in binary form.
/// \param[out]    string_buf    buffer to store converted IP address to.
/// \param[in]     buf_len       length of a string buffer, at least:
///                              - 16 characters for IPv4 address.
///                              - 40 characters for IPv6 address.
/// \return      pointer to \a string_buf with null-terminated IP address string.
///              - NULL in case of parameter error.
extern const char *netIP_ntoa (int16_t addr_type, const uint8_t *ip_addr, char *string_buf, uint32_t buf_len);

/// \brief Convert IP address from text to binary form. [\ref thread-safe]
/// \param[in]     addr_string   network address string:
///                              - in dotted-decimal IPv4 notation.
///                              - in compressed colon-hexadecimal IPv6 notation.
/// \param[in]     addr_type     network address type:
///                              - NET_ADDR_IP4 = IPv4 address.
///                              - NET_ADDR_IP6 = IPv6 address.
/// \param[out]    ip_addr       IPv6 or IPv6 address in binary form.
/// \return
///              - true        = Conversion successful.
///              - false       = Conversion failed.
extern bool      netIP_aton (const char *addr_string, int16_t addr_type, uint8_t *ip_addr);

/// \brief Convert MAC address from binary to text form. [\ref thread-safe]
/// \param[in]     mac_addr      MAC address in binary form.
/// \param[out]    string_buf    buffer to store converted MAC address to.
/// \param[in]     buf_len       length of a string buffer, at least 18 characters.
/// \return      pointer to \a string_buf with null-terminated MAC address string.
///              - NULL in case of parameter error.
extern const char *netMAC_ntoa (const uint8_t *mac_addr, char *string_buf, uint32_t buf_len);

/// \brief Convert MAC address from text to binary form. [\ref thread-safe]
/// \param[in]     mac_string    address string in hyphen MAC-address notation.
/// \param[out]    mac_addr      MAC address in binary form.
/// \return
///              - true        = Conversion successful.
///              - false       = Conversion failed.
extern bool      netMAC_aton (const char *mac_string, uint8_t *mac_addr);

#ifdef __cplusplus
}
#endif

#endif /* __RL_NET_H */
